#include <string>
#include "mesh.h"
#include"stb_image.h"
#include <glad/glad.h> // holds all OpenGL type declarations
#include <../glm/glm.hpp>
#include <../glm/gtc/matrix_transform.hpp>

class Terrain
{
public:
	Terrain()=default;
	
	~Terrain()=default;
	void loadTerrain(const std::string vfileName, const std::string vfragName,const std::string vfragName1)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(vfileName.c_str(), &width, &height, &nrChannels, 0);
		for (int i = 0; i < height-1; i++)
		{
			for (int j = 0; j < width - 1; j++)
			{
				HeightVertex vertexs[4];

				//int ii = i;// height - i - 1;
			/*	glm::vec3 v1(j, i, *(data + (j + i * width) * 3)-100);
				glm::vec3 v2(j, i + 1,  * (data + (j + (i + 1)*width) * 3)-100);
				glm::vec3 v3(j + 1, i + 1,  * (data + (j + 1 + (i + 1)*width) * 3)-100);
				glm::vec3 v4(j + 1, i, *(data + (j + 1 + i * width) * 3)-100);*/
				glm::vec3 v1(j-width/2, *(data + (j + i * width) * 3)-300, i-height/2);
				glm::vec3 v2(j-width/2, * (data + (j + (i + 1)*width) * 3)-300, i + 1 - height / 2);
				glm::vec3 v3(j + 1-width/2, * (data + (j + 1 + (i + 1)*width) * 3)-300, i + 1 - height / 2);
				glm::vec3 v4(j + 1-width/2, *(data + (j + 1 + i * width) * 3)-300, i - height / 2);

				vertexs[0].Position = v1;
				vertexs[1].Position = v2;
				vertexs[2].Position = v3;
				vertexs[3].Position = v4;
				glm::vec2 tx1((v1.x + width / 2 )/ width, (v1.z+height/2) / height);
				glm::vec2 tx2((v2.x+ width / 2) / width, (v2.z + height / 2) / height);
				glm::vec2 tx3((v3.x + width / 2) / width, (v3.z + height / 2) / height);
				glm::vec2 tx4((v4.x + width / 2) / width, (v4.z + height / 2) / height);
				vertexs[0].TexCoords = tx1;
				vertexs[1].TexCoords = tx2;
				vertexs[2].TexCoords = tx3;
				vertexs[3].TexCoords = tx4;
				m_vertexs.push_back(vertexs[0]);
				m_vertexs.push_back(vertexs[1]);
				m_vertexs.push_back(vertexs[2]);
				m_vertexs.push_back(vertexs[0]);
				m_vertexs.push_back(vertexs[2]);
				m_vertexs.push_back(vertexs[3]);
			}
		}
		Texture texture1;
		texture1.id = TextureFromFile(vfragName.c_str());
		m_textures.push_back(texture1);
		Texture texture2;
		texture2.id = TextureFromFile(vfragName1.c_str());
		m_textures.push_back(texture2);	
		Mesh mesh(m_vertexs, m_textures);
		m_mesh = mesh;
	}
	void Draw(Shader shader)
	{
		m_mesh.DrawTerrain(shader);
	}

private:
	std::vector<HeightVertex>m_vertexs;
	std::vector<Texture>m_textures;
	Mesh m_mesh;
	unsigned int TextureFromFile(const char *path)
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

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
};
