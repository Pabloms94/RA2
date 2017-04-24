#include "GLSLProgram.h"

GLSLProgram::GLSLProgram()
{
	numPoint = numSpot = numDirec = 0;
}

void GLSLProgram::InitShader(const char *vname, const char *fname){
	//Creamos un shader de vertices y de fragmentos
	vshader = LoadShader(vname, GL_VERTEX_SHADER);
	fshader = LoadShader(fname, GL_FRAGMENT_SHADER);

	//Ahora hay que vincularlos a un programa.

	//Creamos un programa y vinculamos los shaders
	program = glCreateProgram();
	glAttachShader(program, vshader);
	glAttachShader(program, fshader);

	//Los Atributos SIEMPRE se añaden antes de linkar el programa, se proponen los valores
	glBindAttribLocation(program, 0, "inPos");
	glBindAttribLocation(program, 1, "inColor");
	glBindAttribLocation(program, 2, "inNormal");
	glBindAttribLocation(program, 3, "inTexCoord");
	glBindAttribLocation(program, 4, "inTangent");

	//LINKADO
	glLinkProgram(program);

	//El linkado le da los valores si puede, sino le da un -1
	inPos = glGetAttribLocation(program, "inPos");
	inColor = glGetAttribLocation(program, "inColor");
	inNormal = glGetAttribLocation(program, "inNormal");
	inTexCoord = glGetAttribLocation(program, "inTexCoord");
	inTangent = glGetAttribLocation(program, "inTangent");

	//Las variable uniform se declaran despues del linkado
	uNormalMat = glGetUniformLocation(program, "normal");
	uModelViewMat = glGetUniformLocation(program, "modelView");
	uModelViewProjMat = glGetUniformLocation(program, "modelViewProj");
	//Inicializamos las uniform que se van a usar
	uColorTex = glGetUniformLocation(program, "colorTex");
	uEmiTex = glGetUniformLocation(program, "emiTex");
	uSpecularTex = glGetUniformLocation(program, "specularTex");
	uNormalTex = glGetUniformLocation(program, "normalTex");

	//Uniform Luz
	//Uniform Luz Ambiental
	uIntAmbiental = glGetUniformLocation(program, "Ia");

	//Uniforms luz puntual
	uPosPoint = glGetUniformLocation(program, "PosPoint");
	uIntPoint = glGetUniformLocation(program, "intPoint");
	uNumPoint = glGetUniformLocation(program, "numPoint");

	//Uniforms luz Spot
	uPosSpot = glGetUniformLocation(program, "PosSpot");
	uIntSpot = glGetUniformLocation(program, "intSpot");
	uNumSpot = glGetUniformLocation(program, "numSpot");

	//Uniforms luz Direccional
	uPosDirec = glGetUniformLocation(program, "PosDirec");
	uIntDirec = glGetUniformLocation(program, "intDirec");
	uNumDirec = glGetUniformLocation(program, "numDirec");

	//Preguntamos el status de linkado
	int linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
		char *logString = new char[logLen];
		glGetProgramInfoLog(program, logLen, NULL, logString);
		std::cout << "Error: " << logString << std::endl;
		delete logString;
		glDeleteProgram(program);
		program = 0;
		exit(-1);
	}
}

GLuint GLSLProgram::LoadShader(const char *fileName, GLenum type){
	unsigned int fileLen;
	char *source = loadStringFromFile(fileName, fileLen);
	//////////////////////////////////////////////
	//Creación y compilación del Shader
	GLuint shader;
	shader = glCreateShader(type);

	//Se le asigna al shader, la cadena de caracteres recien cargadas con una longitud
	glShaderSource(shader, 1,
		(const GLchar **)&source, (const GLint *)&fileLen);

	//Compilación del shader
	glCompileShader(shader);

	//Se libera el recurso de la cadena del shader
	delete source;

	//Comprobamos que se compiló bien
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
		char *logString = new char[logLen];
		glGetShaderInfoLog(shader, logLen, NULL, logString);
		std::cout << "FILENAME: " << fileName << std::endl;
		std::cout << "Error: " << logString << std::endl;
		delete logString;
		glDeleteShader(shader);
		exit(-1);
	}

	return shader;
}

void GLSLProgram::Destroy(){

	glDetachShader(program, vshader);
	glDetachShader(program, fshader);
	glDeleteShader(this->vshader);
	glDeleteShader(this->fshader);
	glDeleteProgram(program);
}

void GLSLProgram::UseProgram(){
	glUseProgram(program);
}

void GLSLProgram::AddUnif(glm::mat4 &modelView, glm::mat4 &modelViewProj, glm::mat4 &normal){
	AddUnifMat4fvMV(modelView);
	AddUnifMat4fvMVP(modelViewProj);
	AddUnifMat4fvN(normal);
}

void GLSLProgram::AddUnifMat4fvMV(glm::mat4 &mat){
	glUniformMatrix4fv(uModelViewMat, 1, GL_FALSE, &(mat[0][0]));
}

void GLSLProgram::AddUnifMat4fvMVP(glm::mat4 &mat){
	glUniformMatrix4fv(uModelViewProjMat, 1, GL_FALSE, &(mat[0][0]));
}

void GLSLProgram::AddUnifMat4fvN(glm::mat4 &mat){
	glUniformMatrix4fv(uNormalMat, 1, GL_FALSE, &(mat[0][0]));
}

void GLSLProgram::AddUnifNumLight(int idNum, int num) {
	glUniform1i(idNum, num);
}

void GLSLProgram::AddUnifPosLight(int idUnif, glm::vec3 *vect, int sizeArray){
	glUniform3fv(idUnif, sizeArray, &(vect[0][0]));
}

void GLSLProgram::AddUnifIntLight(int idUnif, glm::vec3 *vect, int sizeArray){
	glUniform3fv(idUnif, sizeArray, &(vect[0][0]));
}

void GLSLProgram::AddUnif1fAmbiental(glm::vec3 ambient) {
	glUniform3fv(uIntAmbiental, 1, &(ambient[0]));
}

void GLSLProgram::AddUnifTex(unsigned int color, unsigned int emi, unsigned int spec, unsigned int nor){
	AddUnifTexC(color);
	AddUnifTexE(emi);
	AddUnifTexS(spec);
	AddUnifTexN(nor);
}

void GLSLProgram::AddUnifTexC(unsigned int color){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color);
	//Le digo al shader que la textura de color la tiene que coger en la 0
	glUniform1i(uColorTex, 0);
}

void GLSLProgram::AddUnifTexE(unsigned int emi){
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, emi);
	glUniform1i(uEmiTex, 1);
}

void GLSLProgram::AddUnifTexS(unsigned int spec){
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, spec);
	glUniform1i(uSpecularTex, 2);
}

void GLSLProgram::AddUnifTexN(unsigned int nor){
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, nor);
	glUniform1i(uNormalTex, 3);
}

int GLSLProgram::getPos(){
	return inPos;
}

int GLSLProgram::getColor(){
	return inColor;
}

int GLSLProgram::getNormal(){
	return inNormal;
}

int GLSLProgram::getTexCoord(){
	return inTexCoord;
}

int GLSLProgram::getTangent(){
	return inTangent;
}

void GLSLProgram::AddLight(Light &light) {
	if (light.GetType() == POINT_LIGHT) 
	{
		numPoint ++;
		if(numPoint < 8)
			lights.push_back(&light);
	}
	else if (light.GetType() == SPOT_LIGHT)
	{
		numSpot ++;
		if (numSpot < 8)
			lights.push_back(&light);
	}
	else
	{
		numDirec ++;
		if (numDirec < 8)
			lights.push_back(&light);
	}
}

void GLSLProgram::AddUnifLight() {
	AddUnif1fAmbiental(ambientLight);		//LUZ AMBIENTAL
	AddUnifNumLight(uNumPoint, numPoint);
	AddUnifNumLight(uNumSpot, numSpot);
	AddUnifNumLight(uNumDirec, numDirec);

	posPointLights = new glm::vec3[numPoint];
	intPointLights = new glm::vec3[numPoint];
	posSpotLights = new glm::vec3[numSpot];
	intSpotLights = new glm::vec3[numSpot];
	posDirLights = new glm::vec3[numDirec];
	intDirLights = new glm::vec3[numDirec];

	for (unsigned int i = 0; i < lights.size(); i++) {
		if ((*lights.at(i)).GetType() == POINT_LIGHT) 
		{
			posPointLights[i] = (*lights.at(i)).GetPosition();
			intPointLights[i] = (*lights.at(i)).GetIntensity();
		}
		else if ((*lights.at(i)).GetType() == SPOT_LIGHT)
		{
			posSpotLights[i] = (*lights.at(i)).GetPosition();
			intSpotLights[i] = (*lights.at(i)).GetIntensity();
		}
		else
		{
			posDirLights[i] = (*lights.at(i)).GetPosition();
			intDirLights[i] = (*lights.at(i)).GetIntensity();
		}
	}

	AddUnifPosLight(uPosPoint, posPointLights, numPoint);
	AddUnifIntLight(uIntPoint, intPointLights, numPoint);

	AddUnifPosLight(uPosSpot, posSpotLights, numSpot);
	AddUnifIntLight(uIntSpot, intSpotLights, numSpot);

	AddUnifPosLight(uPosDirec, posDirLights, numDirec);
	AddUnifIntLight(uIntDirec, intDirLights, numDirec);
}