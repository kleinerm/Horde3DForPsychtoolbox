/*
 * Horde3DCore.cpp -- Source code of Horde3DCore MEX file.
 *
 * Horde3DCore is a thin wrapper around the C/C++ interface of the Horde3D graphics engine.
 * It allows to interface scripting languages with a Matlab MEX API compatible interface to
 * Horde3D. As such it is useable on Matlab and on GNU/Octave Version 3.2 and later.
 *
 * This file needs Psychtoolbox-3 for OpenGL context and window setup and management.
 * It acts as a 3D OpenGL rendering plugin for rendering with Horde3D into Psychtoolbox
 * Onscreen- or Offscreen windows.
 *
 * See also HordeHelper.m for a high-level interface to this routine.
 * See makeHorde.m for a "Makefile" for building this source code into a MEX plugin.
 *
 * You must have the Horde3D runtime dynamic link libraries installed in your system
 * library path or the local folder where this MEX file resides, otherwise loading the
 * MEX file will fail.
 *
 * The Horde3D engine is licensed to you under the Eclipse Public License V 1.0 (EPL).
 * See the file license-epl.html for a copy of the EPL license.
 *
 * This wrapper plugin is licensed as follows:
 *
 * Copyright (c) 2009 - 2012 Mario Kleiner.
 * Copyright (c) 2011 Yasir N. Khan.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

// Include Matlab/Octave MEX API:
#include "mex.h"

// Include standard system libraries:
#include <stdlib.h>
#include <iostream>
#include <sstream>

// Include Horde engine API:
#include "Horde3D.h"
#include "Horde3DUtils.h"
#include <math.h>
#include <iomanip>

using namespace std;

bool mexinitialized = false;
#define MAX_CMD_LENGTH 30
char  cmd[MAX_CMD_LENGTH];
bool  cmdassigned;

#define MAX_STR_LENGTH 1024
char  str[MAX_STR_LENGTH];
char  str1[MAX_STR_LENGTH];
char  str2[MAX_STR_LENGTH];
char  str3[MAX_STR_LENGTH];

int			winWidth, winHeight;
const char*	config;
float		_x, _y, _z, _rx, _ry;	// Viewer position and orientation

// Engine objects
H3DRes		_fontMatRes, _logoMatRes, _panelMatRes;
H3DRes		_pipeRes, _hdrPipeRes, _forwardPipeRes;
H3DNode		_knight, _particleSys;
H3DNode		_cam;

bool hordeInit(void)
{
	// Initialize engine
	if( !h3dInit() )
	{	
		h3dutDumpMessages();
		return(false);
	}
	
	return(true);
}

bool hordeDemoSetup(int winWidth, int winHeight, const char* config)
{
	// Set options
	h3dSetOption( H3DOptions::LoadTextures, 1 );
	h3dSetOption( H3DOptions::TexCompression, 0 );
	h3dSetOption( H3DOptions::FastAnimation, 0 );
	h3dSetOption( H3DOptions::MaxAnisotropy, 4 );
	h3dSetOption( H3DOptions::ShadowMapSize, 2048 );

	// Add resources
	// Pipelines
	_forwardPipeRes = h3dAddResource( H3DResTypes::Pipeline, "pipelines/forward.pipeline.xml", 0 );

	// Overlays
	_fontMatRes = h3dAddResource( H3DResTypes::Material, "overlays/font.material.xml", 0 );
	_panelMatRes = h3dAddResource( H3DResTypes::Material, "overlays/panel.material.xml", 0 );
	_logoMatRes = h3dAddResource( H3DResTypes::Material, "overlays/logo.material.xml", 0 );

	// Environment
	H3DRes envRes = h3dAddResource( H3DResTypes::SceneGraph, "models/sphere/sphere.scene.xml", 0 );

	// Knight
	H3DRes knightRes = h3dAddResource( H3DResTypes::SceneGraph, "models/knight/knight.scene.xml", 0 );
	H3DRes knightAnim1Res = h3dAddResource( H3DResTypes::Animation, "animations/knight_order.anim", 0 );
	H3DRes knightAnim2Res = h3dAddResource( H3DResTypes::Animation, "animations/knight_attack.anim", 0 );

	// Particle system
	H3DRes particleSysRes = h3dAddResource( H3DResTypes::SceneGraph, "particles/particleSys1/particleSys1.scene.xml", 0 );
	
	// Load resources
	h3dutLoadResourcesFromDisk( "Content" );

	// Add scene nodes

	// Add camera
	_cam = h3dAddCameraNode( H3DRootNode, "Camera", _forwardPipeRes);

	// Add environment
	H3DNode env = h3dAddNodes( H3DRootNode, envRes );
	h3dSetNodeTransform( env, 0, -20, 0, 0, 0, 0, 20, 20, 20 );

	// Add knight
	_knight = h3dAddNodes( H3DRootNode, knightRes );
	h3dSetNodeTransform( _knight, 0, 0, 0, 0, 180, 0, 0.1f, 0.1f, 0.1f );
	h3dSetupModelAnimStage( _knight, 0, knightAnim1Res, 0, "", false );
	h3dSetupModelAnimStage( _knight, 1, knightAnim2Res, 0, "", false );

	// Attach particle system to hand joint
	h3dFindNodes( _knight, "Bip01_R_Hand", H3DNodeTypes::Joint );
	H3DNode hand = h3dGetNodeFindResult( 0 );
	_particleSys = h3dAddNodes( hand, particleSysRes );
	h3dSetNodeTransform( _particleSys, 0, 40, 0, 90, 0, 0, 1, 1, 1 );

	// Add light source
	H3DNode light = h3dAddLightNode( H3DRootNode, "Light1", 0, "LIGHTING", "SHADOWMAP" );
	h3dSetNodeTransform( light, 0, 15, 10, -60, 0, 0, 1, 1, 1 );
	h3dSetNodeParamF( light, H3DLight::RadiusF, 0, 30 );
	h3dSetNodeParamF( light, H3DLight::FovF, 0, 90 );
	h3dSetNodeParamI( light, H3DLight::ShadowMapCountI, 1 );
	h3dSetNodeParamF( light, H3DLight::ShadowMapBiasF, 0, 0.01f );
	h3dSetNodeParamF( light, H3DLight::ColorF3, 0, 1.0f );

	// Resize viewport
	h3dSetNodeParamI(_cam, H3DCamera::ViewportXI, 0);
	h3dSetNodeParamI(_cam, H3DCamera::ViewportYI, 0);
	h3dSetNodeParamI(_cam, H3DCamera::ViewportWidthI, winWidth);
	h3dSetNodeParamI(_cam, H3DCamera::ViewportHeightI, winHeight);
	
	// Set virtual camera parameters
	h3dSetupCameraView( _cam, 45.0f, (float)winWidth / winHeight, 0.1f, 1000.0f );

	_x = 5; _y = 3; _z = 19; _rx = 7; _ry = 15;

	// Set camera parameters
	h3dSetNodeTransform( _cam, _x, _y, _z, _rx ,_ry, 0, 1, 1, 1 );

	// Online!
	return true;
}

void shutDown(void)
{
	// Only shutdown if we're online:
	if (!mexinitialized) return;
	
	// Shutdown HORDE engine, release all ressources:
	h3dRelease();

	// We're offline:
	mexinitialized = false;

	return;
}

void hordeUpdate(float tSimulation, float curFPS)
{
	float _weight = 0.0;
	
	// Do animation blending
	h3dSetModelAnimParams( _knight, 0, tSimulation * 24.0f, _weight );
	h3dSetModelAnimParams( _knight, 1, tSimulation * 24.0f, 1.0f - _weight );
	
	// Animate particle systems (several emitters in a group node)
	unsigned int cnt = h3dFindNodes( _particleSys, "", H3DNodeTypes::Emitter );
	for( unsigned int i = 0; i < cnt; ++i ) h3dAdvanceEmitterTime( h3dGetNodeFindResult( i ), 1.0f / curFPS );

	// Show logo
// MK:	h3dShowOverlays( 0.75f, 0.8f, 0, 1, 0.75f, 1, 0, 0,
// 	                      1, 1, 1, 0, 1, 0.8f, 1, 1,
// 	                      1, 1, 1, 1, _logoMatRes, 7 );
	
	// Render scene
	h3dRender( _cam );

	// Finish rendering of frame
	h3dFinalizeFrame();

	// Remove all overlays
	h3dClearOverlays();

	// Write all mesages to log file
	h3dutDumpMessages();

	return;
}

bool IsCommand(char *s2)
{
	char a;
	char* s1 = &cmd[0];
	
	do {
		a=*s1++;
		if(tolower(a)!=tolower(*s2++)) return(false);
	} while(a!='\0');

	// Mark command as detected:
	cmdassigned = true;
	
	return(true);
}

// mexFunction is the main entry point and master dispatcher for Matlab and GNU/Octave:
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{
	float tSimulation, curFPS;
	double* p;
	float v[9];

	int i1, i2, i3, i4, i5;
	int handle;
	double* d_H3DNodes;
	
	const char* me = mexFunctionName();
	
	if (nrhs < 1) {
		mexPrintf("Horde3DCore: A simple Matlab and GNU/Octave MEX file for interfacing Psychtoolbox-3 with the Horde3D graphics engine.\n");
		mexPrintf("Written by Mario Kleiner & Yasir N. Khan. Type %s('License') to see a printout of the full software license.\n\n", me);
		mexPrintf("Acknowledgements: The development of Horde3DCore was supported by the department for Human Perception, Cognition and Action\n");
		mexPrintf("at the Max Planck Institute for Biological Cybernetics, Tuebingen, Germany.\n\n");
		mexPrintf("Website of Horde3D with online documentation, Wiki, mailinglist and other resources: <http://www.horde3d.org/>\n");
		mexPrintf("Using Horde3D engine version: %s, which is licensed to you under Eclipse Public License V 1.0.\n", h3dGetVersionString());
		mexPrintf("\n");
		mexPrintf("Usage:\n\n");
		mexPrintf("%s('Initialize');\n", me);
		mexPrintf("-- Initialize Horde engine. You must call this command before any other command and while an OpenGL context is bound.\n\n");
		mexPrintf("%s('DemoSetup', winWidth, winHeight [,overrideConfig]);\n", me);
		mexPrintf("-- Setup Horde engine for rendering to a window of size 'winWidth' x 'winHeight' pixels.\n\n");
		mexPrintf("%s('Shutdown');\n", me);
		mexPrintf("-- Shutdown Horde engine.\n\n");
		mexPrintf("%s('SetOption', param, value);\n", me);
		mexPrintf("-- Set Horde engine parameter 'param' to new value 'value'\n\n");
		mexPrintf("%s('Clear;\n", me);
		mexPrintf("-- Removes all resources and scene nodes.\n\n");
		mexPrintf("str = %s('GetMessage', level, time);\n", me);
		mexPrintf("-- Gets the next message from the message queue and returns it as a string. Additionally stores message level and time in the corresponding parameters.\n\n");
		mexPrintf("value = %s('GetOption', param);\n", me);
		mexPrintf("-- Gets the specified option parameter of the engine and returns its value as a double.\n\n");
		mexPrintf("value = %s('GetStat', param, reset);\n", me);
		mexPrintf("-- Gets the specified statistic value of the engine and returns it as a double. 'reset' flag specifies whether the statistic value should be reset.\n\n");
		// MK: mexPrintf("%s('ShowOverlays', x_tl, y_tl, u_tl, v_tl, x_bl, y_bl, u_bl, v_bl, x_br, y_br, u_br, v_br, x_tr, y_tr, u_tr, v_tr, colR, colG, colB, colA, materialRes,
		//  layer);\n", me);
		// mexPrintf("-- Shows an overlay on the screen. An overlay is a 2D image that can be used to render 2D GUI elements.  The coordinate system used has
		// its origin (0, 0) at the top-left corner of the screen and its maximum (1, 1) at the bottom-right corner. In texture coordinates (0, 0) corresponds to the lower
		// left corner of the image. Overlays with smaller layer numbers are drawn before overlays with higher layer numbers.\n\n");
		mexPrintf("%s('ClearOverlays');\n", me);
		mexPrintf("-- Removes all overlays that were added usig showOverlay.\n\n");
		mexPrintf("handle = %s('GetResType', res);\n", me);
		mexPrintf("-- Returns the type of a resource.\n\n");
		mexPrintf("str = %s('GetResName', res);\n", me);
		mexPrintf("-- Returns the name of a resource.\n\n");
		mexPrintf("handle = %s('GetNextResource', type, start);\n", me);
		mexPrintf("-- Returns the next resource of the specified type.\n\n");
		mexPrintf("handle = %s('FindResource', type, name);\n", me);
		mexPrintf("-- Finds a resource and returns its handle.\n\n");
		mexPrintf("handle = %s('CloneResource', sourceRes, name);\n", me);
		mexPrintf("-- Duplicates a specified resource with the new name and returns its handle.\n\n");
		mexPrintf("value = %s('RemoveResource', res);\n", me);
		mexPrintf("-- Removes a resource and returns the number of remaining references.\n\n");
		mexPrintf("%s('IsResLoaded', res);\n", me);
		mexPrintf("-- Checks if a resource is loaded. Outputs a debug message if resource not loaded.\n\n");
		mexPrintf("%s('LoadResource', res, data, size);\n", me);
		mexPrintf("-- Loads data of specified for the specified resource. Outputs debug if unsuccessful\n\n");
		mexPrintf("%s('UnloadResource', res);\n", me);
		mexPrintf("-- Unloads a resource.\n\n");
		mexPrintf("value = %s('GetResElemCount', res, element);\n", me);
		mexPrintf("-- Gets the number of elements of the specified element type in a resource.\n\n");
		mexPrintf("index = %s('FindResElem', res, elem, param, value);\n", me);
		mexPrintf("-- Searches in a specified resource for the first element of the specified type that has the property with the specified name set to the specified search value.\n\n");
		mexPrintf("value = %s('GetResParamI', res, elem, elemIndex, param);\n", me);
		mexPrintf("-- Returns a specified property of the specified resource element.  The property must be of the type int.\n\n");
		mexPrintf("%s('SetResParamI', res, elem, elemIndex, param, value);\n", me);
		mexPrintf("-- Sets a specified property of the specified resource element to the specified value.  The property must be of the type int.\n\n");
		mexPrintf("value = %s('GetResParamF', res, elem, elemIndex, param, compIndex);\n", me);
		mexPrintf("-- Returns a specified property of the specified resource element.  The property must be of the type float.\n\n");
		mexPrintf("%s('SetResParamF', res, elem, elemIndex, param, value);\n", me);
		mexPrintf("-- Sets a specified property of the specified resource element to the specified value.  The property must be of the type float.\n\n");
		mexPrintf("string = %s('GetResParamStr', res, elem, elemIndex, param);\n", me);
		mexPrintf("-- Returns a specified property of the specified resource element.  The property must be of the type string.\n\n");
		mexPrintf("%s('SetResParamStr', res, elem, elemIndex, param, value);\n", me);
		mexPrintf("-- Sets a specified property of the specified resource element to the specified value.  The property must be of the type string.\n\n");
		mexPrintf("handle = %s('MapResStream', res, elem, elemIndex, stream, read, write);\n", me);
		mexPrintf("-- Maps the specified stream of a specified resource element and returns a pointer to the stream data or NULL. The required access to data can be specified by read and write flags.\n\n");
		mexPrintf("%s('UnmapResStream', res);\n", me);
		mexPrintf("-- Unmaps a previously mapped resource stream.\n\n");
		mexPrintf("handle = %s('QueryUnloadedResource', index);\n", me);
		mexPrintf("-- Returns handle to an unloaded resource.\n\n");
		mexPrintf("%s('ReleaseUnusedResources');\n", me);
		mexPrintf("-- Frees resources that are no longer used.\n\n");
		mexPrintf("handle = %s('CreateTexture', name, width, height, fmt, flags);\n", me);
		mexPrintf("-- Creates a Texture resource with specified name, dimensions, texture format and flags.\n\n");
		mexPrintf("%s('SetShaderPreambles', vertPreamble, 	fragPreamble);\n", me);
		mexPrintf("-- Sets text preambles of all vertex and fragment shader resources.\n\n");
		mexPrintf("%s('SetMaterialUniform', materialRes, name, a, b, c, d);\n", me);
		mexPrintf("-- Sets the specified shader uniform of the specified material resource to the four specified values.\n\n");
		mexPrintf("%s('GetRenderTargetData', pipelineRes, targetName, bufIndex, width, height, compCount, dataBuffer, bufferSize);\n", me);
		mexPrintf("-- Reads the pixel data of a pipeline render target buffer of the specified render target from the specified pipeline resource and stores it in the specified float array.\n\n");
		mexPrintf("value = %s('GetNodeType', node);\n", me);
		mexPrintf("-- Returns the type of a specified scene node, or 'Unknown' if the node handle is invalid.\n\n");
		mexPrintf("handle = %s('GetNodeParent', node);\n", me);
		mexPrintf("-- Returns the parent node of a specified scene node, or 0 if the node handle is invalid or root node.\n\n");
		mexPrintf("%s('SetNodeParent', node, parent);\n", me);
		mexPrintf("-- Relocates a node in the scene graph from the current parent to the new specified parent.\n\n");
		mexPrintf("handle = %s('GetNodeChild', node, index);\n", me);
		mexPrintf("-- Returns the handle to the n-th child node of the specified node, or 0 if the child doesn't exist.\n\n");
		mexPrintf("%s('RemoveNode', node);\n", me);
		mexPrintf("-- Removes the specified node and all of it's children from the scene.\n\n");
		mexPrintf("%s('SetNodeActivation', node, active);\n", me);
		mexPrintf("-- Sets the activation state of the specified node to active or inactive. Inactive nodes with all their children are excluded from rendering.\n\n");		
		mexPrintf("%s('CheckNodeTransFlag', node, reset);\n", me);
		mexPrintf("-- Checks if a scene node has been transformed by the engine since the last time the transformation flag was reset.\n\n");
		mexPrintf("[relMat, absMat] = %s('GetNodeTransMats', node);\n", me);
		mexPrintf("-- Stores a pointer to the relative and absolute transformation matrices of the specified node in the specified pointer varaibles.\n\n");
		mexPrintf("%s('SetNodeTransMat', node, mat4x4);\n", me);
		mexPrintf("-- Sets the relative transformation matrix of the specified scene node.\n\n");
		mexPrintf("value = %s('GetNodeParamI', node, param);\n", me);
		mexPrintf("-- Gets the specified integer property of the specified scene node.\n\n");
		mexPrintf("value = %s('GetNodeParamF', node, param, compIdx);\n", me);
		mexPrintf("-- Gets the specified float property of the specified scene node.\n\n");
		mexPrintf("string = %s('GetNodeParamStr', node, param);\n", me);
		mexPrintf("-- Gets the specified string property of the specified scene node and returns it as a constant pointer.\n\n");
		mexPrintf("%s('SetNodeParamStr', node, param, value);\n", me);
		mexPrintf("-- Sets a specified string property of the specified node to a specified value.\n\n");
		mexPrintf("[minX, minY, minZ, maxX, maxY, maxZ] = %s('GetNodeAABB', node);\n", me);
		mexPrintf("-- Stores the world coordinates of the axis aligned bounding box of a specified node in the specified variables.\n\n");
		mexPrintf("value = %s('CastRay', node, ox, oy, oz, dx, dy, dz, numNearest);\n", me);
		mexPrintf("-- Checks recursively if the specified ray intersects the specified node or one of its children. It finds intersections relative to the ray origin and returns the number of intersecting scene nodes.\n\n");
		mexPrintf("[distance, intersection] = %s('GetCastRayResult', index, node);\n", me);
		mexPrintf("-- This functions is used to access an indexed result of a previous castRay query. The intersection data is copied to the specified variables.\n\n");
		mexPrintf("value = %s('CheckNodeVisibility', node, cameraNode, checkOcclusion, calcLod);\n", me);
		mexPrintf("-- Checks if a specified node is visible from the perspective of a specified camera, with specified flags.\n\n");
		mexPrintf("handle = %s('AddGroupNode', parent, name);\n", me);
		mexPrintf("-- Creates a new Group node and attaches it to the specified parent node.\n\n");
		mexPrintf("handle = %s('AddModelNode', parent, name, geometryRes);\n", me);
		mexPrintf("-- Creates a new Model node and attaches it to the specified parent node.\n\n");
		mexPrintf("%s('SetModelMorpher', modelNode, target, weight);\n", me);
		mexPrintf("-- Sets the weight of a specified morph target in the specified model node, or of all morph targets if target is an empty string.\n\n");
		mexPrintf("handle = %s('AddMeshNode', parent, name, materialRes, batchStart, batchCount, vertRStart, vertREnd);\n", me);
		mexPrintf("-- Creates a new Mesh node and attaches it to the specified parent node.\n\n");
		mexPrintf("handle = %s('AddJointNode', parent, name, jointIndex);\n", me);
		mexPrintf("-- Creates a new Joint node and attaches it to the specified parent node.\n\n");
		mexPrintf("handle = %s('AddCameraNode', parent, name, pipelineRes);\n", me);
		mexPrintf("-- Creates a new Camera node and attaches it to the specified parent node.\n\n");
		mexPrintf("projMat = %s('GetCameraProjMat', cameraNode);\n", me);
		mexPrintf("-- Gets the camera projection matrix used for bringing the geometry to screen space and copies it to the specified array.\n\n");
		mexPrintf("handle = %s('AddEmitterNode', parent, name, materialRes, particleEffectRes, maxParticleCount, respawnCount);\n", me);
		mexPrintf("-- Creates a new Emitter node and attaches it to the specified parent node.\n\n");
		mexPrintf("%s('AdvanceEmitterTime', emitterNode, timeDelta);\n", me);
		mexPrintf("-- Advances the time value of an Emitter node with timeDelta being the time elapsed since the last call of this function.\n\n");
		mexPrintf("%s('HasEmitterFinished', emitterNode);\n", me);
		mexPrintf("-- Checks if an Emitter node is still alive and has living particles or will spawn new particles.\n\n");
		mexPrintf("handle = %s('AddResource', resId, resName);\n", me);
		mexPrintf("-- Add a resource of type 'resId' from file with name 'resName', return a 'handle' to it.\n\n");
		mexPrintf("%s('LoadResources', basePath);\n", me);
		mexPrintf("-- Load all resources defined by previous 'AddResource' calls from the base directory 'basePath'\n\n");
		mexPrintf("handle = %s('AddCamera', Name, pipelineId);\n", me);
		mexPrintf("-- Add a camera node with name 'Name' and rendering pipeline resource 'pipelineId', return a 'handle' to it.\n\n");
		mexPrintf("handle = %s('AddNodes', parentNode, sceneGraphHandle);\n", me);
		mexPrintf("-- Add a collection of scenegraph nodes with handle 'sceneGraphHandle' to parent node with handle 'parentNode' and return a 'handle' to it.\n\n");
		mexPrintf("%s('SetNodeTransform', H3DNode, posX, posY, posZ, rotX, rotY, rotZ, scaleX, scaleY, scaleZ);\n", me);
		mexPrintf("-- Set rigid position, orientation and scale of scenegraph node 'H3DNode' to given new posXYZ, rotXYZ and scaleXYZ.\n\n");
		mexPrintf("[posX, posY, posZ, rotX, rotY, rotZ, sX, sY, sZ] = %s('GetNodeTransform', H3DNode);\n", me);
		mexPrintf("-- Get rigid position, orientation and scale of scenegraph node 'H3DNode' as returned posXYZ, rotXYZ and scaleXYZ.\n\n");
		mexPrintf("%s('SetNodeTransforms', xforms);\n", me);
		mexPrintf("-- Set rigid positions, orientations and scales of many scenegraph nodes at once.\n");
		mexPrintf("'xforms' is a 10-by-n matrix. Each of the 'n' columns encodes one updated node with handle 'handle' and new rigid parameters [handle, tx, ty, tz, rx, ry, rz, sx, sy, sz].\n\n");
		mexPrintf("xforms = %s('GetNodeTransforms', H3DNodes);\n", me);
		mexPrintf("-- Get rigid positions, orientations and scales of many scenegraph nodes at once. 'H3DNodes' is a vector of H3DNodes to query. 'xforms' is a matrix containing all xforms of all queried nodes.\n\n");
		mexPrintf("%s('SetupModelAnimStage', H3DNode, trackIdx, animationResourceHandle, startNodeName, additiveFlag);\n", me);
		mexPrintf("-- Assign animation resource 'animationResourceHandle' as motion track 'trackIdx' to scenegraph node 'H3DNode'. 'startNodeName' is name of\n");
		mexPrintf("first node to apply to or an empty string if animation should apply to first node of model. Animation is additive if 'additiveFlag' is non-zero.\n\n");
		mexPrintf("handles = %s('FindNodes', parentNode, name, type);\n", me);
		mexPrintf("-- Find nodes with name 'name' of type 'type' within the scene graph rooted at 'parentNode'. Return a vector containing 'handles' of all found nodes, or an empty vector if none are found.\n\n");
		mexPrintf("handle = %s('AddLightNode', parentNode, name, materialHandle, lightContext, shadowContext);\n", me);
		mexPrintf("-- Add a lighting node with name 'name' to 'parentNode'. 'materialHandle' is the name of a materialResource or zero for default. Return a 'handle' to it.\n\n");
		mexPrintf("%s('SetNodeParamf', H3DNode, paramType, value);\n", me);
		mexPrintf("-- Set floating point parameter 'paramType' of node 'H3DNode' to value 'value'.\n\n");
		mexPrintf("%s('SetNodeParami', H3DNode, paramType, value);\n", me);
		mexPrintf("-- Set integer parameter 'paramType' of node 'H3DNode' to value 'value'.\n\n");
		mexPrintf("%s('SetupViewport', camera, x, y, winWidth, winHeight);\n", me);
		mexPrintf("-- Set target area in window for rendering with 'camera' (the Viewport): Top left corner is (x,y), width is winWidth, height is winHeight.\n\n");
		mexPrintf("%s('SetupCameraView', camera, aperture, aspect, nearClip, farClip);\n", me);
		mexPrintf("-- Set intrinsic camera parameters of pinhole camera 'camera': 'aperture' in degrees, 'aspect' = width / height, 'nearClip' and 'farClip' for near- and far clipping planes.\n\n");
		mexPrintf("%s('SetModelAnimParams', modelNode, trackIdx, time, weight);\n", me);
		mexPrintf("-- Set parameters for motiontrack: For model 'modelNode', motiontrack 'trackIdx', set simulation time to 'time' and morphWeight to 'weight'.\n\n");
		mexPrintf("%s('SetModelsAnimParameters', params);\n", me);
		mexPrintf("-- Set parameters for motiontrack: 'params' is a 4-by-n matrix with one column per parameter set 'modelNode', motiontrack 'trackIdx', simulation time 'time' and morphWeight 'weight'.\n\n");
		mexPrintf("%s('Render', camera);\n", me);
		mexPrintf("-- Perform update & render cycle into current active window with camera 'camera'.\n\n");
		mexPrintf("%s('FinalizeFrame');\n", me);
		mexPrintf("-- Mark this frame done.\n\n");
		mexPrintf("%s('DumpMessages');\n", me);
		mexPrintf("-- Dump all pending diagnostic messages to logfile.\n\n");
		mexPrintf("%s('UpdateRender', simulationTime, currentFPS);\n", me);
		mexPrintf("-- Perform update & render cycle into current active window for simulation time 'simulationTime' seconds.\n\n");
		mexPrintf("\n");
		mexPrintf("\n");
		return;
	}

	// Adapt parameter count to simplify checks below:
	nrhs--;
	
	/* First argument must be the command code: */
	cmd[0] = 0;
	mxGetString(prhs[0], (char*) &cmd, MAX_CMD_LENGTH-1);
	cmdassigned = false;
	
	// mexPrintf("CMD: %s\n", cmd);

	if (IsCommand((char*)"License")) {
		// Dump or License info to screen:
		mexPrintf("Copyright (c) 2009 - 2012 Mario Kleiner.\n");
		mexPrintf("Copyright (c) 2011 Yasir N. Khan.\n");
		mexPrintf("\n");
		mexPrintf("Permission is hereby granted, free of charge, to any person obtaining a copy\n");
		mexPrintf("of this software and associated documentation files (the \"Software\"), to deal\n");
		mexPrintf("in the Software without restriction, including without limitation the rights\n");
		mexPrintf("to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n");
		mexPrintf("copies of the Software, and to permit persons to whom the Software is\n");
		mexPrintf("furnished to do so, subject to the following conditions:\n");
		mexPrintf("\n");
		mexPrintf("The above copyright notice and this permission notice shall be included in\n");
		mexPrintf("all copies or substantial portions of the Software.\n");
		mexPrintf("\n");
		mexPrintf("THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n");
		mexPrintf("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n");
		mexPrintf("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n");
		mexPrintf("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n");
		mexPrintf("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n");
		mexPrintf("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN\n");
		mexPrintf("THE SOFTWARE.\n");
		mexPrintf("\n\n");
		return;
	}
	
	// Child protection:

	if (IsCommand((char*)"HordeVersion")) {
	    const char *str = h3dGetVersionString();
	    mexPrintf("Horde3D Version: %s\n", str);
	}
	
	if (IsCommand((char*)"Initialize")) {
		// Init.
		if (mexinitialized) mexErrMsgTxt("Tried to initialize Horde3DCore, but it is already initialized! Call Horde3DCore('Shutdown') first!");

		// Initialize and setup the engine:
		if (!hordeInit()) { h3dRelease(); mexErrMsgTxt("Horde3D: Initialize: Horde3D engine init failed!"); }
		
		// Register shutDown() function for shutdown if MEX file is jettisoned:
		mexAtExit(shutDown);
		
		// We're online:
		mexinitialized = true;
	}

	if (IsCommand((char*)"Shutdown")) {
		// Shutdown and release engine:
		shutDown();
		return;
	}

	// All further commands are only viable if engine is initialized:
	if (!mexinitialized) mexErrMsgTxt("Tried to call Horde3DCore function, but Horde3DCore isn't initialized yet! Call Horde3DCore('Initialize') first!");
	
	if (IsCommand((char*)"DemoSetup")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: DemoSetup: Required parameters 'winWidth' and 'winHeight' missing!");
		if (nrhs < 2) mexErrMsgTxt("Horde3D: DemoSetup: Required parameter 'winHeight' missing!");
		
		// Retrieve startup parameters:
		winWidth = (int) mxGetScalar(prhs[1]);
		winHeight = (int) mxGetScalar(prhs[2]);
		
		if (!hordeDemoSetup(winWidth, winHeight, NULL)) mexErrMsgTxt("Horde3D: Horde3D engine demo setup failed!");
	}
	
	if (IsCommand((char*)"UpdateRender")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: UpdateRender: Required parameters 'simulationTime' and 'currentFPS' missing!");
		if (nrhs < 2) mexErrMsgTxt("Horde3D: UpdateRender: Required parameter 'currentFPS' missing!");
		tSimulation = (float) mxGetScalar(prhs[1]);
		curFPS = (float) mxGetScalar(prhs[2]);
		hordeUpdate(tSimulation, curFPS);
	}

	if (IsCommand((char*)"AddResource")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: AddResource: Required parameters 'Resourcetype' and 'Name' missing!");
		if (nrhs < 2) mexErrMsgTxt("Horde3D: AddResource: Required parameter 'Name' missing!");
		
		// Get resource name:
		str[0] = 0;
		mxGetString(prhs[2], (char*) &str, MAX_STR_LENGTH-1);
		
		// Try to add resource:
		i1 = (int) mxGetScalar(prhs[1]);
		if ((handle = (int) h3dAddResource( i1 , str, 0 )) == 0) {
			mexPrintf("Failed to add Resource %i: %s.\n", i1, str);
			mexErrMsgTxt("Horde3D: AddResource: FAILED!");
		}
		
		// Return resource handle:
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*mxGetPr(plhs[0]) = handle;		
	}

	if (IsCommand((char*)"LoadResources")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: LoadResources: Basepath missing!");
		
		// Get resource path:
		str[0] = 0;
		mxGetString(prhs[1], (char*) &str, MAX_STR_LENGTH-1);
		
		// Load resources
		if (!h3dutLoadResourcesFromDisk( str )) {
			mexPrintf("Failed to load at least one of the requested Resources from basepath %s.\n", str);
			mexErrMsgTxt("Horde3D: LoadResources: FAILED!");
		}
	}

	if (IsCommand((char*)"AddCamera")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: AddCamera: 'camera name' and 'resource handle' for rendering pipeline missing!");
		if (nrhs < 2) mexErrMsgTxt("Horde3D: AddCamera: 'resource handle' for rendering pipeline missing!");
		
		// Get camera name:
		str[0] = 0;
		mxGetString(prhs[1], (char*) &str, MAX_STR_LENGTH-1);
		
		// Try to add resource:
		i1 = (int) mxGetScalar(prhs[2]);
		if ((handle = (int) h3dAddCameraNode( H3DRootNode, str, i1 )) == 0) {
			mexPrintf("Failed to add Camera node %i: %s.\n", i1, str);
			mexErrMsgTxt("Horde3D: AddCamera: FAILED!");
		}
		
		// Return resource handle:
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*mxGetPr(plhs[0]) = handle;		
	}

	if (IsCommand((char*)"AddNodes")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: AddNodes: 'ParentNode' and 'ResourceHandle' of scenegraph to add missing!");
		if (nrhs < 2) mexErrMsgTxt("Horde3D: AddNodes: 'ResourceHandle' of scenegraph to add missing!");
				
		i1 = (int) mxGetScalar(prhs[1]);
		i2 = (int) mxGetScalar(prhs[2]);
		if ((handle = (int) h3dAddNodes( i1, i2 )) == 0) {
			mexPrintf("Failed to add Nodes with resource handle %i to parent node with handle %i.\n", i2, i1);
			mexErrMsgTxt("Horde3D: AddNodes: FAILED!");
		}
		
		// Return resource handle:
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*mxGetPr(plhs[0]) = handle;		
	}

	if (IsCommand((char*)"SetNodeTransform")) {
		if (nrhs < 10) mexErrMsgTxt("Horde3D: SetNodeTransform: One of the 10 required parameters missing!");
		
		// Node handle:
		i1 = (int) mxGetScalar(prhs[1]);
		
		// Apply xform:
		h3dSetNodeTransform( i1, (float) mxGetScalar(prhs[2]), (float) mxGetScalar(prhs[3]), (float) mxGetScalar(prhs[4]),	// x ,  y , z
					(float) mxGetScalar(prhs[5]), (float) mxGetScalar(prhs[6]), (float) mxGetScalar(prhs[7]),		// rx, ry, rz
					(float) mxGetScalar(prhs[8]), (float) mxGetScalar(prhs[9]), (float) mxGetScalar(prhs[10]));					// sx, sy, sz
	}

	if (IsCommand((char*)"GetNodeTransform")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: GetNodeTransform: Required parameter 'Node' missing!");
		
		// Node handle:
		i1 = (int) mxGetScalar(prhs[1]);

		// Retrieve xform:
		h3dGetNodeTransform(i1, &v[0], &v[1], &v[2],	// x ,  y , z
							&v[3], &v[4], &v[5],	// rx, ry, rz
							&v[6], &v[7], &v[8]);	// sx, sy, sz

		for (i2 = 0; i2 < 9; i2++) {
			plhs[i2]  = mxCreateDoubleMatrix(1, 1, mxREAL);
			*(mxGetPr(plhs[i2])) = (double) v[i2];
		}
	}

	if (IsCommand((char*)"SetNodeTransforms")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: SetNodeTransforms: The required parameter 'matrix' missing!");
		
		i1 = mxGetN(prhs[1]); 
		if (mxGetM(prhs[1]) < 10 || i1 < 1) mexErrMsgTxt("Horde3D: SetNodeTransforms: Transform matrix is not a 10 rows by at least one column matrix as required!");
		p = mxGetPr(prhs[1]);
		
		// Batch-Submit updates:
		for (i2 = 0; i2 < i1; i2++) {
			// Apply xform for i2'th column:
			i4 = (int) *(p++);
			for(i3 = 0; i3 < 9; i3++) 
				v[i3] = (float) *(p++);
			h3dSetNodeTransform(i4, v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8]);
						// sx, sy, sz  
		}
	}

	if (IsCommand((char*)"GetNodeTransforms")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: GetNodeTransforms: Required parameter 'Node' missing!");
		
		// Number of node handles to query?
		i1 = mxGetM(prhs[1]) * mxGetN(prhs[1]); 
		if (i1 < 1) mexErrMsgTxt("Horde3D: GetNodeTransforms: Vector of node handles to query is empty!");
		plhs[0] = mxCreateDoubleMatrix(10, i1, mxREAL);
		p = mxGetPr(plhs[0]);

		// Iterate over all passed H3DNodes and execute a query and assign for each of them:
		for (i3 = 0; i3 < i1; i3++) {
			// Retrieve xform:
			i4 = (int) ((mxGetPr(prhs[1]))[i3]);
			h3dGetNodeTransform( i4, &v[0], &v[1], &v[2],	// x ,  y , z
								&v[3], &v[4], &v[5],	// rx, ry, rz
								&v[6], &v[7], &v[8]);	// sx, sy, sz
			
			// Reassign H3DNode as first row parameter:
			*(p++) = (double) i4;
			
			// Assign xform of i3'th node:
			for (i2 = 0; i2 < 9; i2++) {
				*(p++) = (double) v[i2];
			}
		}
	}
		
	if (IsCommand((char*)"SetupModelAnimStage")) {
		if (nrhs < 5) 
		{
			char params[5][30]={"modelNode", "stage", "animationRes", "startNode", "additive"};
			mexErrMsgTxt("Horde3D: SetupModelAnimStage: Required parameters ");
			for (int i=nrhs-1; i<5; i++)
			{
				mexErrMsgTxt(params[i]);
				mexErrMsgTxt(" ");
			}
			mexErrMsgTxt("missing!");
		}
		
		// Node handle:
		i1 = (int) mxGetScalar(prhs[1]);
		i2 = (int) mxGetScalar(prhs[2]);
		i3 = (int) mxGetScalar(prhs[3]);
		// Get name:
		str[0] = 0;
		mxGetString(prhs[4], (char*) &str, MAX_STR_LENGTH-1);
		i4 = (int) mxGetScalar(prhs[5]);
		
		// Apply:
		h3dSetupModelAnimStage( i1, i2, i3, 0, str, (i4 > 0) ? true : false);
	}

	if (IsCommand((char*)"FindNodes")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: FindNodes: Required parameters 'startNode', 'name' and 'type' missing!");
		if (nrhs < 2) mexErrMsgTxt("Horde3D: FindNodes: Required parameters 'name' and 'type' missing!");
		if (nrhs < 3) mexErrMsgTxt("Horde3D: FindNodes: Required parameter 'type' missing!");
		
		// Node handle:
		i1 = (int) mxGetScalar(prhs[1]);
		// Get name:
		str[0] = 0;
		mxGetString(prhs[2], (char*) &str, MAX_STR_LENGTH-1);
		i2 = (int) mxGetScalar(prhs[3]);
		
		// Search for nodes:
		i3 = h3dFindNodes(i1, str, i2);

		// Return resource handles for all found nodes:
		plhs[0] = mxCreateDoubleMatrix(i3, 1, mxREAL);
		d_H3DNodes = mxGetPr(plhs[0]);
		for (i1 = 0; i1 < i3; i1++) *(d_H3DNodes++) = (double) h3dGetNodeFindResult(i1);
	}
	
	if (IsCommand((char*)"AddLightNode")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: AddLightNode: Required parameters 'parent', 'name', 'materialRes', 'lightingContext' and 'shadowContext' missing!");
		if (nrhs < 2) mexErrMsgTxt("Horde3D: AddLightNode: Required parameters 'name', 'materialRes', 'lightingContext' and 'shadowContext' missing!");
		if (nrhs < 3) mexErrMsgTxt("Horde3D: AddLightNode: Required parameters 'materialRes', 'lightingContext' and 'shadowContext' missing!");
		if (nrhs < 4) mexErrMsgTxt("Horde3D: AddLightNode: Required parameters 'lightingContext' and 'shadowContext' missing!");
		if (nrhs < 5) mexErrMsgTxt("Horde3D: AddLightNode: Required parameter 'shadowContext' missing!");
		
		// Node handle:
		i1 = (int) mxGetScalar(prhs[1]);
		// Get name:
		str1[0] = 0;
		mxGetString(prhs[2], (char*) &str1, MAX_STR_LENGTH-1);
		i2 = (int) mxGetScalar(prhs[3]);
		str2[0] = 0;
		mxGetString(prhs[4], (char*) &str2, MAX_STR_LENGTH-1);
		str3[0] = 0;
		mxGetString(prhs[5], (char*) &str3, MAX_STR_LENGTH-1);

		// Return 1st found nodes handle:
		handle = (int) h3dAddLightNode( i1, str1, i2, str2, str3 );

		// Return resource handle:
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*mxGetPr(plhs[0]) = handle;
	}

	if (IsCommand((char*)"SetOption")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: SetOption: Required parameters 'Parameter' and 'Value' missing!");
		if (nrhs < 2) mexErrMsgTxt("Horde3D: SetOption: Required parameter 'Value' missing!");
		
		// Option enum:
		i1 = (int) mxGetScalar(prhs[1]);
		
		if (!h3dSetOption( (H3DOptions::List) i1, (float) mxGetScalar(prhs[2]))) 
			mexErrMsgTxt("Horde3D: SetOption: FAILED!");
	}

	if (IsCommand((char*)"SetupViewport")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: SetupCameraView: Required parameter 'cameraNode' missing!");
		if (nrhs < 2) mexErrMsgTxt("Horde3D: SetupViewport: Required parameters 'x', 'y', 'width', 'height' missing!");
		if (nrhs < 3) mexErrMsgTxt("Horde3D: SetupViewport: Required parameters 'y', 'width', 'height' missing!");
		if (nrhs < 4) mexErrMsgTxt("Horde3D: SetupViewport: Required parameters 'width', 'height' missing!");
		if (nrhs < 5) mexErrMsgTxt("Horde3D: SetupViewport: Required parameters 'height' missing!");
		
		// Dimensions:
		i1 = (int) mxGetScalar(prhs[1]);
		i2 = (int) mxGetScalar(prhs[2]);
		i3 = (int) mxGetScalar(prhs[3]);
		i4 = (int) mxGetScalar(prhs[4]);
		i5 = (int) mxGetScalar(prhs[5]);
		h3dSetNodeParamI(i1, H3DCamera::ViewportXI, i2);
		h3dSetNodeParamI(i1, H3DCamera::ViewportYI, i3);
		h3dSetNodeParamI(i1, H3DCamera::ViewportWidthI, i4);
		h3dSetNodeParamI(i1, H3DCamera::ViewportHeightI, i5);
	}

	if (IsCommand((char*)"SetupCameraView")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: SetupCameraView: Required parameters 'cameraNode', 'fov', 'aspect', 'nearDist' and 'farDist' missing!");
		if (nrhs < 2) mexErrMsgTxt("Horde3D: SetupCameraView: Required parameters 'fov', 'aspect', 'nearDist' and 'farDist' missing!");
		if (nrhs < 3) mexErrMsgTxt("Horde3D: SetupCameraView: Required parameters 'aspect', 'nearDist' and 'farDist' missing!");
		if (nrhs < 4) mexErrMsgTxt("Horde3D: SetupCameraView: Required parameters 'nearDist' and 'farDist' missing!");
		if (nrhs < 5) mexErrMsgTxt("Horde3D: SetupCameraView: Required parameter 'farDist' missing!");
		
		// Dimensions:
		i1 = (int) mxGetScalar(prhs[1]);
		h3dSetupCameraView( i1, (float) mxGetScalar(prhs[2]), (float) mxGetScalar(prhs[3]), (float) mxGetScalar(prhs[4]), (float) mxGetScalar(prhs[5]));
	}
	
	if (IsCommand((char*)"SetModelAnimParams")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: SetModelAnimParams: Required parameters 'modelNode', 'stage', 'time' and 'weight' missing!");
		if (nrhs < 2) mexErrMsgTxt("Horde3D: SetModelAnimParams: Required parameters 'stage', 'time' and 'weight' missing!");
		if (nrhs < 3) mexErrMsgTxt("Horde3D: SetModelAnimParams: Required parameters 'time' and 'weight' missing!");
		if (nrhs < 4) mexErrMsgTxt("Horde3D: SetModelAnimParams: Required parameter 'weight' missing!");
		
		i1 = (int) mxGetScalar(prhs[1]);
		i2 = (int) mxGetScalar(prhs[2]);
		h3dSetModelAnimParams( i1, i2, (float) mxGetScalar(prhs[3]), (float) mxGetScalar(prhs[4]) );
	}

	if (IsCommand((char*)"SetModelsAnimParameters")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: SetModelsAnimParameters: Required parameter 'matrix' missing!");
		
		i1 = mxGetN(prhs[1]); 
		if (mxGetM(prhs[1]) < 4 || i1 < 1) mexErrMsgTxt("Horde3D: SetModelsAnimParameters: Parameters matrix is not a 4 rows by at least one column matrix as required!");
		p = mxGetPr(prhs[1]);
		
		// Batch-Submit updates:
		for (i2 = 0; i2 < i1; i2++) {
			// Apply xform for i2'th column:
			i4 = (int) *(p++);
			i5 = (int) *(p++);
			for(i3 = 0; i3 < 2; i3++) v[i3] = (float) *(p++);
			h3dSetModelAnimParams( i4, i5, v[0], v[1]);
		}
	}

	if (IsCommand((char*)"Render")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: Render: Required parameter 'cameraNode' missing!");
		
		i1 = (int) mxGetScalar(prhs[1]);
		// Render scene
		h3dRender(i1);
	}

	if (IsCommand((char*)"GetError")) {
		// Checks if an error occured
		if(!h3dGetError())
			mexErrMsgTxt("Horde3D: GetError: An error has occured. Check the message queue for details if available!");;
	}

	if (IsCommand((char*)"Clear")) {
		// Removes all resources and scene nodes
		h3dClear();
	}

	if (IsCommand((char*)"FinalizeFrame")) {
		// Finish rendering of frame
		h3dFinalizeFrame();
	}

	if (IsCommand((char*)"GetMessage")) {
		// if (nrhs < 2) mexErrMsgTxt("Horde3D: GetMessage: One of the 2 required parameters missing!");
		// Gets the next message from the message queue
		const char* msg=h3dGetMessage(&i1, &v[0]);
		
		plhs[0] = mxCreateString(msg);
		plhs[1]  = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[1])) = (int) i1;
		plhs[2]  = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[2])) = (double) v[0];
	}

	if (IsCommand((char*)"GetOption")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: GetOption: One required parameters missing!");
		// Option enum:
		i1 = (int) mxGetScalar(prhs[1]);
		// Gets an option parameter of the engine
		v[0] = h3dGetOption( (H3DOptions::List) i1);
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = (double) v[0];
	}

	if (IsCommand((char*)"GetStat")) {
		if (nrhs < 2) mexErrMsgTxt("Horde3D: GetStat: One of the 2 required parameters missing!");
		// Option enum:
		i1 = (int) mxGetScalar(prhs[1]);
		i2 = (int) mxGetScalar(prhs[2]);
		// Gets a statistic value of the engine
		v[0] = h3dGetStat( (H3DStats::List) i1, (bool) i2);
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = (double) v[0];
	}

	if (IsCommand((char*)"ShowOverlay")) {
		if (nrhs < 22) mexErrMsgTxt("Horde3D: ShowOverlay: One of the 22 required parameters missing!");
		
		// Shows an overlay on the screen
// MK		h3dShowOverlays( (float) mxGetScalar(prhs[1]), (float) mxGetScalar(prhs[2]), (float) mxGetScalar(prhs[3]), (float) mxGetScalar(prhs[4]),	// x_tl, y_tl, u_tl, v_tl
// 					(float) mxGetScalar(prhs[5]),  (float) mxGetScalar(prhs[6]),  (float) mxGetScalar(prhs[7]),  (float) mxGetScalar(prhs[8]),  // x_bl, y_bl, u_bl, v_bl
// 					(float) mxGetScalar(prhs[9]),  (float) mxGetScalar(prhs[10]), (float) mxGetScalar(prhs[11]), (float) mxGetScalar(prhs[12]),	// x_br, y_br, u_br, v_br
// 					(float) mxGetScalar(prhs[13]), (float) mxGetScalar(prhs[14]), (float) mxGetScalar(prhs[15]), (float) mxGetScalar(prhs[16]),	// x_tr, y_tr, u_tr, v_tr
// 					(float) mxGetScalar(prhs[17]), (float) mxGetScalar(prhs[18]), (float) mxGetScalar(prhs[19]), (float) mxGetScalar(prhs[20]),	// colR, colG, colB, colA
// 					(H3DRes) mxGetScalar(prhs[21]), (int) mxGetScalar(prhs[22])); // materialRes, layer
	}

	if (IsCommand((char*)"ClearOverlays")) {
		// Removes all overlays
		h3dClearOverlays();
	}

	if (IsCommand((char*)"GetResType")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: GetResType: One required parameters missing!");
		// Returns the type of a resource
		i1 = h3dGetResType((H3DRes) mxGetScalar(prhs[1]));
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = (double) i1;
	}

	if (IsCommand((char*)"GetResName")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: GetResName: One required parameters missing!");
		const char *name;
		// Returns the name of a resource
		name = h3dGetResName((H3DRes) mxGetScalar(prhs[1]));
		plhs[0] = mxCreateString(name);
	}

	if (IsCommand((char*)"GetNextResource")) {
		if (nrhs < 2) mexErrMsgTxt("Horde3D: GetNextResource: One of the 2 required parameters missing!");
		// Returns the next resource of the specified type
		i1 = (int)h3dGetNextResource((int) mxGetScalar(prhs[1]), (H3DRes) mxGetScalar(prhs[2]));
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = i1;
	}

	if (IsCommand((char*)"FindResource")) {
		if (nrhs < 3) mexErrMsgTxt("Horde3D: FindResource: One of the 3 required parameters missing!");
		str[0] = 0;
		mxGetString(prhs[2], (char*) &str, MAX_STR_LENGTH-1);
		// Finds a resource of the specified type		
		i1 = (int)h3dFindResource((int) mxGetScalar(prhs[1]), str);
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = i1;
	}

	if (IsCommand((char*)"CloneResource")) {
		if (nrhs < 2) mexErrMsgTxt("Horde3D: CloneResource: One of the 2 required parameters missing!");
		str[0] = 0;
		mxGetString(prhs[2], (char*) &str, MAX_STR_LENGTH-1);
		// Duplicates a resource
		i1 = (int)h3dCloneResource((int) mxGetScalar(prhs[1]), str);
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = i1;
	}

	if (IsCommand((char*)"RemoveResource")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: RemoveResource: One required parameter missing!");
		// Removes a resource
		i1 = (int)h3dRemoveResource((int) mxGetScalar(prhs[1]));
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = i1;
	}

	if (IsCommand((char*)"IsResLoaded")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: IsResLoaded: One required parameter missing!");
		// Checks if a resource is loaded
		if(!h3dIsResLoaded((int) mxGetScalar(prhs[1])))
			mexErrMsgTxt("Horde3D: IsResLoaded:	The specified Resource is not loaded. Call LoadResource to load this resource.");
	}
	
	if (IsCommand((char*)"LoadResource")) {
		if (nrhs < 3) mexErrMsgTxt("Horde3D: LoadResource: One of the 3 required parameters missing!");
		i1 = (int)prhs[1];
		str[0] = 0;
		mxGetString(prhs[2], (char*) &str, MAX_STR_LENGTH-1);
		// Loads a resource
		if(!h3dLoadResource(i1, str, (int) mxGetScalar(prhs[3])))
			mexErrMsgTxt("Horde3D: LoadAResource: Unable to load the specified Resource. Perhaps the requested resource was not added to the resource manager.");
	}

	if (IsCommand((char*)"UnloadResource")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: UnloadResource: One required parameter missing!");
		i1 = (int)prhs[1];
		// Unloads a resource
		h3dUnloadResource(i1);
	}

	if (IsCommand((char*)"GetResElemCount")) {
		if (nrhs < 2) mexErrMsgTxt("Horde3D: GetResElemCount: One of the 2 required parameters missing!");
		i1 = (int)prhs[1];
		// Gets the number of elements in a resource.
		i2 = (int)h3dGetResElemCount(i1, (int) mxGetScalar(prhs[2]));
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = i2;
	}

	if (IsCommand((char*)"FindResElement")) {
		if (nrhs < 4) mexErrMsgTxt("Horde3D: FindResElement: One of the 4 required parameters missing!");
		i1 = (int)prhs[1];
		str[0] = 0;
		mxGetString(prhs[4], (char*) &str, MAX_STR_LENGTH-1);
		// Finds a resource element with the specified property value.
		i2 = (int)h3dFindResElem(i1, (int) mxGetScalar(prhs[2]), (int) mxGetScalar(prhs[3]), str);
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = i2;
		if(i2 == -1)
			mexErrMsgTxt("Horde3D: FindResElement: The specified resource element was not found.");
	}
	
	if (IsCommand((char*)"GetResParamI")) {
		if (nrhs < 4) mexErrMsgTxt("Horde3D: GetResParamI: One of the 4 required parameters missing!");
		i1 = (int)prhs[1];
		// Gets an integer property of a resource element.
		i2 = (int)h3dGetResParamI(i1, (int) mxGetScalar(prhs[2]), (int) mxGetScalar(prhs[3]), 
			 (int) mxGetScalar(prhs[4]));
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = i2;
	}
	
	if (IsCommand((char*)"SetResParamI")) {
		if (nrhs < 5) mexErrMsgTxt("Horde3D: SetResParamI: One of the 5 required parameters missing!");
		i1 = (int)prhs[1];
		// Sets an integer property of a resource element.
		h3dSetResParamI(i1, (int) mxGetScalar(prhs[2]), (int) mxGetScalar(prhs[3]), 
					   (int) mxGetScalar(prhs[4]), (int) mxGetScalar(prhs[5]));
	}
	
	if (IsCommand((char*)"GetResParamF")) {
		if (nrhs < 5) mexErrMsgTxt("Horde3D: GetResParamF: One of the 5 required parameters missing!");
		i1 = (int)prhs[1];
		// Gets a float property of a resource element.
		v[0] = h3dGetResParamF(i1, (int) mxGetScalar(prhs[2]), (int) mxGetScalar(prhs[3]), 
			 (int) mxGetScalar(prhs[4]), (int) mxGetScalar(prhs[5]));
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = v[0];
	}
	
	if (IsCommand((char*)"SetResParamF")) {
		if (nrhs < 6) mexErrMsgTxt("Horde3D: SetResParamF: One of the 6 required parameters missing!");
		i1 = (int)prhs[1];
		// Sets a float property of a resource element.
		h3dSetResParamF(i1, (int) mxGetScalar(prhs[2]), (int) mxGetScalar(prhs[3]), 
					   (int) mxGetScalar(prhs[4]), (int) mxGetScalar(prhs[5]), (float) mxGetScalar(prhs[6]));
	}
	
	if (IsCommand((char*)"GetResParamStr")) {
		if (nrhs < 4) mexErrMsgTxt("Horde3D: GetResParamStr: One of the 4 required parameters missing!");
		i1 = (int)prhs[1];
		// Gets a string property of a resource element.
		const char *str = h3dGetResParamStr(i1, (int) mxGetScalar(prhs[2]), (int) mxGetScalar(prhs[3]), 
			 (int) mxGetScalar(prhs[4]));
		plhs[0] = mxCreateString(str);
	}
	
	if (IsCommand((char*)"SetResParamStr")) {
		if (nrhs < 5) mexErrMsgTxt("Horde3D: SetResParamStr: One of the 5 required parameters missing!");
		i1 = (int)prhs[1];
		str[0] = 0;
		mxGetString(prhs[2], (char*) &str, MAX_STR_LENGTH-1);
		// Sets a string property of a resource element.
		h3dSetResParamStr(i1, (int) mxGetScalar(prhs[2]), (int) mxGetScalar(prhs[3]), 
						 (int) mxGetScalar(prhs[4]), str);
	}
	
	if (IsCommand((char*)"CheckExtension")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: CheckExtension: One required parameter missing!");
		str[0] = 0;
		mxGetString(prhs[1], (char*) &str, MAX_STR_LENGTH-1);
		//
		if(!h3dCheckExtension(str))
			mexErrMsgTxt("Horde3D: CheckExtension: The specified extension is NOT contained in the DLL/shared object of the engine..");
	}

	if (IsCommand((char*)"MapResourceStream")) {
		if (nrhs < 6) mexErrMsgTxt("Horde3D: MapResourceStream: One of the 6 required parameters missing!");
		i1 = (int)prhs[1];
		// Maps the stream of a resource element.
		void* ptr = h3dMapResStream(i1, (int) mxGetScalar(prhs[2]), (int) mxGetScalar(prhs[3]), 
						 (int) mxGetScalar(prhs[4]), (bool) mxGetScalar(prhs[5]), (bool) mxGetScalar(prhs[6]));
		if (ptr == NULL)
			mexErrMsgTxt("Horde3D: MapResourceStream: The specified extension is NOT contained in the DLL/shared object of the engine..");
		plhs[0]  = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = (int)ptr;
	}
	
	if (IsCommand((char*)"UnmapResourceStream")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: UnmapResourceStream: One required parameter missing!");
		i1 = (int)prhs[1];
		// Unmaps a previously mapped resource stream.
		h3dUnmapResStream(i1);
	}
	
	if (IsCommand((char*)"QueryUnloadedResource")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: QueryUnloadedResource: One required parameter missing!");
		// Returns handle to an unloaded resource.
		i1 = (int)h3dQueryUnloadedResource((int)mxGetScalar(prhs[1]));
		plhs[0]  = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = i1;
	}
	
	if (IsCommand((char*)"ReleaseUnusedResources")) {
		// Frees resources that are no longer used.
		h3dReleaseUnusedResources();
	}

	if (IsCommand((char*)"CreateTexture")) {
		if (nrhs < 5) mexErrMsgTxt("Horde3D: CreateTexture: One of the 5 required parameters missing!");
		str[0] = 0;
		mxGetString(prhs[1], (char*) &str, MAX_STR_LENGTH-1);
		// Creates a Texture resource.
		i1 = (int)h3dCreateTexture(str, (int) mxGetScalar(prhs[2]), (int) mxGetScalar(prhs[3]), 
						 (int) mxGetScalar(prhs[4]), (int) mxGetScalar(prhs[5]));
		plhs[0]  = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = i1;
	}
	
	if (IsCommand((char*)"SetShaderPreambles")) {
		if (nrhs < 2) mexErrMsgTxt("Horde3D: SetShaderPreambles: One of the 2 required parameters missing!");
		str[0] = 0;
		mxGetString(prhs[1], (char*) &str, MAX_STR_LENGTH-1);
		str1[0] = 0;
		mxGetString(prhs[1], (char*) &str1, MAX_STR_LENGTH-1);
		// Sets preambles of all Shader resources.
		h3dSetShaderPreambles(str1, str2);
	}
	
	if (IsCommand((char*)"SetMaterialUniform")) {
		if (nrhs < 6) mexErrMsgTxt("Horde3D: SetMaterialUniform: One of the 6 required parameters missing!");
		i1 = (int)prhs[1];
		str[0] = 0;
		mxGetString(prhs[2], (char*) &str, MAX_STR_LENGTH-1);
		// Sets a shader uniform of a Material resource.
		if(!h3dSetMaterialUniform(i1, str, (float) mxGetScalar(prhs[3]), (float) mxGetScalar(prhs[4]), 
						 (float) mxGetScalar(prhs[5]), (float) mxGetScalar(prhs[6])))
			mexErrMsgTxt("Horde3D: SetMaterialUniform: The specified shader uniform was not found.");
	}
	
	if (IsCommand((char*)"GetRenderTargetData")) {
		if (nrhs < 4) mexErrMsgTxt("Horde3D: GetRenderTargetData: One of the 4 required parameters missing!");
		i1 = (int)prhs[1];
		str[0] = 0;
		mxGetString(prhs[2], (char*) &str, MAX_STR_LENGTH-1);
		float dataBuffer[100];
		// Reads the pixel data of a pipeline render target buffer.
		if(!h3dGetRenderTargetData(i1, str, (int) mxGetScalar(prhs[3]), &i2, &i3, &i4, 
					dataBuffer, (int) mxGetScalar(prhs[nrhs])))
			mexErrMsgTxt("Horde3D: GetRenderTargetData: The specified render target was not found.");
		else
		{
			plhs[0]  = mxCreateDoubleMatrix(1, 1, mxREAL);
			*(mxGetPr(plhs[0])) = i2;
			plhs[1]  = mxCreateDoubleMatrix(1, 1, mxREAL);
			*(mxGetPr(plhs[1])) = i3;
			plhs[2]  = mxCreateDoubleMatrix(1, 1, mxREAL);
			*(mxGetPr(plhs[2])) = i4;
			plhs[3]  = mxCreateDoubleMatrix(1, 1, mxREAL);
			*(mxGetPr(plhs[3])) = (int)dataBuffer;
		}
	}
	
	if (IsCommand((char*)"GetNodeType")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: GetNodeType: One required parameter missing!");
		i1 = (int)prhs[1];
		// Returns the type of a scene node.
		i2 = h3dGetNodeType(i1);
		plhs[0]  = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = i2;
	}
	
	if (IsCommand((char*)"GetNodeParent")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: GetNodeParent: One required parameter missing!");
		i1 = (int)prhs[1];
		// Returns the Parent of a scene node.
		i2 = h3dGetNodeParent(i1);
		plhs[0]  = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = i2;
	}
	
	if (IsCommand((char*)"SetNodeParent")) {
		if (nrhs < 2) mexErrMsgTxt("Horde3D: SetNodeParent: One of the 2 required parameters missing!");
		i1 = (int)prhs[1];
		i2 = (int)prhs[2];
		// Relocates a node in the scene graph.
		if (!h3dSetNodeParent(i1, i2))
			mexErrMsgTxt("Horde3D: SetNodeParent: The specified node could not be relocated.");
	}
	
	if (IsCommand((char*)"GetNodeChild")) {
		if (nrhs < 2) mexErrMsgTxt("Horde3D: GetNodeChild: One of the 2 required parameters missing!");
		i1 = (int)prhs[1];
		i2 = (int)prhs[2];
		// Returns the handle to a child node.
		i3 = (int)h3dSetNodeParent(i1, i2);
		plhs[0]  = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = i3;
		if (i3 == 0)
			mexErrMsgTxt("Horde3D: GetNodeChild: The specified child node doesn't exist.");
	}

	if (IsCommand((char*)"RemoveNode")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: RemoveNode: One required parameter missing!");
		i1 = (int)prhs[1];
		// Removes a node from the scene.
		h3dRemoveNode(i1);
	}
	
	if (IsCommand((char*)"SetNodeActivation")) {
		if (nrhs < 2) mexErrMsgTxt("Horde3D: SetNodeActivation: One of the 2 required parameters missing!");
		i1 = (int)prhs[1];
		bool b1 = (bool)prhs[2];
		// Sets the activation (visibility) state of a node.
		// MK: h3dSetNodeActivation(i1, b1);
	}
	
	if (IsCommand((char*)"CheckNodeTransFlag")) {
		if (nrhs < 2) mexErrMsgTxt("Horde3D: CheckNodeTransFlag: One of the 2 required parameters missing!");
		i1 = (int)prhs[1];
		bool b1 = (bool)prhs[2];
		// Checks if a scene node has been transformed by the engine.
		if (!h3dCheckNodeTransFlag(i1, b1))
			mexErrMsgTxt("Horde3D: CheckNodeTransFlag: The node has not been transformed.");
	}

	if (IsCommand((char*)"GetNodeTransMats")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: GetNodeTransMats: One required parameter missing!");
		// Node handle:
		i1 = (int) mxGetScalar(prhs[1]);

		float relMat[3][3], absMat[3][3];
		// 
		h3dGetNodeTransMats(i1, (const float**)relMat, (const float**)absMat);

		plhs[0] = mxCreateDoubleMatrix(3, 3, mxREAL);
		plhs[1] = mxCreateDoubleMatrix(3, 3, mxREAL);
		for (i2 = 0; i2 < 3; i2++) {
			for (i3 = 0; i3 < 3; i3++) {
				*(mxGetPr(plhs[0])+i2*3+i3) = (double) relMat[i2][i3];
				*(mxGetPr(plhs[1])+i2*3+i3) = (double) absMat[i2][i3];
			}
		}
	}

	if (IsCommand((char*)"SetNodeTransMat")) {
		if (nrhs < 2) mexErrMsgTxt("Horde3D: SetNodeTransMat: One of the 2 required parameters missing!");
		// Node handle:
		i1 = (int) mxGetScalar(prhs[1]);
		float mat[16];
		for (i3 = 0; i3 < 3; i3++) {
			for (i2 = 0; i2 < 3; i2++) {
				mat[i2*3+i3] = (float) *(mxGetPr(prhs[2])+i2*3+i3);
			}
		}
		// 
		h3dSetNodeTransMat(i1, (const float*)mat);
	}

	if (IsCommand((char*)"GetNodeParamI")) {
		if (nrhs < 2) mexErrMsgTxt("Horde3D: GetNodeParamI: One of the 2 required parameters missing!");
		i1 = (int)prhs[1];
		// Gets an integer property of a scene node.
		i2 = (int)h3dGetNodeParamI(i1, (int) mxGetScalar(prhs[2]));
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = i2;
	}
	
	if (IsCommand((char*)"SetNodeParamI")) {
		if (nrhs < 3) mexErrMsgTxt("Horde3D: SetNodeParamI: One of the 3 required parameters missing!");
		i1 = (int) prhs[1];
		// Sets an integer property of a scene node.
		h3dSetNodeParamI(i1, (int) mxGetScalar(prhs[2]), (int) mxGetScalar(prhs[3]));
	}

	if (IsCommand((char*)"GetNodeParamF")) {
		if (nrhs < 3) mexErrMsgTxt("Horde3D: GetNodeParamF: One of the 3 required parameters missing!");
		i1 = (int)prhs[1];
		// Gets a float property of a scene node.
		v[0] = h3dGetNodeParamF(i1, (int) mxGetScalar(prhs[2]), (int) mxGetScalar(prhs[3]));
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = v[0];
	}
	
	if (IsCommand((char*)"SetNodeParamF")) {
		if (nrhs < 4) mexErrMsgTxt("Horde3D: SetNodeParamF: One of the 4 required parameters missing!");
		i1 = (int) prhs[1];
		// Sets a float property of a scene node.
		h3dSetNodeParamF(i1, (int) mxGetScalar(prhs[2]), (int) mxGetScalar(prhs[3]), (float) mxGetScalar(prhs[4]));
	}

	if (IsCommand((char*)"GetNodeParamStr")) {
		if (nrhs < 2) mexErrMsgTxt("Horde3D: GetNodeParamStr: One of the 2 required parameters missing!");
		i1 = (int)prhs[1];
		// Gets a string property of a scene node.
		const char *str = h3dGetNodeParamStr(i1, (int) mxGetScalar(prhs[2]));
		plhs[0] = mxCreateString(str);
	}
	
	if (IsCommand((char*)"SetNodeParamStr")) {
		if (nrhs < 3) mexErrMsgTxt("Horde3D: SetNodeParamStr: One of the 3 required parameters missing!");
		i1 = (int)prhs[1];
		str[0] = 0;
		mxGetString(prhs[2], (char*) &str, MAX_STR_LENGTH-1);
		// Sets a string property of a scene node.
		h3dSetNodeParamStr(i1, (int) mxGetScalar(prhs[2]), str);
	}
	
	if (IsCommand((char*)"GetNodeAABB")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: GetNodeAABB: One required parameter missing!");
		i1 = (int)prhs[1];
		// Get the bounding box of a scene node.
		h3dGetNodeAABB(i1, &v[0], &v[1], &v[2], &v[3], &v[4], &v[5]);
		for (i2=0; i2<6; i2++) {
			plhs[i2+1] = mxCreateDoubleMatrix(1, 1, mxREAL);
			*(mxGetPr(plhs[i2+1])) = v[i2];
		}
	}
	
	if (IsCommand((char*)"CastRay")) {
		if (nrhs < 8) mexErrMsgTxt("Horde3D: CastRay: One of the 8 required parameters missing!");
		i1 = (int)prhs[1];
		// Performs a recursive ray collision query.
		i2 = (int)h3dCastRay(i1, (float) mxGetScalar(prhs[2]), (float) mxGetScalar(prhs[3]), (float) mxGetScalar(prhs[4]), 
					(float) mxGetScalar(prhs[5]), (float) mxGetScalar(prhs[6]), (float) mxGetScalar(prhs[7]), (int) mxGetScalar(prhs[8]));
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = i2;
	}
	
	if (IsCommand((char*)"GetCastRayResult")) {
		if (nrhs < 4) mexErrMsgTxt("Horde3D: GetCastRayResult: One of the 4 required parameters missing!");
		float inters[3];
		// Returns a result of a previous castRay query.
		if(!h3dGetCastRayResult((int) mxGetScalar(prhs[1]), &i1, &v[0], inters))
			mexErrMsgTxt("Horde3d: GetCastRayResult: The specified index is not valied or data could not be copied.");
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = i1;
		plhs[1] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[1])) = v[0];
		plhs[2] = mxCreateDoubleMatrix(1, 3, mxREAL);
		for (i2=0; i2<3; i2++) {
			*(mxGetPr(plhs[2])+i2) = inters[i2];
		}
	}
	
	if (IsCommand((char*)"CheckNodeVisibility")) {
		if (nrhs < 4) mexErrMsgTxt("Horde3D: CheckNodeVisibility: One of the 4 required parameters missing!");
		i1 = (int)prhs[1];
		i2 = (int)prhs[2];
		// Checks if a node is visible.
		i3 = (int)h3dCheckNodeVisibility(i1, i2, (bool) mxGetScalar(prhs[3]), (bool) mxGetScalar(prhs[4]));
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = i3;
		if(i3 == -1)
			mexErrMsgTxt("Horde3d: CheckNodeVisibility: The specified node is not visible.");
	}
	
	if (IsCommand((char*)"AddGroupNode")) {
		if (nrhs < 2) mexErrMsgTxt("Horde3D: AddGroupNode: One of the 2 required parameters missing!");
		i1 = (int)prhs[1];
		str[0] = 0;
		mxGetString(prhs[2], (char*) &str, MAX_STR_LENGTH-1);
		// Adds a Group node to the scene.
		i2 = (int)h3dAddGroupNode(i1, str);
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = i2;
		if(i2 == 0)
			mexErrMsgTxt("Horde3d: AddGroupNode: The specified group node could not be added to the scene.");
	}
	
	if (IsCommand((char*)"AddModelNode")) {
		if (nrhs < 3) mexErrMsgTxt("Horde3D: AddModelNode: One of the 3 required parameters missing!");
		i1 = (int)prhs[1];
		str[0] = 0;
		mxGetString(prhs[2], (char*) &str, MAX_STR_LENGTH-1);
		i2 = (int)prhs[3];
		// Adds a Model node to the scene.
		i3 = (int)h3dAddModelNode(i1, str, i2);
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = i3;
		if(i3 == 0)
			mexErrMsgTxt("Horde3d: AddModelNode: The specified model node could not be added to the scene.");
	}
	
	if (IsCommand((char*)"SetModelMorpher")) {
		if (nrhs < 3) mexErrMsgTxt("Horde3D: SetModelMorpher: One of the 3 required parameters missing!");
		i1 = (int)prhs[1];
		str[0] = 0;
		mxGetString(prhs[2], (char*) &str, MAX_STR_LENGTH-1);
		// Sets the weight of a morph target.
		if(!h3dSetModelMorpher(i1, str, (float) mxGetScalar(prhs[3])))		
			mexErrMsgTxt("Horde3d: SetModelMorpher: The specified morph target was not found.");
	}
	
	if (IsCommand((char*)"AddMeshNode")) {
		if (nrhs < 7) mexErrMsgTxt("Horde3D: AddMeshNode: One of the 7 required parameters missing!");
		i1 = (int)prhs[1];
		str[0] = 0;
		mxGetString(prhs[2], (char*) &str, MAX_STR_LENGTH-1);
		i2 = (int)prhs[3];
		// Adds a Mesh node to the scene.
		i3 = (int)h3dAddMeshNode(i1, str, i2, (int) mxGetScalar(prhs[3]), (int) mxGetScalar(prhs[3]), 
						(int) mxGetScalar(prhs[3]), (int) mxGetScalar(prhs[3]));
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = i3;
		if (i3 == 0)
			mexErrMsgTxt("Horde3d: AddMeshNode: The specified mesh node could not be added to the scene.");
	}
	
	if (IsCommand((char*)"AddJointNode")) {
		if (nrhs < 3) mexErrMsgTxt("Horde3D: AddJointNode: One of the 3 required parameters missing!");
		i1 = (int)prhs[1];
		str[0] = 0;
		mxGetString(prhs[2], (char*) &str, MAX_STR_LENGTH-1);
		// Adds a Joint node to the scene.
		i2 = (int)h3dAddJointNode(i1, str, (int) mxGetScalar(prhs[3]));
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = i2;
		if (i2 == 0)
			mexErrMsgTxt("Horde3d: AddJointNode: The specified joint node could not be added to the scene.");
	}
	
	if (IsCommand((char*)"AddCameraNode")) {
		if (nrhs < 3) mexErrMsgTxt("Horde3D: AddCameraNode: One of the 3 required parameters missing!");
		i1 = (int)prhs[1];
		str[0] = 0;
		mxGetString(prhs[2], (char*) &str, MAX_STR_LENGTH-1);
		i2 = (int)prhs[3];
		// Adds a Camera node to the scene.
		i3 = (int)h3dAddCameraNode(i1, str, i2);
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = i3;
		if (i3 == 0)
			mexErrMsgTxt("Horde3d: AddCameraNode: The specified camera node could not be added to the scene.");
	}
	
	if (IsCommand((char*)"GetCameraProjMat")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: GetCameraProjMat: One required parameter missing!");
		i1 = (int)prhs[1];
		float projM[16];
		// Gets the camera projection matrix.
		h3dGetCameraProjMat(i1, projM);
		plhs[0] = mxCreateDoubleMatrix(1, 16, mxREAL);
		for (i2=0; i2<16; i2++) {
			*(mxGetPr(plhs[0])+i2) = projM[i2];
		}
	}

	if (IsCommand((char*)"AddEmitterNode")) {
		if (nrhs < 6) mexErrMsgTxt("Horde3D: AddEmitterNode: One of the 6 required parameters missing!");
		i1 = (int)prhs[1];
		str[0] = 0;
		mxGetString(prhs[2], (char*) &str, MAX_STR_LENGTH-1);
		i2 = (int)prhs[3];
		i3 = (int)prhs[4];
		// Adds an Emitter node to the scene.
		i4 = (int)h3dAddEmitterNode(i1, str, i2, i3, (int) mxGetScalar(prhs[5]), (int) mxGetScalar(prhs[6]));
		plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*(mxGetPr(plhs[0])) = i4;
		if (i4 == 0)
			mexErrMsgTxt("Horde3d: AddEmitterNode: The specified emitter node could not be added to the scene.");
	}
	
	if (IsCommand((char*)"AdvanceEmitterTime")) {
		if (nrhs < 2) mexErrMsgTxt("Horde3D: AdvanceEmitterTime: One of the 2 required parameters missing!");
		i1 = (int)prhs[1];
		// Advances the time value of an Emitter node.
		h3dAdvanceEmitterTime(i1, (float) mxGetScalar(prhs[2]));
	}

	if (IsCommand((char*)"HasEmitterFinished")) {
		if (nrhs < 1) mexErrMsgTxt("Horde3D: HasEmitterFinished: One required parameter missing!");
		i1 = (int)prhs[1];
		// Checks if an Emitter node is still alive.
		if(!h3dHasEmitterFinished(i1))
			mexErrMsgTxt("Horde3d: HasEmitterFinished: The specified emitter node is not active.");
	}

	if (IsCommand((char*)"DumpMessages")) {
		// Write all mesages to log file
		h3dutDumpMessages();
	}

	// Command dispatched?
	if (!cmdassigned) mexErrMsgTxt("Unknown subcommand specified!");
	
	// Return control to runtime (Matlab or Octave):
	return;
}
