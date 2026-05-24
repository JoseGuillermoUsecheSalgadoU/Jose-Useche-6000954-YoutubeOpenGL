#include"EBO.h"

// Constructor para crear el Element Buffer Object y subirle los índices de una
EBO::EBO(GLuint* indices, GLsizeiptr size)
{
	// Generamos un ID único para el buffer en la GPU
	glGenBuffers(1, &ID);
	// Lo enlazamos como un buffer de índices (ELEMENT_ARRAY_BUFFER)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	// Le mandamos los datos de los índices a la GPU. Usamos STATIC_DRAW porque este orden no va a cambiar en el juego
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

// Para activar/vincular el EBO cuando vayamos a dibujar
void EBO::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

// Para desvincular el EBO y no meter la pata configurando otra malla por ahí
void EBO::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Para borrar el buffer de la memoria de la GPU y limpiar recursos antes de cerrar
void EBO::Delete()
{
	glDeleteBuffers(1, &ID);
}