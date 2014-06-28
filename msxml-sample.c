#define COBJMACROS



#include <stdio.h>
#include <windows.h>
#include <msxml6.h>



int main(){
	IXMLDOMDocument *doc;
	VARIANT_BOOL loaded;
	BSTR xml = NULL;

	CoInitialize(NULL);
	CoCreateInstance(&CLSID_DOMDocument60, NULL, CLSCTX_INPROC_SERVER, &IID_IXMLDOMDocument, (void**) &doc);
	IXMLDOMDocument_loadXML(doc, L"<xml>aaabbb</xml>", &loaded);
	IXMLDOMDocument_get_xml(doc, &xml);


	wprintf(L"XML = %s\n", xml);

	SysFreeString(xml);
	IXMLDOMDocument_Release(doc);

	CoUninitialize();



	puts("test 123");
	system("pause");

	return 0;
}