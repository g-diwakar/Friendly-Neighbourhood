#version 330 core
out vec4 FragColor;

const int MAX_LENGTH = 20;

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

struct Bulbs
{
	BaseLight base;

	vec3 position ;
	vec3 direction;

	float cutoff_angle;
	
	float constant;
	float linear;
	float quadratic;
		
};

uniform Material material;

uniform int NUM_STREET_BULBS;
uniform Bulbs street_bulbs[MAX_LENGTH];

uniform sampler2D texture_diffuse1;
uniform  bool hasTexture;
uniform bool isStreetLight;

uniform SunLight sunlight;
uniform vec3 viewPos;

uniform bool nightMode;


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

vec4 CalcSpotLight(Bulbs street_bulbs, vec3 normal)
{
		vec3 lightDir = FragPos-street_bulbs.position;
		float spotStrength = dot(normalize(lightDir),normalize(street_bulbs.direction));

		if(spotStrength>street_bulbs.cutoff_angle)
		{
				float distance = length(lightDir);
				lightDir = normalize(-lightDir);

				vec4 Color = CalcLightIntensity(street_bulbs.base,lightDir,normal);
				float attenuationFactor = street_bulbs.constant + (street_bulbs.linear * distance) + (street_bulbs.quadratic * distance * distance);

			    float spotLightIntensity = 1.0-((1.0-spotStrength)/(1.0-street_bulbs.cutoff_angle));	

			//return Color /attenuationFactor;
				//return Color * spotLightIntensity / attenuationFactor;
				return Color * spotLightIntensity;
		}
		else
		{
				return vec4(0,0,0,0);
			}


	}

void main()
{    
	vec3 normal = normalize(Normal);
	vec4 totalLight;
	
	if(nightMode)
	{
		for(int i=0;i<NUM_STREET_BULBS;++i)
		{
			totalLight+=CalcSpotLight(street_bulbs[i],normal);
		}

		if(isStreetLight)
			totalLight = vec4(255,178,0,1);
	}	

	else 
	{
		totalLight = CalcDirectionalLight(normal);
	}


	 if (hasTexture)
		 FragColor = texture(texture_diffuse1,TexCoords) * totalLight;
	else 
		FragColor = totalLight;

}
