// CopyMenu.cpp : CCopyMenu 的实现

#include "stdafx.h"
#include "CopyMenu.h"

#include <atlconv.h>
#include <string>

#ifdef WIN32  
#pragma execution_character_set("utf-8")  
#endif

#define INT2STR(_num) std::to_string((long long) _num)

// CCopyMenu

std::wstring s2ws(const std::string& s) {
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

void wstring_replace(std::wstring &str, const std::wstring &src, const std::wstring &dst) {
	int pos = 0;
	int srclen = src.size();
	int dstlen = dst.size();

	while((pos=str.find(src, pos)) != std::string::npos) {
		str.replace(pos, srclen, dst);
		pos += dstlen;
	}
} 

bool wstring_endsWith(const std::wstring &str, const std::wstring &sub) {
	return str.rfind(sub) == (str.length() - sub.length());
}

std::vector<std::wstring> wstring_split(const std::wstring &str, const std::wstring &delim) {
	std::vector<std::wstring> result;
	int len = wcslen(str.data());
	if (str.data() && len) {
		auto *src = new WCHAR[len + 1];
		wcscpy(src, str.data());
		src[len] = '\0';
		WCHAR *tokenptr = wcstok(src, delim.data());
		while (tokenptr != NULL) {
			std::wstring tk = tokenptr;
			result.emplace_back(tk);
			tokenptr = wcstok(NULL, delim.data());
		}
		delete[] src;
	}
	return result;
}

const UINT MAX_PATH_LENGTH = 2048;

HRESULT STDMETHODCALLTYPE CCopyMenu::Initialize( 
	/* [annotation][unique][in] */ 
	__in_opt  PCIDLIST_ABSOLUTE pidlFolder,
	/* [annotation][unique][in] */ 
	__in_opt  IDataObject *pdtobj,
	/* [annotation][unique][in] */ 
	__in_opt  HKEY hkeyProgID)
{
	this->pdtobj = pdtobj;
	return S_OK;
}

const UINT MENU_ID_1 = 0;
const UINT MENU_ID_2 = 1;
const UINT MENU_ID_3 = 2;
const UINT MENU_ID_4 = 3;
const UINT MENU_ID_5 = 4;

HRESULT STDMETHODCALLTYPE CCopyMenu::QueryContextMenu( 
	/* [annotation][in] */ 
	__in  HMENU hmenu,
	/* [annotation][in] */ 
	__in  UINT indexMenu,
	/* [annotation][in] */ 
	__in  UINT idCmdFirst,
	/* [annotation][in] */ 
	__in  UINT idCmdLast,
	/* [annotation][in] */ 
	__in  UINT uFlags)
{

	if (uFlags & CMF_VERBSONLY)
		return S_OK;

	if (uFlags & CMF_DEFAULTONLY)
		return MAKE_SCODE(SEVERITY_SUCCESS, FACILITY_NULL, 0);

	HMENU subMenu = CreateMenu();

	MENUITEMINFO outMenu;

	outMenu.cbSize = sizeof(MENUITEMINFO);
	outMenu.fMask = MIIM_STRING | MIIM_SUBMENU;

	outMenu.dwTypeData = _T("パスのコピー");
	outMenu.hSubMenu = subMenu;

	MENUITEMINFO cpy_menuItem;
	MENUITEMINFO cpy_qte_menuItem;
	MENUITEMINFO cpy_dbl_menuItem;
	MENUITEMINFO cpy_qte_dbl_menuItem;
	MENUITEMINFO cpy_name_menuItem;

	cpy_menuItem.cbSize = sizeof(MENUITEMINFO);
	cpy_menuItem.fMask = MIIM_STRING | MIIM_ID;
	cpy_menuItem.dwTypeData = _T("引用記号抜き");
	cpy_menuItem.wID = idCmdFirst + MENU_ID_1;

	cpy_qte_menuItem.cbSize = sizeof(MENUITEMINFO);
	cpy_qte_menuItem.fMask = MIIM_STRING | MIIM_ID;
	cpy_qte_menuItem.dwTypeData = _T("引用記号付き");
	cpy_qte_menuItem.wID = idCmdFirst + MENU_ID_2;

	cpy_dbl_menuItem.cbSize = sizeof(MENUITEMINFO);
	cpy_dbl_menuItem.fMask = MIIM_STRING | MIIM_ID;
	cpy_dbl_menuItem.dwTypeData = _T("引用記号抜き (ダブルバックスラッシュ)");
	cpy_dbl_menuItem.wID = idCmdFirst + MENU_ID_3;

	cpy_qte_dbl_menuItem.cbSize = sizeof(MENUITEMINFO);
	cpy_qte_dbl_menuItem.fMask = MIIM_STRING | MIIM_ID;
	cpy_qte_dbl_menuItem.dwTypeData = _T("引用記号付き (ダブルバックスラッシュ)");
	cpy_qte_dbl_menuItem.wID = idCmdFirst + MENU_ID_4;

	cpy_name_menuItem.cbSize = sizeof(MENUITEMINFO);
	cpy_name_menuItem.fMask = MIIM_STRING | MIIM_ID;
	cpy_name_menuItem.dwTypeData = _T("名前のみ");
	cpy_name_menuItem.wID = idCmdFirst + MENU_ID_5;

	/*
	InsertMenuItemW(
	__in HMENU hmenu,
	__in UINT item,
	__in BOOL fByPosition,
	__in LPCMENUITEMINFOW lpmi);

	InsertMenuW(
	__in HMENU hMenu,
	__in UINT uPosition,
	__in UINT uFlags,
	__in UINT_PTR uIDNewItem,
	__in_opt LPCWSTR lpNewItem);
	*/

	InsertMenu(hmenu, indexMenu, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
	InsertMenuItem(hmenu, indexMenu + 1, true, &outMenu);
	InsertMenu(hmenu, indexMenu + 2, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);

	InsertMenuItem(subMenu, 0, true, &cpy_menuItem);
	InsertMenuItem(subMenu, 1, true, &cpy_qte_menuItem);
	InsertMenuItem(subMenu, 2, true, &cpy_dbl_menuItem);
	InsertMenuItem(subMenu, 3, true, &cpy_qte_dbl_menuItem);
	InsertMenuItem(subMenu, 4, true, &cpy_name_menuItem);

	return MAKE_SCODE(SEVERITY_SUCCESS, FACILITY_NULL, 5);
}

HRESULT STDMETHODCALLTYPE CCopyMenu::InvokeCommand( 
	/* [annotation][in] */ 
	__in  CMINVOKECOMMANDINFO *pici)
{
	UINT idCmd = LOWORD(pici->lpVerb);
	if (HIWORD(pici->lpVerb) != 0)
		return E_INVALIDARG;

	/*
	MessageBoxW(
		__in_opt HWND hWnd,
		__in_opt LPCWSTR lpText,
		__in_opt LPCWSTR lpCaption,
		__in UINT uType);

	SHSTDAPI_(UINT) DragQueryFileW(
		__in HDROP hDrop,
		__in UINT iFile, 
		__out_ecount_opt(cch) LPWSTR lpszFile,
		__in UINT cch);
	*/

	// Get Filenames
	std::vector<std::wstring> sel_files;
	FORMATETC fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stm = {};
	bool ok = false;
	if (SUCCEEDED(pdtobj->GetData(&fe, &stm))) {
		HDROP hDrop = static_cast<HDROP>(GlobalLock(stm.hGlobal));
		if (hDrop != NULL) {
			UINT cnt = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
			if (cnt != 0) {

				LPWSTR str = new WCHAR[2048];

				for (UINT i = 0; i < cnt; i++) {
					try {
						memset(str, 0, MAX_PATH_LENGTH);
						int size = DragQueryFile(hDrop, i, str, MAX_PATH_LENGTH);
						if (size == 0) continue;

						std::wstring name = str;
						// std::string name = ATL::CW2A(str);
						if (name.empty()) continue;
						// MessageBox(NULL, name.c_str(), _T("MyShell"), MB_OK);
						sel_files.push_back(name);
						ok = true;
					}
					catch (...) { }
				}
				delete []str;
				GlobalUnlock(stm.hGlobal);
			}
			ReleaseStgMedium(&stm);
		}
	}
	
	if (!ok) {
		MessageBox(NULL, _T("ファイルの名前のロードエラー"), _T("エラー"), MB_OK);
		return S_OK;
	}

	// Cat Filenames
	std::wstring name = sel_files.at(0);

	for (auto iter = sel_files.begin() + 1; iter != sel_files.end(); iter++)
		name += _T(";") + *iter;

	switch (idCmd) {
	case MENU_ID_1: // 引用記号抜き
		// MessageBox(NULL, s2ws(std::to_string((long long) sel_files.size())).c_str(), _T("MyShell"), MB_OK);
		break;
	case MENU_ID_2: // 引用記号付き
		name = _T("\"") + name + _T("\"");
		break;
	case MENU_ID_3: // 引用記号抜き ダブルバックスラッシュ
		wstring_replace(name, _T("\\"), _T("\\\\"));
		break;
	case MENU_ID_4: // 引用記号付き ダブルバックスラッシュ
		name = _T("\"") + name + _T("\"");
		wstring_replace(name, _T("\\"), _T("\\\\"));
		break;
	case MENU_ID_5: // 名前のみ
		std::vector<std::wstring> sp = wstring_split(name, _T(";"));
		std::vector<std::wstring> namesp = wstring_split(sp.at(0), _T("\\"));
		std::wstring ret = namesp.at(namesp.size() - 1);
		for (auto iter = sp.begin() + 1; iter != sp.end(); iter++) {
			namesp = wstring_split(*iter, _T("\\"));
			ret.append(_T(";") + namesp.at(namesp.size() - 1));
		}
		name = ret;
		break;
	}
	 
	// Copy Filenames
	if (OpenClipboard(NULL)) {
		EmptyClipboard();
		HGLOBAL buf = GlobalAlloc(GHND | GMEM_SHARE, (name.length() + 1) * sizeof(WCHAR));
		LPWSTR strMem = (LPWSTR) GlobalLock(buf);
		wcscpy(strMem, name.data());
		GlobalUnlock(buf);

		SetClipboardData(CF_UNICODETEXT, buf);
		CloseClipboard();
	}

	return E_INVALIDARG;
}

HRESULT STDMETHODCALLTYPE CCopyMenu::GetCommandString( 
	/* [annotation][in] */ 
	__in  UINT_PTR idCmd,
	/* [annotation][in] */ 
	__in  UINT uType,
	/* [annotation][in] */ 
	__reserved  UINT *pReserved,
	/* [annotation][out] */ 
	__out_awcount(!(uType & GCS_UNICODE), cchMax)  LPSTR pszName,
	/* [annotation][in] */ 
	__in  UINT cchMax)
{
	return S_OK;
}