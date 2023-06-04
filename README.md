# imgui-snackbar

Basic snackbar implementation for imgui

## Usage

This will render snackbar on the bottom middle of the application:

```
ImVec2 app_size = ImGui::GetIO().DisplaySize;
ImGui::RenderSnackbar(
    ImVec2(app_size.x * 0.5f, app_size.y - 8),
    ImGuiSnackbarAlign_BottomCenter,
    ImGuiSnackbarDir_DownUp);
```

This will add a snackbar:

```
  ImGui::Snackbar({"How many days in a year %d", 365});
```

It's recommanded to take a quick look at the top of the **imsnackbar.h** for options

## Styling

This is an example of an helper function for an error snackbar:

```
inline void SnackbarError(ImGuiSnackbar snackbar) {
    ImGui::PushSnackbarStyleColor(ImGuiSnackbarCol_Background, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::PushSnackbarStyleColor(ImGuiSnackbarCol_Background, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    ImGui::Snackbar({"How many days in a year %d", 365});
    ImGui::PopSnackbarStyleColor(2);
}

```

## Todo

- add a progress bar on the snackbar;
- allow to close snackbar on click.

## Notes

Feel free to report bugs and improvements

## Like this work ?

Help me create more of it.

[!["Buy Me A Coffee"](https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png)](https://www.buymeacoffee.com/n67094)

