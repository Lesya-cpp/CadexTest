#version 330 core
#extension GL_ARB_shading_language_420pack: enable
#extension GL_ARB_separate_shader_objects: enable

layout ( location = 0 ) in vec4 worldPos;
layout ( location = 1 ) in vec3 worldNormal;

layout ( location = 0 ) out vec4 color;

layout ( std140, binding = 0 ) uniform Camera {
    mat4 mvpMat;
    vec4 camPosition;
    vec3 scale;
};

void main(void)
{
    color = vec4(0.5*worldNormal+vec3(0.5), 1);
}
