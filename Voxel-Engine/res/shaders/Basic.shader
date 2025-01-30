#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 u_Projection;
uniform mat4 u_Model;

void main()
{
    gl_Position = u_Projection * u_Model * vec4(aPos, 1.0);
}

#shader fragment
#version 330 core

out vec4 FragColor;

uniform vec4 u_Color;

void main()
{
    FragColor = u_Color;
}