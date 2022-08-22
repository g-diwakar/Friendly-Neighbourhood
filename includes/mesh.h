#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
    unsigned int VAO;

    Mesh(vector<Vertex> &vertices, vector<unsigned int> &indices, vector<Texture> &textures, Material &mat,bool& hasTexture): vertices(vertices),indices(indices),textures(textures),mat(mat),hasTexture(hasTexture)
    {
        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
	}


    // render the mesh
    void Draw(Shader &shader) ;
    

    
};
#endif
