#define COBJMACROS


#include <stdio.h>
#include <windows.h>
#include <msxml6.h>

#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "msxml6.lib")



int main(){
	IXMLDOMDocument *doc;
	VARIANT_BOOL loaded;
	BSTR xml;
	BSTR bstr;
	VARIANT varFileName;
	VARIANT_BOOL isSuccessful;

	VariantInit(&varFileName);
	bstr = SysAllocString(L"myData.xml");
	varFileName.vt = VT_BSTR;
	varFileName.bstrVal = bstr;

	
	CoInitialize(NULL);
	CoCreateInstance(&CLSID_DOMDocument60, NULL, CLSCTX_INPROC_SERVER, &IID_IXMLDOMDocument, (void**) &doc);
	
	
	//IXMLDOMDocument_loadXML(doc, L"<root>aaabbb</root>", &loaded);	// load from string
	IXMLDOMDocument_load(doc, varFileName, &isSuccessful);				// load from file

	
	IXMLDOMDocument_get_xml(doc, &xml);
	wprintf(L"XML = %s\n", xml);

	

	SysFreeString(xml);


	IXMLDOMDocument_Release(doc);

	CoUninitialize();



	puts("test 123");
	system("pause");

	return 0;
}