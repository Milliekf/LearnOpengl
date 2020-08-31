#pragma once
#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <../glm/glm.hpp>
#include <../glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

typedef struct {
	//每4个vec4为一个stride，表示 一个叶片的节点 总共有4个节点
	glm::vec4 v[16];
	glm::vec4 & operator[](int index) { return v[index]; }
}GrassBlade;

struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
};
struct HeightVertex
{
	glm::vec3 Position;

	glm::vec2 TexCoords;
};
struct GrassVertexPart
{
	glm::vec4 Position;

	glm::vec4 CentrePos;

	glm::vec4 TexCoords;

	glm::vec4 Random;
};

struct GrassVertex
{
	GrassVertexPart GrassVertexPart[4];
};


struct GrassPlaneVertex
{
	glm::vec3 Position;
};

struct Texture {
	unsigned int id;
	string type;
	string path;
};

class Mesh {
public:
	/*  Mesh Data  */
	vector<Vertex> vertices;
	vector<HeightVertex> heightvertices;
	vector<GrassVertex> grassvertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	vector<GrassPlaneVertex> grassplanevertices;
	vector<GrassBlade> grassvertic;
	unsigned int VAO;
	Mesh() = default;
	/*  Functions  */
	// constructor
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		// now that we have all the required data, set the vertex buffers and its attribute pointers.
		setupMesh();
	}
	Mesh(vector<HeightVertex> vertices, vector<Texture> textures)
	{
		this->heightvertices = vertices;
		this->textures = textures;

		// now that we have all the required data, set the vertex buffers and its attribute pointers.
		setupMeshTerrain();
	}
	//Mesh(vector<GrassVertex> vertices, vector<Texture> textures)
	Mesh(vector<GrassVertex> vertices,vector<Texture> textures)
	{
		this->grassvertices = vertices;
		this->textures = textures;
		setupMeshGrass();
	/*	this->textures = textures;*/
	}
	Mesh(std::vector<GrassBlade> vertices, vector<Texture> textures)
	{
		this->grassvertic = vertices;
		this->textures = textures;
		setupMeshGrass();
		/*	this->textures = textures;*/
	}
	Mesh(vector<GrassPlaneVertex> vertices, vector<unsigned int> indices)
	{
		this->grassplanevertices = vertices;
		this->indices = indices;
		/*this->textures = textures;*/
		setupMeshGrassPlane();
	}

	// render the mesh
	void Draw(Shader shader)
	{
		// bind appropriate textures
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
			// retrieve texture number (the N in diffuse_textureN)
			string number;
			string name = textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++); // transfer unsigned int to stream
			else if (name == "texture_normal")
				number = std::to_string(normalNr++); // transfer unsigned int to stream
			else if (name == "texture_height")
				number = std::to_string(heightNr++); // transfer unsigned int to stream

													 // now set the sampler to the correct texture unit

			auto temp = glGetUniformLocation(shader.ID, (name + number).c_str());
			glUniform1i(-1, i);
			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		// draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}

	void DrawTerrain(Shader shader)
	{
		// bind appropriate textures
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
			// retrieve texture number (the N in diffuse_textureN)
			int number = i + 1;

			glUniform1i(glGetUniformLocation(shader.ID, "texture_ground" + number), i);
			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		// draw mesh
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, heightvertices.size());
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}

	void DrawGrass(Shader shader, glm::mat4 transform)
	{
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader.ID, "alphaTexture"), 0);
		glBindTexture(GL_TEXTURE_2D, textures[0].id);
		glActiveTexture(GL_TEXTURE1);
		glUniform1i(glGetUniformLocation(shader.ID, "diffuseTexture"), 1);
		glBindTexture(GL_TEXTURE_2D, textures[1].id);
		glActiveTexture(GL_TEXTURE2);
		glUniform1i(glGetUniformLocation(shader.ID, "forceMap"), 2);
		glBindTexture(GL_TEXTURE_2D, textures[2].id);

		glBindVertexArray(VAO);
		glm::mat4 origin = transform;
		for (int row = -1; row <= 1; ++row)
		{
			for (int col = -1; col <= 1; ++col)
			{
				glm::vec2 patchPos= { -0.5f + col * 1.0f, -0.5f + row * 1.0f };
				transform = glm::translate(origin, glm::vec3(col, 0, row));
				shader.setMat4("objectTransform", transform);
				shader.setVec2("patchPos", patchPos);
				/*glDrawArrays(GL_PATCHES, 0, grassvertic.size());*/
				glDrawArrays(GL_PATCHES, 0,grassvertices.size());

			}
		}
		glBindVertexArray(0);
		
	}
	void DrawGrassPlane(Shader shader,glm::mat4 transform)
	{
		// draw mesh
		glBindVertexArray(VAO);
		glm::mat4 origin = transform;

		for (int row = -1; row <= 1; ++row)
		{
			for (int col = -1; col <= 1; ++col)
			{
				transform = glm::translate(origin, glm::vec3(col, 0, row));
				//glm::vec2 patchPos = { -0.5f + col * 1.0f, -0.5f + row * 1.0f };
				shader.setMat4("object", transform);
					
				glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
				
			}
		}
		glBindVertexArray(0);
		// always good practice to set everything back to defaults once configured.
	/*	glActiveTexture(GL_TEXTURE0);*/
	}

private:
	/*  Render data  */
	unsigned int VBO, EBO;

	/*  Functions    */
	// initializes all the buffer objects/arrays
	void setupMesh()
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

		glBindVertexArray(0);
	}
	void setupMeshGrass()
	{
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, grassvertices.size() * sizeof(GrassVertex), &grassvertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GrassVertexPart), 0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GrassVertexPart), (void*)offsetof(GrassVertexPart, CentrePos));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(GrassVertexPart), (void*)offsetof(GrassVertexPart, TexCoords));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(GrassVertexPart), (void*)offsetof(GrassVertexPart, Random));
		glEnableVertexAttribArray(3);
	/*	glBufferData(GL_ARRAY_BUFFER, grassvertic.size() *(sizeof(GrassBlade) / sizeof(glm::vec4)), &grassvertic[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 4, 0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 4, (void*)sizeof(glm::vec4));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 4, (void*)(sizeof(glm::vec4)*2));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4) * 4, (void*)(sizeof(glm::vec4)*3));
		glEnableVertexAttribArray(3); 
*/

	}
	void setupMeshGrassPlane()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, grassplanevertices.size()*sizeof(GrassPlaneVertex), &grassplanevertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GrassPlaneVertex), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
	}
	void setupMeshTerrain()
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		/*glGenBuffers(1, &EBO);*/

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, heightvertices.size() * sizeof(HeightVertex), &heightvertices[0], GL_STATIC_DRAW);
		// set the vertex attribute pointers
		// vertex Positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(HeightVertex), (void*)0);
		glEnableVertexAttribArray(0);
		// vertex texture coords
		
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(HeightVertex), (void*)offsetof(HeightVertex, TexCoords));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}
};
#endif