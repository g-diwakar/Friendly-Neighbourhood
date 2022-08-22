#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float shininess;
};

struct BaseLight
{
		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
};

struct SunLight
{
		BaseLight base;

		vec3 position;
		vec3 direction; //sunlight is a directional light 
};


uniform Material material;

uniform sampler2D texture_diffuse1;
uniform  bool hasTexture;

uniform SunLight sunlight;
uniform vec3 viewPos;


vec4 CalcLightIntensity(BaseLight light, vec3 Lightdir, vec3 normal)
{
	vec4 ambientColor = vec4(light.ambient,1.0) * material.ambient.rgba;

	float diffuseStrength = dot(normal,Lightdir);

	vec4 diffuseColor = vec4(0,0,0,0);
	vec4 specularColor = vec4(0,0,0,0);

	if(diffuseStrength>0)
	{
			//diffuse 
			diffuseColor = vec4(light.diffuse,1.0) * material.diffuse.rgba * diffuseStrength;

			//specular 
			vec3 viewDir = normalize(viewPos - FragPos);
			vec3 reflectDir = normalize(reflect(-Lightdir, normal));
			float specularStrength = dot(viewDir,reflectDir);

			if(specularStrength > 0)
			{
				specularColor = vec4(light.specular,1.0) * material.specular.rgba * pow(specularStrength,material.shininess);
			}


	}

	return (ambientColor + diffuseColor + specularColor);

}

vec4 CalcDirectionalLight(vec3 normal)
{
		vec3 dir = normalize (sunlight.position - FragPos);
		//vec3 dir = normalize(sunlight.direction);
		return CalcLightIntensity(sunlight.base, dir, normal);
}

void main()
{    
	vec3 normal = normalize(Normal);
	vec4 totalLight = CalcDirectionalLight(normal);

//   	if (texture(texture_diffuse1,TexCoords) == vec4(0,0,0,0))
//		FragColor = totalLight;
//	else 
//		FragColor = texture(texture_diffuse1,TexCoords) * totalLight;
     
	 if (hasTexture)
		 FragColor = texture(texture_diffuse1,TexCoords) * totalLight;
	else 
		FragColor = totalLight;

}
