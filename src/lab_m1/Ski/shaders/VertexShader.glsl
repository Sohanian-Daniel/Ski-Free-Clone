#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform int isPlane;

// Output
out vec2 texcoord;

// Output values to fragment shader
out vec3 world_position;
out vec3 world_normal;


void main()
{
    // TODO(student): Pass v_texture_coord as output to fragment shader

    if (isPlane == 0) {
    texcoord = v_texture_coord;
    } else {
        texcoord = vec2(v_texture_coord.x / 2560, v_texture_coord.y /2560);
    }


    world_position = (Model * vec4(v_position,1)).xyz;
    world_normal = normalize( mat3(Model) * v_normal );

    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
