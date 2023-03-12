#pragma once


#include "utils/glm_utils.h"
#include "utils/math_utils.h"

class Player {
public:
	glm::vec3 position;
    glm::vec3 size;
	float speed;
    float angle;

	Player(glm::vec3 position, glm::vec3 size, float speed) : position(position), size(size), speed(speed) {};

    bool checkColision(const GameObject& two) // AABB - AABB collision
    {
        bool collisionX = this->position.x + this->size.x >= two.position.x &&
            two.position.x + two.size.x >= this->position.x;
        bool collisionY = this->position.y + this->size.y >= two.position.y &&
            two.position.y + two.size.y >= this->position.y;
        bool collisionZ = this->position.z + this->size.z >= two.position.z &&
            two.position.z + two.size.z >= this->position.z;
        return collisionX && collisionY && collisionZ;
    }
};