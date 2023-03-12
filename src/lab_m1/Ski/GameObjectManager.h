#pragma once

#include <iostream>
#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include <lab_m1/Ski/Ski.h>
#include <lab_m1/Ski/GameObject.h>
#include <random>

using namespace std;

#define max(a, b) (a > b) ? a : b
#define min(a, b) (a < b) ? a : b


#define LIGHTS_PER_OBJ 5
#define CAPACITY (LIGHT_MAX - 1) / LIGHTS_PER_OBJ

#define THRESHOLD 20
#define SPAWN_DISTANCE 3




class GameObjectManager {
public:
	lightSource *lights;
	std::vector<GameObject*> objects;
	int size;

	GameObjectManager(lightSource *lights) : lights(lights) {
		objects.resize(CAPACITY);
		for (int i = 0; i < objects.size(); i++) {
			objects[i] = nullptr;
		}
		size = 0;
	}

	void createPresent(glm::vec3 position) {
		int id = getFirstUnusedObject();
		GameObject* obj = new GameObject(id, position, glm::vec3(0.5f), ObjectType::present);
		objects[id] = obj;
		size = min((size + 1), objects.size());

		lights->resetLights(id * LIGHTS_PER_OBJ, LIGHTS_PER_OBJ);
		lights->setLight(id * LIGHTS_PER_OBJ, LIGHT_DOT, position + glm::vec3(0, 0.25f, 0), glm::vec3(0.2), glm::vec3(0, -1, 0));

	}

	void createSpeedPresent(glm::vec3 position) {
		int id = getFirstUnusedObject();
		GameObject* obj = new GameObject(id, position, glm::vec3(0.5f), ObjectType::speedPresent);
		objects[id] = obj;
		size = min((size + 1), objects.size());

		lights->resetLights(id * LIGHTS_PER_OBJ, LIGHTS_PER_OBJ);
		lights->setLight(id * LIGHTS_PER_OBJ, LIGHT_DOT, position + glm::vec3(0, 0.25f, 0), glm::vec3(0.2, 0, 0), glm::vec3(0, -1, 0));

	}

	void createPole(glm::vec3 position) {
		int id = getFirstUnusedObject();
		GameObject* obj = new GameObject(id, position, glm::vec3(0.17, 4, 0.17), ObjectType::pole);
		objects[id] = obj;
		size = min((size + 1), objects.size());

		lights->resetLights(id * LIGHTS_PER_OBJ, LIGHTS_PER_OBJ);
		lights->setLight(id * LIGHTS_PER_OBJ + 0, LIGHT_SPOT, position + glm::vec3(-0.7, 3.8, 0), fromRGB(255, 175, 100), glm::vec3(0, -1, 0));
		lights->setLight(id * LIGHTS_PER_OBJ + 1, LIGHT_SPOT, position + glm::vec3(0.7, 3.8, 0),  fromRGB(255, 175, 100), glm::vec3(0, -1, 0));

	}

	void createRock(glm::vec3 position) {
		int id = getFirstUnusedObject();
		GameObject* obj = new GameObject(id, position, glm::vec3(0.7, 0.5, 0.3), ObjectType::rock);
		objects[id] = obj;
		size = min((size + 1), objects.size());

	}

	void createTree(glm::vec3 position) {
		int id = getFirstUnusedObject();
		GameObject* obj = new GameObject(id, position, glm::vec3(0.33, 2, 0.33), ObjectType::tree);
		objects[id] = obj;
		size = min((size + 1), objects.size());

		lights->resetLights(id * LIGHTS_PER_OBJ, LIGHTS_PER_OBJ);
		lights->setLight(id * LIGHTS_PER_OBJ + 0, LIGHT_RAINBOW, position + glm::vec3(0, 3.5, 0),         fromRGB(255, 0, 0), glm::vec3(0, -1, 0));
		lights->setLight(id * LIGHTS_PER_OBJ + 1, LIGHT_RAINBOW, position + glm::vec3(-0.3, 1.7, 0.64),   fromRGB(185, 0, 175), glm::vec3(0, -1, 0));
		lights->setLight(id * LIGHTS_PER_OBJ + 2, LIGHT_RAINBOW, position + glm::vec3(0.85, 1.2, -0.6),   fromRGB(0, 0, 255), glm::vec3(0, -1, 0));
		lights->setLight(id * LIGHTS_PER_OBJ + 3, LIGHT_RAINBOW, position + glm::vec3(0.79, 2.27, 0.13),  fromRGB(255, 100, 0), glm::vec3(0, -1, 0));
		lights->setLight(id * LIGHTS_PER_OBJ + 4, LIGHT_RAINBOW, position + glm::vec3(-1.1, 1.2, -0.57),  fromRGB(32, 112, 104), glm::vec3(0, -1, 0));

	}

	void checkDespawn(glm::vec3 player_position) {
		for (int i = 0; i < objects.size(); i++) {
			if (objects[i] != nullptr) {
				if (distance(glm::vec3(0, 0, player_position.z), glm::vec3(0, 0, objects[i]->position.z)) > THRESHOLD) {
					lastUsedObject = i;

					auto obj = objects[i];
					delete obj;
					objects[i] = nullptr;
				}
			}
		}
	}

	void despawnObject(GameObject *obj) {
		for (int i = 0; i < objects.size(); i++) {
			if (objects[i] != nullptr) {
				if (objects[i] == obj) {
					lastUsedObject = i;
					auto obj = objects[i];
					lights->resetLights(obj->id * LIGHTS_PER_OBJ, LIGHTS_PER_OBJ);
					delete obj;
					objects[i] = nullptr;
				}
			}
		}
	}

	void despawnAll() {
		for (int i = 0; i < objects.size(); i++) {
			if (objects[i] != nullptr) {
				lastUsedObject = i;
				auto obj = objects[i];
				lights->resetLights(obj->id * LIGHTS_PER_OBJ, LIGHTS_PER_OBJ);
				delete obj;
				objects[i] = nullptr;
			}
		}
		lastUsedObject = 0;
	}

	void spawnRandomObjects(glm::vec3 player_position, float deltaZ, glm::vec3 direction) {
		totalDeltaZ += deltaZ;
		if (totalDeltaZ > SPAWN_DISTANCE) {
			totalDeltaZ = 0;
			// generate an object
			float z = 20 * direction.z + player_position.z;
			glm::vec3 pos = glm::vec3(player_position.x + randomf(-7, 7) + (15 * direction.x), player_position.y - tan(RADIANS(30)) * (20 * direction.z), z);

			switch (random<int>(0, 3)) {
				case 0: createTree(pos); break;
				case 1: createPole(pos); break;
				case 2: {
					if (chance(40)) {
						createSpeedPresent(pos);
					}
					else createPresent(pos);
					break;
				}
				case 3: createRock(pos); break;
			}
		}
	}

protected:
	float totalDeltaZ = 0;
	unsigned int lastUsedObject = 0;
	int getFirstUnusedObject() {
		for (unsigned int i = lastUsedObject; i < objects.size(); ++i) {
			if (objects[i] == nullptr) {
				lastUsedObject = i;
				return i;
			}
		}

		for (unsigned int i = 0; i < lastUsedObject; ++i) {
			if (objects[i] == nullptr) {
				lastUsedObject = i;
				return i;
			}
		}

		lastUsedObject = 0;
		delete objects[0];
		return 0;
	}
	private:
		float between(float a, float b) { return (a + (float)(rand()) / ((float)(RAND_MAX / ((b)-(a))))); };
		bool chance(float a) { return (((float)rand()) / ((float)RAND_MAX)) < ((float)(a) / 100); };

		template<typename T>
		T random(T range_from, T range_to) {
			std::random_device                  rand_dev;
			std::mt19937                        generator(rand_dev());
			std::uniform_int_distribution<T>    distr(range_from, range_to);
			return distr(generator);
		}

		float randomf(float range_from, float range_to) {
			std::random_device                  rand_dev;
			std::mt19937                        generator(rand_dev());
			std::uniform_real<float>            distr(range_from, range_to);
			return distr(generator);
		}
};

