#pragma once
#include <string>
#include "mesh.h"
#include"stb_image.h"
#include <glad/glad.h> // holds all OpenGL type declarations
#include <../glm/glm.hpp>
#include <../glm/gtc/matrix_transform.hpp>
#include"ParamConfiguration.h"

//typedef struct {
//	//每4个vec4为一个stride，表示 一个叶片的节点 总共有4个节点
//	glm::vec4 v[16];
//	glm::vec4 & operator[](int index) { return v[index]; }
//}GrassBlade;

class GrassRelatedModel
{
public:
	GrassRelatedModel();
	~GrassRelatedModel() = default;
	std::vector<GrassBlade> blades;
	int gengerateGrassPatch(std::vector<GrassBlade>* blades);
	float getRandom();
	void LoadGrassPlane();
	void LoadGrass();
	void DrawGrassPlane(Shader shader, glm::mat4 transform);
	void DrawGrass(Shader shader, glm::mat4 transform);
	void getGrassPlane(glm::vec3 & voFirstgrassplane, glm::vec3 &voSecondgrassplane);
	unsigned int TextureFromFile(const char *path);
private:
	glm::vec3 m_grassPlane[4];
	glm::vec3 m_grassPlaneTriangles[2];
	std::vector<GrassVertex>m_Grassvertexs;
	std::vector<GrassPlaneVertex>m_GrassPlanevertexs;
	vector<unsigned int> m_GrassPlaneindices;
	std::vector<Texture>m_Grasstextures;
	std::vector<Texture>m_GrassPlaneTextures;
	Mesh m_GrassPlaneMesh;
	Mesh m_GrassMesh;
	int m_numberBladeVertices;
	std::vector<Texture> m_textures;
};

