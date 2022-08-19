/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.
Copyright (C) 2013-2020 Robert Beckebans

This file is part of the Doom 3 BFG Edition GPL Source Code ("Doom 3 BFG Edition Source Code").

Doom 3 BFG Edition Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 BFG Edition Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 BFG Edition Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 BFG Edition Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 BFG Edition Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/
#ifndef __RENDERLOG_H__
#define __RENDERLOG_H__

/*
================================================================================================
Contains the RenderLog declaration.
================================================================================================
*/


enum renderLogMainBlock_t
{
	// each block will require to allocate 2 GPU query timestamps
	MRB_GPU_TIME,
	MRB_BEGIN_DRAWING_VIEW,
	MRB_FILL_DEPTH_BUFFER,
	MRB_FILL_GEOMETRY_BUFFER,
	MRB_SSAO_PASS,
	MRB_AMBIENT_PASS,
	MRB_DRAW_INTERACTIONS,
	MRB_DRAW_SHADER_PASSES,
	MRB_FOG_ALL_LIGHTS,
	MRB_BLOOM,
	MRB_DRAW_SHADER_PASSES_POST,
	MRB_DRAW_DEBUG_TOOLS,
	MRB_CAPTURE_COLORBUFFER,
	MRB_POSTPROCESS,
	MRB_DRAW_GUI,
	MRB_TOTAL,

	MRB_TOTAL_QUERIES = MRB_TOTAL * 2,
};

// these are used to make sure each Indent() is properly paired with an Outdent()
enum renderLogIndentLabel_t
{
	RENDER_LOG_INDENT_DEFAULT,
	RENDER_LOG_INDENT_MAIN_BLOCK,
	RENDER_LOG_INDENT_BLOCK,
	RENDER_LOG_INDENT_TEST
};

// using this macro avoids printf parameter overhead if the renderlog isn't active
#define RENDERLOG_PRINTF( ... ) if ( renderLog.activeLevel ) renderLog.Printf( __VA_ARGS__ );



/*
================================================
idRenderLog stubbed version for the SPUs and high
performance rendering in retail builds.

// Performance Events abstraction layer for OpenGL, Vulkan, DX12
// see https://devblogs.nvidia.com/best-practices-gpu-performance-events/
================================================
*/
class idRenderLog
{
private:
	renderLogMainBlock_t mainBlock;

public:
	idRenderLog();

	void		StartFrame() {}
	void		EndFrame() {}
	void		Close() {}
	int			Active()
	{
		return 0;
	}

	void		OpenBlock( const char* label, const idVec4& color = colorBlack );
	void		CloseBlock();
	void		OpenMainBlock( renderLogMainBlock_t block );// {}
	void		CloseMainBlock();// {}
	void		Indent( renderLogIndentLabel_t label = RENDER_LOG_INDENT_DEFAULT ) {}
	void		Outdent( renderLogIndentLabel_t label = RENDER_LOG_INDENT_DEFAULT ) {}

	void		Printf( VERIFY_FORMAT_STRING const char* fmt, ... ) {}

	int			activeLevel;
};

extern idRenderLog renderLog;

#endif // !__RENDERLOG_H__
