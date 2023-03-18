#include <fmt/format.h>
#ifdef _WIN32
	#include <windows.h>
#endif

void enableUTF8()
{
	#ifdef _WIN32
		SetConsoleOutputCP(65001);
		HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD lpMode = 0;
		GetConsoleMode(out, &lpMode);
		SetConsoleMode(out, lpMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	#endif

	std::string testFmt = fmt::format("UTF-8 TEST: áéíóúÁÉÍÓÚñÑ¿¡");
	printf("%s\n", testFmt.data());
	return;
}
