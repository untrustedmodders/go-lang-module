#pragma once

#include <asmjit/asmjit.h>
#include <dyncall/dyncall.h>
#include <module_export.h>
#include <plugify/assembly.h>
#include <plugify/function.h>
#include <plugify/language_module.h>
#include <plugify/module.h>

typedef signed char GoInt8;
typedef unsigned char GoUint8;
typedef short GoInt16;
typedef unsigned short GoUint16;
typedef int GoInt32;
typedef unsigned int GoUint32;
typedef long long GoInt64;
typedef unsigned long long GoUint64;
typedef GoInt64 GoInt;
typedef GoUint64 GoUint;
typedef void* GoUintptr;
typedef float GoFloat32;
typedef double GoFloat64;

typedef struct { const char* p; GoInt n; } GoString;
typedef void* GoMap;
typedef void* GoChan;
typedef struct { void* t; void* v; } GoInterface;
typedef struct { void* data; GoInt len; GoInt cap; } GoSlice;

template <>
struct std::default_delete<DCaggr> {
	void operator()(DCaggr* p) const noexcept { dcFreeAggr(p); }
};

template <>
struct std::default_delete<DCCallVM> {
	void operator()(DCCallVM* p) const noexcept { dcFree(p); }
};

namespace golm {
	constexpr int kApiVersion = 1;

	using InitFunc = int (*)(GoSlice, int, const void*);
	using StartFunc = void (*)();
	using EndFunc = void (*)();

	class AssemblyHolder {
	public:
		AssemblyHolder(std::unique_ptr<plugify::Assembly> assembly, StartFunc startFunc, EndFunc endFunc) : _assembly{std::move(assembly)}, _startFunc{startFunc}, _endFunc{endFunc} {}

		StartFunc GetStartFunc() const { return _startFunc; }
		EndFunc GetEndFunc() const { return _endFunc; }

	private:
		std::unique_ptr<plugify::Assembly> _assembly;
		StartFunc _startFunc{ nullptr };
		EndFunc _endFunc{ nullptr };
	};

	//using MethodRef = std::reference_wrapper<const plugify::Method>;
	using ArgumentList = std::vector<GoSlice>;
	using AggrList = std::vector<std::unique_ptr<DCaggr>>;
	using StringHolder = std::vector<std::unique_ptr<GoString[]>>;
	using BoolHolder = std::vector<std::unique_ptr<bool[]>>;

	class GoLanguageModule final : public plugify::ILanguageModule {
	public:
		GoLanguageModule() = default;
		~GoLanguageModule() = default;

		// ILanguageModule
		plugify::InitResult Initialize(std::weak_ptr<plugify::IPlugifyProvider> provider, const plugify::IModule& module) override;
		void Shutdown() override;
		plugify::LoadResult OnPluginLoad(const plugify::IPlugin& plugin) override;
		void OnPluginStart(const plugify::IPlugin& plugin) override;
		void OnPluginEnd(const plugify::IPlugin& plugin) override;
		void OnMethodExport(const plugify::IPlugin& plugin) override;

		const std::shared_ptr<plugify::IPlugifyProvider>& GetProvider() { return _provider; }
		plugify::MemAddr GetNativeMethod(const std::string& methodName) const;
		
	private:
		static void InternalCall(const plugify::Method* method, plugify::MemAddr data, const plugify::Parameters* params, uint8_t count, const plugify::ReturnValue* ret);

	private:
		std::shared_ptr<asmjit::JitRuntime> _rt;
		std::shared_ptr<plugify::IPlugifyProvider> _provider;

		std::map<plugify::UniqueId, AssemblyHolder> _assemblyMap;
		std::unordered_map<std::string, plugify::MemAddr> _nativesMap;
		std::vector<std::unique_ptr<plugify::Function>> _functions;

		std::unique_ptr<DCCallVM> _callVirtMachine;
		std::mutex _mutex;

		static const std::array<void*, 34> _pluginApi;
	};

	extern GoLanguageModule g_golm;
}

extern "C" GOLM_EXPORT plugify::ILanguageModule* GetLanguageModule();
