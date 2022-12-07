#pragma once

#include <time.h>
#include <stdio.h>
#include <locale.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <tlhelp32.h>
#include <shellapi.h>
#include <filesystem>

#ifdef CHACHA_SDK_DLL
#define CHACHA_EXPORT __declspec(dllexport)
#else
#define CHACHA_EXPORT __declspec(dllimport)
#endif

#define END_OF_BUFFER '\0'

CHACHA_EXPORT size_t
fstrlen(const char* deflated);

time_t
get_epoch(void);

CHACHA_EXPORT
FILE* get_log_fd(void);

CHACHA_EXPORT void
log(const char* msg);

CHACHA_EXPORT bool
open_log_file(void);

#ifdef _MSC_VER
#define PACKED_STRUCT( decl ) __pragma( pack(push, 1) ) decl; __pragma( pack(pop))
#endif

#ifdef __cplusplus

#include <thread>
#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include <concepts>
#include <stdexcept>
#include <algorithm>

namespace Foundation {
	class win32_exception : public std::runtime_error {
	public:
		win32_exception(const std::string& what) : std::runtime_error(what) { m_hr = GetLastError(); }
		~win32_exception() = default; // let the ABI define that.

		win32_exception& operator=(const win32_exception&) = default;
		win32_exception(const win32_exception&) = default;

		HRESULT get_last_error() { return m_hr; }

	private:
		HRESULT m_hr;

	};

	template <typename T>
	class module_type;
	class win32_helpers;
	class module_manager;

	template <typename T>
	class module_type final {
		module_type() = default;
		friend module_manager;

	public:
		module_type(T t) : m_T(t) {}

		T get() { return m_T; }
		T operator->() { return m_T; }

	private:
		T m_T;

	};

	class win32_helpers final {
	public:
		static HWND find_wnd(const char* compare) {
			if (!compare) return nullptr;

			HWND hCurWnd = nullptr;
			char string[256];

			do
			{
				ZeroMemory(&string, 255);

				hCurWnd = FindWindowEx(nullptr, hCurWnd, nullptr, nullptr);
				GetWindowTextA(hCurWnd, string, 256);

				if (string == compare || !strcmp(compare, string))
					return hCurWnd;
			} while (hCurWnd != nullptr);

			return nullptr;
		}

	};

	class module_manager final {
	public:
		explicit module_manager(const std::string& module_name) { 
			assert(!module_name.empty());
			hMod = LoadLibraryExA(module_name.c_str(), nullptr, 0);
			if (!hMod) throw win32_exception("LoadLibraryExA failed!");
		}

		~module_manager() {
			if (hMod)
				FreeLibrary(hMod);
		}

		module_manager& operator=(const module_manager&) = default;
		module_manager(const module_manager&) = default;

		template <typename T>
		module_type<T> load_from_module(const std::string& ordinal) {
			if (!hMod) return {};
			if (ordinal.empty()) return {};

			T the{ reinterpret_cast<T>(GetProcAddress(hMod, ordinal.c_str())) };

			return the;
		}

	private:
		HMODULE hMod{};

	};

	template<typename T>
	concept IsPointer = (std::is_pointer<T>::value);

	template<typename T>
	concept NotNull = (!std::is_same<T, std::nullptr_t>::value);

	template<typename T>
	concept IsPointerAndNotNull = (IsPointer<T> && NotNull<T>);

	template <typename T>
	class non_nil {
	public:
		non_nil() = default;
		non_nil(IsPointerAndNotNull auto the) : m_T(the) {}

		~non_nil() = default;

		T get() { return m_T; }
		T operator->() { return m_T; }

	public:
		non_nil& operator=(const non_nil&) = default;
		non_nil(const non_nil&) = default;

		operator bool() { return m_T; }

	private:
		T m_T;

	};

	using priv_data_t = HWND;

#define SHELL_MANAGER_EXEC_OPEN "open"

	class shell_manager {
	public:
		static non_nil<HINSTANCE> open(std::string& appname, priv_data_t priv) {
			if (appname.empty()) return {};

			non_nil<HINSTANCE> instance = ShellExecuteA(priv, SHELL_MANAGER_EXEC_OPEN, appname.c_str(), nullptr, nullptr, SW_SHOW);
			if (!instance) throw win32_exception("HINSTANCE has a problem, check get_last_error()");

			return instance;
		}

		static non_nil<HINSTANCE> open(const char* appname, priv_data_t priv) {
			if (!appname) return {};

			non_nil<HINSTANCE> instance = ShellExecuteA(priv, SHELL_MANAGER_EXEC_OPEN, appname, nullptr, nullptr, SW_SHOW);
			if (!instance) throw win32_exception("HINSTANCE has a problem, check get_last_error()");

			return instance;
		}

	};

	template <typename Fn, typename... Args>
	class callback final {
	public:
		using function_type = Fn;

	public:
		callback() = delete;

		callback(function_type fn, Args&&... args) {
			assert(fn);

			m_Thread = std::thread(fn, args...);
		}

		~callback() {}

		void should_join() {
			try {
				m_ShouldJoin = true; m_Thread.join();
			} catch (...) {
				log("Failed to join thread! ");
				log("Maybe busy ?\n");
			}
		}

	public:
		callback& operator=(const callback&) = default;
		callback(const callback&) = default;

	private:
		bool m_ShouldJoin{ false };
		std::thread m_Thread;

	};

	static inline std::string get_at_current_path(const std::string& program_name) {
		auto path = std::filesystem::current_path();
		path /= program_name;

		try {
			std::ifstream io(path);
			if (!io.is_open()) throw std::runtime_error("IO Test error");
		}
		catch (...) {
			return "";
		}

		return path.string() + program_name;
	}
}

#endif