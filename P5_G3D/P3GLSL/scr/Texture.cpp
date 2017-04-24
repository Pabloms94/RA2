#include "Texture.h"

Texture::Texture(char *filename)
{
	//LoadTexture(filename);
	name = filename;
}

void Texture::LoadTexture()
{
	unsigned char *map;
	unsigned int w, h;
	//Primero se carga la textura desde fichero
	map = loadTexture(name, w, h);
	if (!map)
	{
		std::cout << "Error cargando el fichero: "
			<< name << std::endl;
		exit(-1);
	}

	glGenTextures(1, &this->id);
	glBindTexture(GL_TEXTURE_2D, this->id);
	//Tipo de textura a modificar, numero de midmap, formato de canal interno de la tarjeta gráfica, ancho, alto, borde, los canales de los datos que le voy a pasar,
	//tipo de canal, puntero a los datos.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)map);

	delete[] map;

	//Generar el Mipmap
	glGenerateMipmap(GL_TEXTURE_2D);

	//Indicar el acceso a la textura
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
}

void Texture::Load(unsigned char *map, int h, int w)
{
	mymap = map;
	myh = h;
	myw = w;
}

void Texture::LoadPerlinTexture()
{
	if (!mymap)
	{
		std::cout << "Error cargando el fichero: "
			<< name << std::endl;
		exit(-1);
	}

	glGenTextures(1, &this->id);
	glBindTexture(GL_TEXTURE_2D, this->id);
	//Tipo de textura a modificar, numero de midmap, formato de canal interno de la tarjeta gráfica, ancho, alto, borde, los canales de los datos que le voy a pasar,
	//tipo de canal, puntero a los datos.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, myw, myh, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)mymap);

	//Generar el Mipmap
	glGenerateMipmap(GL_TEXTURE_2D);

	//Indicar el acceso a la textura
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
}

void Texture::Destroy()
{
	//Borramos las texturas
	glDeleteTextures(1, &id);
}