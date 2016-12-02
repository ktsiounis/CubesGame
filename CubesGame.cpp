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
// XZY position of the camera
int x=0, z=5, y=1;
int temp,cubesCounter,layer;


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
		int xpos,ypos,zpos;
	public:
		cube() { colour = 0; }
		void setColour() { colour = (1+(rand() % 4)); }
		void setSpecificColour(int c) {colour = c;}
		int getColour() {return colour;}
		void eraseCube() {colour =0;}
};
cube ***C; //initialiaze an 3D array of cubes

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
			C[i][j][0].setColour(); //x,z,y y are the layers
		}
	}
}

void collapseAllCubes(){
	layer = y; //set the current layer(y) as layer so as to change it without problems
	while(layer < N){ //do it for every layer
		for(int i=0; i<N; i++){
			for(int j=0; j<N; j++){
				//check every cube on this layer
				if(C[i][j][layer-1].getColour() == 0 && C[i][j][layer].getColour() != 0){//if the current cube has a colour and under him
					int step = 1;														 //there is nothing
					while(C[i][j][layer-step].getColour() == 0  && C[i][j][layer-step+1].getColour() != 0){//drop this cube until you find the next cube under him
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
	
	switch(c) //check the cube's colour and set the appropriate colour
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
void drawtext(float xPos, float zPos, float yPos, const char *string){
	char *message = (char *)string;
	int i =0;

	glColor3f(1.0, 1.0, 1.0); //red
	glRasterPos3f(xPos,yPos+0.2,zPos); //set the message at this postition on the screen

	while(*message != '\0'){
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *message++); //draw the text character by character
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
		case 'w':
			if( (x+lx)<N && (z+lz)<N && (x+lx)>=0  && (z+lz)>=0 ){
				if( (C[x+lx][z+lz][y-1].getColour() == 0) && (C[x+lx][z+lz][y].getColour() == 0) ){// if there is no cube in front of you
					player.points-=5*(y-2); //your points will be decreased by 5
					while(y >= 1 && C[x+lx][z+lz][y-1].getColour() == 0) y--; //find the next available cube and touch down to this cube
					x += lx;
					z += lz;
				}
				else if( C[x+lx][z+lz][y].getColour() == 0 ){//check if you have a cube in front of you and if not then go forward
						x += lx;
						z += lz;	
				}
				else{ //else climb on the cube
						x += lx;
						z += lz;
						y++;
				}
			}

			checkTheBoxUnderMe();

			if(y == N){ //If the player reach the last layer, takes one life and 100 points
				player.lives++;
				player.points+=100;
			}
			break;

		case 'A':
		case 'a':
			temp = lx; //I'm using this variable so as to exchange the values between lx & lz 
			lx = lz;
			lz = -temp;
			break;

		case 'D':
		case 'd':
			temp = lx; //I'm using this variable so as to exchange the values between lx & lz 
			lx = -lz;
			lz = temp;
			break;

		case 'S':
		case 's':
			lx = -lx;//reverse the values so as to look back
			lz = -lz;
			break;

		case 'E':
		case 'e':
			if(C[x+lx][z+lz][y].getColour() != 0){//if there is a cube in front of player
				cubesCounter = 0;//count the cubes

				while(cubesCounter < N){
					C[x+lx][z+lz][y+cubesCounter].eraseCube();//erase the cube on this layer
					cubesCounter++;//go to the next layer
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
			if(player.cubes==5) player.cubes=1;
			C[x][z][y-1].setSpecificColour(player.cubes);//and the cube below changes colour
			break;

		case 'R':
		case 'r':
			collapseAllCubes();
			player.points+=10;
			break;
	}
}

void mouse_keys(int button, int state, int w, int h){
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		int i=0,count=0;
		if(player.cubes>0 && ((x+lx)<N && (z+lz)<N && (x+lx)>=0  && (z+lz)>=0)){//check if the player has at least one cube to draw and if his not on the bounds
			while(i<N && count==0){//if the player has a cube in front of him draw the cube above
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
		if( (x+lx)<N && (z+lz)<N && (x+lx)>=0  && (z+lz)>=0 ){//check if it's in the bounds
			if(C[x+lx][z+lz][y].getColour() !=0 ){
				int i=lx;
				int j=lz;
				int count=0;
				
				while((x+i)<N && (z+j)<N && (x+i)>=0  && (z+j)>=0 ){//count how many cubes must be moved
					if(C[x+i][z+j][y].getColour() != 0){
						count++;
						i+=lx;
						j+=lz;
					}
					else break;
				}

				while((i!=lx || j!=lz) && count>0){ //move the cubes one by one
					if((x+i)<N && (z+j)<N && (x+i)>=0  && (z+j)>=0){
							if(C[x+i][z+j][y].getColour() == 0){
								C[x+i][z+j][y].setSpecificColour(C[x+i-lx][z+j-lz][y].getColour());
								C[x+i-lx][z+j-lz][y].eraseCube();
							}
					}
					else{
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
	
		glutInit(&argc,argv);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
		glutInitWindowSize(650,650);
		glutInitWindowPosition(100,100);
		glutCreateWindow("Cubes Game");
	
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