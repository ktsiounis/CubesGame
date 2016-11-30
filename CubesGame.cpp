// ------------------------------------------------------- //
// This is a project, crated by Konstantinos Tsiounis,     //
// about graphics which is a 3D game similar to Minecraft //
// ------------------------------------------------------ //

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

int N;
// actual vector representing the camera's direction
int lx=0,lz=-1;
// XZ position of the camera
int x=0, z=5, y=1;
int temp;
int temp1;
int layer;


//I use struct for player
typedef struct{
	int lives;
	int points;
	int cubes;
}Player;
Player player = {3, 50, 0};

//class cube
class cube{
	private:
		int colour;
		int status;
		int posX, posY, posZ;
		int id;
	public:
		cube() { status = 0; colour = 0; }
		void setColour() { colour = (1+(rand() % 4)); }
		void setSpecificColour(int c) {colour = c;}
		int getColour() {return colour;}
		int getStatus() {return status;}
		void eraseCube() {colour =0;}
};
cube ***C;

//allocate memory for the cubes
void initializeCubeArray(){
	C = (cube ***)malloc(N*sizeof(cube**));
	for(int i=0; i<N; i++){
		C[i] = (cube **)malloc(N*sizeof(cube *));
		for(int j=0; j<N; j++){
			C[i][j] = (cube *)malloc(N*sizeof(cube));
		}
	}
	for(int i=0; i<N; i++) {//Set colours to the first layer's cubes
		for(int j=0; j<N; j++) {
			C[i][j][0].setColour(); //x,z,y y is the layers
		}
	}
}

void collapseAllCubes(){
	layer = y;
	while(layer < N){
		for(int i=0; i<N; i++){
			for(int j=0; j<N; j++){
				if(C[i][j][layer-1].getColour() == 0 && C[i][j][layer].getColour() != 0){
					int step = 1;
					while(C[i][j][layer-step].getColour() == 0  && C[i][j][layer-step+1].getColour() != 0){
						C[i][j][layer-step].setSpecificColour(C[i][j][layer-step+1].getColour());
						C[i][j][layer-step+1].eraseCube();
						step++;
					}
				}
			}
		}
		layer++;
	}
}

void checkTheBoxUnderMe(){//This function checks what's underneath player so as to set his cubes reserve
	if(C[x][z][y-1].getColour()==1){
		player.cubes = 1;
	}
	else if(C[x][z][y-1].getColour()==2){
		player.cubes = 2;
	}
	else if(C[x][z][y-1].getColour()==3){
		player.cubes = 3;
	}
	else{
		player.cubes = 0;
	}
}

//This function draws a cube
void drawcube(int x, int y, int z, int c){
	
	switch(c)
	{
		case 0:
			break;
		case 1:
			glColor3f(1.0, 1.0, 0.0); //yellow
			break;
		case 2:
			glColor3f(1.0, 0.0, 0.0); //red
			break;
		case 3:
			glColor3f(0.0, 1.0, 0.0); //green
			break;
		case 4:
			glColor3f(0.0, 0.0, 1.0); //blue
			break;
		case 5:
			glColor3f(1.0, 0.0, 1.0); //magenta
			break;
		default:
			break;
	}

	if(c != 0){//if the colour is 0 don't draw solid cube
		glPushMatrix();
		glTranslatef(x,y,z);
		glutSolidCube(1.0);
		glPopMatrix();
	}
}

void drawLayers(){
	for(int i=0; i<N; i++){
		for(int j=0; j<N; j++){
			for(int k=1; k<N; k++){
				drawcube(i,k,j,C[i][j][k].getColour());
			}
		}
	}
}

//This function draws the first layer
void drawfirstlayer(){
	for(int i=0; i<N; i++){
		for(int j=0; j<N; j++){
		
			if(i == N/2 && j == N/2){
				drawcube(i, 0, j, 5);}
			else{
				drawcube(i, 0, j, C[i][j][0].getColour());
			}
		}
	}
	
}

//This function draws text in the game. The text indicates player's points, lives and his cubes
void drawtext(float xpos, float zpos, float ypos, const char *string){
	char *message = (char *)string;
	int i =0;

	glColor3f(1.0, 0.0, 0.0); //red
	glRasterPos3f(xpos,ypos+0.2,zpos);

	while(*message != '\0'){
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *message++);
	}
}

//This function is called when you reshape the window so as to adjust the settings
void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;
	float ratio = 1.0* w / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

        // Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45,ratio,1,1000);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

//This fuction creat the game's world
void display(){
	char string1[60],string2[60],string3[60];
	
	//------Clear the buffers----------------------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//---------------------------------------------------------------------
	 
	//------Reset the matrix------------------------------------------------
	glLoadIdentity();
	//----------------------------------------------------------------------

	//------I'm setting up my camera----------------------------------------
	gluLookAt(	x, y, z,
				x+lx, y, z+lz,
				0.0f, 1.0f,  0.0f);
	//----------------------------------------------------------------------

	//------Check player's points and lives and if player's ponts are negative, player loses one life.
	//------If player's lives are negative then he loses the game and the game exits.
	if(player.points<0){
		player.lives--;
		player.points = 0;
	}
	if(player.lives<0){
		printf("-----------------GAME OVER-------------------\n");
		exit(0);
	}
	//------------------------------------------------------------------------------------------------

	//-------Fill the first layer with cubes-------------------------------------
	drawfirstlayer();
	//---------------------------------------------------------------------------

	//-------In this place I'm printing the informations to the screen-----------
	sprintf(string1,"You have %d lives,",player.lives);
	drawtext(x+lx,z+lz,y,string1);
	sprintf(string2,"%d points and",player.points);
	drawtext(x+lx,z+lz,y-0.05,string2);
	sprintf(string3,"%d cubes left",player.cubes);
	drawtext(x+lx,z+lz,y-0.1,string3);
	//---------------------------------------------------------------------------

	//------Here I'm drawing all the layers--------------------------------------
	drawLayers();
	//---------------------------------------------------------------------------
	
	glutSwapBuffers();
	glFlush();
	glutPostRedisplay();
}

void normal_keys(unsigned char key, int w, int h){; 
	switch(key) 
	{
		case (27) :
			exit(0); //Escape exits program
			break;

		case 'W':
		case 'w' :
			if( (x+lx)<N && (z+lz)<N && (x+lx)>=0  && (z+lz)>=0){
				if( ((C[x][z][y-1].getColour()) == 0)){
					x += lx;
					z += lz;
					player.points-=5*y;
					y=1;
				}
				else{
					if( (C[x+lx][z+lz][y].getColour()) == 0 ){//check if you have a cube in front of you and if not then go forward
						x += lx;
						z += lz;	
					}
					else if( (C[x+lx][z+lz][y].getColour()) != 0){
						x += lx;
						z += lz;
						y++;
					}
				}
			}
			checkTheBoxUnderMe();

			if(y == N){
				player.lives++;
				player.points+=100;
			}
			break;

		case 'A':
		case 'a' :
			temp = lx;
			lx = lz;
			lz = -temp;
			break;

		case 'D':
		case 'd' :
			temp = lx;
			lx = -lz;
			lz = temp;
			break;

		case 'S':
		case 's' :
			lx = -lx;
			lz = -lz;
			break;

		case 'E':
		case 'e':
			if(C[x+lx][z+lz][y].getColour() != 0){
				temp1 = 0;

				while(temp1<N){
					C[x+lx][z+lz][y+temp1].eraseCube();
					temp1++;
				}

				player.lives++;
				player.points-=20;
			}
			break;

		case 'Q':
		case 'q':
			C[x+lx][z+lz][y].eraseCube();
			break;

		case (32) : //when the player push the space button the cubes are increased by 1 and he loses 5 points
			player.cubes++;
			player.points-=5;
			C[x][z][y-1].setSpecificColour(player.cubes);
			break;

		case 'R':
		case 'r':
			collapseAllCubes();
			break;
	}
}

void mouse_keys(int button, int state, int w, int h){
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		int i=0,count=0;
		if(player.cubes>0 && (x+lx)<N && (z+lz)<N && (x+lx)>=0  && (z+lz)>=0){
			while(i<N && count==0){
				if(C[x+lx][z+lz][y+i].getColour() == 0){
					C[x+lx][z+lz][y+i].setColour();
					player.cubes--;
					player.points+=5;
					count = 1;
				}
				else{
					i++;
				}
			}
		}
	}
	else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){

		if( (x+lx)<N && (z+lz)<N && (x+lx)>=0  && (z+lz)>=0 ){
			if(C[x+lx][z+lz][y].getColour() !=0 ){
				int i=lx;
				int j=lz;
				int count=0;
				
				while((x+i)<N && (z+j)<N && (x+i)>=0  && (z+j)>=0 ){
					if(C[x+i][z+j][y].getColour() != 0){
						count++;
						i+=lx;
						j+=lz;
					}
					else break;
				}

				while((i!=lx || j!=lz) && count>0){
					if((x+i)<N && (z+j)<N && (x+i)>=0  && (z+j)>=0){
							if(C[x+i][z+j][y].getColour() == 0){
								C[x+i][z+j][y].setSpecificColour(C[x+i-lx][z+j-lz][y].getColour());
								C[x+i-lx][z+j-lz][y].eraseCube();
							}
					}
					else {
						C[x+i-lx][z+j-lz][y].eraseCube();
					}

					i-=lx;
					j-=lz;
					count--;
				}
			}
		}
	}
}


//--------------------------------------//
// Main                                 //
//  - inits glut and creates the window //
//  - assigns functions to glut         //
//  - starts the main rendering loop    //
//--------------------------------------//
int main(int argc,char* argv[]){
	
	
	//Read the N so as to create the grid
	if( argc != 2 ) {
		printf("GRRRRR wrong command!\n");
	}else {
		N = atoi(argv[1]);
		
		x=N/2;
		z=N/2;
	
		//init
		glutInit(&argc,argv);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
		glutInitWindowSize(650,650);
		glutInitWindowPosition(100,100);
		glutCreateWindow("Graphics Project 1");
	
		glutDisplayFunc(display);
		glutReshapeFunc(changeSize);
	
		glutKeyboardFunc(normal_keys);
		glutMouseFunc(mouse_keys);
	
		initializeCubeArray();
		glEnable(GL_DEPTH_TEST);
		glutMainLoop();
	}
	return 0;
}
