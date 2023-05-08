#version 430 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D floorTexture;

void main()
{
    vec3 textureColor = texture(floorTexture, TexCoord).rgb;
    FragColor = vec4(textureColor, 1.0);
}
