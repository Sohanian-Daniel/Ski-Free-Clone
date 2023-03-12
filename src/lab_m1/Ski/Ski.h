#pragma once

#include <string>
#include <unordered_map>

#include "components/simple_scene.h"
#include "components/transform.h"
#include <lab_m1/Ski/GameObjectManager.h>
#include <lab_m1/Ski/Player.h>
#include <components/text_renderer.h>
#include <lab_m1/Ski/ParticleGenerator.h>



typedef struct material
{
    float ambient;
    float diffuse;
    float specular;
    unsigned int shininess;

} material;

namespace m1
{
    class Ski : public gfxc::SimpleScene
    {
     public:
        Ski();
        ~Ski();

        void Init() override;

     private:
        void FrameStart() override;
        void RenderTree(glm::vec3 position);
        void RenderSpeedPresent(glm::vec3& position);
        void RenderPresent(glm::vec3& position);
        void RenderPole(glm::vec3& position);
        void RenderPlayer(glm::vec3& position, float angle);
        void RenderParticles();
        void RenderPlane(glm::vec3 &position);
        void Update(float deltaTimeSeconds) override;
        void RenderRock(glm::vec3 position);
        void RestartGame();
        void FrameEnd() override;

        void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, Texture2D *texture = NULL, int isEarth = 0);
        void RenderParticle(Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        std::unordered_map<std::string, Texture2D *> mapTextures;

        material def;
        unsigned int materialShininess;
        float materialKd;
        float materialKs;

        lightSource lights;
        GameObjectManager *objManager = new GameObjectManager(&lights);

        glm::vec3 camera_offset = glm::vec3(0, 2, 7.25) * 1.5f;
        Player *player;

        gfxc::TextRenderer* tr;
        float score;
        int state;
        float boostTimer = 0;
        float hitTimer = 0;
        ParticleGenerator* particleGen;
        float deltaDistance;
        int lives;
        
    };
}   // namespace m1
