#ifndef _IGRAPHICSMAC_
#define _IGRAPHICSMAC_

#if defined(__APPLE__) && defined(__LP64__) && !defined(IPLUG_NO_CARBON_SUPPORT)
	#define IPLUG_NO_CARBON_SUPPORT
#endif

#include "IGraphicsLice.h"
#include "../swell/swell.h"
#include <Carbon/Carbon.h>

#ifndef IPLUG_NO_CARBON_SUPPORT
	class IGraphicsCarbon;
#endif

class NSMutableDictionary;

class IGraphicsMac : public IGraphicsLice
{
public:

	IGraphicsMac(IPlugBase* pPlug, int w, int h, int refreshFPS);
	virtual ~IGraphicsMac();

  void SetBundleID(const char* bundleID) { mBundleID.Set(bundleID); }
  
  bool DrawScreen(IRECT* pR);

  void* OpenWindow(void* pWindow);
#ifndef IPLUG_NO_CARBON_SUPPORT
  void* OpenWindow(void* pWindow, void* pControl);
#endif
  
	void* OpenCocoaWindow(void* pParentView);  
#ifndef IPLUG_NO_CARBON_SUPPORT
  void* OpenCarbonWindow(void* pParentWnd, void* pParentControl);
#endif
  
	void CloseWindow();
	bool WindowIsOpen();
  void Resize(int w, int h);
  
	void HostPath(WDL_String* pPath); 
  void PluginPath(WDL_String* pPath);

	void PromptForFile(WDL_String* pFilename, EFileAction action = kFileOpen, char* dir = "",
    char* extensions = "");   // extensions = "txt wav" for example.
  bool PromptForColor(IColor* pColor, char* prompt = "");
	void PromptUserInput(IControl* pControl, IParam* pParam);
  bool OpenURL(const char* url, const char* msgWindowTitle = 0, const char* confirmMsg = 0, const char* errMsgOnFailure = 0);

	void* GetWindow();

	int mIdleTicks;

  const char* GetBundleID()  { return mBundleID.Get(); }
  static int GetUserOSVersion();   // Returns a number like 0x1050 (10.5).
  bool DrawIText(IText* pTxt, char* str, IRECT* pR);
  
protected:
  
  virtual LICE_IBitmap* OSLoadBitmap(int ID, const char* name);
  
private:
  
#ifndef IPLUG_NO_CARBON_SUPPORT
  IGraphicsCarbon* mGraphicsCarbon; 
#endif
  void* mGraphicsCocoa;   // Can't forward-declare IGraphicsCocoa because it's an obj-C object.
  
  WDL_String mBundleID;
  NSMutableDictionary* mTxtAttrs;
  IText mTxt;
};

inline CFStringRef MakeCFString(const char* cStr)
{
  return CFStringCreateWithCString(0, cStr, kCFStringEncodingUTF8); 
}

struct CFStrLocal 
{
  CFStringRef mCFStr;
  CFStrLocal(const char* cStr) 
  {
    mCFStr = MakeCFString(cStr); 
  }
  ~CFStrLocal() 
  {
    CFRelease(mCFStr); 
  }
};

struct CStrLocal
{
  char* mCStr;
  CStrLocal(CFStringRef cfStr) 
  {
    int n = CFStringGetLength(cfStr) + 1;
    mCStr = (char*) malloc(n);
    CFStringGetCString(cfStr, mCStr, n, kCFStringEncodingUTF8);
  }
  ~CStrLocal() 
  {
    FREE_NULL(mCStr); 
  }
};

#endif