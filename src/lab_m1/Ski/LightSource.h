#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"

#define LIGHT_NONE 0
#define LIGHT_DOT 2
#define LIGHT_SPOT 1
#define LIGHT_DIRECTIONAL 3
#define LIGHT_RAINBOW 4
#define LIGHT_MAX 101
#define fromRGB(r, g, b) {(float) r/255.0f, (float) g/255.0f, (float) b/255.0f}


class lightSource {
public:
    int  type[LIGHT_MAX];
    glm::vec3 position[LIGHT_MAX];
    glm::vec3 color[LIGHT_MAX];
    glm::vec3 direction[LIGHT_MAX];

    void resetLight(int i) {
        type[i] = 0;
        color[i] = glm::vec3(0);
        direction[i] = glm::vec3(0);
        position[i] = glm::vec3(0);
    }

    void setLight(int i, int _type, glm::vec3 pos, glm::vec3 col, glm::vec3 dir) {
        type[i] = _type;
        position[i] = pos;
        color[i] = col;
        direction[i] = dir;
    }

    void resetLights(int start, int count) {
        for (int i = start; i < start + count; i++) {
            resetLight(i);
        }
    }
};