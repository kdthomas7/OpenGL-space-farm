///////////////////////////////////////////////////////////////////////////////////////          
// 
// Author:  Kyra Thomas
// Environment/Compiler: Visual Studio 2019
// Date: April 15, 2020
// References:
// textures from: http://texturelib.com/, 
//
// Interactions:
//
// 
/////////////////////////////////////////////////////////////////////////////////////// 



#pragma once
#ifndef MY_CLASS_H // include guard
#define MY_CLASS_H

#define MAX_NUM_PARTICLES 1000
#define INITIAL_NUM_PARTICLES 15
#define INITIAL_POINT_SIZE 1.5
#define INITIAL_SPEED 15.0

//typedef int bool;
#define TRUE 1
#define FALSE 0

/* particle struct */

typedef struct particle
{
    int color;
    float position[3];
    float velocity[3];
    float mass;
} particle;


/*void drawRain();
void rainIdle();
void rainCollision(int);
float rainForces(int, int);

void rainInit();*/

//particle system class
class particleSys {
    /* particle struct */

   // friend struct particle;

    public:
        //boundaries of the rain particle system
        int rain_x0;
        int rain_x1;
        int rain_z0;
        int rain_z1;
        bool on;

        void drawRain();
        void rainIdle();
        void rainCollision(int);
        float rainForces(int, int);

        void rainInit();

        /* initial state of particle system */
        float ceiling = 60;//starting position of rain
        float floor = 5.9;//stopping position of rain
        int present_time;
        int last_time;
        int num_particles = INITIAL_NUM_PARTICLES;
        float point_size = INITIAL_POINT_SIZE+2;
        float speed = INITIAL_SPEED;
        bool gravity = FALSE; /* gravity off */
        bool elastic = FALSE; /* restitution off */
        bool repulsion = FALSE; /* repulsion off */
        float coef = 1.0; /* perfectly elastic collisions */
        particle particles[MAX_NUM_PARTICLES]; /* particle system */
        float d2[MAX_NUM_PARTICLES][MAX_NUM_PARTICLES]; /* array for interparticle distances */
};



#endif /* MY_CLASS_H */
