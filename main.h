///	\file
///	\brief Test program for the ParallelPort class
///	\author Yi Yao (http://yyao.ca/)
///
///	$LastChangedDate: 2009-11-28 12:39:43 -0500 (Sat, 28 Nov 2009) $
///
///	This library is distributed under the zlib license:
///	\verbatim
///	  Copyright (C) 2000-2009 Yi Yao
///
///	  This software is provided 'as-is', without any express or implied
///	  warranty. In no event will the authors be held liable for any damages
///	  arising from the use of this software.
///
///	  Permission is granted to anyone to use this software for any purpose,
///	  including commercial applications, and to alter it and redistribute it
///	  freely, subject to the following restrictions:
///
///	  1. The origin of this software must not be misrepresented; you must not
///	     claim that you wrote the original software. If you use this software
///	     in a product, an acknowledgment in the product documentation would be
///	     appreciated but is not required.
///	  2. Altered source versions must be plainly marked as such, and must not be
///	     misrepresented as being the original software.
///	  3. This notice may not be removed or altered from any source distribution.
///	\endverbatim

//You can compile documentation for this file using Doxygen:
//http://www.stack.nl/~dimitri/doxygen/

#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <errno.h>
#include <string.h>
#include <GL/glut.h>

#include "ParallelPort.h"

using namespace std;


///	\defgroup GlobalVars Global variables
/// \{

///	\brief Frames per second counter (increments every time frame is drawn)
volatile unsigned int	FPSCtr;

///	\brief Frames per second in the previous second
volatile unsigned int	FPS;

/// \brief Samples per second counter (increments every time a sample is taken)
volatile unsigned int	SPSCtr;

/// \brief Samples per second in the previous second
volatile unsigned int	SPS;

///	\brief Parallel port object to communicate through
volatile ParallelPort	Port;

///	\brief Data register read from parallel port
volatile unsigned char	DataRReg;

///	\brief Status register read from parallel port
volatile unsigned char	StatRReg;

///	\brief Control register read from parallel port
volatile unsigned char	CtrlRReg;

///	\brief Data register to be written to parallel port
volatile unsigned char	DataWReg;

///	\brief Control register to be written to parallel port
volatile unsigned char	CtrlWReg;

/// \}


///	\defgroup GLUTHandlers GLUT callback event handlers
/// \{

///	\brief Window resize GLUT event handler
void	HandleResize(int width, int height);

///	\brief Window redraw GLUT event handler
void	HandleDisplay(void);

///	\brief Keystroke GLUT event handler
void	HandleKeyboard(unsigned char key, int x, int y);

///	\brief Idle processing GLUT event handler
void	HandleIdle(void);

/// \}


///	\defgroup OpenGLHelpers Supplimentary OpenGL functions
/// \{

///	\brief Draw circle at origin
///
///	This function draws a circle of given size and style at (0, 0, 0). It is up
///	to you to translate and rotate it. The circle is drawn as a sequence of
///	lines. To control the trade off between smoothness and performance, the
///	number of line segments in a circle can be specified.
///
///	@param[in]		r				Circle radius
///	@param[in]		f				Whether or not circle is filled
///	@param[in]		s				Number of sides to construct circle with
void	glCircle(const GLfloat& r, const bool& f = false, const unsigned int& s = 32);


///	\brief Draws a string
///
///	This function prints the specified string using glutStrokeCharacter. All
/// characters will be scaled to the specified height. The bottom left corner of
///	the first character will be anchored to (0, 0, 0). It is up to you to
/// translate and scale the text to where you want it.
///
///	@param[in]		str				String to print
///	@param[in]		h				Height of each character
///	@param[in]		font			GLUT_STROKE_ROMAN or GLUT_STROKE_MONO_ROMAN
void	glText(const char* str, const GLfloat& h, const void* font = GLUT_STROKE_MONO_ROMAN);

/// \}
