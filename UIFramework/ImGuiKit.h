#pragma once

#include <vector>
#include <functional>
#include <Foundation.h>

#define UI_TOOLKIT_STRING "1.4.0RC"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

#define UI_TOOLKIT_CENTER(w, h, x, y) ImGui::SetCursorPos(ImVec2(((w)-(x)) * 0.5f, ((h)-(y)) * 0.5f))

namespace Foundation {
	class view {
	public:
		view() = default;
		virtual ~view() = default;

		view& operator=(const view&) = delete;
		view(const view&) = delete;

		void body(std::function<void()> pre_env, std::function<void()> post_env) {
			assert(pre_env);
			assert(post_env);
			
			pre_env();
			post_env();
		}

	};

	class popup : public view {
	public:
		void operator()(const std::string& name, std::function<void()> pre, bool should_draw = false, std::function<void()> post = nullptr) {
			if (!should_draw) return;

			this->body([name, pre]() -> void {
					ImGui::OpenPopup(name.c_str());
					if (ImGui::BeginPopupModal(name.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
					{
						if (pre)
							pre();
					}
				},
				[name, post]() -> void {
					if (post) post();
				});
		}

	};

	class menu final {
	public:
		void operator()(const char* text, std::function<void()> on_hover = nullptr, std::function<void()> on_click = nullptr, float offset_from_start = 0.f, float spacing = 0.f) {
			ImGui::TextDisabled(text);
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
				if (on_hover) on_hover();

			if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				if (on_click) on_click();

			ImGui::SameLine(offset_from_start, spacing);
		}

	};

	class tooltip final {
	public:
		void operator()(std::vector<std::string> vec_string) {
			std::string str;

			for (auto str_in_vec : vec_string)
				str += str_in_vec + "\r\n";

			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(str.c_str());
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

	};

	class gui_manager {
	public:
		static std::tuple<ImGuiIO*, ImGuiContext*> create(const bool dark_theme = false) {
			auto ctx = ImGui::CreateContext();
			if (!ctx) return {};

			dark_theme ? ImGui::StyleColorsDark() : ImGui::StyleColorsLight();
			
			return { &ImGui::GetIO(), ctx };
		}

		static void destroy(ImGuiContext* ctx) {
			if (ctx) ImGui::DestroyContext();
		}

		static void open_debug_console(FILE* fout = stdout, FILE* fin = stdin) {
			AllocConsole();
			SetConsoleTitleA(UI_TOOLKIT_STRING);

			// ignore the return values.
			(void)freopen("CONOUT$", "w", fout);
			(void)freopen("CONIN$", "r", fin);
		}

	};
}