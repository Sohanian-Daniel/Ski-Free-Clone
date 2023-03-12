#pragma once

#include <iostream>
#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include <lab_m1/Ski/Ski.h>
#include <lab_m1/Ski/LightSource.h>

enum class ObjectType { tree, pole, present, rock, speedPresent };

class GameObject {
public:
    int id;
    glm::vec3 position;
    glm::vec3 size;
    ObjectType type;

    GameObject(int id, glm::vec3 position, glm::vec3 size, ObjectType type) : id(id), position(position), size(size), type(type) {
    }


    
};