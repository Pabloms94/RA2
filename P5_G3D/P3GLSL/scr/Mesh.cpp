#include "Mesh.h"

Mesh::Mesh()
{
	colorTex = Texture("../img/color.png");
	emiTex = Texture("../img/emissive.png");
	specularTex = Texture("../img/specMap.png");
	normalTex = Texture("../img/normal.png");
}

Mesh::Mesh(Texture tex)
{
	colorTex = tex;
	emiTex = Texture("../img/emissive.png");
	specularTex = Texture("../img/specMap.png");
	normalTex = Texture("../img/normal.png");
}

Mesh::Mesh(char *nameTex1, char *nameTex2)
{
	colorTex = Texture(nameTex1);
	specularTex = Texture(nameTex2);
}

Mesh::Mesh(char *nameTex1, char *nameTex2, char *nameTex3, char *nameTex4)
{
	colorTex = Texture(nameTex1);
	emiTex = Texture(nameTex2);
	specularTex = Texture(nameTex3);
	normalTex = Texture(nameTex4);
}

void Mesh::InitRender(Camera &camera)
{
	(*programa).UseProgram();

	camera.SetModelView(camera.GetView() * model);
	camera.SetModelViewProj(camera.GetProj() * camera.GetView() * model);
	camera.SetNormal(glm::transpose(glm::inverse(camera.GetModelView())));

	(*programa).AddUnif(camera.GetModelView(), camera.GetModelViewProj(), camera.GetNormal());
	(*programa).AddUnifTex(GetColorId(), GetEmiteId(), GetSpecularId(), GetNormalId());
	(*programa).AddUnifLight();
}

void Mesh::Render()
{
	//Activamos el VAO del objeto, activandose todos los VBO 
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_INT, (void*)0);

	glUseProgram(NULL);
}

void Mesh::LoadVBO(unsigned int &VBO, int dataSize, const float *vertexArray, GLint size, int idAtrib)
{
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, dataSize, &vertexArray[0], GL_STATIC_DRAW);
	glVertexAttribPointer(idAtrib, size, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(idAtrib);
}

void Mesh::LoadIBO(unsigned int &IBO, int dataSize, const unsigned int *indexArray)
{
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, &indexArray[0], GL_STATIC_DRAW);
}

void Mesh::Rotate(float &angle, glm::vec3 axis)
{
	model *= glm::rotate(glm::mat4(1.0f), angle, axis);
}

void Mesh::Translate(glm::vec3 cord)
{
	model *= glm::translate(glm::mat4(1.0f), cord);
}

void Mesh::Rotation(float &angle, glm::vec3 axis) {
	model = glm::rotate(glm::mat4(1.0f), angle, axis);
}

void Mesh::Orbit(float &internAngle, float &externAngle, glm::vec3 externRadius) {
	Rotation(internAngle, glm::vec3(0, 1.0f, 0));
	Translate(externRadius);
	Rotate(externAngle, glm::vec3(0, 1.0f, 0));
}

void Mesh::Destroy(GLSLProgram &programa) 
{
	//Borramos los distintos buffers anteriormente creados
	if (programa.getPos() != -1) glDeleteBuffers(1, &posVBO);
	if (programa.getColor() != -1) glDeleteBuffers(1, &colorVBO);
	if (programa.getNormal() != -1) glDeleteBuffers(1, &normalVBO);
	if (programa.getTexCoord() != -1) glDeleteBuffers(1, &texCoordVBO);
	if (programa.getTangent() != -1) glDeleteBuffers(1, &tangentVBO);
	glDeleteBuffers(1, &triangleIndexVBO);

	colorTex.Destroy();
	emiTex.Destroy();
	specularTex.Destroy();
	normalTex.Destroy();

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vao);
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &posVBO);
	glDeleteBuffers(1, &colorVBO);
	glDeleteBuffers(1, &normalVBO);
	glDeleteBuffers(1, &texCoordVBO);
	glDeleteBuffers(1, &tangentVBO);
	glDeleteBuffers(1, &triangleIndexVBO);
}

void Mesh::InitDefaultMesh() {
	//Creo el VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	if ((*programa).getPos() != -1)
	{
		LoadVBO(posVBO, cubeNVertex * sizeof(float) * 3, cubeVertexPos, 3, (*programa).getPos());
	}
	if ((*programa).getColor() != -1)
	{
		LoadVBO(colorVBO, cubeNVertex * sizeof(float) * 3, cubeVertexColor, 3, (*programa).getColor());
	}
	if ((*programa).getNormal() != -1)
	{
		LoadVBO(normalVBO, cubeNVertex * sizeof(float) * 3, cubeVertexNormal, 3, (*programa).getNormal());
	}
	if ((*programa).getTexCoord() != -1)
	{
		LoadVBO(texCoordVBO, cubeNVertex * sizeof(float) * 2, cubeVertexTexCoord, 2, (*programa).getTexCoord());
	}if ((*programa).getTangent() != -1)
	{
		LoadVBO(tangentVBO, cubeNVertex * sizeof(float) * 3, cubeVertexTangent, 3, (*programa).getTangent());
	}

	LoadIBO(triangleIndexVBO, cubeNTriangleIndex * sizeof(unsigned int) * 3, cubeTriangleIndex);

	
	colorTex.LoadPerlinTexture();
	/*emiTex.LoadTexture();
	specularTex.LoadTexture();
	normalTex.LoadTexture();*/

	numFaces = cubeNTriangleIndex;

	model = glm::mat4(1.0f);
}

void Mesh::InitMesh(const std::string &pFile) {
	ImportMesh(pFile);
	//Creo el VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	if ((*programa).getPos() != -1)
	{
		LoadVBO(posVBO, numVerts * sizeof(float) * 3, vertexArray, 3, (*programa).getPos());
	}
	if ((*programa).getColor() != -1)
	{
		LoadVBO(colorVBO, cubeNVertex * sizeof(float) * 3, cubeVertexColor, 3, (*programa).getColor());
	}
	if ((*programa).getNormal() != -1)
	{
		LoadVBO(normalVBO, numVerts * sizeof(float) * 3, normalArray, 3, (*programa).getNormal());
	}
	if ((*programa).getTexCoord() != -1)
	{
		LoadVBO(texCoordVBO, numVerts * sizeof(float) * 2, uvArray, 2, (*programa).getTexCoord());
	}
	if ((*programa).getTangent() != -1)
	{
		LoadVBO(tangentVBO, numVerts * sizeof(float) * 3, tangentArray, 3, (*programa).getTangent());
	}

	LoadIBO(triangleIndexVBO, numFaces * sizeof(unsigned int) * 3, arrayIndex);

	colorTex.LoadTexture();
	specularTex.LoadTexture();

	model = glm::mat4(1.0f);
}

void Mesh::ImportMesh(const std::string &pFile) {
	Assimp::Importer importer;

	importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS,
		aiComponent_CAMERAS | aiComponent_LIGHTS | aiComponent_COLORS |
		aiComponent_BONEWEIGHTS | aiComponent_ANIMATIONS | aiComponent_TANGENTS_AND_BITANGENTS);
	
	const aiScene *scene = importer.ReadFile(pFile, aiProcess_GenSmoothNormals | aiProcess_RemoveComponent | aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	aiMesh *mesh = scene->mMeshes[0];

	numVerts = mesh->mNumFaces * 3;

	vertexArray = new float[mesh->mNumFaces * 3 * 3];
	normalArray = new float[mesh->mNumFaces * 3 * 3];
	uvArray = new float[mesh->mNumFaces * 3 * 2];
	arrayIndex = new unsigned[mesh->mNumFaces * 3];
	tangentArray = new float[mesh->mNumFaces * 3 * 3];

	int cont = 0;
	numFaces = 0;

	for (unsigned int i = 0; i<mesh->mNumFaces; i++)
	{
		const aiFace& face = mesh->mFaces[i];
		numFaces++;
		for (int j = 0; j<3; j++)
		{
			aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[j]];
			memcpy(uvArray, &uv, sizeof(float) * 2);
			uvArray += 2;

			aiVector3D normal = mesh->mNormals[face.mIndices[j]];
			memcpy(normalArray, &normal, sizeof(float) * 3);
			normalArray += 3;

			aiVector3D tangent = mesh->mTangents[face.mIndices[j]];
			memcpy(tangentArray, &tangent, sizeof(float) * 3);
			tangentArray += 3;
			
			aiVector3D pos = mesh->mVertices[face.mIndices[j]];
			memcpy(vertexArray, &pos, sizeof(float) * 3);
			vertexArray += 3;

			arrayIndex[cont] = face.mIndices[j];
			cont++;
		}
	}

	uvArray -= mesh->mNumFaces * 3 * 2;
	normalArray -= mesh->mNumFaces * 3 * 3;
	vertexArray -= mesh->mNumFaces * 3 * 3;
	tangentArray -= mesh->mNumFaces * 3 * 3;
}