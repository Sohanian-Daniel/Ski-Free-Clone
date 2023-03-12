/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include <core/gpu/shader.h>
#include <core/gpu/texture2D.h>

// Represents a single particle and its state
struct Particle {
    glm::vec3 Position, Velocity;
    glm::vec4 Color;
    float     Life;

    Particle() : Position(glm::vec3(0.0f)), Velocity(glm::vec3(0.0f)), Color(1.0f), Life(0.0f) { }
};


// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleGenerator
{
public:
    // constructor
    ParticleGenerator(Texture2D texture, unsigned int amount);
    // update all particles
    void Update(float dt, glm::vec3 position, glm::vec3 direction, float speed, unsigned int newParticles, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
    // render all particles

    std::vector<Particle> particles;
    Texture2D texture;
protected:
    unsigned int amount;
    
    
    // initializes buffer and vertex attributes
    void init();
    // returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    unsigned int firstUnusedParticle();
    // respawns particle
    void respawnParticle(Particle& particle, glm::vec3 position, glm::vec3 direction, float speed, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
};