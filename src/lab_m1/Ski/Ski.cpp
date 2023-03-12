#include "lab_m1/Ski/Ski.h"

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

using namespace std;
using namespace m1;

#define PLAYER_SPEED 5
#define ALIVE 0
#define DEAD 1
#define LIVES 3

std::string pad_right(std::string const& str, size_t s)
{
    if (str.size() < s)
        return str + std::string(s - str.size(), ' ');
    else
        return str;
}

std::string pad_left(std::string const& str, size_t s)
{
    if (str.size() < s)
        return std::string(s - str.size(), ' ') + str;
    else
        return str;
}

std::string center(const std::string str, const int padding) {
    std::stringstream ss;
    int length = str.length();
    int total_padding = padding - length;
    int left_padding = total_padding / 2;
    ss << std::setw(left_padding + length) << std::setfill(' ') << str;
    return ss.str();
}

bool squareWave(float x, float frequency) {
    float period = 1.0 / frequency;
    float t = fmod(x, period) / period;
    return t < 0.5;
}


Ski::Ski()
{
    
}


Ski::~Ski()
{
}


void Ski::Init()
{
    const string sourceTextureDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Ski", "textures");
    const string sourceModelsDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Ski", "models");
    {
        // Load textures
        {
            Texture2D* texture = new Texture2D();
            texture->Load2D(PATH_JOIN(sourceTextureDir, "snow.png").c_str(), GL_MIRRORED_REPEAT);
            mapTextures["snow"] = texture;
        }

        {
            Texture2D* texture = new Texture2D();
            texture->Load2D(PATH_JOIN(sourceTextureDir, "speed.png").c_str(), GL_MIRRORED_REPEAT);
            mapTextures["speed"] = texture;
        }

        {
            Texture2D* texture = new Texture2D();
            texture->Load2D(PATH_JOIN(sourceTextureDir, "stone.png").c_str(), GL_REPEAT);
            mapTextures["stone"] = texture;
        }

        {
            Texture2D* texture = new Texture2D();
            texture->Load2D(PATH_JOIN(sourceTextureDir, "grass.png").c_str(), GL_REPEAT);
            mapTextures["grass"] = texture;
        }

        {
            Texture2D* texture = new Texture2D();
            texture->Load2D(PATH_JOIN(sourceTextureDir, "bark.png").c_str(), GL_REPEAT);
            texture->SetWrappingMode(GL_REPEAT);
            mapTextures["bark"] = texture;
        }

        {
            Texture2D* texture = new Texture2D();
            texture->Load2D(PATH_JOIN(sourceTextureDir, "steel.png").c_str(), GL_REPEAT);
            mapTextures["steel"] = texture;
        }

        {
            Texture2D* texture = new Texture2D();
            texture->Load2D(PATH_JOIN(sourceTextureDir, "present.png").c_str(), GL_REPEAT);
            mapTextures["present"] = texture;
        }

        {
            Texture2D* texture = new Texture2D();
            texture->Load2D(PATH_JOIN(sourceTextureDir, "troll.png").c_str(), GL_REPEAT);
            mapTextures["troll"] = texture;
        }

    }
    {
        // Load meshes
        {
            Mesh* mesh = new Mesh("box");
            mesh->LoadMesh(PATH_JOIN(sourceModelsDir), "box.obj");
            meshes[mesh->GetMeshID()] = mesh;
        }

        {
            Mesh* mesh = new Mesh("plane");
            mesh->LoadMesh(PATH_JOIN(sourceModelsDir), "plane50.obj");
            meshes[mesh->GetMeshID()] = mesh;
        }

        {
            Mesh* mesh = new Mesh("quad");
            mesh->LoadMesh(PATH_JOIN(sourceModelsDir), "quad.obj");
            meshes[mesh->GetMeshID()] = mesh;
        }

        {
            Mesh* mesh = new Mesh("cone");
            mesh->LoadMesh(PATH_JOIN(sourceModelsDir), "cone.obj");
            meshes[mesh->GetMeshID()] = mesh;
        }

        {
            Mesh* mesh = new Mesh("sphere");
            mesh->LoadMesh(PATH_JOIN(sourceModelsDir), "sphere.obj");
            meshes[mesh->GetMeshID()] = mesh;
        }
    }


    // Create a shader program for drawing face polygon with the color of the normal
    {
        Shader *shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Ski", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Ski", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Load Fonts
    tr = new gfxc::TextRenderer(window->props.selfDir, window->GetResolution().x, window->GetResolution().y);
    tr->Load(window->props.selfDir + "\\assets\\fonts\\Hack-Bold.ttf", 128);

    // Particle Generator
    particleGen = new ParticleGenerator(*mapTextures["snow"], (unsigned int)1000);

    def.shininess = 30;
    def.diffuse = 0.5;
    def.specular = 0.5;
    def.ambient = 0.2;

    for (int i = 0; i < 101; i++) {
        lights.type[i] = 0;
        lights.color[i] = glm::vec3(0);
        lights.direction[i] = glm::vec3(0);
        lights.position[i] = glm::vec3(0);
    }

    lights.setLight(100, LIGHT_DIRECTIONAL, glm::vec3(0), glm::vec3(2), glm::normalize(glm::vec3(0, 0, -1)));

    player = new Player(glm::vec3(0, 0, 0), glm::vec3(0.5, 0.6, 0.5), PLAYER_SPEED);
    score = 0;
    state = ALIVE;
    lives = LIVES;

    GetSceneCamera()->m_transform->SetWorldRotation(glm::vec3(-20, 0, 0));
    GetSceneCamera()->m_transform->SetWorldPosition(player->position + camera_offset);
    GetSceneCamera()->Update();
}


void Ski::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void m1::Ski::RenderTree(glm::vec3 position)
{
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, position + glm::vec3(0, 1.0f, 0));
        glm::mat4 trunkMatrix = glm::scale(modelMatrix, glm::vec3(0.33f, 2.0f, 0.33f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], trunkMatrix, mapTextures["bark"]);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1.0f, 0));
        RenderSimpleMesh(meshes["cone"], shaders["LabShader"], modelMatrix, mapTextures["grass"]);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.8f, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.75f));
        RenderSimpleMesh(meshes["cone"], shaders["LabShader"], modelMatrix, mapTextures["grass"]);
    }
}

void m1::Ski::RenderSpeedPresent(glm::vec3& position)
{
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.25f, 0) + position);
        modelMatrix = glm::rotate(modelMatrix, RADIANS(30), glm::vec3(1, 0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, mapTextures["speed"]);
    }
}

void m1::Ski::RenderPresent(glm::vec3& position)
{
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.25f, 0) + position);
        modelMatrix = glm::rotate(modelMatrix, RADIANS(30), glm::vec3(1, 0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, mapTextures["present"]);
    }
}

void m1::Ski::RenderRock(glm::vec3 position)
{
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, position);
        glm::mat4 rock1 = glm::scale(modelMatrix, glm::vec3(0.44f));
        RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], rock1, mapTextures["stone"]);

        glm::mat4 rock2 = glm::translate(modelMatrix, glm::vec3(-0.25f, 0.f, 0.0f));
        rock2 = glm::scale(rock2, glm::vec3(0.5f));
        RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], rock2, mapTextures["stone"]);

        glm::mat4 rock3 = glm::translate(modelMatrix, glm::vec3(0.3f, 0.f, 0.0f));
        rock3 = glm::scale(rock3, glm::vec3(0.66f));
        RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], rock3, mapTextures["stone"]);
    }
}

void m1::Ski::RenderPole(glm::vec3& position)
{
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 2.0f, 0) + position);
        glm::mat4 pole = glm::scale(modelMatrix, glm::vec3(0.17f, 4.0f, 0.17f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], pole, mapTextures["steel"]);

        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1.875f, 0));
        glm::mat4 bar = glm::scale(modelMatrix, glm::vec3(2.0f, 0.25f, 0.25f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], bar, mapTextures["steel"]);
    }
}

void m1::Ski::RenderPlayer(glm::vec3& position, float angle)
{
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.3f, 0) + position);
        modelMatrix = glm::rotate(modelMatrix, RADIANS(30), glm::vec3(1, 0, 0));
        modelMatrix = glm::rotate(modelMatrix, angle,       glm::vec3(0, 1, 0));
        glm::mat4 playerMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.6f, 0.5f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], playerMatrix, mapTextures["troll"]);

        glm::mat4 ski1 = glm::translate(modelMatrix, glm::vec3(0.2f, -0.3f, 0.f));
        ski1 = glm::rotate(ski1, RADIANS(90), glm::vec3(1, 0, 0));
        ski1 = glm::scale(ski1, glm::vec3(0.1f, 1.0f, 0.35f));
        RenderSimpleMesh(meshes["quad"], shaders["LabShader"], ski1, mapTextures["stone"]);

        glm::mat4 ski2 = glm::translate(modelMatrix, glm::vec3(-0.2f, -0.3f, 0.f));
        ski2 = glm::rotate(ski2, RADIANS(90), glm::vec3(1, 0, 0));
        ski2 = glm::scale(ski2, glm::vec3(0.1f, 1.0f, 0.35f));
        RenderSimpleMesh(meshes["quad"], shaders["LabShader"], ski2, mapTextures["stone"]);

    }

    
}

void Ski::RestartGame() {
    player->position = glm::vec3(0, 0, 0);
    objManager->despawnAll();
    score = 0;
    boostTimer = 0;
    hitTimer = 0;
    state = ALIVE;
    lives = LIVES;
}

void m1::Ski::RenderParticles()
{
    // Render Particles
    for (auto& particle : particleGen->particles) {
        if (particle.Life > 0.0f) {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, particle.Position);

            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));

            RenderParticle(shaders["LabShader"], modelMatrix, &particleGen->texture);
        }
    }
}

void m1::Ski::RenderPlane(glm::vec3 &position)
{
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, position + glm::vec3(0, -0.1f, 0));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(30), glm::vec3(1, 0, 0));
        RenderSimpleMesh(meshes["plane"], shaders["LabShader"], modelMatrix, mapTextures["snow"], 1);
    }
}

void Ski::Update(float deltaTimeSeconds)
{
    // Get Cursor Info
    glm::ivec2 cursor_int = window->GetCursorPosition();
    glm::ivec2 resolution = window->GetResolution();
    glm::vec2 centre = glm::vec2((float)(resolution.x) / 2, (float)(resolution.y) * 0.394f);
    glm::vec2 dir = -glm::normalize(centre - glm::vec2((float)cursor_int.x, (float)cursor_int.y));
    
    bool movement = false;
    // If the player is alive
    if (state == ALIVE) {
        // Update Angle
        player->angle = acosf(glm::dot(glm::normalize(glm::vec3(0, 0, -1)), normalize(glm::vec3(dir.x, 0, dir.y)))) * ((dir.x < 0) ? 1 : -1);
        if (player->angle < RADIANS(135) && player->angle > RADIANS(-135)) {
            player->angle = RADIANS(135) * ((dir.x < 0) ? 1 : -1);

            glm::vec4 angleVec = glm::vec4(0, 0, 1, 1) * glm::rotate(glm::mat4(1), player->angle, glm::vec3(0, 1, 0));
            dir = glm::vec2(angleVec.x, -angleVec.z);
        }

        // Update Position
        if (dir.y > 0) {
            float y_movement = -tan(RADIANS(30)) * dir.y;
            glm::vec3 delta = glm::vec3(dir.x, y_movement, dir.y) * player->speed * deltaTimeSeconds;
            player->position += delta;

            movement = true;
        }

        // Update Player Speed based on Distance. Apply Boost Effect.
        deltaDistance = distance(glm::vec3(0, 0, 0), glm::vec3(0, player->position.y, player->position.z));
        boostTimer = (boostTimer > 0) ? boostTimer - deltaTimeSeconds : 0;
        hitTimer = (hitTimer > 0) ? hitTimer - deltaTimeSeconds : 0;
        player->speed = PLAYER_SPEED + pow(deltaDistance + 1, 0.33) + ((boostTimer > 0) ? 10 : 0);
        player->speed *= ((hitTimer > 0) ? 0.5 : 1);
        
    }

    // Update particles
    auto forward = glm::vec3(dir.x, 0, dir.y);
    particleGen->Update(deltaTimeSeconds, player->position - 0.33f * forward, forward, -player->speed, (movement) ? 3 : 0);
    
    // Debug to show camera
    if (!window->KeyHold(GLFW_KEY_T)) {
        auto* camera = GetSceneCamera();
        camera->m_transform->SetWorldPosition(player->position + camera_offset);
        GetSceneCamera()->m_transform->SetWorldRotation(glm::vec3(-20, 0, 0));
        camera->Update();
    }
    

    // Render Objects
    bool render = true;
    if (hitTimer > 0) {
        render = squareWave(hitTimer, 2);
    }
    if (render) RenderPlayer(player->position, player->angle);
    RenderPlane(player->position);
    RenderParticles();
    for (int i = 0; i < objManager->size; i++) {
        const auto& object = objManager->objects[i];
        if (object != nullptr) {
            switch (object->type) {
                case ObjectType::present: RenderPresent(object->position);  break;
                case ObjectType::pole: RenderPole(object->position); break;
                case ObjectType::rock: RenderRock(object->position); break;
                case ObjectType::tree: RenderTree(object->position); break;
                case ObjectType::speedPresent: RenderSpeedPresent(object->position); break;
            }
            
            // Handle Collisions
            if (player->checkColision(*object)) {
                if (object->type == ObjectType::present) {
                    objManager->despawnObject(object);
                    score += 500;
                }
                else if (object->type == ObjectType::speedPresent) {
                    objManager->despawnObject(object);
                    boostTimer = 3;
                    score += 5000;
                }
                else {
                    lives--;
                    if (lives == 0) {
                        lives = 0;
                        state = DEAD;
                    }
                    else {
                        hitTimer = 3;
                        objManager->despawnObject(object);
                    }
                }
            }
        }
    }
    
    // Spawn/Despawn objects
    objManager->checkDespawn(player->position);
    objManager->spawnRandomObjects(player->position, dir.y * player->speed * deltaTimeSeconds, glm::vec3(dir.x, 0, dir.y));

    

    // Render Text
    glClear(GL_DEPTH_BUFFER_BIT);
    tr->RenderText("Score: " + pad_right(to_string((int)score / 10), 2), resolution.x * 0.025f, resolution.y * 0.05f, 0.3f, glm::vec3(0, 0, 0));
    tr->RenderText("Lives: " + to_string(lives), resolution.x * 0.025f, resolution.y * 0.10f, 0.3f, glm::vec3(0, 0, 0));
    tr->RenderText("FPS: " + to_string(1/deltaTimeSeconds), resolution.x * 0.025f, resolution.y * 0.15f, 0.3f, glm::vec3(0, 0, 0));

    string str = to_string((int)deltaDistance);
    tr->RenderText(center(str + " m", 18), resolution.x * 0.26f, resolution.y * 0.05f, 0.5f, glm::vec3(0, 0, 0));

    if (boostTimer > 0) {
        std::stringstream stream;
        stream << std::fixed << std::setprecision(2) << boostTimer;
        std::string s = stream.str();

        tr->RenderText("Extra Speed! " + s, resolution.x * 0.35f, resolution.y * 0.89f, 0.3f, glm::vec3(1, 1, 0));
    }

    if (state == DEAD) {
        lives = 0;
        tr->RenderText("You died!", resolution.x * 0.36f, resolution.y * 0.35f, 0.5f, glm::vec3(0.8, 0.2, 0.2));
        tr->RenderText("Press space to retry.", resolution.x * 0.20f, resolution.y * 0.45f, 0.5f, glm::vec3(0.8, 0.2, 0.2));
        
        if (window->KeyHold(GLFW_KEY_SPACE)) {
            RestartGame();
        }
    }

    
}




void Ski::FrameEnd()
{
    glFlush();
}


void Ski::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture, int isPlane)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    
    // Update lights on object spawn
    glUseProgram(shader->program);


    // Send Light info
    glUniform3fv(glGetUniformLocation(shader->program, "light_position"), LIGHT_MAX, glm::value_ptr(lights.position[0]));
    glUniform3fv(glGetUniformLocation(shader->program, "light_color"), LIGHT_MAX, glm::value_ptr(lights.color[0]));
    glUniform3fv(glGetUniformLocation(shader->program, "light_direction"), LIGHT_MAX, glm::value_ptr(lights.direction[0]));
    glUniform1iv(glGetUniformLocation(shader->program, "light_type"), LIGHT_MAX, lights.type);

    // Set eye position (camera position) uniform
    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
    int eye_position = glGetUniformLocation(shader->program, "eye_position");
    glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

    // Set material property uniforms (shininess, kd, ks, object color) 
    int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(material_shininess, def.shininess);

    int material_kd = glGetUniformLocation(shader->program, "material_kd");
    glUniform1f(material_kd, def.diffuse);

    int material_ks = glGetUniformLocation(shader->program, "material_ks");
    glUniform1f(material_ks, def.specular);

    int material_ka = glGetUniformLocation(shader->program, "material_ka");
    glUniform1f(material_ka, def.ambient);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    float time = (float) Engine::GetElapsedTime();
    int loc = glGetUniformLocation(shader->program, "time");
    glUniform1f(loc, time);

    loc = glGetUniformLocation(shader->program, "isPlane");
    glUniform1i(loc, isPlane);

    if (texture)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture"), 0);
    }
   

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Ski::RenderParticle(Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture)
{
    if (!shader || !shader->program)
        return;

    shader->Use();

    int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(material_shininess, def.shininess);

    int material_kd = glGetUniformLocation(shader->program, "material_kd");
    glUniform1f(material_kd, 10);

    int material_ks = glGetUniformLocation(shader->program, "material_ks");
    glUniform1f(material_ks, 10);

    int material_ka = glGetUniformLocation(shader->program, "material_ka");
    glUniform1f(material_ka, 10);


    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
    glUniform1i(glGetUniformLocation(shader->program, "sprite"), 0);

    glBindVertexArray(meshes["box"]->GetBuffers()->m_VAO);
    glDrawElements(meshes["box"]->GetDrawMode(), static_cast<int>(meshes["box"]->indices.size()), GL_UNSIGNED_INT, 0);
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Ski::OnInputUpdate(float deltaTime, int mods)
{
    if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        glm::vec3 up = glm::vec3(0, 1, 0);
        glm::vec3 right = GetSceneCamera()->m_transform->GetLocalOXVector();
        glm::vec3 forward = GetSceneCamera()->m_transform->GetLocalOZVector();
        forward = glm::normalize(glm::vec3(forward.x, 0, forward.z));
    }
}


void Ski::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_B) {
        cout << GetSceneCamera()->m_transform->GetWorldPosition() << "\n";
        cout << GetSceneCamera()->m_transform->GetRotationEuler360() << "\n";

        cout << "Player Pos: " << player->position << endl;
    }

    if (window->KeyHold(GLFW_KEY_R)) {
        RestartGame();
    }
}


void Ski::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Ski::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Ski::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Ski::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Ski::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Ski::OnWindowResize(int width, int height)
{
}
