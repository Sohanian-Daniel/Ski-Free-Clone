#version 330

// Input
in vec3 normal;
in vec3 color;
in vec2 texcoord;

in vec3 world_position;
in vec3 world_normal;

// Uniform properties
uniform sampler2D texture;
uniform float time;
uniform int isPlane;

// Light Sources
#define MAX 101
#define PER_OBJ 5

// uniform light_source lights[MAX];

uniform int  light_type[MAX];
uniform vec3 light_position[MAX];
uniform vec3 light_color[MAX];
uniform vec3 light_direction[MAX];



// Eye Position
uniform vec3 eye_position;

// Material uniforms
uniform float material_ka; // ambient
uniform float material_kd; // diffuse
uniform float material_ks; // specular

uniform int material_shininess;

// Output
layout(location = 0) out vec4 out_color;

vec3 rainbow[] = vec3[](vec3(1, 0, 0), vec3(1, 0.5, 0), vec3(1, 1, 0), vec3(0.5, 1, 0), vec3(0, 1, 0), vec3(0, 1, 0.5), vec3(0, 0.5, 1), vec3(0, 0, 1), vec3(0.5, 0, 0.5), vec3(1, 0, 1));

vec3 point_light(vec3 light_pos, vec3 light_color, vec3 object_color)
{
	vec3 color;
    vec3 light_col = light_color;
	
    vec3 L = normalize( light_pos - world_position );
    vec3 V = normalize( eye_position - world_position );
    vec3 H = normalize( L + V );
    vec3 R = reflect (-L, world_normal);

    float ambient_light = material_ka;
    vec3  diffuse_light = material_kd * light_col * max(dot(world_normal, L), 0);
    vec3  specular_light = material_ks * light_col * ((dot(world_normal, L) > 0) ? 1 : 0) * pow(max(dot(V, R), 0), material_shininess);
    float dist = distance(world_position, light_pos);

    color = object_color * ( ambient_light * light_col + diffuse_light + specular_light) * (1.0 / pow(dist, 2));

	return color;
}

vec3 rainbow_light(vec3 light_pos, vec3 light_color, vec3 object_color)
{
	vec3 color;

    float t = time * 0.2;
    float index = fract(t) * rainbow.length();
    int index1 = int(index);
    int index2 = (index1 + 1) % int(rainbow.length());
    float interpolation = smoothstep(0.0, 1.0, fract(index));
    vec3 light_col = mix(rainbow[index1], rainbow[index2], interpolation) * light_color;
	
    vec3 L = normalize( light_pos - world_position );
    vec3 V = normalize( eye_position - world_position );
    vec3 H = normalize( L + V );
    vec3 R = reflect (-L, world_normal);

    float ambient_light = material_ka;
    vec3  diffuse_light = material_kd * light_col * max(dot(world_normal, L), 0);
    vec3  specular_light = material_ks * light_col * ((dot(world_normal, L) > 0) ? 1 : 0) * pow(max(dot(V, R), 0), material_shininess);
    float dist = distance(world_position, light_pos);

    color = object_color * ( ambient_light * light_col + diffuse_light + specular_light) * (1.0 / pow(dist, 2));

	return color;
}

vec3 spot_light(vec3 light_pos, vec3 light_color, vec3 light_dir, vec3 object_color)
{
    vec3 color;

    vec3 L = normalize( light_pos - world_position );
    vec3 V = normalize( eye_position - world_position );
    vec3 H = normalize( L + V );
    vec3 R = reflect (-L, world_normal);

    float ambient_light = material_ka;
    vec3 diffuse_light = material_kd * light_color * max(dot(world_normal, L), 0);
    vec3 specular_light = material_ks * light_color * ((dot(world_normal, L) > 0) ? 1 : 0) * pow(max(dot(V, R), 0), material_shininess); 
    float dist = distance(world_position, light_pos);

    float cut_off = radians(30.0);
    float spot_light = dot(-L, light_dir);
    if (spot_light > cos(cut_off))
    {
        float spot_light_limit = cos(cut_off);
 
        // Quadratic attenuation
        float linear_att = 2 * (spot_light - spot_light_limit) / (1.0f - spot_light_limit);
        float light_att_factor = pow(linear_att, 0.5);
	    color = object_color * light_att_factor * ( ambient_light * light_color + diffuse_light + specular_light) * (1.0 / pow(dist, 1));
    } else {
        color = vec3(0);
    }

    return color;
}

vec3 directional_light(vec3 light_color, vec3 light_dir, vec3 object_color)
{
    vec3 color;

    vec3 L = normalize(-light_dir);
    vec3 V = normalize( eye_position - world_position );
    vec3 H = normalize( L + V );
    vec3 R = reflect (L, world_normal);

    float ambient_light = material_ka;
    vec3  diffuse_light = material_kd * light_color * max(dot(world_normal, L), 0); // * culoare lumina
    vec3  specular_light = material_ks * light_color * ((dot(world_normal, L) > 0) ? 1 : 0) * pow(max(dot(V, R), 0), material_shininess); // * culoare lumina

    color = object_color * ( ambient_light + diffuse_light + specular_light); // TODO

    return color;
}

void main()
{
    vec4 tex_color;
    if(isPlane == 1) {
        tex_color = texture2D(texture, texcoord  + world_position.xz * 0.01 );
    } else {
        tex_color = texture2D(texture, texcoord);
    }
    
    //    if (tex_color.a < 0.5) {
    //        discard;
    //    }

    vec3 color;
    for (int i = 0; i < 101; i++) {
        vec3 partial_color = vec3(0);
        if (light_type[i] == 1) {
            partial_color = spot_light(light_position[i], light_color[i], light_direction[i], tex_color.rgb);
        } else if (light_type[i] == 2) {
            partial_color = point_light(light_position[i], light_color[i], tex_color.rgb);
        } else if (light_type[i] == 3) {
            partial_color = directional_light(light_color[i], light_direction[i], tex_color.rgb);
        } else if (light_type[i] == 4) {
            partial_color = rainbow_light(light_position[i], light_color[i], tex_color.rgb);
        }
        color = color + partial_color;
    }


    out_color = vec4(color, tex_color.a);

}
