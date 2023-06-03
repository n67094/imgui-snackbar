#ifndef IM_SNACKBAR_H
#define IM_SNACKBAR_H

#pragma once

#define SNACKBAR_SPACING 4
#define SNACKBAR_RENDER_SIZE 4  // number of max snackbar on screen
#define SNACKBAR_WIDTH 250
#define SNACKBAR_DIRECTION -1  // -1 bottom to top. 1 top to bottom

#include <iostream>
#include <string>
#include <vector>

// #include "imgui.h"

#ifndef IMGUI_VERSION
#  error "include imgui.h before this header"
#endif

enum ImGuiSnackbarType
{
  ImGuiSnackbarType_Info,
  ImGuiSnackbarType_Success,
  ImGuiSnackbarType_Warning,
  ImGuiSnackbarType_Error,
};

const ImGuiWindowFlags SNACKBAR_WINDOW_FLAGS =
    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav |
    ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize;

class ImGuiSnackbar
{
 private:
  char msg[256];
  ImGuiSnackbarType type;
  double duration;
  double elapsed_time = 0;
  double prev_time    = -1;

  // TODO REMOVE MALLOC or use a list instead of vector
  void Construct(ImGuiSnackbarType type, int duration, std::string msg, va_list args)
  {
    this->type     = type;
    this->duration = duration;
    // this->msg      = NULL;

    va_list args_1;
    va_copy(args_1, args);

    int msg_size = std::vsnprintf(NULL, 0, msg.c_str(), args_1);

    va_end(args_1);

    va_list args_2;
    va_copy(args_2, args);

    // this->msg = (char *)malloc(sizeof(char) * (msg_size + 1));
    std::vsnprintf(this->msg, msg_size + 1, msg.c_str(), args_2);

    va_end(args_2);
  }

 public:
  ImGuiSnackbar(ImGuiSnackbarType type, int duration, std::string msg, ...)
  {
    va_list args;
    va_start(args, msg);

    this->Construct(type, duration, msg, args);

    va_end(args);
  }

  ImGuiSnackbar(ImGuiSnackbarType type, std::string msg, ...)
  {
    va_list args;
    va_start(args, msg);

    this->Construct(type, 3, msg, args);

    va_end(args);
  }

  char *GetMessage(void) { return this->msg; }

  int GetElapsedTime(void) { return elapsed_time; }

  bool IsTimeout(void) { return this->elapsed_time >= duration; }

  void UpdateTimer(void)
  {
    if (this->prev_time == -1) {
      this->prev_time = ImGui::GetTime();
      return;
    }

    double time = ImGui::GetTime();

    this->elapsed_time += time - this->prev_time;
    this->prev_time = time;
  }

  ~ImGuiSnackbar(void)
  {
    /*
    if (this->msg != NULL) {
      free(this->msg);
      this->msg = NULL;
    }
    */
  }
};

namespace ImGui {

inline std::vector<ImGuiSnackbar> im_snackbars;

inline void AddSnackbar(const ImGuiSnackbar &snackbar)
{
  im_snackbars.push_back(snackbar);
}

inline void RemoveSnackbar(int index)
{
  im_snackbars.erase(im_snackbars.begin() + index);
}

inline void RenderSnackbar(ImVec2 pos, ImVec2 pivot = ImVec2(0, 0))
{
  float height = 0.0f;

  int snackbar_render_size = SNACKBAR_RENDER_SIZE;
  if (im_snackbars.size() < SNACKBAR_RENDER_SIZE)
    snackbar_render_size = im_snackbars.size();

  for (int i = 0; i < snackbar_render_size; ++i) {
    ImGuiSnackbar *snackbar = &im_snackbars[i];

    snackbar->UpdateTimer();

    if (snackbar->IsTimeout()) {
      RemoveSnackbar(i);
      continue;
    }

    ImVec2 snackbar_size =
        ImGui::CalcTextSize(snackbar->GetMessage(), NULL, false, SNACKBAR_WIDTH);
    snackbar_size.x += ImGui::GetStyle().WindowPadding.x * 2;
    snackbar_size.y += ImGui::GetStyle().WindowPadding.y * 2;

    if (SNACKBAR_DIRECTION == -1) height += snackbar_size.y;

    SetNextWindowPos(ImVec2(pos.x, pos.y + (SNACKBAR_DIRECTION * height)));
    SetNextWindowSizeConstraints(snackbar_size, snackbar_size);

    std::string snackbar_name = "##Snackbar-" + std::to_string(i);

    ImGui::Begin(snackbar_name.c_str(), NULL, SNACKBAR_WINDOW_FLAGS);
    {
      ImGui::PushTextWrapPos(SNACKBAR_WIDTH);
      ImGui::Text(snackbar->GetMessage());
      ImGui::PopTextWrapPos();
    }
    ImGui::End();

    height += SNACKBAR_SPACING;

    if (SNACKBAR_DIRECTION == 1) height += snackbar_size.y;
  }
};

}  // namespace ImGui

#endif
