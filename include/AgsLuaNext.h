// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the AGSLUANEXT_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// AGSLUANEXT_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef AGSLUANEXT_EXPORTS
#define AGSLUANEXT_API __declspec(dllexport)
#else
#define AGSLUANEXT_API __declspec(dllimport)
#endif

// This class is exported from the dll
class AGSLUANEXT_API CAgsLuaNext {
public:
	CAgsLuaNext(void);
	// TODO: add your methods here.
};

extern AGSLUANEXT_API int nAgsLuaNext;

AGSLUANEXT_API int fnAgsLuaNext(void);
