#shader vertex
#version 330 core

layout(location = 0) in vec4 position;	// 0=index of our position
layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
	//gl_Position = position;
	gl_Position = projection * view * model * position;
	v_TexCoord = texCoord;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#shader fragment
#version 330 core

out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D u_Texture0;


void main()
{
	//vec4 texColor = mix(texture(u_Texture0, v_TexCoord), texture(u_Texture1, v_TexCoord), 0.5) * u_Color;
	vec4 texColor = texture(u_Texture0, v_TexCoord);
	color = texColor;
};