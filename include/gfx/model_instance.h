// This class represents a physical instantiation of a ModelInfo. In other words, a ModelInfo
// describes a model, but we must use that ModelInfo to create a ModelInstance which represents
// the model in physical space. The ModelInstance allows the developer to specify position, scale,
// and rotation.
//
// Brian Ho (brian@brkho.com)

#ifndef GFX_MODEL_INSTANCE_H
#define GFX_MODEL_INSTANCE_H

#include "gfx/color.h"
#include "gfx/model_info.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace gfx {

class ModelInstance {
  public:
    // Position of the model in 3D scene space.
    glm::vec3 position;
    // Scale of the model.
    glm::vec3 scale;
    // Rotation of the model as a quaternion.
    glm::quat rotation;
    // Color of the model.
    gfx::Color color;

    // Default constructor which initializes the position to origin, the scale to (1.0, 1.0, 1.0),
    // the rotation to no rotation, and the color to white.
    ModelInstance(gfx::ModelInfo* model_info);

    // Constructor that creates a ModelInstance with a specified position, but defaults scale,
    // rotation, and color.
    ModelInstance(gfx::ModelInfo* model_info, glm::vec3 position);

    // Constructor that creates a ModelInstance with all fields specified.
    ModelInstance(gfx::ModelInfo* model_info, glm::vec3 position, glm::vec3 scale,
        glm::quat rotation, gfx::Color color);

    // Updates the model and normal transforms from the position, scale, and rotation. This must be
    // called after any changes to the ModelInstance properties.
    void Update();

    // Draws the ModelInstance to the current OpenGL context given a shader program.
    void Draw(GLuint program);
  private:
    // The underlying ModelInfo that ther object is an instance of.
    gfx::ModelInfo* model_info;

    // The model transform.
    glm::mat4 model_transform;

    // The normal transform.
    glm::mat4 normal_transform;
};

}
#endif // GFX_MODEL_INSTANCE_H
