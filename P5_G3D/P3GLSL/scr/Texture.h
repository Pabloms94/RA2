#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "auxiliar.h"
#include <iostream>
#include <gl/glew.h>

class Texture
{
	private:
		unsigned int id;
		char *name;
		unsigned char *mymap;
		int myh, myw;
	public:
		Texture(){};
		Texture(char *filename);
		
		void Load(unsigned char *map, int h, int w);
		void LoadTexture();
		void LoadPerlinTexture();
		inline unsigned int GetId(){ return this->id; }
		void Destroy();

		inline ~Texture() { Destroy(); }
};

#endif