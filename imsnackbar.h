#ifndef IM_SNACKBAR_H
#define IM_SNACKBAR_H

#pragma once

#define SNACKBAR_SPACING 4
#define SNACKBAR_RENDER_SIZE 4  // number of max snackbar on screen
#define SNACKBAR_MAX_WIDTH 250
#define SNACKBAR_MAX_CHAR 256

#include <string>
#include <vector>

#ifndef IMGUI_VERSION
#  error "include imgui.h before this header"
#endif

static const ImVec2 ImGuiSnackbarAlign_TopLeft      = ImVec2(0.0f, 0.0f);
static const ImVec2 ImGuiSnackbarAlign_TopCenter    = ImVec2(0.5f, 0.0f);
static const ImVec2 ImGuiSnackbarAlign_TopRight     = ImVec2(1.0f, 0.0f);
static const ImVec2 ImGuiSnackbarAlign_BottomLeft   = ImVec2(0.0f, 1.0f);
static const ImVec2 ImGuiSnackbarAlign_BottomCenter = ImVec2(0.5f, 1.0f);
static const ImVec2 ImGuiSnackbarAlign_BottomRight  = ImVec2(1.0f, 1.0f);

static const int ImGuiSnackbarDir_UpDown = 1;
static const int ImGuiSnackbarDir_DownUp = -1;

const ImGuiWindowFlags SNACKBAR_WINDOW_FLAGS =
    ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration |
    ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav |
    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;

class ImGuiSnackbar
{
 private:
  char msg[SNACKBAR_MAX_CHAR];
  double duration;
  double elapsed_time = 0;
  double prev_time    = -1;

  void Construct(int duration, std::string msg, va_list args)
  {
    this->duration = duration;

    va_list args_1;
    va_copy(args_1, args);

    int msg_size = std::vsnprintf(NULL, 0, msg.c_str(), args_1);

    va_end(args_1);

    va_list args_2;
    va_copy(args_2, args);

    std::vsnprintf(this->msg, msg_size + 1, msg.c_str(), args_2);

    va_end(args_2);
  }

 public:
  ImGuiSnackbar(int duration, std::string msg, ...)
  {
    va_list args;
    va_start(args, msg);

    this->Construct(duration, msg, args);

    va_end(args);
  }

  ImGuiSnackbar(std::string msg, ...)
  {
    va_list args;
    va_start(args, msg);

    this->Construct(3, msg, args);

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
};

namespace ImGui {

inline std::vector<ImGuiSnackbar> im_snackbars;

inline void Snackbar(const ImGuiSnackbar &snackbar) { im_snackbars.push_back(snackbar); }

inline void RemoveSnackbar(int index)
{
  im_snackbars.erase(im_snackbars.begin() + index);
}

inline void RenderSnackbar(ImVec2 anchor, ImVec2 align, int dir)
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

    SetNextWindowPos(
        ImVec2(anchor.x, anchor.y + (dir * height)), ImGuiCond_Always, align);

    std::string snackbar_name = "##Snackbar-" + std::to_string(i);
    ImGui::Begin(snackbar_name.c_str(), NULL, SNACKBAR_WINDOW_FLAGS);
    {
      ImGui::PushTextWrapPos(SNACKBAR_MAX_WIDTH);
      ImGui::Text(snackbar->GetMessage());
      ImGui::PopTextWrapPos();

      height += GetWindowHeight() + SNACKBAR_SPACING;
    }

    ImGui::End();
  }
};

}  // namespace ImGui

#endif
