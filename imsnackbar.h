#ifndef IM_SNACKBAR_H
#define IM_SNACKBAR_H

#pragma once

#define SNACKBAR_SPACING 4
#define SNACKBAR_RENDER_SIZE 4  // number of max snackbar on screen
#define SNACKBAR_MAX_WIDTH 250
#define SNACKBAR_MAX_CHAR 256

#include <string>
#include <vector>

#include "imgui.h"

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
    ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize |
    ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration |
    ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing |
    ImGuiWindowFlags_NoBringToFrontOnFocus;

enum ImGuiSnackbarCol
{
  ImGuiSnackbarCol_Text,
  ImGuiSnackbarCol_Background,
};

class ImGuiSnackbar
{
 private:
  char msg[SNACKBAR_MAX_CHAR];
  double duration;
  double elapsed_time = 0;
  double prev_time    = -1;
  ImVec4 background_col;
  ImVec4 text_col;
  bool has_background_col = false;
  bool has_text_col       = false;

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

  void SetBackgroundColor(ImVec4 color)
  {
    this->has_background_col = true;
    this->background_col     = color;
  };

  ImVec4 GetBackgroundColor(void) { return this->background_col; };

  void SetTextColor(ImVec4 color)
  {
    this->has_text_col = true;
    this->text_col     = color;
  };

  ImVec4 GetTextColor(void) { return this->text_col; };

  bool HasBackgroundColor(void) { return this->has_background_col; }

  bool HasTextColor(void) { return this->has_text_col; }

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

struct ImGuiSnackbarStyle
{
  ImGuiSnackbarCol idx;
  ImVec4 color;
};

inline std::vector<ImGuiSnackbar> im_snackbars;
inline std::vector<ImGuiSnackbarStyle> im_snackbars_styles;
inline void PushSnackbarStyleColor(ImGuiSnackbarCol idx, ImVec4 color)
{
  ImGuiSnackbarStyle backup;
  backup.idx   = idx;
  backup.color = color;
  im_snackbars_styles.push_back(backup);
}

inline void PopSnackbarStyleColor(int count)
{
  if ((int)im_snackbars_styles.size() < count) {
    // TODO
    //   IM_ASSERT_USER_ERROR(
    //      im_snackbars_colors.size() > count,
    //      "Calling PopStyleColor() too many times: stack underflow.");
    count = im_snackbars_styles.size();
  }

  while (count > 0) {
    im_snackbars_styles.pop_back();
    count--;
  }
}

inline void Snackbar(ImGuiSnackbar snackbar)
{
  if ((int)im_snackbars_styles.size() > 0) {
    int count = 0;
    while (count < (int)im_snackbars_styles.size()) {
      ImGuiSnackbarStyle styles = im_snackbars_styles.at(count);

      switch (styles.idx) {
        case ImGuiSnackbarCol_Background:
          snackbar.SetBackgroundColor(styles.color);
          break;
        case ImGuiSnackbarCol_Text:
          snackbar.SetTextColor(styles.color);
          break;
      }

      ++count;
    }
  }

  im_snackbars.push_back(snackbar);
}

inline void RenderSnackbar(ImVec2 anchor, ImVec2 align, int dir)
{
  float height = 0.0f;

  int max_iteration = SNACKBAR_RENDER_SIZE;
  if (im_snackbars.size() < SNACKBAR_RENDER_SIZE)
    max_iteration = im_snackbars.size();

  for (auto it = im_snackbars.begin();
       it != im_snackbars.end() && max_iteration > 0;
       --max_iteration) {
    ImGuiSnackbar *snackbar = &(*it);

    snackbar->UpdateTimer();

    if (snackbar->IsTimeout()) {
      im_snackbars.erase(it);
      continue;
    }

    SetNextWindowPos(
        ImVec2(anchor.x, anchor.y + (dir * height)),
        ImGuiCond_Always,
        align);

    int pop_style_count = 0;
    if (snackbar->HasBackgroundColor()) {
      PushStyleColor(ImGuiCol_WindowBg, snackbar->GetBackgroundColor());
      ++pop_style_count;
    }

    if (snackbar->HasTextColor()) {
      PushStyleColor(ImGuiCol_Text, snackbar->GetTextColor());
      ++pop_style_count;
    }

    int index = std::distance(im_snackbars.begin(), it);

    std::string snackbar_name = "##Snackbar-" + std::to_string(index);
    ImGui::Begin(snackbar_name.c_str(), NULL, SNACKBAR_WINDOW_FLAGS);
    {
      ImGui::PushTextWrapPos(SNACKBAR_MAX_WIDTH);
      ImGui::Text(snackbar->GetMessage());
      ImGui::PopTextWrapPos();

      height += GetWindowHeight() + SNACKBAR_SPACING;
    }
    ImGui::End();

    if (pop_style_count > 0) { PopStyleColor(pop_style_count); }

    ++it;
  }
};

}  // namespace ImGui

#endif
