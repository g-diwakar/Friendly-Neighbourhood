#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<assimp/types.h>
#include<string>

#include "shader.h"

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
};

struct Material 
{
	//ambient
	glm::vec4 Ka;
	//diffuse 
	glm::vec4 Kd;
    //specular
	glm::vec4 Ks;

	//shininess 
	float shininess;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh();

public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
	Material mat;
	bool hasTexture;
	bool isStreetLight;
	aiString materialName;
    unsigned int VAO;

    Mesh(vector<Vertex> &vertices, vector<unsigned int> &indices, vector<Texture> &textures, Material &mat,bool& hasTexture,aiString& materialName): vertices(vertices),indices(indices),textures(textures),mat(mat),hasTexture(hasTexture),materialName(materialName)
    {
		if(std::strcmp(materialName.C_Str(),"street_light_right") == 0 || std::strcmp(materialName.C_Str(), "street_light_left") ==0)
		//if(std::strcmp(materialName.C_Str(),"street_light")==0)
			isStreetLight = true;
		else
			isStreetLight = false;
        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
	}


    // render the mesh
    void Draw(Shader &shader) ;
    

    
};
#endif
