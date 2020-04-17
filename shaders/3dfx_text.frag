#version 330 core
out vec4 FragColor;

uniform sampler2D ourTexture;

void main()
{
    FragColor = vec4(0.439216, 0.858824, 0.858824, 1.0);
}