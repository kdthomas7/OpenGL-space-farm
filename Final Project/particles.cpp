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
// 
/////////////////////////////////////////////////////////////////////////////////////// 


/* Particles in a box */

#define MAX_NUM_PARTICLES 1000
#define INITIAL_NUM_PARTICLES 15
#define INITIAL_POINT_SIZE 1.5
#define INITIAL_SPEED 15.0

//typedef int bool;
#define TRUE 1
#define FALSE 0

#include <stdlib.h>
#include <stdio.h>
#include "particles.h"
//#include <iostream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif



/* globals */

//int num_particles; /* number of particles */

//particle particles[MAX_NUM_PARTICLES]; /* particle system */
/*
// initial state of particle system 
float ceiling = 60;//starting position of rain
float floor = 5.4;//stopping position of rain
//boundaries of the rain particle system
/*int rain_x0 = 20;
int rain_x1 = 16;
int rain_z0 = 20;
int rain_z1 = 6;*/
/*
int present_time;
int last_time;
int num_particles = INITIAL_NUM_PARTICLES;
float point_size = INITIAL_POINT_SIZE;
float speed = INITIAL_SPEED;
bool gravity = FALSE; 
bool elastic = FALSE;
bool repulsion = FALSE; 
float coef = 1.0; /* perfectly elastic collisions */
//float d2[MAX_NUM_PARTICLES][MAX_NUM_PARTICLES]; /* array for interparticle distances */

GLsizei wh = 500, ww = 500; /* initial window size */


GLfloat colors[8][3] = { {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0},{0.0, 1.0, 0.0},
    {0.0, 0.0, 1.0}, {0.0, 1.0, 1.0}, {1.0, 0.0, 1.0}, {1.0, 1.0, 0.0},
    {1.0, 1.0, 1.0} };

void particleSys::rainInit()
{
    int  i, j;

    /* set up particles with random locations and velocities */

    for (i = 0; i < num_particles; i++)
    {
        particles[i].color = 4;//blue KDT
        particles[i].mass = 1.0;

        for (j = 0; j < 3; j++)
        {
            if (j == 1) {//for the y position and velocity KDT
                particles[i].position[j] = ceiling;
                particles[i].velocity[j] = speed * 2.0 * (rand() % 15 + 10) * .09 - 1.0; // KDT
            }
            else {//for the x and z position and velocity
                particles[i].position[0] = (rand() % (rain_x1 - rain_x0) + rain_x0);//x KDT
                particles[i].position[2] = (rand() % (rain_z1 - rain_z0) + rain_z0);//z
                //particles[i].position[j] = 2.0*((float) rand()/RAND_MAX)-1.0;
                particles[i].velocity[j] = 0;
            }
            //printf("%f, %f, %f\n", particles[i].position[0], particles[i].position[1], particles[i].position[2]);
        }
    }
    glPointSize(point_size);

}

void particleSys::rainIdle()
{
    int i, j, k;
    float dt;
    present_time = glutGet(GLUT_ELAPSED_TIME);
    dt = 0.001 * (present_time - last_time);
    for (i = 0; i < num_particles; i++)
    {
        for (j = 0; j < 3; j++)
        {
            particles[i].position[j] += dt * particles[i].velocity[j];
            particles[i].velocity[j] += dt * rainForces(i, j) / particles[i].mass;
        }
        rainCollision(i);
    }
    if (repulsion) for (i = 0; i < num_particles; i++) for (k = 0; k < i; k++)
    {
        d2[i][k] = 0.0;
        for (j = 0; j < 3; j++) d2[i][k] += (particles[i].position[j] -
            particles[k].position[j]) * (particles[i].position[j] -
                particles[k].position[j]);
        d2[k][i] = d2[i][k];
    }
    last_time = present_time;
  //  glutPostRedisplay();
}

float particleSys::rainForces(int i, int j)
{
    int k;
    float force = 0.0;
    if (gravity && j == 1) force = -1.0; /* simple gravity */
    if (repulsion) for (k = 0; k < num_particles; k++)  /* repulsive force */
    {
        if (k != i) force += 0.001 * (particles[i].position[j] - particles[k].position[j]) / (0.001 + d2[i][k]);
    }
    return(force);
}

void particleSys::rainCollision(int n)

/* tests for collisions against cube and reflect particles if necessary */

{
    int i;
    for (i = 0; i < 3; i++)
    {
        /*if (particles[n].position[i] >= 1.0)
        {
            particles[n].velocity[i] = -coef * particles[n].velocity[i];
            particles[n].position[i] = 1.0 - coef * (particles[n].position[i] - 1.0);
        }*/
        if (i == 1 && particles[n].velocity[i] > 0 && particles[n].position[i] >= 7) {//the particle has bounced and reached the max bounce height, reset particle KDT
            particles[n].position[i] = ceiling;
            particles[n].velocity[i] = -coef * particles[n].velocity[i];
        }
        if (particles[n].position[i] <= floor)
        {
            // particles[n].velocity[i] = -coef*particles[n].velocity[i];
            // particles[n].position[i] = -1.0-coef*(particles[n].position[i]+1.0);

            if (i != 1) {//set new x and z position KDT
               // particles[n].position[i] = 2.0 * ((float)rand() / RAND_MAX) - 1.0;
                particles[n].position[0] = (rand() % (rain_x1 - rain_x0) + rain_x0);//x
                particles[n].position[2] = (rand() % (rain_z1 - rain_z0) + rain_z0);//z

            }
            if (i == 1) {//bounce particle back up KDT
                //particles[n].position[i] = .9;
                particles[n].velocity[i] = -coef * particles[n].velocity[i];
            }
        }
        
    }

}


void particleSys::drawRain()
{
    int i;

   // glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POINTS); /* render all particles */
    for (i = 0; i < num_particles; i++)
    {
        glColor4f(0,.7,1,.9);
        //glColor3fv(colors[particles[i].color]);
        glVertex3fv(particles[i].position);
    }
    glEnd();
    glColor3f(0.0, 0.0, 0.0);
    glutWireCube(2.2);  /* outline of box */
    //glutSwapBuffers();
}

