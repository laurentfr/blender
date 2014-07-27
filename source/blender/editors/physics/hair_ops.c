/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2014 Blender Foundation.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): Blender Foundation,
 *                 Lukas Toenne
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file blender/editors/physics/hair_ops.c
 *  \ingroup edphys
 */

#include "MEM_guardedalloc.h"

#include "BLI_listbase.h"
#include "BLI_math.h"
#include "BLI_utildefines.h"

#include "DNA_hair_types.h"
#include "DNA_modifier_types.h"
#include "DNA_object_types.h"
#include "DNA_particle_types.h"

#include "BKE_context.h"
#include "BKE_hair.h"
#include "BKE_modifier.h"
#include "BKE_object.h"
#include "BKE_particle.h"
#include "BKE_report.h"

#include "HAIR_capi.h"

#include "ED_screen.h"

#include "WM_types.h"
#include "WM_api.h"

#include "physics_intern.h" // own include

static bool ED_hair_get(bContext *C, Object **r_ob, HairSystem **r_hsys)
{
	Object *ob;
	HairModifierData *hmd;
	
	ob = CTX_data_active_object(C);
	if (!ob)
		return false;
	
	hmd = (HairModifierData *)modifiers_findByType(ob, eModifierType_Hair);
	if (!hmd)
		return false;
	
	if (r_ob) *r_ob = ob;
	if (r_hsys) *r_hsys = hmd->hairsys;
	return true;
}

static int ED_hair_active_poll(bContext *C)
{
	return ED_hair_get(C, NULL, NULL);
}

/************************ run simulation *********************/

static int hair_simulate_exec(bContext *C, wmOperator *UNUSED(op))
{
	Object *ob;
	HairSystem *hsys;
	struct HAIR_Solver *solver;
	ED_hair_get(C, &ob, &hsys);
	
	solver = HAIR_solver_new();
	HAIR_solver_init(solver, hsys);
	
	HAIR_solver_step(solver, 0.1f);
	
	HAIR_solver_apply(solver, hsys);
	
	HAIR_solver_free(solver);
	
	WM_event_add_notifier(C, NC_OBJECT|ND_DRAW, ob);
	return OPERATOR_FINISHED;
}

void HAIR_OT_simulate(wmOperatorType *ot)
{
	/* identifiers */
	ot->idname = "HAIR_OT_simulate";
	ot->name = "Simulate Hair";
	ot->description = "Evaluate a number of discrete simulation time steps in the hair system";

	/* callbacks */
	ot->exec = hair_simulate_exec;
	ot->poll = ED_hair_active_poll;

	/* flags */
	ot->flag = OPTYPE_REGISTER|OPTYPE_UNDO;
}

/************************ copy hair data from old particles *********************/

static void hair_copy_from_particles_psys(Object *ob, HairSystem *hsys, Object *UNUSED(pob), ParticleSystem *psys)
{
	HairCurve *hairs;
	int tothairs;
	float mat[4][4];
	int i, k;
	
	/* matrix for bringing hairs from pob to ob space */
	invert_m4_m4(mat, ob->obmat);
	
	tothairs = psys->totpart;
	hairs = BKE_hair_curve_add_multi(hsys, tothairs);
	
	for (i = 0; i < tothairs; ++i) {
		ParticleCacheKey *pa_cache = psys->pathcache[i];
		HairCurve *hair = hairs + i;
		HairPoint *points;
		int totpoints;
		
		totpoints = pa_cache->steps + 1;
		points = BKE_hair_point_append_multi(hsys, hair, totpoints);
		
		for (k = 0; k < totpoints; ++k) {
			ParticleCacheKey *pa_key = pa_cache + k;
			HairPoint *point = points + k;
			
			mul_v3_m4v3(point->co, mat, pa_key->co);
			zero_v3(point->vel);
		}
	}
}

static int hair_copy_from_particles_exec(bContext *C, wmOperator *UNUSED(op))
{
	Object *ob;
	HairSystem *hsys;
	ED_hair_get(C, &ob, &hsys);
	
	CTX_DATA_BEGIN (C, Base *, base, selected_bases) {
		Object *pob = base->object;
		ParticleSystem *psys;
		
		for (psys = pob->particlesystem.first; psys; psys = psys->next) {
			if (psys->part->type != PART_HAIR)
				continue;
			
			hair_copy_from_particles_psys(ob, hsys, pob, psys);
		}
	}
	CTX_DATA_END;
	
	WM_event_add_notifier(C, NC_OBJECT|ND_DRAW, ob);
	return OPERATOR_FINISHED;
}

void HAIR_OT_copy_from_particles(wmOperatorType *ot)
{
	/* identifiers */
	ot->idname = "HAIR_OT_copy_from_particles";
	ot->name = "Copy from particles";
	ot->description = "Copy hair data from particles to the hair system";

	/* callbacks */
	ot->exec = hair_copy_from_particles_exec;
	ot->poll = ED_hair_active_poll;

	/* flags */
	ot->flag = OPTYPE_REGISTER|OPTYPE_UNDO;
}
