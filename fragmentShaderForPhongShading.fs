#version 330 core
out vec4 FragColor;

// Material properties
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

// Point light definition (omni-directional with distance attenuation)
struct PointLight {
    vec3 position;
    
    float k_c;  // constant attenuation factor
    float k_l;  // linear attenuation factor
    float k_q;  // quadratic attenuation factor
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// Spot light definition (directed cone light with smooth edges and attenuation)
struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;       // cos of inner cutoff angle
    float outerCutOff;  // cos of outer cutoff angle
    
    float k_c;  // constant attenuation factor
    float k_l;  // linear attenuation factor
    float k_q;  // quadratic attenuation factor
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    bool isOn;
};

#define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;

// Function prototypes
vec3 CalcPointLight(Material material, PointLight light, vec3 N, vec3 fragPos, vec3 V);
vec3 CalcSpotLight(Material material, SpotLight light, vec3 N, vec3 fragPos, vec3 V);

void main()
{
    // Surface normal and view direction vectors
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);
    
    vec3 result = vec3(0.0);
    
    // 1. Calculate lighting from all active point lights (ceiling lamps)
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        result += CalcPointLight(material, pointLights[i], N, FragPos, V);
    }
    
    // 2. Calculate lighting from the spotlight (focused on blackboard / podium)
    result += CalcSpotLight(material, spotLight, N, FragPos, V);
      
    FragColor = vec4(result, 1.0);
}

// Calculates color contribution from a point light
vec3 CalcPointLight(Material material, PointLight light, vec3 N, vec3 fragPos, vec3 V)
{
    vec3 L = normalize(light.position - fragPos);
    vec3 R = reflect(-L, N);
    
    vec3 K_A = material.ambient;
    vec3 K_D = material.diffuse;
    vec3 K_S = material.specular;
    
    // Distance attenuation: 1.0 / (k_c + k_l * d + k_q * d^2)
    float d = length(light.position - fragPos);
    float attenuation = 1.0 / (light.k_c + light.k_l * d + light.k_q * (d * d));
    
    vec3 ambient = K_A * light.ambient;
    vec3 diffuse = K_D * max(dot(N, L), 0.0) * light.diffuse;
    vec3 specular = K_S * pow(max(dot(V, R), 0.0), material.shininess) * light.specular;
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}

// Calculates color contribution from a spotlight with smooth soft edges
vec3 CalcSpotLight(Material material, SpotLight light, vec3 N, vec3 fragPos, vec3 V)
{
    if (!light.isOn)
        return vec3(0.0);

    vec3 L = normalize(light.position - fragPos);
    
    // Spotlight cone intensity calculation: (theta - outer) / (inner - outer)
    float theta = dot(L, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // Distance attenuation
    float d = length(light.position - fragPos);
    float attenuation = 1.0 / (light.k_c + light.k_l * d + light.k_q * (d * d));
    
    vec3 K_A = material.ambient;
    vec3 K_D = material.diffuse;
    vec3 K_S = material.specular;
    
    vec3 ambient = K_A * light.ambient;
    vec3 diffuse = K_D * max(dot(N, L), 0.0) * light.diffuse;
    
    vec3 R = reflect(-L, N);
    vec3 specular = K_S * pow(max(dot(V, R), 0.0), material.shininess) * light.specular;
    
    ambient *= attenuation;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
    return (ambient + diffuse + specular);
}
