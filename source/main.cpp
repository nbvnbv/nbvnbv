#include <main.hpp>
#include <netfilter.hpp>
#include <GarrysMod/Lua/Interface.h>
#include <scanning/symbolfinder.hpp>
#include <iserver.h>
#include <Platform.hpp>

namespace global
{

	#if defined SYSTEM_WINDOWS

	static const char IServer_sig[] =
		"\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\xD8\x6D\x24\x83\x4D\xEC\x10";
	static const size_t IServer_siglen = sizeof( IServer_sig ) - 1;

#elif defined SYSTEM_POSIX

	static const char IServer_sig[] = "@sv";
	static const size_t IServer_siglen = 0;

#endif

	SourceSDK::FactoryLoader engine_loader( "engine" );
	IServer *server = nullptr;

	LUA_FUNCTION_STATIC( GetClientCount )
	{
		LUA->PushNumber( server->GetClientCount( ) );
		return 1;
}

	static void PreInitialize( GarrysMod::Lua::ILuaBase *LUA )
	{
		{
			SymbolFinder symfinder;

			void *temp_server = symfinder.Resolve(
				engine_loader.GetModuleLoader( ).GetModule( ),
				IServer_sig,
				IServer_siglen
			);
			if( temp_server == nullptr )
				LUA->ThrowError( "failed to locate IServer" );

			server =

#if defined SYSTEM_POSIX

				reinterpret_cast<IServer *>

#else

				*reinterpret_cast<IServer **>

#endif

				( temp_server );
		}

		if( server == nullptr )
LUA->ThrowError( "failed to dereference IServer" );

		LUA->CreateTable( );

		LUA->PushString( "Query 1.1" );
		LUA->SetField( -2, "Version" );

		// version num follows LuaJIT style, xxyyzz
		LUA->PushNumber( 010000 );
		LUA->SetField( -2, "VersionNum" );
	}

	static void Initialize( GarrysMod::Lua::ILuaBase *LUA )
	{
		LUA->SetField( GarrysMod::Lua::INDEX_GLOBAL, "query" );
	}

	static void Deinitialize( GarrysMod::Lua::ILuaBase *LUA )
	{
		LUA->PushNil( );
		LUA->SetField( GarrysMod::Lua::INDEX_GLOBAL, "query" );
	}

}

GMOD_MODULE_OPEN( )
{
	global::PreInitialize( LUA );
	netfilter::Initialize( LUA );
	global::Initialize( LUA );
	return 1;
}

GMOD_MODULE_CLOSE( )
{
	netfilter::Deinitialize( LUA );
	global::Deinitialize( LUA );
	return 0;
}
