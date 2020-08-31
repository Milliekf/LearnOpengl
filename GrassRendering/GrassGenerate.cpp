#include"pch.h"
#include"GrassGenerate.h"

GrassRelatedModel::GrassRelatedModel()
{
	m_grassPlane[0] = glm::vec3(-0.5f, 0.0f, -0.5f);
	m_grassPlane[1] = glm::vec3(0.5f, 0.0f, -0.5f);
	m_grassPlane[2] = glm::vec3(0.5f, 0.0f, 0.5f);
	m_grassPlane[3] = glm::vec3(-0.5f, 0.0f, 0.5f);

	m_grassPlaneTriangles[0] = glm::vec3(2, 1, 0);
	m_grassPlaneTriangles[1] = glm::vec3(2, 0, 3);
}
void GrassRelatedModel::getGrassPlane(glm::vec3 & voFirstgrassplane,glm::vec3 &voSecondgrassplane)
{
	voFirstgrassplane = m_grassPlane[0];
	voSecondgrassplane = m_grassPlane[2];
}
//return 0~1
float GrassRelatedModel::getRandom()
{
	//RAND_MAX ox7fff 32767
	return (float)rand() / (float)RAND_MAX;
}

int GrassRelatedModel::gengerateGrassPatch(std::vector<GrassBlade> * blades)
{
	float minWidth = 0.0025f;
	float maxWidth = 0.0075f;
	float minHeight = 0.05f;
	float maxHeight = 0.125f;
	uint32_t numBladesToGenerate = NUM_BLADES_TO_GENERATE;
	for (uint32_t i = 0; i < numBladesToGenerate; i++)
	{
		float randomValues[8];
		for (uint32_t randIndex = 0; randIndex < 8; randIndex++)
		{
			randomValues[randIndex] = getRandom();
		}
		//width 0.0025~0.0075
		float width = minWidth + getRandom()*(maxWidth - minWidth);
		//height 0.05~0.125
		float height = minHeight + getRandom()*(maxHeight - minHeight);
		//x -0.5~0.5
		float x = getRandom() - 0.5f;
		//z -0.5~0.5
		float z = getRandom() - 0.5f;
		//bladeCenter (-0.5~0.5 , 0, -0.5~0.5, 0~1)
		glm::vec4 bladeCenter = glm::vec4(x, m_grassPlane[0].y, z, randomValues[0]);
		GrassBlade blade;

		// setting quad vertices
		//blade[0] (x-0.5width ,0,z,0~1)
		blade[0] = bladeCenter - glm::vec4(0.5f*width, 0.0f, 0.0f, 0.0f);
		//blade[4] (x-0.5width ,height,z,0~1)
		blade[4] = bladeCenter - glm::vec4(0.5f*width, -height, 0.0f, 0.0f);
		//blade[8] (x+0.5width ,height ,z,0~1)
		blade[8] = bladeCenter + glm::vec4(0.5f*width, height, 0.0f, 0.0f);
		//blade[12] (x+0.5width ,0,z,0~1)
		blade[12] = bladeCenter + glm::vec4(0.5f*width, 0.0f, 0.0f, 0.0f);

		// setting centre positions for each vertex
		{
			glm::vec4 centerLower = glm::vec4(bladeCenter.x, 0, bladeCenter.z, randomValues[1]);
			glm::vec4 centerUpper = centerLower;
			centerUpper.y = 1;
			blade[1] = centerLower;
			blade[5] = centerUpper;
			blade[9] = centerUpper;
			blade[13] = centerLower;
		}
		// setting texture coords for each vertex
		blade[2] = glm::vec4(0.0f, 1.0f, randomValues[2], randomValues[3]);
		blade[6] = glm::vec4(1.0f, 1.0f, randomValues[2], randomValues[3]);
		blade[10] = glm::vec4(1.0f, 0.0f, randomValues[2], randomValues[3]);
		blade[14] = glm::vec4(0.0f, 0.0f, randomValues[2], randomValues[3]);

		// setting an extra vector filled with only random values for some randomness in the shaders
		glm::vec4 randomVector = glm::vec4(randomValues[4], randomValues[5], randomValues[6], randomValues[7]);
		blade[3] = randomVector;
		blade[7] = randomVector;
		blade[11] = randomVector;
		blade[15] = randomVector;

		blades->push_back(blade);

	} 
	return blades->size() * 4;
}
void GrassRelatedModel::LoadGrassPlane()
{
	GrassPlaneVertex vertex;
	unsigned int indices;
	for (int i = 0; i < 4; i++)
	{
		vertex.Position = m_grassPlane[i];
		m_GrassPlanevertexs.push_back(vertex);
	}
	for (int i = 0; i < 2; i++)
	{
		m_GrassPlaneindices.push_back(m_grassPlaneTriangles[i].x);
		m_GrassPlaneindices.push_back(m_grassPlaneTriangles[i].y);
		m_GrassPlaneindices.push_back(m_grassPlaneTriangles[i].z);
	}
	m_GrassPlaneMesh = Mesh(m_GrassPlanevertexs, m_GrassPlaneindices);
}
void GrassRelatedModel::LoadGrass()
{
	//返回所有叶片(包含4个结点)中总结点的个数(每个节点包含4个vec4分别表示顶点，贴图等数据)
	m_numberBladeVertices=gengerateGrassPatch(&blades);
	//表示每个叶面
	GrassVertex vertex;
	//blade Sort by: position centrepos texcoords random* 4
	for (int i = 0;i < blades.size(); i++)
	{
		for (int j = 0; j < 4; j++)
		{
			vertex.GrassVertexPart[j].Position=blades[i].v[j*4+0];
			vertex.GrassVertexPart[j].CentrePos = blades[i].v[j*4+1];
			vertex.GrassVertexPart[j].TexCoords = blades[i].v[j * 4 + 2];
			vertex.GrassVertexPart[j].Random = blades[i].v[j * 4 + 3];
		}

		m_Grassvertexs.push_back(vertex);
	}
	Texture alphaTexture;
	//alphaTexture.id = TextureFromFile("D:\\Work\\RealtimeDepthImage\\Blackboard\\stereo.im1.640x533.jpg");
	//alphaTexture.id = TextureFromFile("D:\\GraduationProject\\GrassRendering\\GrassRendering\\resource\\grass\\alpha.png");
	alphaTexture.id = TextureFromFile("D:\\GraduationProject\\GrassRendering\\GrassRendering\\resource\\grass\\grass_alpha_texture.png");
	m_textures.push_back(alphaTexture);
	Texture grassDiffuseTexture;
	grassDiffuseTexture.id = TextureFromFile("D:\\GraduationProject\\GrassRendering\\GrassRendering\\resource\\grass\\grass_diffuse_texture.jpg");
	m_textures.push_back(grassDiffuseTexture);
	Texture forceMapTexture;
	forceMapTexture.id = TextureFromFile("D:\\GraduationProject\\GrassRendering\\GrassRendering\\resource\\grass\\default_force_map.png");
	m_textures.push_back(forceMapTexture);
	m_GrassMesh = Mesh(m_Grassvertexs,m_textures);
	/*m_GrassMesh = Mesh(blades, m_textures);*/

}
void GrassRelatedModel::DrawGrassPlane(Shader shader,glm::mat4 transform)
{
	m_GrassPlaneMesh.DrawGrassPlane(shader, transform);
}

void GrassRelatedModel::DrawGrass(Shader shader,glm::mat4 transform)
{
	m_GrassMesh.DrawGrass(shader,transform);
}

unsigned int GrassRelatedModel::TextureFromFile(const char *path)
{
	string filename = string(path);

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
