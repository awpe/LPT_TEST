///	\file
///	\brief Test program for the ParallelPort class
///	\author Yi Yao (http://yyao.ca/)
///
///	$LastChangedDate: 2009-11-29 15:16:55 -0500 (Sun, 29 Nov 2009) $
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

#include "main.h"
#include <unistd.h>
#include <time.h>


int main(int argc, char *argv[]) {
	string	PortName;

	//Initialize GLUT and OpenGL
	glutInit(&argc, argv);
	glutInitWindowSize(480, 480);
	glutInitWindowPosition(10, 10);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("OpenGL parallel port controller");

	//Register GLUT callback functions
	glutReshapeFunc(HandleResize);
	glutDisplayFunc(HandleDisplay);
	glutKeyboardFunc(HandleKeyboard);
	glutIdleFunc(HandleIdle);

	//Populate options via command line arguments
	PortName = "/dev/parport0";
	if (argc > 2) {
		cout	<< "OpenGL parallel port debugger" << endl
				<< "Written by Yi Yao (http://yyao.ca/)" << endl
				<< "Usage: " << argv[0] << " [portname]" << endl
				<< "\t[portname]\tOptional - default is " << PortName << endl;
		return 1;
	} else if (argc == 2) {
		PortName = argv[1];
	};
	cout << "Using " << PortName << " for session" << endl;

	//Open port and initialize shadow variables
	try {
		Port.Open(PortName);
		DataWReg = DataRReg = Port.Data();
		StatRReg = Port.Stat();
		CtrlWReg = CtrlRReg = Port.Ctrl();
	} catch (...) {
		int		Err = errno;
		cout	<< "Unable to open " << PortName << ". Error from operating system is:" << endl
				<< "Error #" << Err << endl
				<< strerror(Err) << endl;
		return 1;
	};

	//Initialize performance counters
	FPSCtr = FPS = 0;
	SPSCtr = SPS = 0;

	//Let GLUT handle things from now on
	glutMainLoop();

	return 0;
};


void glCircle(const GLfloat& r, const bool& f, const unsigned int& s) {
	unsigned int	i;
	GLfloat	theta;

	if (f) {
		glBegin(GL_POLYGON);
	} else {
		glBegin(GL_LINE_LOOP);
	};

	//Draw circle in counter clockwise fashion starting at (1, 0, 0)
	for (i = 0; i < s; ++i) {
		theta = i * 6.28318530717958647688 / s;
		glVertex3f(r * cos(theta), r * sin(theta), 0);
	};

	glEnd();
};


void glText(const char* str, const GLfloat& h, const void* font) {
	GLfloat	LineWidth = 0;				//Cumilative width of line being printed

	glPushMatrix();
		glScalef(h / 152.38, h / 152.38, 1);

		while (*str) {
			if (*str == '\r') {
				//Carriage return: move to x = 0, but don't move to new line
				glTranslatef(LineWidth, 0, 0);
				LineWidth = 0;
			} else if (*str == '\n') {
				//Line feed: move to new line, but don't go back to x = 0
				glTranslatef(0, -152.38, 0);
			} else {
				//Print ordinary characters
				glutStrokeCharacter((void*) font, *str);
				LineWidth -= glutStrokeWidth((void*) font, *str);
			};
			++str;
		};
	glPopMatrix();
};


void HandleResize(int width, int height) {
	const float ar = (float) width / (float) height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//Make visible display extend from (0, 0, 0) to (1, 1, 0)
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (ar > 1.0) {
		//Window is wider than it is tall, make unit block occupy full height
		glScalef(2 / ar, 2, 1);
	} else {
		//Window is taller than it is tall, make unit block occupy full width
		glScalef(2, 2 * ar, 1);
	};
	glTranslatef(-0.5, -0.5, 0);
};


void HandleDisplay(void) {
	stringstream	msg;
	int		i;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
		//Print register labels
		glPushMatrix();
			glColor3f(1, 1, 1);
			glTranslatef(0.05, 0.8, 0);
			glScalef(0.5, 1, 1);
			glText("Data\r\n\r\nStat\r\n\r\nCtrl", 0.1);
			glTranslatef(0, -0.05, 0);
			if (Port.DataOut()) {
				glColor3f(1, 0, 0);
				glText("Write", 0.05);
			} else {
				glColor3f(0, 1, 0);
				glText("Read", 0.05);
			};
		glPopMatrix();
		glPushMatrix();
			glColor3f(0, 1, 0);
			glTranslatef(0.2, 0.85, 0);
			glText("R\r\n\r\n\r\n\r\nR\r\n\r\n\r\n\r\nR", 0.05);
			glColor3f(1, 0, 0);
			glTranslatef(0, -0.05, 0);
			glText("W\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\nW", 0.05);
		glPopMatrix();

		//Draw LEDs
		glPushMatrix();
			//Data R
			glTranslatef(0.3, 0.875, 0);
			glColor3f(0, 1, 0);
			for (i = 0; i < 8; ++i) {
				if (DataRReg & (0x01 << (7 - i))) {
					glCircle(0.02, true);
				} else {
					glCircle(0.02, false);
				};
				glTranslatef(0.075 * ((i == 3) ? 2 : 1), 0, 0);
			};
			//Data W
			glTranslatef(-0.675, -0.06, 0);
			for (i = 0; i < 8; ++i) {
				glColor3f(1, 0, 0);
				if (DataWReg & (0x01 << (7 - i))) {
					glCircle(0.02, true);
				} else {
					glCircle(0.02, false);
				};
				glPushMatrix();
					glTranslatef(-0.02, -0.05, 0);
					msg.str(string());
					msg << "D" << (7 - i) << "\r\n"
						<< (9 - i);
					glColor3f(1, 1, 1);
					glText(msg.str().c_str(), 0.03);
				glPopMatrix();
				glTranslatef(0.075 * ((i == 3) ? 2 : 1), 0, 0);
			};
			//Stat R
			glTranslatef(-0.675, -0.14, 0);
			for (i = 0; i < 8; ++i) {
				glColor3f(0, 1, 0);
				if (StatRReg & (0x01 << (7 - i))) {
					glCircle(0.02, true);
				} else {
					glCircle(0.02, false);
				};
				glPushMatrix();
					glTranslatef(-0.02, -0.05, 0);
					msg.str(string());
					msg <<	((i == 0) ? "!Bs\r\n11" :
							((i == 1) ? "nAc\r\n10" :
							((i == 2) ? "PE\r\n12" :
							((i == 3) ? "Sel\r\n13" :
							((i == 4) ? "nEr\r\n15" :
							((i == 5) ? "nIR\r\n--" :
							((i == 6) ? "--\r\n--" :
								"--\r\n--")))))));
					glColor3f(1, 1, 1);
					glText(msg.str().c_str(), 0.03);
				glPopMatrix();
				glTranslatef(0.075 * ((i == 3) ? 2 : 1), 0, 0);
			};
			//Ctrl R
			glTranslatef(-0.675, -0.2, 0);
			glColor3f(0, 1, 0);
			for (i = 0; i < 8; ++i) {
				if (CtrlRReg & (0x01 << (7 - i))) {
					glCircle(0.02, true);
				} else {
					glCircle(0.02, false);
				};
				glTranslatef(0.075 * ((i == 3) ? 2 : 1), 0, 0);
			};
			//Ctrl W
			glTranslatef(-0.675, -0.06, 0);
			for (i = 0; i < 8; ++i) {
				glColor3f(1, 0, 0);
				if (CtrlWReg & (0x01 << (7 - i))) {
					glCircle(0.02, true);
				} else {
					glCircle(0.02, false);
				};
				glPushMatrix();
					glTranslatef(-0.02, -0.05, 0);
					msg.str(string());
					msg <<	((i == 0) ? "--\r\n--" :
							((i == 1) ? "--\r\n--" :
							((i == 2) ? "EnBi\r\n--" :
							((i == 3) ? "EnIR\r\n--" :
							((i == 4) ? "!nSe\r\n17" :
							((i == 5) ? "nIni\r\n16" :
							((i == 6) ? "!nAL\r\n14" :
								"!nSt\r\n1")))))));
					glScalef(0.75, 1, 1);
					glColor3f(1, 1, 1);
					glText(msg.str().c_str(), 0.03);
				glPopMatrix();
				glTranslatef(0.075 * ((i == 3) ? 2 : 1), 0, 0);
			};
		glPopMatrix();

		//Draw MSB and LSB markers
		glPushMatrix();
			glTranslatef(0.285, 0.925, 0);
			glText("7", 0.05);
			glTranslatef(0.6, 0, 0);
			glText("0", 0.05);
		glPopMatrix();

		//Draw diagnostic info
		glPushMatrix();
			glTranslatef(0.05, 0.25, 0);
			glScalef(0.75, 1, 1);
			msg.str(string());
			msg << "Using device " << Port.PortName() << "\r\n"
				<< FPS << " FPS    " << SPS << " SPS\r\n"
				<< "Press 3 to 0 to control data pins\r\n"
				<< "Press e to p to control control pins\r\n"
				<< "Press SPACE to toggle data direction";
			glText(msg.str().c_str(), 0.05);
		glPopMatrix();
	glPopMatrix();

	++FPSCtr;

	glutSwapBuffers();
};


void MakeSteps(int count, volatile unsigned char addr,int DataOrCtrl,volatile unsigned char & Register){
	double prev=800;
	double desiredRPM=480;
	double pause=1/((desiredRPM/60.0)*prev);
	pause/=4;
	pause*=1000000;
	cout<<pause<<endl;
	for(int i=0;i<count*prev;i++){
		Register ^=addr;
		Port.Set(Register,DataOrCtrl);
		usleep(pause);  
		Register ^=addr;
		Port.Set(Register,DataOrCtrl);
		usleep(pause);//250KHz
	}
}


void HandleKeyboard(unsigned char key, int x, int y) {
			volatile unsigned char addr;
			if (key == '1'){//enable
				addr = (0b10000000);
				DataWReg^=addr;
				Port.Set(DataWReg,1);
			}
			if (key == '2'){//dirx
				addr = (0b00100000);
				DataWReg^=addr;
				Port.Set(DataWReg,1);
			}  
			if (key == '3'){//stepx
				addr = (0b01000000);
				MakeSteps(7,addr,1,DataWReg);
			}    
			if (key == '4'){//diry
				addr = (0b00010000);
				DataWReg^=addr;
				Port.Set(DataWReg,1);
			}  
			if (key == '5'){//stepy
				addr = 0x04;
				MakeSteps(8,addr,0,CtrlWReg);
			}
			if (key == '6'){//dirz
				addr = 0x02;
				CtrlWReg^=addr;
				Port.Set(CtrlWReg,0);
			}  
			if (key == '7'){//stepz
				addr = 0x01;
				MakeSteps(8,addr,0,CtrlWReg);
			}    
			if (key == '8'){//led
				addr = 0x08;
				CtrlWReg^=addr;
				Port.Set(CtrlWReg,0);
			}
	glutPostRedisplay();
};


void HandleIdle(void) {
	static time_t	LastTime = 0;

	//Perform acquisition here
	DataRReg = Port.Data();
	StatRReg = Port.Stat();
	CtrlRReg = Port.Ctrl();
	++SPSCtr;

	//Update performance counters
	if (LastTime != time(NULL)) {
		FPS = FPSCtr;
		FPSCtr = 0;
		SPS = SPSCtr;
		SPSCtr = 0;
		time(&LastTime);
	};

	//Redraw updated data
	glutPostRedisplay();
};
