#include "imgui.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <curl/curl.h>
#include <filesystem>
#include <iostream>
#include <stdio.h>
#include <string>
#define GL_SILENCE_DEPRECATION

static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

int main(int, char **) {
  CURL *curl;
  CURLcode res;
  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();

  ImVec2 windowS = ImVec2(400, 450);

  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return 1;

  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);
  GLFWwindow *window = glfwCreateWindow(
      windowS.x, windowS.y, "Dear ImGui Login", nullptr, nullptr);
  if (window == nullptr)
    return 1;
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  glfwSetWindowSize(window, windowS.x, windowS.y);
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImFont *font = nullptr;
  std::filesystem::path font_path =
      "/usr/share/fonts/TTF/JetBrainsMono-Regular.ttf";
  font = io.Fonts->AddFontFromFileTTF(font_path.string().c_str(), 18.0f);
  if (!font) {
    std::cerr << "Font did not load: " << font_path << std::endl;
  }
  io.FontDefault = font;

  ImFont *fontbig = nullptr;
  std::filesystem::path bigfont_path =
      "/usr/share/fonts/TTF/JetBrainsMono-Regular.ttf";
  fontbig = io.Fonts->AddFontFromFileTTF(bigfont_path.string().c_str(), 36.0f);
  if (!fontbig) {
    std::cerr << "Font did not load: " << bigfont_path << std::endl;
  }

  ImGui::StyleColorsDark();
  ImGui::GetStyle().Colors[ImGuiCol_WindowBg] =
      ImVec4(0.12f, 0.12f, 0.12f, 0.2f);
  ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0.27f, 0.47f, 0.78f, 1.0f);
  ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] =
      ImVec4(0.36f, 0.61f, 0.98f, 1.0f);
  ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] =
      ImVec4(0.20f, 0.40f, 0.65f, 1.0f);
  ImGui::GetStyle().Colors[ImGuiCol_FrameBg] =
      ImVec4(0.16f, 0.16f, 0.16f, 1.0f);
  ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered] =
      ImVec4(0.22f, 0.22f, 0.22f, 1.0f);
  ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive] =
      ImVec4(0.28f, 0.28f, 0.28f, 1.0f);
  ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
  ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
  ImGuiWindowFlags flags =
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove;

  static char username[30] = "";
  static char password[30] = "";
  enum LoginState { LOGIN_SCREEN, SUCCESS_SCREEN, ERROR_SCREEN };
  static LoginState currentState = LOGIN_SCREEN;
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    if (currentState == LOGIN_SCREEN) {
      ImGui::SetNextWindowSize(windowS);
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::Begin("LOGIN", nullptr, flags);

      ImVec2 windowSize = windowS;

      ImVec2 inputSize(200, 25);
      ImVec2 buttonSize(200, 30);
      float formHeight = inputSize.y * 2 + buttonSize.y + 30;
      float formWidth = inputSize.x;

      float centerX = (windowSize.x - formWidth) / 2.0f;
      float centerY = (windowSize.y - formHeight) / 2.6f;

      ImGui::SetCursorPos(ImVec2(centerX, centerY));
      ImGui::PushFont(fontbig);
      ImGui::Text("Login");
      ImGui::PopFont();
      ImGui::Spacing();

      ImGui::SetCursorPos(ImVec2(centerX, ImGui::GetCursorPosY() + 10));
      ImGui::Text("Username:");
      ImGui::SetCursorPosX(centerX);
      ImGui::InputTextEx("##Username", "", username, sizeof(username),
                         inputSize, ImGuiInputTextFlags_None);

      ImGui::Spacing();
      ImGui::SetCursorPos(ImVec2(centerX, ImGui::GetCursorPosY() + 10));
      ImGui::Text("Password:");
      ImGui::SetCursorPosX(centerX);
      ImGui::InputTextEx("##Password", "", password, sizeof(password),
                         inputSize, ImGuiInputTextFlags_Password);

      ImGui::Spacing();
      ImGui::SetCursorPos(ImVec2(centerX, ImGui::GetCursorPosY() + 10));
      if (ImGui::Button("Login", buttonSize)) {
        if (curl) {
          std::string responseBody;
          char postData[256];
          snprintf(postData, sizeof(postData), "username=%s&password=%s",
                   username, password);

          curl_easy_setopt(curl, CURLOPT_URL, "http://localhost/api/login.php");
          curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);
          curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
          curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);

          res = curl_easy_perform(curl);
          if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
          } else {
            if (responseBody.find("\"status\":\"success\"") !=
                std::string::npos) {
              currentState = SUCCESS_SCREEN;
            } else {
              currentState = ERROR_SCREEN;
            }
          }
        }
      }

      ImGui::End();
    } else if (currentState == SUCCESS_SCREEN) {
      ImGui::SetNextWindowSize(windowS);
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::Begin("Success", nullptr, flags);
      ImGui::SetCursorPos(ImVec2{windowS.x / 2, windowS.y / 2});
      ImGui::PushFont(fontbig);
      ImGui::Text("Welcome!");
      ImGui::PopFont();
      if (ImGui::Button("Logout")) {
        currentState = LOGIN_SCREEN;
      }
      ImGui::End();
    } else if (currentState == ERROR_SCREEN) {
      ImGui::SetNextWindowSize(windowS);
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::Begin("Error", nullptr, flags);
      ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
                         "Incorrect username or password.");
      if (ImGui::Button("Retry")) {
        currentState = LOGIN_SCREEN;
      }
      ImGui::End();
    }

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  if (curl) {
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();

  return 0;
}
