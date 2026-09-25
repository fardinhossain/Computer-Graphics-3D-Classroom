#ifndef spotLight_h
#define spotLight_h

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"

// =========================================================================
// SpotLight Class: Models a directional cone-shaped light source (e.g. spotlight)
// with position, pointing direction, inner/outer cutoff angles, attenuation,
// and toggleable components.
// =========================================================================
class SpotLight {
public:
    glm::vec3 position;
    glm::vec3 direction;
    float cutOff;       // cosine of inner cutoff angle
    float outerCutOff;  // cosine of outer cutoff angle

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float k_c;  // constant attenuation
    float k_l;  // linear attenuation
    float k_q;  // quadratic attenuation

    int lightNumber;
    bool isOn;

    SpotLight(
        float posX, float posY, float posZ,
        float dirX, float dirY, float dirZ,
        float cutOffDeg, float outerCutOffDeg,
        float ambR, float ambG, float ambB,
        float diffR, float diffG, float diffB,
        float specR, float specG, float specB,
        float constant, float linear, float quadratic,
        int num = 1
    ) {
        position = glm::vec3(posX, posY, posZ);
        direction = glm::normalize(glm::vec3(dirX, dirY, dirZ));
        cutOff = glm::cos(glm::radians(cutOffDeg));
        outerCutOff = glm::cos(glm::radians(outerCutOffDeg));

        ambient = glm::vec3(ambR, ambG, ambB);
        diffuse = glm::vec3(diffR, diffG, diffB);
        specular = glm::vec3(specR, specG, specB);

        k_c = constant;
        k_l = linear;
        k_q = quadratic;

        lightNumber = num;
        isOn = true;
    }

    // Set uniforms in the lighting shader for this spotlight
    void setUpSpotLight(Shader& lightingShader)
    {
        lightingShader.use();

        std::string prefix = "spotLight.";

        lightingShader.setVec3(prefix + "position", position);
        lightingShader.setVec3(prefix + "direction", direction);
        lightingShader.setFloat(prefix + "cutOff", cutOff);
        lightingShader.setFloat(prefix + "outerCutOff", outerCutOff);

        if (isOn) {
            lightingShader.setVec3(prefix + "ambient", ambientOn * ambient);
            lightingShader.setVec3(prefix + "diffuse", diffuseOn * diffuse);
            lightingShader.setVec3(prefix + "specular", specularOn * specular);
        } else {
            lightingShader.setVec3(prefix + "ambient", glm::vec3(0.0f));
            lightingShader.setVec3(prefix + "diffuse", glm::vec3(0.0f));
            lightingShader.setVec3(prefix + "specular", glm::vec3(0.0f));
        }

        lightingShader.setFloat(prefix + "k_c", k_c);
        lightingShader.setFloat(prefix + "k_l", k_l);
        lightingShader.setFloat(prefix + "k_q", k_q);
        lightingShader.setBool(prefix + "isOn", isOn);
    }

    void turnOff()
    {
        isOn = false;
    }

    void turnOn()
    {
        isOn = true;
    }

    void toggle()
    {
        isOn = !isOn;
    }

    void turnAmbientOn() { ambientOn = 1.0f; }
    void turnAmbientOff() { ambientOn = 0.0f; }
    void turnDiffuseOn() { diffuseOn = 1.0f; }
    void turnDiffuseOff() { diffuseOn = 0.0f; }
    void turnSpecularOn() { specularOn = 1.0f; }
    void turnSpecularOff() { specularOn = 0.0f; }

private:
    float ambientOn = 1.0f;
    float diffuseOn = 1.0f;
    float specularOn = 1.0f;
};

#endif /* spotLight_h */
