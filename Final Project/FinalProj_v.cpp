///////////////////////////////////////////////////////////////////////////////////////          
// 
// Author:  Kyra Thomas
// Email: kdthomas@syr.edu
// Assignment: Final Project
// Environment/Compiler: Visual Studio 2019
// Date: April 15, 2020
// References:
// textures from: http://texturelib.com/, 
//
// Interactions:
//
 /*


d - day
n - night
1 - dome 1 lights
2 - dome 2 lights
3 - rocket spotlight
" " - float
'v' - sink / move down
r - get in rocket, press r again to launch rocket
up arrow key - move forward
down arrow - move back
left arrow - turn left
right arrow - turn right
right click to plant, water, or remove plants
move mouse up or down to look up or down


*/
/////////////////////////////////////////////////////////////////////////////////////// 

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include "particles.h"
//#include "getbmp.h"
//#include "getbmp.cp"

#ifdef __APPLE__
#  include <GLUT/glut.h>
#  include <OpenGL/glext.h>
#else
#  include <GL/glut.h>
#endif

#define PI 3.14159265
#define ROOT3 1.73205081
#define ONEBYROOT3 0.57735027
#define RATIO 0.85 // Growth ratio = length of tree sub-branch to length of branch.
#define ANGLE 40 // Angle between the two tree sub-branches.


using namespace std;

int mouseX, mouseY;
float  mouseMotionY;
static int mainWin, mapWin; //window ids
static GLUquadricObj* qobj; // Create a pointer to a quadric object.
static GLUquadricObj* qobj_sky; // Create a pointer to a quadric object.
GLdouble camX = 0;//lookat cam orientation variables
GLdouble camY = 1;//lookat cam orientation variables
bool shakeDir = 0; //cam shake direction - 0: right, 1:left
static float fogDensity = 0.0008; // Fog density.
static float fogStart = 0.0; // Fog start z value.
static float fogEnd = 100.0; // Fog end z value.
int seed = time(0);//for rand
int seed2 = time(0);//for rand plants
int initTime = time(0);
const int num_textures = 10; //number of textures
static unsigned int texture[num_textures]; // Array of texture indices.
float height, width;//window measurements
static float meX = 0, meY = 15, meZ = 0;
static float angle = 180;
static float stepsize = 5.0;  //step size
static float turnsize = 10.0; //degrees to turn
static int animationPeriod = 50; // Time interval between frames.
//plot plant growing bools
bool growing1 = 0;
bool growing2 = 0;
bool growing3 = 0;
bool growing4 = 0;
//sky color variables
float skR = 0;
float skG = 2.0/255.0;
float skB = 7.0/255.0;
float shadowA = -0.45; //animated alpha value
//day and nigth animation bools
bool day = 0;
bool night = 1;
int smA = 90; //sun/moon rotation var
//sun/moon material emission variables
float smR = .7;
float smG = .7;
float smB = .7;
//rocket position variables
float rX = 250;
float rY = 20;
float rZ = -200;
bool board = 0;//on rocket or not?
bool launch = 0; //launch rocket or not?
float spaceRotateAngl = 0; //angle to rotate space sphere
//water bezier surface variables
float a = 0;
float b = 0;
bool waterUp = 0;//water animation bool
int        uSize = 4;
int        vSize = 6;
// The number of subdivisions in the grid
int        gridSize = 20;
GLfloat textureControlPoints[2][2][2] = { {{0.0, 0.0}, {0.0, 1.0}},
                                         {{1.0, 0.0}, {1.0, 1.0}} };
GLfloat textureControlPoints2[2][2][2] = { {{0.0, 0.0}, {0.0, 50.0}},
                                         {{50.0, 0.0}, {50.0, 50.0}} };

// Begin globals.
// Initial control points.
float controlPoints[6][4][3] =
{
    /*{{-3.0, 0.0, 5.0}, {-0.25, 0.0, 5.0}, {0.25, 0.0, 5.0}, {3.0, 0.0, 5.0}},
    {{-3.0, 0.0, 3.0}, {-0.25, 0.0, 3.0}, {0.25, 0.0, 3.0}, {3.0, 0.0, 3.0}},
    {{-3.0, 0.0, 1.0}, {-0.25, 0.0, 1.0}, {0.25, 0.0, 1.0}, {3.0, 0.0, 1.0}},
    {{-3.0, 0.0, -1.0}, {-0.25, 0.0, -1.0}, {0.25, 0.0, -1.0}, {3.0, 0.0, -1.0}},
    {{-3.0, 0.0, -3.0}, {-0.25, 0.0, -3.0}, {0.25, 0.0, -3.0}, {3.0, 0.0, -3.0}},
    {{-3.0, 0.0, -5.0}, {-0.25, 0.0, -5.0}, {0.25, 0.0, -5.0}, {3.0, 0.0, -5.0}},*/

    {{-3.000000,0.000000,5.000000}, {-0.250000,0.000000,5.000000}, {0.250000,0.000000,5.000000}, {3.000000,0.000000,5.000000}},
{{-3.000000,a,3.000000}, {-0.250000,b,3.000000}, {0.250000,-b,3.000000}, {3.000000,-a,3.000000}},
{{-3.000000,0.000000,1.000000}, {-0.250000,0.0,1.000000}, {0.250000,-1.200000,1.000000}, {3.000000,1.600000,1.000000}},
{{-3.000000,0.900000,-1.000000}, {-0.250000,0.000000,-1.000000}, {0.250000,2.200000,-1.000000}, {3.000000,-1.600000,-1.000000}},
{{-3.000000,a,-3.000000}, {-0.250000,b,-3.000000}, {0.250000,-b,-3.000000}, {3.000000,-a,-3.000000}},
{{-3.000000,0.000000,-5.000000}, {-0.250000,0.000000,-5.000000}, {0.250000,0.00000,-5.000000}, {3.000000,0.00000,-5.000000}}
};

//terrain control points
float terrainControlPoints[40][40][3];

//lighting variables
bool dome1LightOn = 1;
bool dome2LightOn = 1;
bool rocketSpotLightOn = 1;
static float spotExponent = 2.0; // Spotlight exponent = attenuation.
static float spotAngle = 40; // spot cone half-angle.
float amb = .25; //global ambient light
bool rocketSpotOn = 1;

//based off of fractals.cpp
class Source; // Make source class visible.

// Sequel class.
class Sequel
{
public:
    Sequel() { coords.clear(); v.clear(); }

    void drawPlant(); // Routine to draw Sequel object in case it represents a tree.

    friend class Source;

private:
    vector<float> coords; // Vector of x, y co-ordinates of points specifying sequel object.
    vector<Source> v; // Vector of associated source objects to be produced recursively.
};

// Routine to draw Sequel object in case it represents a tree.
void Sequel::drawPlant()
{
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 3; i++)
        glVertex2f(coords[2 * i], coords[2 * i + 1]);
    glEnd();
}

// Source class.
class Source
{
public:
    Source() { }
    Source(float coordsVal[4])
    {
        for (int i = 0; i < 4; i++) coords[i] = coordsVal[i];
    }

    void draw(); // Routine to draw source line segment.

    Sequel sourceToSequelPlant(); // Routine to generate Sequel object in case of tree.
    void producePlant(int level, int maxLevel, GLfloat *c); // Recursive routine to produce tree.

    friend class Sequel;

private:
    float coords[4]; // x, y co-ordinates of a line segment endpoints.
};

// Routine to draw source line segment.
void Source::draw()
{
    glBegin(GL_LINES);
    for (int i = 0; i < 2; i++)
        glVertex2f(coords[2 * i], coords[2 * i + 1]);
    glEnd();
}

// Routine to generate Sequel object in case of tree.
Sequel Source::sourceToSequelPlant()
{
    float x0, y0, x1, y1, coordsVal[10], coordsVal1[4];
    int i, j;
    Source s;
    Sequel seq = *new Sequel();

    x0 = coords[0]; y0 = coords[1]; x1 = coords[2]; y1 = coords[3];

    // From the co-ordinates of the two segment endpoints calculate the co-ordinates of
    // the 3 vertices of the sub-branch V-shape.
    coordsVal[0] = x1 + RATIO * cos((PI / 180.0) * ANGLE / 2.0) * (x1 - x0) - RATIO * sin((PI / 180.0) * ANGLE / 2.0) * (y1 - y0);//0:left side
    coordsVal[1] = y1 + RATIO * cos((PI / 180.0) * ANGLE / 2.0) * (y1 - y0) + RATIO * sin((PI / 180.0) * ANGLE / 2.0) * (x1 - x0);

    coordsVal[2] = x1;
    coordsVal[3] = y1;

    coordsVal[4] = x1 + RATIO * cos((PI / 180.0) * ANGLE / 2.0) * (x1 - x0) + RATIO * sin((PI / 180.0) * ANGLE / 2.0) * (y1 - y0);//1:right side
    coordsVal[5] = y1 + RATIO * cos((PI / 180.0) * ANGLE / 2.0) * (y1 - y0) - RATIO * sin((PI / 180.0) * ANGLE / 2.0) * (x1 - x0);

    // Enter V-shape vertices into sequel object.
    for (i = 0; i < 6; i++) seq.coords.push_back(coordsVal[i]);

    // Specify both sub-branch segments of the V-shape for recursive production.
    for (i = 0; i < 2; i++)
    {
        coordsVal1[0] = coordsVal[2];
        coordsVal1[1] = coordsVal[3];
        for (j = 2; j < 4; j++)
        {
            coordsVal1[j] = coordsVal[4 * i + j - 2];
        }
        s = *new Source(coordsVal1);
        seq.v.push_back(s);
    }

    return seq;
}

// Routine to draw leaf called by the following routine to produce tree.
void drawLeaf(float x, float y, GLfloat *c)
{
    glEnable(GL_LIGHTING);
    float matSpec[] = { 0,0,0, 1.0 };
    float matShine[] = { 30 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c);

    //glColor3fv(c);
    //glColor3f(0.5, 0.0, 0.5);
    glPushMatrix();
    glTranslatef(x, y, 0.0);
    glRotatef(rand() % 360, 0.0, 0.0, 1.0);
    glScalef(.7, .7, .7);
    glutSolidIcosahedron();
   /* glBegin(GL_QUADS);
    glVertex2f(0.0, 0.0);
    glVertex2f(1.0, 2.0);
    glVertex2f(0.0, 4.0);
    glVertex2f(-1.0, 2.0);
    glEnd();*/
    glPopMatrix();

    float matEmission[] = { 0,0,0,1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmission);
    glDisable(GL_LIGHTING);
}

// Recursive routine to produce tree.
void Source::producePlant(int level, int maxLevel, GLfloat *c)
{
    srand(seed); //to use the same randomly generated values each time
    glColor3f(0.4, 0.5, 0.5);

    // Branches are thinner up the tree.
    //glLineWidth(.50 * (maxLevel - level)); //KDT
    glLineWidth(-.5 * pow(1, level) + .05*maxLevel); //KDT exp function to make line width change exponentially with the level and max height

    // Source and sequels at all prior levels are drawn (different from Kock and Koch variant).
    if (maxLevel == 0) this->draw();
    else if (maxLevel == 1) { this->draw(); this->sourceToSequelPlant().drawPlant(); }
    else if (level < maxLevel)
    {
        if (level == 0) this->draw();//draw base/stump
        this->sourceToSequelPlant().drawPlant();
        //KDT
       // cout << "seed " << seed << endl;
        srand(seed++); 
        if (seed >= initTime + (maxLevel-1/2)) { seed = initTime; }
        int side = rand() % 2; //randomly choose which side will be the longer side
        this->sourceToSequelPlant().v[side].producePlant(level + 1, maxLevel,c);

    }

    // Embellish with leaves.
    if (level == maxLevel - 1)
    {
        drawLeaf(this->sourceToSequelPlant().coords[0], this->sourceToSequelPlant().coords[1],c);
        drawLeaf(this->sourceToSequelPlant().coords[4], this->sourceToSequelPlant().coords[5],c);
    }

    // Restore line width.
    glLineWidth(1.0);
    srand(NULL);
}


class plant {
public:
    float h, x, y, z, r, g, b;
    float x0, y0, x1;
    float y1 = 2.0;

    void draw() {//draws plant at origin, then translates it to x,y,z
        x0 = 0;
        x1 = 0;
        y0 = 0;
        float coordsVal1[4] = { x0, y0, x1, y1};

        Source src1 = *new Source(coordsVal1); // Edge of an equilateral triangle.

        glPushMatrix();
        glTranslatef(x, y, z);
        GLfloat c[4];
        c[0] = r;
        c[1] = g;
        c[2] = b;
        c[3] = .8;
        src1.producePlant(0, h,c);//produceTree(level,maxLevel)
        glPopMatrix();
    }
};

vector<plant> plants;
plant p1, p2, p3, p4;

void drawPlants() {
    //plot 1
    p1 = { 1,-18,5,-10, 0.5, 0.0, 0.5 }; //{height, x, y, z, r, g, b}
    p1.draw();
    plants.push_back(p1);
    p3 = { 1,-18,5,-15, 0.5, 0.0, 0.5 }; //{height, x, y, z, r, g, b}
    p3.draw();
    plants.push_back(p3);

    //plot 2
    p2 = { 1,-18,5,15, 0.5, 0.0, 0.5 }; //{height, x, y, z, r, g, b}
    p2.draw();
    plants.push_back(p2);
    p4 = { 1,-18,5,23, 0.5, 0.0, 0.5}; //{height, x, y, z, r, g, b}
    p4.draw();
    plants.push_back(p4);

    //cout << "plants insitialized" << endl;
}

//from fieldAndSkyFiltered.cpp
// Struct of bitmap file.
struct BitMapFile
{
    int sizeX;
    int sizeY;
    unsigned char* data;
};

// Routine to read a bitmap file.
// Works only for uncompressed bmp files of 24-bit color.
BitMapFile* getBMPData(string filename)
{
    BitMapFile* bmp = new BitMapFile;
    unsigned int size, offset, headerSize;

    // Read input file name.
    ifstream infile(filename.c_str(), ios::binary);

    // Get the starting point of the image data.
    infile.seekg(10);
    infile.read((char*)&offset, 4);

    // Get the header size of the bitmap.
    infile.read((char*)&headerSize, 4);

    // Get width and height values in the bitmap header.
    infile.seekg(18);
    infile.read((char*)&bmp->sizeX, 4);
    infile.read((char*)&bmp->sizeY, 4);

    // Allocate buffer for the image.
    size = bmp->sizeX * bmp->sizeY * 24;
    bmp->data = new unsigned char[size];

    // Read bitmap data.
    infile.seekg(offset);
    infile.read((char*)bmp->data, size);

    // Reverse color from bgr to rgb.
    int temp;
    for (int i = 0; i < size; i += 3)
    {
        temp = bmp->data[i];
        bmp->data[i] = bmp->data[i + 2];
        bmp->data[i + 2] = temp;
    }

    return bmp;
}

// Load external textures.
void loadExternalTextures()
{
    // Local storage for bmp image data.
    BitMapFile* image[num_textures];

    // Load the texture.
    image[0] = getBMPData("Final Project/Textures/soil-mud-0034-01-small.bmp"); //mud for plots from http://texturelib.com/
    image[1] = getBMPData("Final Project/Textures/metal_bare_0006_01_s.bmp");  //metal for flooring from http://texturelib.com/
    image[2] = getBMPData("Final Project/Textures/concrete-clean-0040-01-s.bmp");  //concrete for base sides from http://texturelib.com/
    image[3] = getBMPData("Final Project/Textures/water_water_0056_01_s.bmp"); //water tank water from http://texturelib.com/
    image[4] = getBMPData("Final Project/Textures/soil_cracked_0016_01_s.bmp"); //terrain texture from http://texturelib.com/
    image[5] = getBMPData("Final Project/Textures/blue_space.bmp"); //space texture from https://opengameart.org/content/space-skyboxes-0
    image[7] = getBMPData("Final Project/Textures/rocket_body_diffuse.bmp"); //https://opengameart.org/content/free-tiling-textures-42
    image[8] = getBMPData("Final Project/Textures/rocket_fins_diffuse.bmp"); //https://opengameart.org/content/free-tiling-textures-42
   // image[6] = getBMPData("Final Project/Textures/dashboard.bmp"); //space texture from https://i.pinimg.com/originals/49/63/01/49630159fd23dd1117000123058cf0e3.jpg

    // Activate texture index texture[0]. 
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[0]->sizeX, image[0]->sizeY, 0,
        GL_RGB, GL_UNSIGNED_BYTE, image[0]->data);
    
   // Activate texture index texture[1]. 
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[1]->sizeX, image[1]->sizeY, 0,
        GL_RGB, GL_UNSIGNED_BYTE, image[1]->data);
    
    // Activate texture index texture[2]. 
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[2]->sizeX, image[2]->sizeY, 0,
        GL_RGB, GL_UNSIGNED_BYTE, image[2]->data);

    // Activate texture index texture[3]. 
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[3]->sizeX, image[3]->sizeY, 0,
        GL_RGB, GL_UNSIGNED_BYTE, image[3]->data);

    // Activate texture index texture[4]. 
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[4]->sizeX, image[4]->sizeY, 0,
        GL_RGB, GL_UNSIGNED_BYTE, image[4]->data);

    // Activate texture index texture[5]. 
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[5]->sizeX, image[5]->sizeY, 0,
        GL_RGB, GL_UNSIGNED_BYTE, image[5]->data);

    // Activate texture index texture[7]. 
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[7]->sizeX, image[7]->sizeY, 0,
        GL_RGB, GL_UNSIGNED_BYTE, image[7]->data);

    // Activate texture index texture[8]. 
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    // Set texture parameters for wrapping.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Specify an image as the texture to be bound with the currently active texture index.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[8]->sizeX, image[8]->sizeY, 0,
        GL_RGB, GL_UNSIGNED_BYTE, image[8]->data);

    
    //water texture
    // Map the texture
    glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2,
        &textureControlPoints[0][0][0]);

    // Create the surface
    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, uSize, 0, 1, uSize * 3, vSize,
        &controlPoints[0][0][0]);
    glMapGrid2f(gridSize, 0.0, 1.0, gridSize, 0.0, 1.0);

    // Enable everything
    glEnable(GL_MAP2_TEXTURE_COORD_2);
    glEnable(GL_MAP2_VERTEX_3);
    
    //terrain texture
    // Map the texture
    glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2,
        &textureControlPoints2[0][0][0]);

    // Create the surface
    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 40, 0, 1, 40 * 3, 40,
        &terrainControlPoints[0][0][0]);
    glMapGrid2f(gridSize, 0.0, 1.0, gridSize, 0.0, 1.0);

    // Enable everything
    glEnable(GL_MAP2_TEXTURE_COORD_2);
    glEnable(GL_MAP2_VERTEX_3);
    
}

//particle system vector and declaration
vector<particleSys> particleSystems;
//systems for plot 1 and plot 2
particleSys ps1 = { -19, -11, -19, -1, false };//{rain_x0,rain_x1,rain_z0,rain_z1,on} 0-lower bound, 1-upper bound
particleSys ps2 = { -19, -11, 5, 23, false };//{rain_x0,rain_x1,rain_z0,rain_z1,on} 0-lower bound, 1-upper bound
particleSys ps3 = { -32, -24, -19, -1, false };//{rain_x0,rain_x1,rain_z0,rain_z1,on} 0-lower bound, 1-upper bound
particleSys ps4 = { -32, -24, 5, 23, false };//{rain_x0,rain_x1,rain_z0,rain_z1,on} 0-lower bound, 1-upper bound

void produceTerrain() {
    int count = 0;

    for (int i = 0; i < 20; i++) {//horizontal
        for (int j = 0; j < 20; j++) {//vertical
            for (int k = 0; k < 3; k++) {
                if (k == 1) {//y value
                    terrainControlPoints[i][j][k] = rand() % 50 + -25; //in the range -25 to 25
                }
                else if (k == 0) {//x
                    terrainControlPoints[i][j][k] = i;
                }
                else if (k == 2) {//z
                    terrainControlPoints[i][j][k] = j;
                }
                cout << terrainControlPoints[i][j][0] << ", " << terrainControlPoints[i][j][1] << ", " << terrainControlPoints[i][j][2] << endl;

            }

        }

    }
}

class plot {
public:
    float x, y, z;
    int age;
    vector<plant> plants;
    void drawPlot() {
        //plot mud
        glEnable(GL_LIGHTING);
        glColor3f(1, 1, 1);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture[0]);//mud texture
        glBegin(GL_POLYGON);//plot 1
        glTexCoord2f(0.0, 0.0); glVertex3f(x + 4, 5.35, z - 9);
        glTexCoord2f(1.0, 0.0); glVertex3f(x + 4, 5.35, z + 9);
        glTexCoord2f(1.0, 1.0); glVertex3f(x - 4, 5.35, z + 9);
        glTexCoord2f(0.0, 1.0); glVertex3f(x - 4, 5.35, z - 9);
        glEnd();

        glDisable(GL_TEXTURE_2D);


        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor3f(.3, .0, .0);
        glPushMatrix();//plot base
        glTranslatef(x, 5, z);
        glScalef(1, .05, 2);
        glutSolidCube(10);
        glPopMatrix();
        glDisable(GL_COLOR_MATERIAL);

        glDisable(GL_LIGHTING);
        //cout << "plot drawn" << endl;
    }

    void drawPlants(int num) {//vector of plants to draw in plot
        if (age == 45) {//dome plants
            float dist = 18 / num; //distance between each plant = plot length (in z-axis) / number of plants
            for (int i = z - 7; i < z + 7; i += dist) { //draw each plant
                //create a plant
                srand(seed2++);
                double r = (rand() % 5 + 1) / 10.0;
                double g = (rand() % 1 + 0) / 10.0;
                double b = (rand() % 7 + 1) / 10.0;
                int h = rand() % 4 + 1; //in the range 1 to 5

                plant p = { h,x,5,i, r, g, b }; //{height, x, y, z, r, g, b}
                p.y1 = 17;
                plants.push_back(p);
                p.draw();


                if (seed2 - initTime > 14) { seed2 = initTime; }

            }
        }
        else {
            float dist = 18 / num; //distance between each plant = plot length (in z-axis) / number of plants
            for (int i = z - 7; i < z + 7; i += dist) { //draw each plant
                //create a plant
                srand(seed2++);
                double r = (rand() % 5 + 1) / 10.0;
                double g = (rand() % 1 + 0) / 10.0;
                double b = (rand() % 7 + 1) / 10.0;
                int h;
                if (age == 1) {
                    h = 1;
                }
                else {
                    h = age;
                }
                plant p = { h,x,5,i, r, g, b }; //{height, x, y, z, r, g, b}
                p.y1 = 17;
                plants.push_back(p);
                p.draw();

            }
        }
        
    }

};

plot plot1, plot2, plot3, plot4; //plots player can plant on

vector<plot> menue_plots = { plot1, plot2, plot3, plot4 };

void initPlots() {
    //drawPlants();
    menue_plots[0] = { -15, 5.35, -10,0 };
    menue_plots[1] = { -15, 5.35, 14,0 };
    menue_plots[2] = { -28, 5.35, -10,0 };
    menue_plots[3] = { -28, 5.35, 14,0 };

    /*glTexCoord2f(0.0, 0.0); glVertex3f(-16, 5.35, -24);
    glTexCoord2f(1.0, 0.0); glVertex3f(-16, 5.35, -6);
    glTexCoord2f(1.0, 1.0); glVertex3f(-24, 5.35, -6);
    glTexCoord2f(0.0, 1.0); glVertex3f(-24, 5.35, -24);*/

    menue_plots[0].drawPlot();
    menue_plots[1].drawPlot();
    menue_plots[2].drawPlot();
    menue_plots[3].drawPlot();
}

// Initialization routine.
void setup(void)
{
    glClearColor(1, 1, 1, 0.0);
    glShadeModel(GL_FLAT);

    srand(NULL); //to use the different randomly generated values each time

   // produceTerrain();//generate plantet terrain control points

    //from fieldAndSkyFiltered.cpp
    // Create texture index array.
    glGenTextures(num_textures, texture);
    // Load external textures.
    loadExternalTextures();

    // Turn on OpenGL texturing.
    //glEnable(GL_TEXTURE_2D);

    initPlots(); //initialize player plots

    particleSystems.push_back(ps1);
    particleSystems.push_back(ps2);
    particleSystems.push_back(ps3);
    particleSystems.push_back(ps4);
    for (int i = 0; i < particleSystems.size(); i++) {//initialize the particle systems
        particleSystems[i].rainInit();
    }

    
    //lighting stuff
    glEnable(GL_LIGHTING);
    float globAmb[] = { amb, amb, amb, 1.0 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); // Global ambient light.
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint
     // Specify how texture values combine with current surface color values.
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Specify automatic texture generation for a sphere map.
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

    //space sphere
    // Create the new quadric object, drawn filled with a normal generated per vertex.
    qobj = gluNewQuadric();
    qobj_sky = gluNewQuadric();
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluQuadricNormals(qobj, GLU_SMOOTH);
    gluQuadricDrawStyle(qobj_sky, GLU_FILL);
    gluQuadricNormals(qobj_sky, GLU_SMOOTH);

}

void drawOutside() {
    float matEmission1[] = { 0,0,0,1 };//no emission

    float lightAmb0[] = { skR, skG, skB, 1.0 };
    float lightDifAndSpec0[] = { skR, skG, skB, 1.0 };
    float lightPos0[] = { 2000, 0, 0, 1 };

    // Light quadratic attenuation factor.
    glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, -.005);
    glLightfv(GL_LIGHT3, GL_AMBIENT, lightAmb0);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, lightDifAndSpec0);
    glLightfv(GL_LIGHT3, GL_SPECULAR, lightDifAndSpec0);

    //lighting
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glLightfv(GL_LIGHT3, GL_POSITION, lightPos0);
    glColor3f(1.0, 1.0, 1.0);
    glPopMatrix();



    glEnable(GL_LIGHTING);
    float matAmb[] = { 1, 1, 1, 1 };
    float matDif[] = { 1, 1, 1, 1 };
    float matSpec[] = { 0,0,0, 1.0 };
    float matShine[] = { 30 };
    float matEmission[] = { 1, 1, 1, 1 };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmission);


    srand(2); //to use the same randomly generated values each time
    //stars
    float sX, sY, sZ;
    int i = -3725;
    if (night) {//only draw stars at night
        while (i <= 400) {
            if (i == 0 || i == -10 || i == -100) { //to avoid division by zero error
                i = 1;
                sX = rand() % (i + 2801) + i; //in the range i to i+2800
            }
            else {
                sX = rand() % (i + 2801) + i; //in the range i to i+2800
            }
            sY = rand() % 3500 + 1000; //in the range 1000 to 4500
            sZ = rand() % 6000 + -3000; //in the range -3000 to 2099?

            //draw the star
            glPushMatrix();
            // glColor4f(1.0, 1.0, 1.0, starA);
            glTranslatef(sX, sY, sZ);
            glutSolidSphere(2, 10, 10);
            glPopMatrix();

            i += 5;

        }
    }
    srand(NULL); //to use the different randomly generated values each time

    //sun/moon material emission color
    matEmission[0] = smR;
    matEmission[1] = smG;
    matEmission[2] = smB;
    matEmission[3] = 1;
    matAmb[0] = smR;
    matAmb[1] = smG;
    matAmb[2] = smB;
    matAmb[3] = 1;
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmission);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matAmb);

    //moon/sun
    glPushMatrix();
    glRotatef(smA, 0, 0, 1);
    glTranslatef(2000, 0, 0);
    glutSolidSphere(80, 80, 80);
    glPopMatrix();

    
   
   // glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT2);
    //sky sphere
   // glEnable(GL_COLOR_MATERIAL);
   // glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    matEmission[0] = skR;
    matEmission[1] = skG;
    matEmission[2] = skB;
    matEmission[3] = 1;
    matAmb[0] = skR;
    matAmb[1] = skG;
    matAmb[2] = skB;
    matAmb[3] = 1;
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmission);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matAmb);

    glPushMatrix();
    //glColor4f(skR, skG, skB+.3,1);
    

   // glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, texture[5]);//space texture

   // glEnable(GL_TEXTURE_GEN_S);
    //glEnable(GL_TEXTURE_GEN_T);

    //glutSolidSphere(3500, 200, 200);
    gluSphere(qobj_sky, 3500, 200, 200);

  //  glDisable(GL_TEXTURE_GEN_S);
    //glDisable(GL_TEXTURE_GEN_T);

  //  glDisable(GL_TEXTURE_2D);

    glPopMatrix();
    glDisable(GL_LIGHTING);
    glEnable(GL_LIGHT2);
    //glDisable(GL_COLOR_MATERIAL);

    matEmission[2] = 0;//to keep other things from turining colors
    matEmission[1] = 0;
    matEmission[0] = 0;
    matEmission[3] = 1;
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmission);

}

void spaceSphere() {
       
   // glEnable(GL_LIGHTING);
    glPushMatrix();
    glColor3f(1, 1, 1);

    glTranslatef(250, 4500, -200);
    glRotatef(spaceRotateAngl, 1, 0, 0);

    glEnable(GL_TEXTURE_2D);

    
    glBindTexture(GL_TEXTURE_2D, texture[5]);//space texture

    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

   
    
    
    gluSphere(qobj, 1000, 80, 80);

   

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
   // glDisable(GL_LIGHTING);
}
/*
void drawTerrain() {

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glPushMatrix();
    //glTranslatef(0, 0, -40);

    //terrain bezier surface
    glColor3f(1.0, 1.0, 1.0);
    // Map the texture
    glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2, &textureControlPoints2[0][0][0]);
    // Create the surface
    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 40, 0, 1, 40 * 3, 40,
        &terrainControlPoints[0][0][0]);
    glMapGrid2f(gridSize, 0.0, 1.0, gridSize, 0.0, 1.0);
    // Enable everything
    glEnable(GL_MAP2_TEXTURE_COORD_2);
    glEnable(GL_MAP2_VERTEX_3);
    // Enable textures
    glEnable(GL_TEXTURE_2D);
    // Select the texture object
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    glPushMatrix();
   // glTranslatef(0, 9, 0);
    glScalef(200, 1, 200);
    glEvalMesh2(GL_FILL, 0, gridSize, 0, gridSize);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glFlush();

    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);
}*/

void drawPlanet() {
   /* glPushMatrix(); //circle planet
    glTranslatef(0, -800, 0);
    glColor3f(.3, 0, .3);
    glutSolidSphere(800, 20, 20);
    glPopMatrix();*/

   // drawTerrain();

   glEnable(GL_LIGHTING);

    glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[0]);//mud texture
    glPushMatrix(); //flat planet
    glBegin(GL_POLYGON);    
        glTexCoord2f(0.0, 0.0); glVertex3f(2000, 0, -2000);//back right
        glTexCoord2f(50.0, 0.0); glVertex3f(2000, 0, 2000);//front right
        glTexCoord2f(50.0, 50.0); glVertex3f(-2000, 0, 2000);//front left
        glTexCoord2f(0.0, 50.0); glVertex3f(-2000, 0, -2000);//back left
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
}

void drawPlanet2() {

    glColor3f(1, .5, .3);
    glPushMatrix(); //flat planet
    glBegin(GL_POLYGON);
    glVertex3f(1000, 0, -1000);//back right
    glVertex3f(1000, 0, 1000);//front right
    glVertex3f(-1000, 0, 1000);//front left
    glVertex3f(-1000, 0, -1000);//back left
    glEnd();
    glPopMatrix();

}


void drawTank() {

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glPushMatrix(); //water and tank
    glTranslatef(0, 0, -40);

    //water bezier surface
    glColor3f(1.0, 1.0, 1.0);
    // Map the texture
    glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2, &textureControlPoints[0][0][0]);
    // Create the surface
    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, uSize, 0, 1, uSize * 3, vSize,
        &controlPoints[0][0][0]);
    glMapGrid2f(gridSize, 0.0, 1.0, gridSize, 0.0, 1.0);
    // Enable everything
    glEnable(GL_MAP2_TEXTURE_COORD_2);
    glEnable(GL_MAP2_VERTEX_3);
    // Enable textures
    glEnable(GL_TEXTURE_2D);
    // Select the texture object
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glPushMatrix();
    glTranslatef(0, 9, 0);
    glScalef(2.7, 1, 2);
    glEvalMesh2(GL_FILL, 0, 20, 0, 20);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glFlush();

    //water body
    glColor4f(.31, .67, .768, .9);
    glPushMatrix();
    glScalef(7, 3, 7);
    glTranslatef(0, 2, 0);
    //glRotatef(90, 0, 0, 1);
    int numVertCyl = 80;
    float t = 0; // Angle parameter.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= numVertCyl; ++i)
    {
        glVertex3f(2 * cos(t), 1, 2 * sin(t) - 0);
        glVertex3f(2 * cos(t), -2, 2 * sin(t) - 0);
        t += 2 * PI / numVertCyl;
    }
    glEnd();
    glPopMatrix();

    //water top
    glPushMatrix();
    glTranslatef(0, 8, 0);
    glScalef(1, .1, 1);
    glutSolidSphere(14, 20, 20);
    glPopMatrix();

    glDisable(GL_COLOR_MATERIAL);

    glEnable(GL_LIGHTING);
    float matEmission[] = { 0,0,0,1 };
    float matAmbDiff[] = { .6, .6, .9, .4 };
    float matShine[] = { 30 };
    float matSpec[] = { .6,.6,.6, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbDiff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbDiff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matAmbDiff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);

    //tank
    //glColor4f(.6, .6, .9, .4);
    glPushMatrix();
    glScalef(8, 7, 8);
    glTranslatef(0, 1, 0);
    //glRotatef(90, 0, 0, 1);
     numVertCyl = 80;
     t = 0; // Angle parameter.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= numVertCyl; ++i)
    {
        glVertex3f(2 * cos(t), 1, 2 * sin(t) -0);
        glVertex3f(2 * cos(t), -2, 2 * sin(t) - 0);
        t += 2 * PI / numVertCyl;
    }
    glEnd();
    glPopMatrix();

    glPopMatrix();//water and tank

    matEmission[2] = 0;//to keep other things from turining colors
    matEmission[1] = 0;
    matEmission[0] = 0;
    matEmission[3] = 1;
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmission);

    glDisable(GL_LIGHTING);
}

void drawDome() {

int i;
float angle;
//glEnable(GL_LIGHTING);
    //dome 1 floor
    glColor3f(1, 1, 1);
    glPushMatrix();
    glTranslatef(0, 2, 0);
    glRotatef(90, 1, 0, 0);
    // from litTexturedCylider.cpp
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0);
    int p = 30; // Number of grid columns.
    for (i = 0; i <= p; i++)
    {
        glNormal3f(0.0, 1.0, 0.0);
        glTexCoord2f(15 + 15 * cos(((float)i / p) * PI), 15 + 15 * sin(((float)i / p) * PI));
        // glVertex3f(100 * cos(((float)i / p) * PI), 100 * sin(((float)i / p) * PI), 1.0);
        glVertex2f(cos(2* PI * i / p) * 100.0, sin(2 * PI * i / p) * 100.0);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    //tunnel floor
    glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 0.0); glVertex3f(205, 2, -6);//back right
        glTexCoord2f(10.0, 0.0); glVertex3f(205, 2, 6);//front right
        glTexCoord2f(10.0, 10.0); glVertex3f(95, 2, 6);//front left
        glTexCoord2f(0.0, 10.0); glVertex3f(95, 2, -6);//back left
    glEnd();
    glDisable(GL_TEXTURE_2D);

    //dome 2 floor
    glColor3f(1, 1, 1);
    glPushMatrix();
    glTranslatef(300, 2, 0);
    glRotatef(90, 1, 0, 0);
    // from litTexturedCylider.cpp
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0);
    p = 30; // Number of grid columns.
    for (i = 0; i <= p; i++)
    {
        glNormal3f(0.0, 1.0, 0.0);
        glTexCoord2f(15 + 15 * cos(((float)i / p) * PI), 15 + 15 * sin(((float)i / p) * PI));
        // glVertex3f(100 * cos(((float)i / p) * PI), 100 * sin(((float)i / p) * PI), 1.0);
        glVertex2f(cos(2 * PI * i / p) * 100.0, sin(2 * PI * i / p) * 100.0);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    //lighting for clear dome pieces
    glEnable(GL_LIGHTING);
    float matEmission[] = { 0,0,0,1 };
    float matAmbDiff[] = { .5, .5, .7, .4 };
    float matShine[] = { 30 };
    float matSpec[] = { .6,.6,.6, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbDiff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbDiff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matAmbDiff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);

    //dome top 1
    //glColor4f(.6, .6, .8, .3);
    glPushMatrix();
    glutSolidSphere(100, 80, 80);
    glPopMatrix();


    //tunnel
   // glColor4f(.6, .6, .8, .3);
    glPushMatrix();
    glScalef(40, 5, 5);
    glTranslatef(3.25, 2, 0);
    glRotatef(90, 0, 0, 1);
    int numVertCyl = 40;
    float t = 0; // Angle parameter.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= numVertCyl; ++i)
    {
        glVertex3f(2 * cos(t), 1, 2 * sin(t));
        glVertex3f(2 * cos(t), -2, 2 * sin(t));
        t += 2 * PI / numVertCyl;
    }
    glEnd();
    glPopMatrix();

    //dome top 2
    //glColor4f(.6, .6, .8, .3);
    glPushMatrix();
    glTranslatef(300, 0, 0);
    glutSolidSphere(100, 80, 80);
    glPopMatrix();


    float matEmission2[] = { 0,0,0,1 };
    float matAmbDiff2[] = { .5, .5, .7, .4 };
    float matShine2[] = { 60 };
    float matSpec2[] = { .4,.4,.4, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbDiff2);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine2);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbDiff2);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matAmbDiff2);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec2);


    //dome 1 clear floor
    glPushMatrix();
    glTranslatef(0, 4, 0);
    glRotatef(90, 1, 0, 0);
    glBegin(GL_POLYGON);
    //int p = 30; // Number of grid columns.
    for (i = 0; i <= 30; i += 1)
    {
        angle = (2 * PI * i / 30);
        glVertex2f(cos(angle) * 100.0, sin(angle) * 100.0);

    }
    glEnd();
    glPopMatrix();

    //tunnel clear floor
    glBegin(GL_POLYGON);
    glVertex3f(205, 4, -6);//back right
    glVertex3f(205, 4, 6);//front right
    glVertex3f(95, 4, 6);//front left
    glVertex3f(95, 4, -6);//back left
    glEnd();

    //dome 2 clear floor
    glPushMatrix();
    glTranslatef(300, 4, 0);
    glRotatef(90, 1, 0, 0);
    glBegin(GL_POLYGON);
    //int p = 30; // Number of grid columns.
    for (i = 0; i <= 30; i += 1)
    {
        angle = (2 * PI * i / 30);
        glVertex2f(cos(angle) * 100.0, sin(angle) * 100.0);

    }
    glEnd();
    glPopMatrix();

    glDisable(GL_LIGHTING);

    matEmission[2] = 0;//to keep other things from turining colors
    matEmission[1] = 0;
    matEmission[0] = 0;
    matEmission[3] = 1;
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmission);
}

void drawDome2() {
    int i;
    float angle;
    glColor3f(1, 1, 1);
    //dome 1 clear floor
    glPushMatrix();
    glTranslatef(0, 4, 0);
    glRotatef(90, 1, 0, 0);
    glBegin(GL_POLYGON);
    //int p = 30; // Number of grid columns.
    for (i = 0; i <= 30; i += 1)
    {
        angle = (2 * PI * i / 30);
        glVertex2f(cos(angle) * 100.0, sin(angle) * 100.0);

    }
    glEnd();
    glPopMatrix();

    //tunnel clear floor
    glBegin(GL_POLYGON);
    glVertex3f(205, 4, -6);//back right
    glVertex3f(205, 4, 6);//front right
    glVertex3f(95, 4, 6);//front left
    glVertex3f(95, 4, -6);//back left
    glEnd();

    //dome 2 clear floor
    glPushMatrix();
    glTranslatef(300, 4, 0);
    glRotatef(90, 1, 0, 0);
    glBegin(GL_POLYGON);
    //int p = 30; // Number of grid columns.
    for (i = 0; i <= 30; i += 1)
    {
        angle = (2 * PI * i / 30);
        glVertex2f(cos(angle) * 100.0, sin(angle) * 100.0);

    }
    glEnd();
    glPopMatrix();
}

//based off of woodenCube from woodTable4.cpp
//id2 is for the top, id1 is for the sides
void texturedCube(int texture_id1, int texture_id2)
{
    GLfloat textCoordFact_top = 20.0f;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[texture_id1]);
    glBegin(GL_QUADS);
    // Front Face
      //Modified the way to apply the texture, to be just a strip of wood.
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, .15f); glVertex3f(1.0f, 1.0f, 1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, .15f); glVertex3f(-1.0f, 1.0f, 1.0f);	// Top Left Of The Texture and Quad
    // Back Face
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad

       // Bottom Face
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);	// Bottom Right Of The Texture and Quad
    // Right face
      //Modified the way to apply the texture, to be just a strip of wood.
    glTexCoord2f(.7f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(.8f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(.8f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(.7f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);	// Bottom Left Of The Texture and Quad
    // Left Face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glEnd();


    glBindTexture(GL_TEXTURE_2D, texture[texture_id2]);
    glBegin(GL_QUADS);
    // Top Face

    glTexCoord2f(0.0f, textCoordFact_top); glVertex3f(-1.0f, 1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(textCoordFact_top, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(textCoordFact_top, textCoordFact_top); glVertex3f(1.0f, 1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glEnd();

    glDisable(GL_TEXTURE_2D);
}


void drawBase() {//space farm base
    glEnable(GL_LIGHTING);
    glPushMatrix();
    glColor3f(1, 1, 1);
    glTranslatef(125, 0, 0);
    glScalef(275, 1.5, 150);
    texturedCube(2, 2);
    /*glColor3f(.2, .2, .2);
    glTranslatef(100, 0, 0);
    glScalef(40, .75, 25);
    glutSolidCube(10);*/
    glPopMatrix();
    glDisable(GL_LIGHTING);
}

void drawBase2() {//space farm base
    glPushMatrix();
    glColor3f(.5, .5, .5);
    glTranslatef(125, 1, 0);
    glScalef(275, 1.5, 150);
    texturedCube(2, 2);
    /*glColor3f(.2, .2, .2);
    glTranslatef(100, 0, 0);
    glScalef(40, .75, 25);
    glutSolidCube(10);*/
    glPopMatrix();

}

/*
void drawPlots() {//plant plots
    glEnable(GL_LIGHTING);
    glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[0]);//mud texture
    glBegin(GL_POLYGON);//plot 1
        glTexCoord2f(0.0, 0.0); glVertex3f(-16, 5.35, -24);
        glTexCoord2f(1.0, 0.0); glVertex3f(-16, 5.35, -6);
        glTexCoord2f(1.0, 1.0); glVertex3f(-24, 5.35, -6);
        glTexCoord2f(0.0, 1.0); glVertex3f(-24, 5.35, -24);
    glEnd();

    glPushMatrix();
    glTranslatef(0, 0, 30);
    glBegin(GL_POLYGON);//plot 2
        glTexCoord2f(0.0, 0.0); glVertex3f(-16, 5.35, -24);
        glTexCoord2f(1.0, 0.0); glVertex3f(-16, 5.35, -6);
        glTexCoord2f(1.0, 1.0); glVertex3f(-24, 5.35, -6);
        glTexCoord2f(0.0, 1.0); glVertex3f(-24, 5.35, -24);
    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glPushMatrix();//plot 1
    glColor3f(.3, .0, .0);
    glTranslatef(-20, 5, -15);
    glScalef(1, .05, 2);
    glutSolidCube(10);
    glPopMatrix();

    glPushMatrix();//plot 2
    glColor3f(.3, .0, .0);
    glTranslatef(-20, 5, 15);
    glScalef(1, .05, 2);
    glutSolidCube(10);
    glPopMatrix();
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);
}*/

void drawRocket() {

    double eqn0[4] = { 0.0, -0.1, -.9, -142 }; // Data for clipping plane for rocket window
    glClipPlane(GL_CLIP_PLANE1, eqn0); // Specify clipping plane for rocket window
    double eqn1[4] = { 0.0, 0.1, .9, 142 }; // Data for clipping plane for rocket window glass
    glClipPlane(GL_CLIP_PLANE2, eqn1); // Specify clipping plane for rocket window glass

    float rocketSpotAngle = 55;
    float matEmission1[] = { 0,0,0,1 };//no emission

    float lightAmb0[] = { .0, .0, .0, 1.0 };
    float lightDifAndSpec0[] = { 1.0, 1.0, 1.0, 1.0 };
    float lightPos0[] = { 200, 5, -120, 1 };
    float spotDirection[] = { 0.0, 0.6, -0.4 }; // Spotlight direction. 

    // Light quadratic attenuation factor.
    glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, -.005);
    glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmb0);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDifAndSpec0);
    glLightfv(GL_LIGHT2, GL_SPECULAR, lightDifAndSpec0);

    //physical light
    glEnable(GL_LIGHTING);
    glPushMatrix();
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmission1);
    if (rocketSpotOn) glColor3f(1, 1, 1);
    else glColor3f(.2, .2, .2);
    glTranslatef(lightPos0[0], lightPos0[1], lightPos0[2]+4);
    glutSolidSphere(4, 20, 20);
    glPopMatrix();

    glColor3f(.6, .5, .5);
    glPushMatrix();
    glTranslatef(lightPos0[0], lightPos0[1], lightPos0[2]+5);
    glRotatef(30, 1, 0, 0);
    glRotatef(20, 0, -1, 0);
    glutSolidCone(5, 7, 30, 30);
    glPopMatrix();

    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);

    
    //lighting
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glLightfv(GL_LIGHT2, GL_POSITION, lightPos0);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, rocketSpotAngle);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spotDirection);
    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, spotExponent);
    glColor3f(1.0, 1.0, 1.0);
    glPopMatrix();

    
    glEnable(GL_LIGHTING);
   // glBindTexture(GL_TEXTURE_2D, texture[7]);

    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glColor3f(.6, .6, .6);    
    
    glPushMatrix();//rocket body
    glEnable(GL_CLIP_PLANE1);
    glTranslatef(rX, rY, rZ);
    glTranslatef(0, 100, 0);
    glScalef(3, 10, 3);
    glutSolidSphere(10,90,90);
    glDisable(GL_CLIP_PLANE1);
    glPopMatrix();//rocket body
    

    glPushMatrix();//rocket matrix
    glTranslatef(rX, rY, rZ);

    glPushMatrix();//rocket nose
    glTranslatef(0, 175, 0);
    glRotatef(90, -1, 0, 0);
    glScalef(2, 2, 1);
    glutSolidCone(10, 60, 90, 90);
    glPopMatrix();//rocket nose

    
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[8]);

   // glColor3f(.7, .0, .0);
    glPushMatrix(); //rocket fin 1
    glTranslatef(0, 10, 20);
    glScalef(.25, 2.5, 3.5);
    glRotatef(35, 1, 0, 0);
    glutSolidCube(10);
    glPopMatrix(); //rocket fin 1

    glPushMatrix();//rocket fin 2
    glRotatef(135, 0, 1, 0);
    glTranslatef(0, 10, 20);
    glScalef(.25, 2.5, 3.5);
    glRotatef(35, 1, 0, 0);
    glutSolidCube(10);
    glPopMatrix();//rocket fin 2
    
    glPushMatrix();//rocket fin 3
    glRotatef(225, 0, 1, 0);
    glTranslatef(0, 10, 20);
    glScalef(.25, 2.5, 3.5);
    glRotatef(35, 1, 0, 0);
    glutSolidCube(10);
    glPopMatrix();//rocket fin 3

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);

    //smoke thing at the bottom
    glColor3f(.3, .3, .3);
    glPushMatrix();
    glTranslatef(0, -10, 0);
    glRotatef(90, -1, 0, 0);
    glutSolidCone(10, 20, 90, 90);
    glPopMatrix();

    glPopMatrix();//rocket matrix

    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);
}

void drawRocketGlass() {
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glColor4f(.4, .4, .8, .3);
    glPushMatrix();//rocket window glass
    glEnable(GL_CLIP_PLANE2);
    glTranslatef(rX, rY, rZ);
    glTranslatef(0, 100, 0);
    glScalef(3, 10, 3);
    glutSolidSphere(10, 90, 90);
    glPopMatrix();//rocket window glass
    glDisable(GL_CLIP_PLANE2);

    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);
}

void drawDomeLights() {
    float matEmission1[] = { 0,0,0,1 };//no emission

    //light 0 ceiling light
    float lightAmb0[] = { .0, .0, .0, 1.0 };
    float lightDifAndSpec0[] = { 1.0, 1.0, 1.0, 1.0 };
    float lightPos0[] = { 0, 90, 0, 1 };
    float lightPos1[] = { 300, 90, 0, 1 };
    float spotDirection[] = { 0.0, -1.0, 0.0 }; // Spotlight direction. 
   
    // Light quadratic attenuation factor.
    //dome 1
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, -.005);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec0);
    //dome 2
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, -.005);
    glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmb0);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDifAndSpec0);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightDifAndSpec0);

    //dome 1 physical light
    glEnable(GL_LIGHTING);
    glPushMatrix();
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmission1);
    if (dome1LightOn) glColor3f(1, 1, 1);
    else glColor3f(.2, .2, .2);
    glTranslatef(lightPos0[0], lightPos0[1]+7, lightPos0[2]);
    glutSolidSphere(5, 20, 20);//center light

    glPushMatrix();//outer light 1
    glTranslatef(7, 0, 0);
    glutSolidSphere(3, 20, 20);
    glPopMatrix();

    glPushMatrix();//outer light 
    glTranslatef(-7, 0, 0);
    glutSolidSphere(3, 20, 20);
    glPopMatrix();

    glDisable(GL_COLOR_MATERIAL);
    glPopMatrix();
    glDisable(GL_LIGHTING);

    //dome 1 lighting
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spotAngle);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDirection);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, spotExponent);
    glColor3f(1.0, 1.0, 1.0);
    glPopMatrix();



    //dome 2 physical light
    glEnable(GL_LIGHTING);
    glPushMatrix();
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmission1);
    if (dome2LightOn) glColor3f(1, 1, 1);
    else glColor3f(.2, .2, .2);
    glTranslatef(lightPos1[0], lightPos1[1] + 7, lightPos1[2]);
    glutSolidSphere(5, 20, 20);//center light

    glPushMatrix();//outer light 1
    glTranslatef(7, 0, 0);
    glutSolidSphere(3, 20, 20);
    glPopMatrix();

    glPushMatrix();//outer light 
    glTranslatef(-7, 0, 0);
    glutSolidSphere(3, 20, 20);
    glPopMatrix();

    glDisable(GL_COLOR_MATERIAL);
    glPopMatrix();
    glDisable(GL_LIGHTING);

    //dome 2 lighting
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, spotAngle);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDirection);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, spotExponent);
    glColor3f(1.0, 1.0, 1.0);
    glPopMatrix();


}

void drawShadows() {

    glColor4f(.2, .2, .2, shadowA);

     //dome top 1
    glPushMatrix();
    glTranslatef(-120, 2, 0);
    glRotatef(90, 0, 0, 1);
    glScalef(.01, 1, 1);//squish
    glutSolidSphere(100, 80, 80);
    glPopMatrix();
    

    glPushMatrix();//rocket shadow

   glTranslatef(-8, 0, 0);
   glTranslatef(rX, rY-3, rZ);
   glRotatef(90, 0, 0, 1);
   glScalef(.0001, 1, 1);//squish
   glTranslatef(-rX, -rY, -rZ);

    glPushMatrix();//rocket body
    glEnable(GL_CLIP_PLANE1);
    glTranslatef(rX, rY, rZ);
    glTranslatef(0, 100, 0);
    glScalef(3, 10, 3);
    glutSolidSphere(10, 90, 90);
    glDisable(GL_CLIP_PLANE1);
    glPopMatrix();//rocket body


    glPushMatrix();//rocket matrix
    glTranslatef(rX, rY, rZ);

    glPushMatrix();//rocket nose
    glTranslatef(0, 175, 0);
    glRotatef(90, -1, 0, 0);
    glScalef(2, 2, 1);
    glutSolidCone(10, 60, 90, 90);
    glPopMatrix();//rocket nose

    glPushMatrix(); //rocket fin 1
    glTranslatef(0, 10, 20);
    glScalef(.25, 2.5, 3.5);
    glRotatef(35, 1, 0, 0);
    glutSolidCube(10);
    glPopMatrix(); //rocket fin 1

    glPushMatrix();//rocket fin 2
    glRotatef(135, 0, 1, 0);
    glTranslatef(0, 10, 20);
    glScalef(.25, 2.5, 3.5);
    glRotatef(35, 1, 0, 0);
    glutSolidCube(10);
    glPopMatrix();//rocket fin 2

    glPushMatrix();//rocket fin 3
    glRotatef(225, 0, 1, 0);
    glTranslatef(0, 10, 20);
    glScalef(.25, 2.5, 3.5);
    glRotatef(35, 1, 0, 0);
    glutSolidCube(10);
    glPopMatrix();//rocket fin 3

    //smoke thing at the bottom
    glColor3f(.3, .3, .3);
    glPushMatrix();
    glTranslatef(0, -10, 0);
    glRotatef(90, -1, 0, 0);
    glutSolidCone(10, 20, 90, 90);
    glPopMatrix();

    glPopMatrix();//rocket matrix

    glPopMatrix(); //shadows
}


void drawScene(void)
{
    glutSetWindow(mainWin);
    
    if (dome1LightOn) { glEnable(GL_LIGHT0); }
    else { glDisable(GL_LIGHT0); }// Enable or disable particular light source.
    if (dome2LightOn) { glEnable(GL_LIGHT1); }
    else { glDisable(GL_LIGHT1); }// Enable or disable particular light source
    if (rocketSpotLightOn) { glEnable(GL_LIGHT2); }
    else { glDisable(GL_LIGHT2); }//rocket spotlight Enable or disable particular light source.
    glEnable(GL_LIGHT3);//sun/moon

    float fogColor[4] = { 0.5, 0.5, 0.5, 1.0 };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_NORMALIZE);

    //two sided lighting
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    float globAmb[] = { amb, amb, amb, 1.0 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); // Global ambient light.

    // Fog controls.
    glDisable(GL_FOG);
    glHint(GL_FOG_HINT, GL_NICEST);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogi(GL_FOG_MODE, GL_EXP);
    glFogf(GL_FOG_START, fogStart);
    glFogf(GL_FOG_END, fogEnd);
    glFogf(GL_FOG_DENSITY, fogDensity);

    glLoadIdentity();

    mouseMotionY = ((height/2) - mouseY)/600;//ammount to move the "camera" up or down
    
    if (launch && rY < 4500) {//on rocket
    gluLookAt(rX, rY + 150, rZ + 5, rX, rY + 150 + mouseMotionY, rZ + 50, camX, camY, 0);
    }
    else if (rY >= 4500 || board) {//in space
        gluLookAt(rX, rY + 150, rZ, rX, rY + 150 + mouseMotionY, rZ + 50, 0, 1, 0);
    }
    else if (rY == 20) {//not on rocket
        gluLookAt(meX, meY, meZ, sin(angle * PI / 180) + (meX), meY + mouseMotionY, cos(angle * PI / 180) + (meZ), 0, 1, 0);
    }

    //gluLookAt(meX, meY, meZ, sin(angle * PI / 180) + (meX), meY, cos(angle * PI / 180) + (meZ), 0, 1, 0);
    glEnable(GL_DEPTH_TEST);

    // glEnable(GL_CLIP_PLANE0);
    if (day) {
        glEnable(GL_FOG);//sky looks good with fog
    }
    else {
        glDisable(GL_FOG);
    }
    drawOutside();
    glDisable(GL_FOG);
    spaceSphere();
    // glDisable(GL_CLIP_PLANE0);

   // glEnable(GL_CLIP_PLANE0);
    glEnable(GL_FOG);
    drawPlanet();
    glDisable(GL_FOG);
   // glDisable(GL_CLIP_PLANE0);
    drawBase();    

    //plants in dome 2
    plot pl1, pl2,pl3,pl4,pl5;
    pl1 = { 310,0,-20,45 };
    pl2 = { 310,0,20,45 };
    pl3 = { 330,0,-25,45 };
    pl4 = { 330,0,0,45 };
    pl5 = { 330,0,25,45 };
    pl1.drawPlot();
    pl2.drawPlot();
    pl3.drawPlot();
    pl4.drawPlot();
    pl5.drawPlot();
    pl1.drawPlants(3);
    pl2.drawPlants(2);
    pl3.drawPlants(3);
    pl4.drawPlants(4);
    pl5.drawPlants(3);
    
    //draw plot 1 and its plants
    menue_plots[0].drawPlot();
    if (menue_plots[0].age >= 1) {
        menue_plots[0].drawPlants(3);
    }

    //draw plot 2 and its plants
    menue_plots[1].drawPlot();
    if (menue_plots[1].age >= 1) {
        menue_plots[1].drawPlants(3);
    }

    //draw plot 3 and its plants
    menue_plots[2].drawPlot();
    if (menue_plots[2].age >= 1) {
        menue_plots[2].drawPlants(3);
    }

    //draw plot 4 and its plants
    menue_plots[3].drawPlot();
    if (menue_plots[3].age >= 1) {
        menue_plots[3].drawPlants(3);
    }


    for (int i = 0; i < particleSystems.size(); i++) {//draw the particle systems
        if (particleSystems[i].on) {
            particleSystems[i].drawRain();
        }
    }

     drawDomeLights();

    drawRocket();
    drawRocketGlass();//draw after rocket so you cant see through the back
    drawDome();//keep close to last
    drawTank();
    drawShadows();

    glutSwapBuffers();
}

void setup2() {
    glClearColor(222.0/255.0, 184.0/255.0, 135.0/255.0, 0.0);
    glShadeModel(GL_FLAT);

}

//from book code
void writeStrokeString(void* font, char* string)
{
    char* c;
    for (c = string; *c != '\0'; c++) glutStrokeCharacter(font, *c);
}


void drawScene2() {
    glutSetWindow(mapWin);


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_NORMALIZE);


    glLoadIdentity();
 
    gluLookAt(meX,200,meZ, meX, meY, meZ, 0, 0, -1);

    glEnable(GL_DEPTH_TEST);

    //player dot
    glColor3f(1, 0, 0);
    glPushMatrix();
    glTranslatef(meX, meY, meZ);
    glScalef(1, .1, 1);
    glutSolidSphere(5, 20, 20);
    glPopMatrix();


   // drawOutside();

    // glDisable(GL_CLIP_PLANE0);

   // glEnable(GL_CLIP_PLANE0);

    drawPlanet2();

    // glDisable(GL_CLIP_PLANE0);
    drawBase2();

    //plants in dome 2
    plot pl1, pl2, pl3, pl4, pl5;
    pl1 = { 310,0,-20,45 };
    pl2 = { 310,0,20,45 };
    pl3 = { 330,0,-25,45 };
    pl4 = { 330,0,0,45 };
    pl5 = { 330,0,25,45 };
    pl1.drawPlot();
    pl2.drawPlot();
    pl3.drawPlot();
    pl4.drawPlot();
    pl5.drawPlot();
    pl1.drawPlants(3);
    pl2.drawPlants(2);
    pl3.drawPlants(3);
    pl4.drawPlants(4);
    pl5.drawPlants(3);

    //draw plot 1 and its plants
    menue_plots[0].drawPlot();
    if (menue_plots[0].age >= 1) {
        menue_plots[0].drawPlants(3);
    }

    //draw plot 2 and its plants
    menue_plots[1].drawPlot();
    if (menue_plots[1].age >= 1) {
        menue_plots[1].drawPlants(3);
    }

    //draw plot 3 and its plants
    menue_plots[2].drawPlot();
    if (menue_plots[2].age >= 1) {
        menue_plots[2].drawPlants(3);
    }

    //draw plot 4 and its plants
    menue_plots[3].drawPlot();
    if (menue_plots[3].age >= 1) {
        menue_plots[3].drawPlants(3);
    }

    glColor3f(.5, .0, .0);
    glPushMatrix();
    glTranslatef(-9, 10, -5);
    glScalef(.05, .05, .05);
    glRotatef(90, -1, 0, 0);
    //glRotatef(90, 0, 0, 1);
    glLineWidth(2.0);
    char text2[] = "Plot 1";
    writeStrokeString(GLUT_STROKE_ROMAN, text2);
    glPopMatrix();

    //glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
    glTranslatef(-9, 10, 15);
    glScalef(.05, .05, .05);
    glRotatef(90, -1, 0, 0);
    //glRotatef(90, 0, 0, 1);
    glLineWidth(2.0);
    char text3[] = "Plot 2";
    writeStrokeString(GLUT_STROKE_ROMAN, text3);
    glPopMatrix();

    //glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
    glTranslatef(-58, 10, -5);
    glScalef(.05, .05, .05);
    glRotatef(90, -1, 0, 0);
    //glRotatef(90, 0, 0, 1);
    glLineWidth(2.0);
    char text4[] = "Plot 3";
    writeStrokeString(GLUT_STROKE_ROMAN, text4);
    glPopMatrix();

    //glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
    glTranslatef(-58, 10, 15);
    glScalef(.05, .05, .05);
    glRotatef(90, -1, 0, 0);
    //glRotatef(90, 0, 0, 1);
    glLineWidth(2.0);
    char text5[] = "Plot 4";
    writeStrokeString(GLUT_STROKE_ROMAN, text5);
    glPopMatrix();

    //drawDomeLights();

    //glColor3f(0.8, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(rX-10, rY+10, rZ+3);
    glScalef(.05, .05, .05);
    glRotatef(90, -1, 0, 0);
    //glRotatef(90, 0, 0, 1);
    glLineWidth(2.0);
    char text1[] = "Rocket";
    writeStrokeString(GLUT_STROKE_ROMAN, text1);
    glPopMatrix();

    //rocket dot
    glColor3f(0, 0, 1);
    glPushMatrix();
    glTranslatef(rX, rY, rZ);
    glScalef(1, .1, 1);
    glutSolidSphere(15, 20, 20);
    glPopMatrix();

    //drawRocket();
    //drawRocketGlass();//draw after rocket so you cant see through the back
    //drawDome();//keep close to last
    drawDome2();

    //water tank
    glColor3f(.3, .5, 1);
    glPushMatrix();
    glTranslatef(0, 8, -40);
    glScalef(1, .1, 1);
    glutSolidSphere(14, 20, 20);
    glPopMatrix();

    glutSwapBuffers();

    glutSetWindow(mainWin);
}

void rocketLaunch() {
    if (rY <= 4500) {//increase until rocket is in space sphere
        rY += 20;
    }
}

void idle_func() {
    for (int i = 0; i < particleSystems.size(); i++) {//particle system idle function
        if (particleSystems[i].on == 1) {
            particleSystems[i].rainIdle();
        }
    }

    //water animation
    if (!waterUp) {
        a -= .0055;
        b -= .055;
    }
    if (waterUp) {
        a += .0055;
        b += .055;
    }
    if (b <= -8.5 || b >= 8.5) { waterUp = !waterUp; }

    controlPoints[0][0][1] = a / 2;
    controlPoints[1][0][1] = a;
    controlPoints[0][1][1] = b / 2;
    controlPoints[1][1][1] = b;
    controlPoints[0][2][1] = -b / 2;
    controlPoints[1][2][1] = -b;
    controlPoints[0][3][1] = -a / 2;
    controlPoints[1][3][1] = -a;

    controlPoints[5][0][1] = a / 2;
    controlPoints[4][0][1] = a;
    controlPoints[5][1][1] = b / 2;
    controlPoints[4][1][1] = b;
    controlPoints[5][2][1] = -b / 2;
    controlPoints[4][2][1] = -b;
    controlPoints[4][3][1] = -a;
    controlPoints[5][3][1] = -a / 2;

    

    glutPostRedisplay();
}

/*void growPlants(plot plot) {
    bool g = 0;
        
            for (int i = 0; i < plot.plants.size(); i++) {//draw the particle systems
                if (plot.plants[i].h <= 40) {
                plot.plants[i].h++;
                plot.age++;
                }
            //plot.age += 1; // increase the plants height
           // cout << "p " << i << "h= " << plants[i].h << endl;
            }
    glutPostRedisplay();
}*/

void animate(int someValue) {
    if (growing1) {//grow plot 1's plants
        if (menue_plots[0].age <= 20) {
            for (int i = 0; i < menue_plots[0].plants.size(); i++) {//draw the particle systems
                if (menue_plots[0].plants[i].h <= 20) {
                    menue_plots[0].plants[i].h++;
                }
            }
            menue_plots[0].age++;
        }
    }
    if (growing2) {//grow plot 2's plants
        if (menue_plots[1].age <= 20) {
            for (int i = 0; i < menue_plots[1].plants.size(); i++) {//draw the particle systems
                if (menue_plots[1].plants[i].h <= 20) {
                    menue_plots[1].plants[i].h++;
                }
            }
            menue_plots[1].age++;
        }
    }
    if (growing3) {//grow plot 3's plants
        if (menue_plots[2].age <= 20) {
            for (int i = 0; i < menue_plots[2].plants.size(); i++) {//draw the particle systems
                if (menue_plots[2].plants[i].h <= 20) {
                    menue_plots[2].plants[i].h++;
                }
            }
            menue_plots[2].age++;
        }
    }
    if (growing4) {//grow plot 4's plants
        if (menue_plots[3].age <= 20) {
            for (int i = 0; i < menue_plots[3].plants.size(); i++) {//draw the particle systems
                if (menue_plots[3].plants[i].h <= 20) {
                    menue_plots[3].plants[i].h++;
                }
            }
            menue_plots[3].age++;
        }
    }

    //switching between day and night
    if (day && skB <= 1) {
    //    cout << shadowA << endl;
        if (smA <= 400) {
            smA += 10;// rotate sun/moon 
        }
        shadowA += .025;//make shadows visible
        //change sky color
        skB += .025;
        if (skR <= .3) {
            skR += .01;
        }
        skG += .0125;

       if (smA == 300) {//change moon material color
            smR = .8;
            smG = .6;
            smB = 0.0;
        }

        if (amb <= .4) {//increase global ambient light
            amb += .05;
        }
    }
    if (night && skB >= 0) {
        if (smA >= 90) {
            smA -= 10;// rotate sun/moon
        }
        shadowA -= .025;//make shaodws invisible
        //change sky color
        skB -= .025;
        if (skR >= 0) {
            skR -= .01;
        }
        skG -= .0125;

        if (smA == 300) {//change moon material color
            smR = .8;
            smG = .8;
            smB = .7;
        }

        if (amb >= .05) {//decrease global ambient light
            amb -= .05;
        }

    }

    if (launch) {
        rocketLaunch();
        spaceRotateAngl += 10;//rotate space sphere 

        //camera shake!
        if (!shakeDir) {// 0 shake right
            camX = .01;
            camY = .99;
            shakeDir = 1;
        }
        else if (shakeDir) {// 1 shake left
            camX = -.01;
            camY = .99;
            shakeDir = 0;
        }

    }


    glutTimerFunc(animationPeriod, animate, 1);
}

// The top menu callback function.
void top_menu(int id)
{
    if (id == 1) exit(0);
}

// The sub-menu callback functions.
void plot1_menu(int id)
{
    int i = 0;
    if (id == 1) {//draw the baby plants in the plot
        menue_plots[i].age = 1;
        menue_plots[i].drawPlants(3);
    }

    if (id == 2) {//turn water on for this plot
        particleSystems[i].on = 1;
        growing1 = 1;
    }

    if (id == 3) {//turn water off for this plot
        particleSystems[i].on = 0;
        growing1 = 0;
    }

    if (id == 4) {//remove the plants
        menue_plots[i].age = 0;
    }
    
    glutPostRedisplay();
}

void plot2_menu(int id)
{
    int i = 1;
    if (id == 1) {//draw the baby plants in the plot
        menue_plots[i].age = 1;
        menue_plots[i].drawPlants(3);
    }

    if (id == 2) {//turn water on for this plot
        if (particleSystems[i].on == 0) {
            particleSystems[i].on = !particleSystems[i].on;
        }
        growing2 = 1;
    }

    if (id == 3) {//turn water off for this plot
        if (particleSystems[i].on == 1) {
            particleSystems[i].on = !particleSystems[i].on;
        }
        growing2 = 0;
    }

    if (id == 4) {//remove the plants
        menue_plots[i].age = 0;
    }
    glutPostRedisplay();
}

void plot3_menu(int id)
{
    int i = 2;
    if (id == 1) {//draw the baby plants in the plot
        menue_plots[i].age = 1;
        menue_plots[i].drawPlants(3);
    }

    if (id == 2) {//turn water on for this plot
        particleSystems[i].on = 1;
        growing3 = 1;
    }

    if (id == 3) {//turn water off for this plot
        particleSystems[i].on = 0;
        growing3 = 0;
    }

    if (id == 4) {//remove the plants
        menue_plots[i].age = 0;
    }

    glutPostRedisplay();
}

void plot4_menu(int id)
{
    int i = 3;
    if (id == 1) {//draw the baby plants in the plot
        menue_plots[i].age = 1;
        menue_plots[i].drawPlants(3);
    }

    if (id == 2) {//turn water on for this plot
        particleSystems[i].on = 1;
        growing4 = 1;
    }

    if (id == 3) {//turn water off for this plot
        particleSystems[i].on = 0;
        growing4 = 0;
    }

    if (id == 4) {//remove the plants
        menue_plots[i].age = 0;
    }
    glutPostRedisplay();
}

// Routine to make the menu.
void makeMenu(void)
{
    // The sub-menu is created first (because it should be visible when the top
    // menu is created): its callback function is registered and menu entries added.

    /*int plant1_sub_menu;
    plant1_sub_menu = glutCreateMenu(plot1_menu);
    glutAddMenuEntry("Plant", 1);
    glutAddMenuEntry("Water", 2);
    glutAddMenuEntry("Remove", 3);*/

    /*int plant2_sub_menu;
    plant2_sub_menu = glutCreateMenu(plot1_menu);
    glutAddMenuEntry("Plant", 1);
    glutAddMenuEntry("Water", 2);
    glutAddMenuEntry("Remove", 3);

    int plant3_sub_menu;
    plant3_sub_menu = glutCreateMenu(plot1_menu);
    glutAddMenuEntry("Plant", 1);
    glutAddMenuEntry("Water", 2);
    glutAddMenuEntry("Remove", 3);*/

    int plot1_sub_menu;
    plot1_sub_menu = glutCreateMenu(plot1_menu);
    glutAddMenuEntry("Plant", 1);
    glutAddMenuEntry("Water On", 2);
    glutAddMenuEntry("Water Off", 3);
    glutAddMenuEntry("Remove", 4);

    /*vector<plant> plants1 = menue_plots[0].plants;
    for (int i = 1; i < plants1.size(); i++) {//sub menues for each plant
        //glutAddMenuEntry("Plant " + i, i);

        cout << "size " << plants1.size() << endl;

        int plant_sub_menu;
        plant_sub_menu = glutCreateMenu(plant_menu);
        glutAddMenuEntry("Plant", 1);
        glutAddMenuEntry("Water", 2);
        glutAddMenuEntry("Water", 3);
        
        glutAddSubMenu("Plant " + i, plant_sub_menu);


    }*/

    //glutAddSubMenu("Plant 1", plant1_sub_menu);
    //glutAddSubMenu("Plant 2", plant2_sub_menu);
    //glutAddSubMenu("Plant 3", plant3_sub_menu);


    int plot2_sub_menu;
    plot2_sub_menu = glutCreateMenu(plot2_menu);
    glutAddMenuEntry("Plant", 1);
    glutAddMenuEntry("Water On", 2);
    glutAddMenuEntry("Water Off", 3);
    glutAddMenuEntry("Remove", 4);

    int plot3_sub_menu;
    plot3_sub_menu = glutCreateMenu(plot3_menu);
    glutAddMenuEntry("Plant", 1);
    glutAddMenuEntry("Water On", 2);
    glutAddMenuEntry("Water Off", 3);
    glutAddMenuEntry("Remove", 4);

    int plot4_sub_menu;
    plot4_sub_menu = glutCreateMenu(plot4_menu);
    glutAddMenuEntry("Plant", 1);
    glutAddMenuEntry("Water On", 2);
    glutAddMenuEntry("Water Off", 3);
    glutAddMenuEntry("Remove", 4);

    // The top menu is created: its callback function is registered and menu entries,
    // including a submenu, added.
    glutCreateMenu(top_menu);
    glutAddSubMenu("Plot 1", plot1_sub_menu);
    glutAddSubMenu("Plot 2", plot2_sub_menu);
    glutAddSubMenu("Plot 3", plot3_sub_menu);
    glutAddSubMenu("Plot 4", plot4_sub_menu);
    //glutAddMenuEntry("Quit", 1);

    // The menu is attached to a mouse button.
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}


void setProjection() {

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    gluPerspective(120.0, width / height, 1.0, 5000);

    glMatrixMode(GL_MODELVIEW);

}

void setProjection2() {

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    //gluPerspective(120.0, width / height, 1.0, 3000);
    glOrtho(-100, 100, -100, 100, 1.0, 200.0);

    glMatrixMode(GL_MODELVIEW);

}

// OpenGL window reshape routine.
void resize(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    width = w;
    height = h;
    setProjection();
}

void resize2(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    width = w;
    height = h;
    setProjection2();
}

void specialKeyInput(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_UP: //forward
        meZ = meZ + stepsize * cos(angle * PI / 180);
        meX = meX + stepsize * sin(angle * PI / 180);
        break;
    case GLUT_KEY_DOWN: //back
        meZ = meZ - stepsize * cos(angle * PI / 180);
        meX = meX - stepsize * sin(angle * PI / 180);
        break;
    case GLUT_KEY_RIGHT: //turn right
        angle -= turnsize;
        //if (angle <= 0) { angle = 0; }
        break;
    case GLUT_KEY_LEFT: //turn left
        angle += turnsize;
        //if (angle >= 360) { angle = 360; }
        break;
    }//end switch

    glutSetWindow(mapWin);
    glutPostRedisplay();
    glutSetWindow(mainWin);
    glutPostRedisplay();
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
        exit(0);
        break;
    case 'd':
        day = 1;
        night = 0;
        //glutPostRedisplay();
        break;
    case 'n':
        day = 0;
        night = 1;
       // glutPostRedisplay();
        break;
    case '1':
        dome1LightOn = !dome1LightOn;
        //glutPostRedisplay();
    case '2':
        dome2LightOn = !dome2LightOn;
        //glutPostRedisplay();
        break;
    case '3':
        rocketSpotLightOn = !rocketSpotLightOn;
        //glutPostRedisplay();
        break;
    case ' '://move up
        meY += 5;
        if (meY >= 1000) { meY = 1000; }
        //glutPostRedisplay();
        break;
    case 'v'://move down
        meY -= 2.5;
        if (meY <= 15) { meY = 15; }
        //glutPostRedisplay();
        break;
    case 'r'://move up
        if (board == 1) {//if on rocket, launch it
            launch = 1;
        }
        else {
            board = 1;
            meX = rX;
            meZ = rZ;
        }
        rocketLaunch();
        break;
    default:
        break;
    }
    glutSetWindow(mapWin);
    glutPostRedisplay();
    glutSetWindow(mainWin);
    glutPostRedisplay();
}

// Mouse motion callback routine.
void mousePassiveMotion(int x, int y)
{
    mouseX = x;
    mouseY = y;
   // cout << "x " << x << "y " << y << endl;

    glutSetWindow(mapWin);
    glutPostRedisplay();
    glutSetWindow(mainWin);
    glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
    cout << "Interaction:" << endl;
    cout << "Press +/- to increase/decrease the number of vertices on the circle." << endl;
    cout << "d - day" << endl;
    cout <<   "n - night" << endl;
            cout << "  1 - dome 1 lights" << endl;
            cout << "   2 - dome 2 lights" << endl;
            cout << "  3 - rocket spotlight" << endl;
            cout << "  " " - float" << endl;
            cout << "  'v' - sink / move down" << endl;
            cout << "  r - get in rocket, press r again to launch rocket" << endl;
            cout << "  up arrow key - move forward" << endl;
            cout << "  down arrow - move back" << endl;
            cout << "  left arrow - turn left" << endl;
            cout << "  right arrow - turn right" << endl;
            cout << "right click to plant, water, or remove plants" << endl;
       cout << " move mouse up or down to look up or down" << endl;

}

// Main routine.
int main(int argc, char** argv)
{
    printInteraction();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    
    //main window
    glutInitWindowSize(750, 750);
    glutInitWindowPosition(700, 50);
    mainWin = glutCreateWindow("Space Farm");
    setup();
    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutSpecialFunc(specialKeyInput);
    glutKeyboardFunc(keyInput);
    glutPassiveMotionFunc(mousePassiveMotion);
    makeMenu();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //map window
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(50, 100);
    mapWin = glutCreateWindow("Space Farm Map");
    setup2();
    glutDisplayFunc(drawScene2);
    glutReshapeFunc(resize2);
    glutSpecialFunc(specialKeyInput);
    glutKeyboardFunc(keyInput);
    makeMenu();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutTimerFunc(5, animate, 1);
    glutIdleFunc(idle_func);
    
    
    glutMainLoop();

    return 0;
}
