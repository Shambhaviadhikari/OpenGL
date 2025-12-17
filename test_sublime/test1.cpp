#include <iostream>

enum class ShaderType
{
	NONE = -1, VERTEX = 0, FRGMENT = 1
};

int main()
{
	ShaderType type = NONE;
	std::cout << type << std::endl;
	type = ShaderType::VERTEX;
	std::cout << type << std::endl;


}