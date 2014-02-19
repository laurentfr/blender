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
 * The Original Code is Copyright (C) 2013 Blender Foundation.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): Jason Wilkins
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file ghost/intern/GHOST_ContextCGL.mm
 *  \ingroup GHOST
 * Definition of GHOST_ContextCGL class.
 */

#include "GHOST_ContextCGL.h"



GHOST_ContextCGL::GHOST_ContextCGL(
	int  contextProfileMask,
	int  contextMajorVersion,
	int  contextMinorVersion,
	int  contextFlags)
{

}



GHOST_ContextCGL::~GHOST_ContextCGL()
{

}



GHOST_TSuccess GHOST_ContextCGL::swapBuffers()
{
    return GHOST_kFailure;
}



GHOST_TSuccess GHOST_ContextCGL::activateDrawingContext()
{
    return GHOST_kFailure;
}



GHOST_TSuccess GHOST_ContextCGL::initializeDrawingContext(bool stereoVisual, GHOST_TUns16 numOfAASamples)
{
    return GHOST_kFailure;
}



GHOST_TSuccess GHOST_ContextCGL::releaseNativeHandles()
{
    return GHOST_kFailure;
}
