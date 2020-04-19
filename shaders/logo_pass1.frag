#version 330 core

// In variables
in vec3 frag_vertex;                // Transformed vertex in eye space
in vec3 frag_vertex_color;          // Color of this vertex
in vec3 frag_normal;                // Translated normal
flat in int frag_material;

// Out variables
out vec4 frag_color;                // The output color of this fragment

vec3 light0_position = vec3(50.0f, 500.0f, -1700.0f);
vec3 light1_position = vec3(100.0f, -300.0f, -1000.0f);
vec3 light = vec3(-0.57735f, -0.57735f, -0.57735f);
vec3 cam_position = vec3(-10, 0, -450);
vec3 light_color = vec3(1.0, 1.0, 1.0);
float ambient_strength = 0.3;
float specularStrength = 0.6;
// Pass 1 is already the ambient based on the normals

vec3 calculate_diffuse(vec3 normal, vec3 light_position, vec3 light_col)
{
    vec3 ret;

    vec3 norm = normalize(normal);
    vec3 light_direction = normalize(light_position - frag_vertex);
    float diff = max(dot(norm, light_direction), 0.0);

    ret = diff * light_col;
    return ret;
}

void main()
{
    // Ambient lighting
    vec3 ambient = ambient_strength * light_color;
    vec3 diffuse = vec3(0, 0, 0);

    diffuse += calculate_diffuse(frag_normal, light0_position, vec3(1.0, 1.0, 1.0));

    float factor = ((light.x * frag_normal.x + light.y * frag_normal.y + light.z * frag_normal.z) + 0.4);

    // Only apply specular to the `3D`
    // This doens't work!
    if(frag_material == 0)
    {
        /**
        vec3 norm = normalize(frag_normal);
        vec3 light_direction = normalize(light0_position - frag_vertex);
        vec3 view_direction = normalize(cam_position - frag_vertex);
        vec3 reflect_direction = reflect(-light_direction, norm);

        float spec = pow(max(dot(view_direction, reflect_direction), 0.5), 32);
        vec3 specular = specularStrength * spec * vec3(1.0, 0.0, 0.0);
        **/
        diffuse += calculate_diffuse(frag_normal, light1_position, vec3(1.0, 1.0, 1.0)) * 0.5;
        frag_color = vec4((ambient + diffuse) * 0.8 * frag_vertex_color, 1.0);
    }
    else
    {
        frag_color = vec4((ambient + diffuse) * 0.7 * frag_vertex_color, 1.0);
    }
}
