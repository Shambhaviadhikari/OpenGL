#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 transpose_inverse_model;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
	FragPos = vec3(model * vec4(position, 1.0));
	//Normal = normal;
	//Normal = mat3(transpose(inverse(model))) * normal;	//matrix inversion in shaders are costly since they are evaluated many times in a frame
	Normal = transpose_inverse_model * normal;
};



#shader fragment
#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 color;


uniform vec3 viewPos;

struct Light
{
	vec3 color;
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform Light light;
uniform Material material;

void main()
{
	vec3 ambientLight = material.ambient * light.color * light.ambient;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuseLight = diff * material.diffuse * light.color * light.diffuse;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0), material.shininess);
	vec3 specularLight = spec * material.specular * light.color * light.specular;

	color = vec4((ambientLight + diffuseLight + specularLight), 1.0f);
};