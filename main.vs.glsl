#version 330 core
#extension GL_ARB_shading_language_420pack: enable
#extension GL_ARB_separate_shader_objects: enable

layout ( location = 0 ) in vec4 qt_Vertex;
layout ( location = 1 ) in vec3 qt_Normal;

layout ( std140, binding = 0 ) uniform Camera {
    mat4 mvpMat;
    vec4 camPosition;
    vec3 scale;
};

layout ( location = 0 ) out vec4 worldPos;
layout ( location = 1 ) out vec3 worldNormal;

float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}

float realNoise(vec3 p) {
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * (1.0 / 41.0));
    vec4 o2 = fract(k4 * (1.0 / 41.0));

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}

void main(void)
{
    vec4 vert = qt_Vertex;
    vert.xyz *= scale;
    vert.x += 4 * realNoise(vec3(gl_InstanceID, camPosition.w, 0));
    vert.y += 4 * realNoise(vec3(gl_InstanceID, camPosition.w, 1));
    vert.z += 4 * realNoise(vec3(gl_InstanceID, camPosition.w, 2));
    gl_Position = mvpMat * vert;
    worldNormal = qt_Normal;
    worldPos = vert;
}
