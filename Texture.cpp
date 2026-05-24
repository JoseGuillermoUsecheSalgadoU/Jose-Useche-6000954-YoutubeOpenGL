#include"Texture.h"

// Constructor para cargar la imagen, configurar los filtros y subir la textura a la GPU de una
Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
	// Guardamos el tipo de textura (por ejemplo, GL_TEXTURE_2D) en la variable de la clase
	type = texType;

	// Variables para guardar el ancho, alto y los canales de color (RGB/RGBA) de la imagen
	int widthImg, heightImg, numColCh;

	// Volteamos la imagen al cargarla porque OpenGL lee las texturas al revés (de abajo hacia arriba)
	stbi_set_flip_vertically_on_load(true);

	// Cargamos los píxeles del archivo y los guardamos en un puntero de bytes
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

	// Generamos un ID único para la textura en la GPU
	glGenTextures(1, &ID);
	// Activamos la ranura de textura (el slot como GL_TEXTURE0) y enlazamos la textura
	glActiveTexture(slot);
	glBindTexture(texType, ID);

	// Configuramos los filtros para cuando la imagen se vea más lejos (minificación) o más cerca (magnificación)
	// NEAREST da ese toque pixelado/retro nítido, y usamos MIPMAP_LINEAR para que no se vea ruidoso a la distancia
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Configuramos qué pasa si las coordenadas UV se salen de los límites (0 a 1). Con REPEAT la textura se repite en bucle
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Líneas extra por si prefieres usar GL_CLAMP_TO_BORDER (para poner un color plano en los bordes)
	// float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

	// Subimos los píxeles cargados en 'bytes' a la memoria de la GPU
	glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);
	// Generamos los MipMaps automáticos (versiones más pequeñas de la textura para optimizar el rendimiento)
	glGenerateMipmap(texType);

	// Liberamos la memoria RAM porque los datos ya están guardados a salvo en la GPU
	stbi_image_free(bytes);

	// Desvinculamos la textura para no editarla por error en otro lado
	glBindTexture(texType, 0);
}

// Vincula el sampler del shader con la ranura (unit) de textura que le toca
void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	// Buscamos la ubicación de la variable uniform en el shader
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	// Ojo: el shader DEBE estar activo antes de cambiar el valor de cualquier uniform
	shader.Activate();
	// Le asignamos la ranura correspondiente
	glUniform1i(texUni, unit);
}

// Activa/vincula esta textura para usarla al renderizar el objeto
void Texture::Bind()
{
	glBindTexture(type, ID);
}

// Desvincula la textura para dejar el pipeline limpio
void Texture::Unbind()
{
	glBindTexture(type, 0);
}

// Borra la textura de la GPU para liberar memoria antes de cerrar el programa
void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}