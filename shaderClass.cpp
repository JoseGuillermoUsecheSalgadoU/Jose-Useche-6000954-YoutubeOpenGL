#include"shaderClass.h"

// Lee un archivo de texto completo y lo devuelve como un string (para cargar los scripts de los shaders)
std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg()); // Mueve el puntero al final para saber cuánto mide el archivo y darle el tamaño exacto al string
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size()); // Lee todo el archivo de un solo golpe
		in.close();
		return(contents);
	}
	throw(errno); // Si no encuentra el archivo o falla, lanza el error
}

// Constructor para armar el Shader Program completo juntando el Vertex y el Fragment shader
Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	// Leemos los archivos de texto de ambos shaders y guardamos el código en strings
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	// Convertimos los strings a arreglos de caracteres (const char*) que es lo que entiende OpenGL
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	// --- CONFIGURACIÓN DEL VERTEX SHADER ---
	// Crea el objeto del Vertex Shader en la GPU y guarda su ID
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Le mete el código fuente que leímos del archivo
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	// Compila el shader a código máquina de la GPU
	glCompileShader(vertexShader);
	// Revisa si compiló bien o si saltó algún error de sintaxis
	compileErrors(vertexShader, "VERTEX");

	// --- CONFIGURACIÓN DEL FRAGMENT SHADER ---
	// Lo mismo pero para el Fragment Shader (el que se encarga de los colores de los píxeles)
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	// Revisa si hubo errores de compilación aquí
	compileErrors(fragmentShader, "FRAGMENT");

	// --- CREACIÓN DEL SHADER PROGRAM (EL CONTENEDOR) ---
	// Crea el programa final donde se van a fusionar ambos shaders
	ID = glCreateProgram();
	// Adjunta los dos shaders ya compilados al programa
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	// Enlaza (linkea) los shaders para que funcionen juntos en el pipeline gráfico
	glLinkProgram(ID);
	// Revisa si el linkeo falló (por ejemplo, si las variables 'in' y 'out' no cuadran)
	compileErrors(ID, "PROGRAM");

	// Borramos los objetos de los shaders sueltos porque ya están clonados dentro del programa final y solo hacen bulto
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

// Activa este Shader Program para empezar a usarlo al dibujar los objetos en el main
void Shader::Activate()
{
	glUseProgram(ID);
}

// Borra el programa de la GPU para liberar memoria limpia antes de cerrar la app
void Shader::Delete()
{
	glDeleteProgram(ID);
}

// Función salvavidas para revisar si algo explotó al compilar o al linkear
void Shader::compileErrors(unsigned int shader, const char* type)
{
	GLint hasCompiled; // Guarda el estado del resultado (GL_TRUE o GL_FALSE)
	char infoLog[1024]; // Buffer para guardar el texto del error que nos escupa la GPU

	if (type != "PROGRAM")
	{
		// Revisa errores de compilación de un shader individual (Vertex o Fragment)
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
	else
	{
		// Revisa errores al enlazar todo el programa completo
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
}