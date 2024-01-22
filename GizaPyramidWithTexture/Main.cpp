#include <glut.h>
#include <math.h>
#include <SOIL2.h>
#include <stdio.h>

// Camera position
GLfloat camX = 0.0;
GLfloat camY = 0.0;
GLfloat camZ = 0.0;

// Scene translation and rotation
GLfloat sceRX = 0.0;
GLfloat sceRY = 0.0;
GLfloat sceRZ = 0.0;
GLfloat sceTX = 0.0;
GLfloat sceTY = 0.0;
GLfloat sceTZ = 0.0;

// Object rotation and scene translation
double objR = 0.0;
double sceneX = 0.0;
double sceneZ = 0.0;
double rotateSceneY = 0.0;

// Window dimensions
int width;
int height;
int width_sand;
int height_sand;

// Constants
constexpr float PI = 3.14159265358979323846;

// Texture data
unsigned char* image;
unsigned char* sand;
GLuint tex, tex1;

#define IMAGE_ROWS 64
#define IMAGE_COLS 64

GLubyte imageData[IMAGE_ROWS][IMAGE_COLS][3];

// Vertices of the objects
float vertices[][3] = {
{1,1,-1},{-1,1,-1},{-1,1,1},{1,1,1}, //Cube coordinates
{1,-1,-1},{1,-1,1},{-1,-1,1},{-1,-1,-1},
{0,1,0}                              //Pyramid apex
};

void loadTextureDataFromImage() {
	image = SOIL_load_image("giza.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	sand = SOIL_load_image("dessert_sand.jpeg", &width_sand, &height_sand, 0, SOIL_LOAD_RGB);

	if (image == NULL || sand == NULL) {
		printf("Error : %s", SOIL_last_result());
	}
}

void loadPyramidTexture() {
	loadTextureDataFromImage();
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, IMAGE_COLS, IMAGE_ROWS, 0, GL_RGB,
		GL_UNSIGNED_BYTE, image);  
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

void loadSandTexture() {
	loadTextureDataFromImage();
	glGenTextures(1, &tex1);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, IMAGE_COLS, IMAGE_ROWS, 0, GL_RGB,
		GL_UNSIGNED_BYTE, sand); 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

void drawGrid() {
	GLfloat step = 1.0f;
	GLint line;

	glBegin(GL_LINES);
	for (line = -20; line <= 20; line += step) {
		glVertex3f(line, -0.4, 20);
		glVertex3f(line, -0.4, -20);

		glVertex3f(20, -0.4, line);
		glVertex3f(-20, -0.4, line);
	}
	glEnd();
}

void drawAxes() {

	glBegin(GL_LINES);

	glLineWidth(1.5);

	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(-200, 0, 0);
	glVertex3f(200, 0, 0);

	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0, -200, 0);
	glVertex3f(0, 200, 0);

	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0, 0, -200);
	glVertex3f(0, 0, 200);

	glEnd();
}

void init(void) {
	glClearColor(0.14, 0.58, 0.75, 1.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
}

void setLighting() {
	// Lighting set up
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	// Set lighting intensity and color
	GLfloat qaAmbientLight[] = { 0.7, 0.7, 0.6, 1.0 };  
	GLfloat qaDiffuseLight[] = { 0.8, 0.8, 0.7, 1.0 };  
	GLfloat qaSpecularLight[] = { 1.0, 1.0, 1.0, 1.0 };  


	glLightfv(GL_LIGHT0, GL_AMBIENT, qaAmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, qaDiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, qaSpecularLight);

	// Set the light position
	GLfloat qaLightPosition[] = { 200.0, -100.0, -400.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, qaLightPosition);

	//set light 1
	glLightfv(GL_LIGHT1, GL_AMBIENT, qaAmbientLight);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, qaDiffuseLight);
	glLightfv(GL_LIGHT1, GL_SPECULAR, qaSpecularLight);

	//Set the light position
	GLfloat qaLightPosition2[] = { 200.0, -500.0, -400.0, 0.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, qaLightPosition2);

	//set light 3
	glLightfv(GL_LIGHT2, GL_AMBIENT, qaAmbientLight);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, qaDiffuseLight);
	glLightfv(GL_LIGHT2, GL_SPECULAR, qaSpecularLight);

	//Set the light position
	GLfloat qaLightPosition3[] = { -500.0, 0.0, -600.0, 1.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, qaLightPosition3);

}

//Draw a pyramid
void drawPyramid() {
	//Base (square)
	glBegin(GL_POLYGON);
	glNormal3f(0, 1, 0); // Normal for the base
	glTexCoord2f(1.0f, 0.0f); glVertex3fv(vertices[4]);
	glTexCoord2f(1.0f, 1.0f); glVertex3fv(vertices[7]);
	glTexCoord2f(0.0f, 1.0f); glVertex3fv(vertices[6]);
	glTexCoord2f(0.0f, 0.0f); glVertex3fv(vertices[5]);
	glEnd();

	//4 sides (triangles)
	glBegin(GL_TRIANGLES);
	glNormal3f(0, 0, -1); // Normal for the front face
	glTexCoord2f(0.5f, 1.0f); glVertex3fv(vertices[8]);
	glTexCoord2f(0.0f, 0.0f); glVertex3fv(vertices[6]);
	glTexCoord2f(1.0f, 0.0f); glVertex3fv(vertices[5]);

	glNormal3f(1, 0, 0); // Normal for the right face
	glTexCoord2f(0.5f, 1.0f); glVertex3fv(vertices[8]);
	glTexCoord2f(0.0f, 0.0f); glVertex3fv(vertices[6]);
	glTexCoord2f(1.0f, 0.0f); glVertex3fv(vertices[7]);

	glNormal3f(0, 0, 1); // Normal for the back face
	glTexCoord2f(0.5f, 1.0f); glVertex3fv(vertices[8]);
	glTexCoord2f(0.0f, 0.0f); glVertex3fv(vertices[7]);
	glTexCoord2f(1.0f, 0.0f); glVertex3fv(vertices[4]);

	glNormal3f(-1, 0, 0); // Normal for the left face
	glTexCoord2f(0.5f, 1.0f); glVertex3fv(vertices[8]);
	glTexCoord2f(0.0f, 0.0f); glVertex3fv(vertices[5]);
	glTexCoord2f(1.0f, 0.0f); glVertex3fv(vertices[4]);
	glEnd();
}

void drawLand() {
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-100, 0.0, 100);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-100, 0.0, -100);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(100, 0.0, -100);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(100, 0.0, 100);
	glEnd();
}

void drawPyramidWithLighting() {
	glShadeModel(GL_SMOOTH);

	// Set material properties
	GLfloat qaWhite[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat qaSky[] = { 0.14, 0.58, 0.75, 1.0 };
	GLfloat qaPyramid_light[] = { 0.76, 0.59, 0.36, 1.0 };
	GLfloat qaPyramid[] = { 0.8, 0.58, 0.16, 1.0 };
	GLfloat qaPyramidDiffuse[] = { 0.85, 0.67, 0.36, 1.0 };
	GLfloat qaSand[] = { 0.95, 0.73, 0.24, 1.0};
	GLfloat qaSandDark[] = { 0.85, 0.73, 0.24, 1.0};
	GLfloat qaLowAmbient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat qaFullAmbient[] = { 1.0, 1.0, 1.0, 1.0 };

	//Set, ambient, diffuse and specular lighting. Set ambient to 20%.

	glMaterialfv(GL_FRONT, GL_SPECULAR, qaWhite);
	glLightfv(GL_LIGHT0, GL_AMBIENT, qaLowAmbient);
	glLightfv(GL_LIGHT1, GL_AMBIENT, qaLowAmbient);
	glLightfv(GL_LIGHT2, GL_AMBIENT, qaLowAmbient);

	for (int iIndex = 0; iIndex < 6; ++iIndex) {

		glPushMatrix();

		//rotate six pyramids
		glRotated(objR, 0.0, 1.0, 0.0);

		float translate_addition = iIndex * 4;

		//transform three pyramids at back
		if (iIndex < 3) {
			//translate and scale three pyramids at the back
			glTranslatef(3.0 + (translate_addition), 2.0, -2.0 - (translate_addition / 2.2));
			glScaled(2.5, 2.2, 2.5);

			if (iIndex == 1) {
				//translate and scale middle pyramid at the back
				glTranslatef(0.0, 0.5, 0.0);
				glScaled(1.4, 1.4, 1.4);
			}
			if (iIndex == 2) {
				//translate last pyramid at the back
				glTranslatef(1.0, 0.0, 0.5);
			}
		}
		else {
			//translate three pyramids at the front
			glTranslated(-8.0 + (translate_addition / 1.5), 0.5, 1.3);
		}

		glEnable(GL_TEXTURE_2D);
		loadPyramidTexture();

		glEnable(GL_LIGHTING);

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, qaPyramid);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, qaPyramidDiffuse);
		glMaterialf(GL_FRONT, GL_SHININESS, 200.0);

		drawPyramid();

		glDisable(GL_LIGHTING);

		glDisable(GL_TEXTURE_2D);

		glPopMatrix();
	}

	glPushMatrix();

	glEnable(GL_TEXTURE_2D);
	loadSandTexture();

	glEnable(GL_LIGHTING);

	glShadeModel(GL_FLAT);
	glMaterialfv(GL_FRONT, GL_AMBIENT, qaSand);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, qaSand);
	glMaterialf(GL_FRONT, GL_SHININESS, 10.0);

	drawLand();

	glDisable(GL_LIGHTING);

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	// camera orientation (eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ)
	gluLookAt(-2.0, 2.0 + camY, 6.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// move the scene (all the rendered environment) using keyboard keys
	glTranslatef(sceTX, sceTY, sceTZ);

	////draw a grid on the xz plane
	glColor3f(1, 1, 1);
	drawGrid();
	////draw the three axes
	//drawAxes();

	setLighting();
	glPushMatrix();
	glScaled(1.2, 1.2, 1.2);
	drawPyramidWithLighting();
	glPopMatrix();

	glPopMatrix();
	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h) {
	glViewport(0, 0, w, h);
	GLfloat aspect_ratio = h == 0 ? w / 1 : (GLfloat)w / (GLfloat)h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//Define the Perspective projection frustum
	// (FOV_in_vertical, aspect_ratio, z-distance to the near plane from the camera position, z-distance to far plane from the camera position)
	gluPerspective(120.0, aspect_ratio, 1.0, 100.0);
}

void keyboardSpecial(int key, int x, int y) {
	if (key == GLUT_KEY_UP)
		camY += 1;

	if (key == GLUT_KEY_DOWN)
		camY -= 1;

	if (key == GLUT_KEY_RIGHT)
		sceTX += 1;

	if (key == GLUT_KEY_LEFT)
		sceTX -= 1;

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	//obj rotate clockwise
	if (key == 'l') {
		objR++;
	}
	//obj rotate anti-clockwise
	if (key == 'r') {
		objR--;
	}

	if (key == 'z') {
		sceneZ++;
	}
	if (key == 'Z') {
		sceneZ--;
	}

	if (key == 'y') {
		camY++;
	}
	if (key == 'Y') {
		camY--;
	}

	if (key == 'v') {
		rotateSceneY++;
	}
	if (key == 'V') {
		rotateSceneY--;
	}


	glutPostRedisplay();
}


int main(void) {
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1080, 720);
	glutInitWindowPosition(145, 0);
	glutCreateWindow("Giza Pyramids");
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboardSpecial);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	init();
	glutMainLoop();
	return 0;
}