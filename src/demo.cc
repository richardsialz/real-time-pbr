// This is the main entry point of the demo program.
// Brian Ho (brian@brkho.com)

#include "gfx/camera.h"
#include "gfx/color.h"
#include "gfx/directional_light.h"
#include "gfx/game_window.h"
#include "gfx/model_info.h"
#include "gfx/model_instance.h"
#include "gfx/texture_manager.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>

const int kWindowWidth = 1280;
const int kWindowHeight = 800;
const double kRotateSensitivity = 0.005;
const double kPanSensitivity = 0.005;
const double kZoomSensitivity = 0.01;
const std::string kVertexShaderPath = "shaders/main.vert";
const std::string kFragmentShaderPath = "shaders/main.frag";

struct Position {
  double x;
  double y;
};

gfx::Camera camera;
Position previous_pos;
double current_yaw;
double current_pitch;
double distance;
bool keys[1024];
bool clicking = false;
glm::vec3 pan_offset;

void update_camera() {
  // Convert from spherical coordinates to Cartesian coordinates.
  camera.camera_position = glm::vec3(
      distance * sin(current_pitch) * cos(current_yaw),
      distance * sin(current_pitch) * sin(current_yaw),
      distance * cos(current_pitch));
  camera.camera_position += pan_offset;
  float up_direction = sin(current_pitch) > 0 ? 1.0 : -1.0;
  camera.camera_target = pan_offset;
  camera.camera_up = glm::vec3(0.0f, 0.0f, up_direction);
}

void initialize_camera() {
  current_yaw = 0.0;
  current_pitch = 0.78;
  distance = 3.0;
  pan_offset = glm::vec3(0.0, 0.0, 0.0);
  update_camera();
}

// The callback for a key event that gets regstered with GLFW.
void key_callback(GLFWwindow* /* window */, int key, int /* scancode */, int action, int /* mode */) {
  if(action == GLFW_PRESS) {
    keys[key] = true;
  } else if(action == GLFW_RELEASE) {
    keys[key] = false;
  }
}

// The callback for a mouse click that gets registered with GLFW.
void mouse_button_callback(GLFWwindow* window, int button, int action, int /* mods */) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    previous_pos.x = x;
    previous_pos.y = y;
    clicking = action == GLFW_PRESS;
  }
}

// The callback for a mouse scroll that gets registered with GLFW.
void scroll_callback(GLFWwindow* /* window */, double /* x */, double y) {
  distance = std::max(0.1, distance - y * kZoomSensitivity);
}

void handle_input(GLFWwindow* window) {
  if (keys[GLFW_KEY_ESCAPE]) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  } else if (keys[GLFW_KEY_F]){
    initialize_camera();
  }
  if (clicking) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    if (keys[GLFW_KEY_LEFT_ALT]) {
      current_yaw += (previous_pos.x - x) * kRotateSensitivity;
      current_pitch += (previous_pos.y - y) * kRotateSensitivity;
    } else if (keys[GLFW_KEY_LEFT_CONTROL]) {
      pan_offset += camera.GetRightVector() * (previous_pos.x - x) * kPanSensitivity;
      pan_offset -= camera.GetUpVector() * (previous_pos.y - y) * kPanSensitivity;
    }
    previous_pos.x = x;
    previous_pos.y = y;
  }
}

// Main point of entry for the code.
int main(int /* argc */, char* /* argv */[]) {
  camera = gfx::Camera();
  initialize_camera();
  gfx::GameWindow game_window(kWindowWidth, kWindowHeight, kVertexShaderPath, kFragmentShaderPath,
      &camera, 45.0f, gfx::Color(0.15f, 0.15f, 0.15f));
  gfx::DirectionalLight directional_light = gfx::DirectionalLight(glm::vec3(-1.0f, 1.0f, -1.0f),
      glm::vec3(2.5f, 2.5f, 2.5f));
  game_window.SetDirectionalLight(&directional_light);

  gfx::TextureManager texture_manager;

  gfx::ModelInfo prism_info = gfx::ModelInfo("assets/prism.fbx", &texture_manager, true);
  gfx::ModelInstance prism_instance = gfx::ModelInstance(&prism_info,
      glm::vec3(0.0f, 0.0f, 0.0f));
  prism_instance.scale = glm::vec3(1.0f, 1.0f, 1.0f);
  prism_instance.Update();

  // gfx::ModelInfo box_info = gfx::ModelInfo("assets/box.obj", &texture_manager, true);
  // gfx::ModelInstance box_instance = gfx::ModelInstance(&box_info,
  //     glm::vec3(0.0f, 0.0f, 0.0f));
  // box_instance.scale = glm::vec3(1.0f, 1.0f, 1.0f);
  // box_instance.Update();

  // gfx::ModelInfo cylinder_info = gfx::ModelInfo("assets/plane.fbx", &texture_manager, true);
  // gfx::ModelInstance cylinder_instance = gfx::ModelInstance(&cylinder_info,
  //     glm::vec3(0.0f, 0.0f, 0.0f));
  // // cylinder_instance.scale = glm::vec3(1.0f, 1.0f, 1.0f);
  // cylinder_instance.Update();

  // gfx::ModelInfo buddha_info = gfx::ModelInfo("assets/buddha.obj", true);
  // gfx::ModelInstance buddha_instance = gfx::ModelInstance(&buddha_info,
  //     glm::vec3(0.0f, 2.5f, 0.0f));
  // buddha_instance.scale = glm::vec3(0.1f, 0.1f, 0.1f);
  // buddha_instance.color = gfx::Color(1.0f, 0.0f, 0.0f);
  // buddha_instance.Update();

  std::fill_n(keys, 1024, 0);
  glfwSetKeyCallback(game_window.window, key_callback);
  glfwSetMouseButtonCallback(game_window.window, mouse_button_callback);
  glfwSetScrollCallback(game_window.window, scroll_callback);

  double fps_print_time = 2.5;
  double last_time = game_window.GetElapsedTime();

  // Main rendering loop.
  while(game_window.IsRunning()) {
    double current_time = game_window.GetElapsedTime();
    double frame_time = current_time - last_time;
    fps_print_time -= frame_time;
    if (fps_print_time <= 0.0) {
      std::cout << "FPS: " << 1.0 / frame_time << std::endl;
      fps_print_time = 2.5;
    }
    last_time = current_time;

    // Move the light in a circle.
    // box_instance.position = glm::vec3(-sin(current_time) * 5.0, -cos(current_time) * 5.0, 0.0f);
    // box_instance.Update();

    directional_light.direction = glm::vec3(sin(current_time), cos(current_time), 0.0f);
    game_window.UpdateDirectionalLight();

    game_window.PollForEvents();
    handle_input(game_window.window);
    update_camera();

    game_window.PrepareRender();
    // game_window.RenderModel(&box_instance);
    game_window.RenderModel(&prism_instance);
    game_window.FinishRender();
  }

  glfwTerminate();
  return EXIT_SUCCESS;
}