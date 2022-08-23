#include "../includes/model.h"
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>


void Model::loadModel(string const &path)
{
    // read file via ASSIMP
    Assimp::Importer importer;

      // const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices  |aiProcess_SortByPType | aiProcess_FlipUVs);
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }

    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);
}


void Model::processNode(aiNode *node, const aiScene *scene)
{
    // process each mesh located at the current node
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }

}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    // data to fill
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
	

	   Vertex vertex;
        glm::vec3 vector; 
		glm::vec3 bulbPosition;//for street bulbs
    // walk through each of the mesh's vertices
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.

        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
		bulbPosition = vector;

        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
            // texture coordinates
        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
		}
        else
		{
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

        vertices.push_back(vertex);
    }

    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        // retrieve all indices of the face and store them in the indices vector
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);        
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    

    //get the street light position and other parameters
	aiString materialName;
	aiGetMaterialString(material, AI_MATKEY_NAME, &materialName);

	//checking the material name 
	bool isLeftLight = strcmp(materialName.C_Str(),"street_light_left") ==0;
	bool isRightLight = strcmp(materialName.C_Str(),"street_light_right") ==0;
	bool isGlass = strcmp(materialName.C_Str(),"glass") == 0;

	//setting property for street light
	if(isLeftLight || isRightLight)
	//if(strcmp(materialName.C_Str(),"street_light")==0)
	{
		Bulbs bulb;

		bulb.position = bulbPosition;
		bulb.ambient = glm::vec3(0.24725,0.1995,0.0745);
        bulb.diffuse = glm::vec3(0.75164,0.60648,0.22648);
        bulb.specular = glm::vec3(0.628281,0.555802,0.366065);

		bulb.direction = isLeftLight ? glm::vec3(1.0f,-1.0f,1.0f) : glm::vec3(-1.5f,-1.0f,1.0f);
		//bulb.direction = glm::vec3(0.0f,-1.0f,0.0f);

		bulb.angle = 20.0f * 3.145/180.0f;
		bulb.constant = 1.0f;
		bulb.linear = 0.0f;
		bulb.quadratic = 0.1f;

		
		bulbs.push_back(bulb);
		
	}
	



	//read ambient , diffuse , specular and transparency of the material 

	Material mat;
	aiColor3D color;

	float shininess, transparency;
    material->Get(AI_MATKEY_SHININESS, shininess);
    mat.shininess = shininess;
    material->Get(AI_MATKEY_COLOR_TRANSPARENT, transparency);
	transparency = isGlass ? 0.2f : 1.0f;
    material->Get(AI_MATKEY_COLOR_AMBIENT, color);
    mat.Ka = glm::vec4(color.r,color.g,color.b,transparency);
    material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    mat.Kd = glm::vec4(color.r,color.g,color.b,transparency);       
    material->Get(AI_MATKEY_COLOR_SPECULAR, color);
    mat.Ks = glm::vec4(color.r,color.g,color.b,transparency);




    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

	
    // 1. diffuse maps
	if(material->GetTextureCount(aiTextureType_DIFFUSE) == 0) hasTexture = false;
	else hasTexture = true;
	std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	

    // 2. specular maps
	std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    // 3. normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

    // 4. height maps
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        
    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, textures,mat,hasTexture,materialName);
}


vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;

    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;

        for(unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if(!skip)
        {   // if texture hasn't been loaded already, load it

            Texture texture;
            texture.id = DoLoadTextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }
    return textures;
}

unsigned int DoLoadTextureFromFile(const char *path, const string &directory, bool gamma)
{
	string filename = string(path);
    filename = directory + '/' + filename;

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
void Model::Draw(Shader &shader)
{

	shader.use();
	if(bulbs.size()>0)
	{
		shader.setInt("NUM_STREET_BULBS",bulbs.size());

		for(int i=0;i<(int)bulbs.size();++i)
		{
			//std::cout<<bulbs[i].position.x<<" "<<bulbs[i].position.y<<" "<<bulbs[i].position.z<<std::endl;
			shader.setVec3((string("street_bulbs[")+to_string(i)+string("].position")).c_str(), bulbs[i].position);
			shader.setVec3((string("street_bulbs[")+to_string(i)+string("].base.ambient")).c_str(), bulbs[i].ambient);
			shader.setVec3((string("street_bulbs[")+to_string(i)+string("].base.diffuse")).c_str(), bulbs[i].diffuse);
			shader.setVec3((string("street_bulbs[")+to_string(i)+string("].base.specular")).c_str(), bulbs[i].specular);

			shader.setVec3((string("street_bulbs[")+to_string(i)+string("].direction")).c_str(), bulbs[i].direction);

			shader.setFloat((string("street_bulbs[")+to_string(i)+string("].cutoff_angle")).c_str(), cos(bulbs[i].angle));

			shader.setFloat(string("street_bulbs[")+to_string(i)+string("].constant").c_str(),bulbs[i].constant);
			shader.setFloat(string("street_bulbs[")+to_string(i)+string("].linear").c_str(),bulbs[i].linear);
			shader.setFloat(string("street_bulbs[")+to_string(i)+string("].quadratic").c_str(),bulbs[i].quadratic);
		}
	}
	else
		shader.setInt("NUM_STREET_BULBS",0);

	for(unsigned int i = 0; i < meshes.size(); i++)
    	meshes[i].Draw(shader);
}
