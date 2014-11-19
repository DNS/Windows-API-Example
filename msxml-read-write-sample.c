
/*
COM explanation:
	C	: IXMLDOMDocument_loadXML(xmlobject, BSTR bstrXML, VARIANT_BOOL * isSuccessful);
		  // or
		  xmlobject->lpVtbl->loadXML(xmlobject, BSTR bstrXML, VARIANT_BOOL * isSuccessful);

	C++	: 
		xmlobject->loadXML(BSTR bstrXML, VARIANT_BOOL * isSuccessful);
*/


#include <stdio.h>
#include <windows.h>
#include <msxml6.h>

#pragma comment(lib, "msxml6.lib")


int main(){
	IXMLDOMDocument *doc;
	VARIANT_BOOL loaded;
	BSTR xml;
	BSTR bstr;
	VARIANT varFileName;
	VARIANT_BOOL isSuccessful;
	BSTR tag_name;
	IXMLDOMNodeList *resultList;
	long list_length;
	IXMLDOMNode *list_item;

	VariantInit(&varFileName);
	bstr = SysAllocString(L"myData.xml");
	varFileName.vt = VT_BSTR;
	varFileName.bstrVal = bstr;

	CoInitialize(NULL);
	CoCreateInstance(&CLSID_DOMDocument60, NULL, CLSCTX_INPROC_SERVER, &IID_IXMLDOMDocument, &doc);
	
	doc->lpVtbl->loadXML(doc, L"<root>initial value</root>", &loaded);		// load from string
	//doc->lpVtbl->load(doc, varFileName, &isSuccessful);			// load from file
	//VariantClear(&varFileName);
	
	
	doc->lpVtbl->get_xml(doc, &bstr);
	
	wprintf(L"XML = %s\n", bstr);

	tag_name = SysAllocString(L"root");
	doc->lpVtbl->getElementsByTagName(doc, tag_name, &resultList);
	
	resultList->lpVtbl->get_length(resultList, &list_length);
	wprintf(L"list_length = %d\n", list_length);

	
	resultList->lpVtbl->get_item(resultList, 0, &list_item);

	
	list_item->lpVtbl->get_text(list_item, &bstr);
	wprintf(L"bstr = %s\n", bstr);
	

	bstr = SysAllocString(L"modified value");
	list_item->lpVtbl->put_text(list_item, bstr);
	
	list_item->lpVtbl->get_text(list_item, &bstr);
	wprintf(L"bstr = %s\n", bstr);

	doc->lpVtbl->save(doc, varFileName);

	SysFreeString(bstr);
	doc->lpVtbl->Release(doc);
	CoUninitialize();



	//puts("test 123");
	system("pause");

	return 0;
}

