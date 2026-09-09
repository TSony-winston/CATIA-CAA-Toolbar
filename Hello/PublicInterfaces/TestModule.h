#ifdef  _WINDOWS_SOURCE
#ifdef  __TestModule
#define ExportedByTestModule     __declspec(dllexport)
#else
#define ExportedByTestModule     __declspec(dllimport)
#endif
#else
#define ExportedByTestModule
#endif
