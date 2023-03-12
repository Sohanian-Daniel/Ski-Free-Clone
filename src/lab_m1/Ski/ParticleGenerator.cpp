/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "ParticleGenerator.h"

ParticleGenerator::ParticleGenerator(Texture2D texture, unsigned int amount)
    : texture(texture), amount(amount)
{
    this->init();
}

void ParticleGenerator::Update(float dt, glm::vec3 position, glm::vec3 direction, float speed, unsigned int newParticles, glm::vec3 offset)
{
    // add new particles 
    for (unsigned int i = 0; i < newParticles; ++i)
    {
        int unusedParticle = this->firstUnusedParticle();
        this->respawnParticle(this->particles[unusedParticle], position, direction, speed, offset);
    }
    // update all particles
    for (unsigned int i = 0; i < this->amount; ++i)
    {
        Particle& p = this->particles[i];
        p.Life -= dt; // reduce life
        if (p.Life > 0.0f)
        {	// particle is alive, thus update
            p.Position -= (p.Velocity + glm::vec3(0, -1.0f, 0) * (rand() % 100 / 150.0f)) * dt;
            p.Color.a -= dt * 2.5f;
        }
    }
}



void ParticleGenerator::init()
{
    // create this->amount default particle instances
    for (unsigned int i = 0; i < this->amount; ++i)
        this->particles.push_back(Particle());
}

// stores the index of the last particle used (for quick access to next dead particle)
unsigned int lastUsedParticle = 0;
unsigned int ParticleGenerator::firstUnusedParticle()
{
    // first search from last used particle, this will usually return almost instantly
    for (unsigned int i = lastUsedParticle; i < this->amount; ++i) {
        if (this->particles[i].Life <= 0.0f) {
            lastUsedParticle = i;
            return i;
        }
    }
    // otherwise, do a linear search
    for (unsigned int i = 0; i < lastUsedParticle; ++i) {
        if (this->particles[i].Life <= 0.0f) {
            lastUsedParticle = i;
            return i;
        }
    }
    // all particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
    lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator::respawnParticle(Particle& particle, glm::vec3 position, glm::vec3 direction, float speed, glm::vec3 offset)
{
    float random = ((rand() % 100) - 50) / 400.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    particle.Position = position + (random + offset) * glm::cross(direction, glm::vec3(0,1.0f,0));
    particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.Life = 1.0f;
    particle.Velocity = direction * speed * 0.1f;
}