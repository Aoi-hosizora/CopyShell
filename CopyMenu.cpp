#include "stdafx.h"
#include "CopyMenu.h"
#include "Utils.hpp"

#include <atlconv.h>
#include <string>

#ifdef WIN32  
#pragma execution_character_set("utf-8")  
#endif

HRESULT STDMETHODCALLTYPE CCopyMenu::Initialize( 
    __in_opt PCIDLIST_ABSOLUTE pidlFolder,
    __in_opt IDataObject *pdtobj,
    __in_opt HKEY hkeyProgID
) {
    this->pdtobj = pdtobj;
    this->pidlFolder = pidlFolder;

    FORMATETC fe = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stm = {};
    this->count = 0;
    if (pdtobj != nullptr && SUCCEEDED(pdtobj->GetData(&fe, &stm))) {
        auto hDrop = static_cast<HDROP>(GlobalLock(stm.hGlobal));
        if (hDrop != nullptr) {
            this->count = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
        }
    }
    return S_OK;
}

const UINT MAX_PATH_LENGTH = 2048;
const UINT MENU_ID_PREFIX = 0;

const UINT MENU_ID_NQ_BCK = MENU_ID_PREFIX + 0;
const UINT MENU_ID_Q_BCK  = MENU_ID_PREFIX + 1;
const UINT MENU_ID_NQ_FNT = MENU_ID_PREFIX + 2;
const UINT MENU_ID_Q_FNT  = MENU_ID_PREFIX + 3;
const UINT MENU_ID_NQ_DBL = MENU_ID_PREFIX + 4;
const UINT MENU_ID_Q_DBL  = MENU_ID_PREFIX + 5;
const UINT MENU_ID_NQ_NME = MENU_ID_PREFIX + 6;
const UINT MENU_ID_Q_NME  = MENU_ID_PREFIX + 7;

const UINT MENU_ID_NQ_BCK_COMMA = MENU_ID_PREFIX + 8;
const UINT MENU_ID_Q_BCK_COMMA  = MENU_ID_PREFIX + 9;
const UINT MENU_ID_NQ_FNT_COMMA = MENU_ID_PREFIX + 10;
const UINT MENU_ID_Q_FNT_COMMA  = MENU_ID_PREFIX + 11;
const UINT MENU_ID_NQ_DBL_COMMA = MENU_ID_PREFIX + 12;
const UINT MENU_ID_Q_DBL_COMMA  = MENU_ID_PREFIX + 13;
const UINT MENU_ID_NQ_NME_COMMA = MENU_ID_PREFIX + 14;
const UINT MENU_ID_Q_NME_COMMA  = MENU_ID_PREFIX + 15;

HRESULT STDMETHODCALLTYPE CCopyMenu::QueryContextMenu( 
    __in HMENU hmenu,
    __in UINT indexMenu,
    __in UINT idCmdFirst,
    __in UINT idCmdLast,
    __in UINT uFlags
) {
    if (uFlags & CMF_VERBSONLY) {
        return S_OK;
    } else if (this->pdtobj == nullptr && this->pidlFolder == nullptr) {
        return S_OK;
    } else if (uFlags & CMF_DEFAULTONLY) {
        return MAKE_SCODE(SEVERITY_SUCCESS, FACILITY_NULL, 0);
    }

    MENUITEMINFO mii_out;
    HMENU mii_sub = CreateMenu();
    mii_out.cbSize = sizeof(MENUITEMINFO);
    mii_out.fMask = MIIM_STRING | MIIM_SUBMENU;
    mii_out.dwTypeData = _T("パスのコピー");
    mii_out.hSubMenu = mii_sub;

    MENUITEMINFO mii_split;
    mii_split.cbSize = sizeof(MENUITEMINFO);
    mii_split.fMask = MIIM_FTYPE;
    mii_split.fType = MFT_SEPARATOR;

    if (this->pdtobj == nullptr) { // background
        InsertMenuItem(hmenu, indexMenu, true, &mii_out);
    } else { // file
        InsertMenu(hmenu, indexMenu, MF_SEPARATOR | MF_BYPOSITION, 0, nullptr);
        InsertMenuItem(hmenu, indexMenu + 1, true, &mii_out);
        InsertMenu(hmenu, indexMenu + 2, MF_SEPARATOR | MF_BYPOSITION, 0, nullptr);
    }

    int code;
    MENUITEMINFO mii;
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_STRING | MIIM_ID;

    if (this->count == 0 || this->count == 1) {
        // \ -> F:\Projects\CopyShell\.gitignore
        mii.dwTypeData = _T("引用符なし バックスラッシュ (\\)");
        mii.wID = idCmdFirst + MENU_ID_NQ_BCK;
        InsertMenuItem(mii_sub, 0, true, &mii);

        // "\" -> "F:\Projects\CopyShell\.gitignore"
        mii.dwTypeData = _T("引用符付き バックスラッシュ (\"\\\")");
        mii.wID = idCmdFirst + MENU_ID_Q_BCK;
        InsertMenuItem(mii_sub, 1, true, &mii);

        // / -> F:/Projects/CopyShell/.gitignore
        mii.dwTypeData = _T("引用符なし フロントスラッシュ (/)");
        mii.wID = idCmdFirst + MENU_ID_NQ_FNT;
        InsertMenuItem(mii_sub, 2, true, &mii);

        // "/" -> "F:/Projects/CopyShell/.gitignore"
        mii.dwTypeData = _T("引用符付き フロントスラッシュ (\"/\")");
        mii.wID = idCmdFirst + MENU_ID_Q_FNT;
        InsertMenuItem(mii_sub, 3, true, &mii);

        // \\ -> F:\\Projects\\CopyShell\\.gitignore
        mii.dwTypeData = _T("引用符なし ダブルバックスラッシュ (\\\\)"); 
        mii.wID = idCmdFirst + MENU_ID_NQ_DBL;
        InsertMenuItem(mii_sub, 4, true, &mii);

        // "\\" -> "F:\\Projects\\CopyShell\\.gitignore"
        mii.dwTypeData = _T("引用符付き ダブルバックスラッシュ (\"\\\\\")"); 
        mii.wID = idCmdFirst + MENU_ID_Q_DBL;
        InsertMenuItem(mii_sub, 5, true, &mii);

        InsertMenuItem(mii_sub, 6, true, &mii_split);

        // -> .gitignore
        mii.dwTypeData = _T("引用符なし 名前のみ (x)"); 
        mii.wID = idCmdFirst + MENU_ID_NQ_NME;
        InsertMenuItem(mii_sub, 7, true, &mii);

        // -> ".gitignore"
        mii.dwTypeData = _T("引用符付き 名前のみ (\"x\")"); 
        mii.wID = idCmdFirst + MENU_ID_Q_NME;
        InsertMenuItem(mii_sub, 8, true, &mii);
        code = 9;
    } else {
        // \ -> F:\Projects\CopyShell\.gitignore
        mii.dwTypeData = _T("引用符なし バックスラッシュ セミコロン (\\;\\)");
        mii.wID = idCmdFirst + MENU_ID_NQ_BCK;
        InsertMenuItem(mii_sub, 0, true, &mii);

        // "\" -> "F:\Projects\CopyShell\.gitignore"
        mii.dwTypeData = _T("引用符付き バックスラッシュ セミコロン (\"\\\";\"\\\")");
        mii.wID = idCmdFirst + MENU_ID_Q_BCK;
        InsertMenuItem(mii_sub, 1, true, &mii);

        // / -> F:/Projects/CopyShell/.gitignore
        mii.dwTypeData = _T("引用符なし フロントスラッシュ セミコロン (/;/)");
        mii.wID = idCmdFirst + MENU_ID_NQ_FNT;
        InsertMenuItem(mii_sub, 2, true, &mii);

        // "/" -> "F:/Projects/CopyShell/.gitignore"
        mii.dwTypeData = _T("引用符付き フロントスラッシュ セミコロン (\"/\";\"/\")");
        mii.wID = idCmdFirst + MENU_ID_Q_FNT;
        InsertMenuItem(mii_sub, 3, true, &mii);

        // \\ -> F:\\Projects\\CopyShell\\.gitignore
        mii.dwTypeData = _T("引用符なし ダブルバックスラッシュ セミコロン (\\\\;\\\\)"); 
        mii.wID = idCmdFirst + MENU_ID_NQ_DBL;
        InsertMenuItem(mii_sub, 4, true, &mii);

        // "\\" -> "F:\\Projects\\CopyShell\\.gitignore"
        mii.dwTypeData = _T("引用符付き ダブルバックスラッシュ セミコロン (\"\\\\\";\"\\\\\")"); 
        mii.wID = idCmdFirst + MENU_ID_Q_DBL;
        InsertMenuItem(mii_sub, 5, true, &mii);

        InsertMenuItem(mii_sub, 6, true, &mii_split);

        // -> .gitignore
        mii.dwTypeData = _T("引用符なし 名前のみ セミコロン (;)"); 
        mii.wID = idCmdFirst + MENU_ID_NQ_NME;
        InsertMenuItem(mii_sub, 7, true, &mii);

        // -> ".gitignore"
        mii.dwTypeData = _T("引用符付き 名前のみ セミコロン (;)"); 
        mii.wID = idCmdFirst + MENU_ID_Q_NME;
        InsertMenuItem(mii_sub, 8, true, &mii);

        InsertMenuItem(mii_sub, 9, true, &mii_split);

        // \ -> F:\Projects\CopyShell\.gitignore
        mii.dwTypeData = _T("引用符なし バックスラッシュ セミコロン (\\,\\)");
        mii.wID = idCmdFirst + MENU_ID_NQ_BCK_COMMA;
        InsertMenuItem(mii_sub, 10, true, &mii);

        // "\" -> "F:\Projects\CopyShell\.gitignore"
        mii.dwTypeData = _T("引用符付き バックスラッシュ コンマ (\"\\\",\"\\\")");
        mii.wID = idCmdFirst + MENU_ID_Q_BCK_COMMA;
        InsertMenuItem(mii_sub, 11, true, &mii);

        // / -> F:/Projects/CopyShell/.gitignore
        mii.dwTypeData = _T("引用符なし フロントスラッシュ コンマ (/,/)");
        mii.wID = idCmdFirst + MENU_ID_NQ_FNT_COMMA;
        InsertMenuItem(mii_sub, 12, true, &mii);

        // "/" -> "F:/Projects/CopyShell/.gitignore"
        mii.dwTypeData = _T("引用符付き フロントスラッシュ コンマ (\"/\",\"/\")");
        mii.wID = idCmdFirst + MENU_ID_Q_FNT_COMMA;
        InsertMenuItem(mii_sub, 13, true, &mii);

        // \\ -> F:\\Projects\\CopyShell\\.gitignore
        mii.dwTypeData = _T("引用符なし ダブルバックスラッシュ コンマ (\\\\,\\\\)"); 
        mii.wID = idCmdFirst + MENU_ID_NQ_DBL_COMMA;
        InsertMenuItem(mii_sub, 14, true, &mii);

        // "\\" -> "F:\\Projects\\CopyShell\\.gitignore"
        mii.dwTypeData = _T("引用符付き ダブルバックスラッシュ コンマ (\"\\\\\",\"\\\\\")"); 
        mii.wID = idCmdFirst + MENU_ID_Q_DBL_COMMA;
        InsertMenuItem(mii_sub, 15, true, &mii);

        InsertMenuItem(mii_sub, 16, true, &mii_split);

        // -> .gitignore
        mii.dwTypeData = _T("引用符なし 名前のみ コンマ (x,y)"); 
        mii.wID = idCmdFirst + MENU_ID_NQ_NME_COMMA;
        InsertMenuItem(mii_sub, 17, true, &mii);

        // -> ".gitignore"
        mii.dwTypeData = _T("引用符付き 名前のみ コンマ (\"x\",\"y\")"); 
        mii.wID = idCmdFirst + MENU_ID_Q_NME_COMMA;
        InsertMenuItem(mii_sub, 18, true, &mii);
        code = 19;
    }
   
    return MAKE_SCODE(SEVERITY_SUCCESS, FACILITY_NULL, code);
}

std::vector<std::wstring> getFilenameFromDataObject(IDataObject *pdtobj, bool *ok) {
    std::vector<std::wstring> sel_files;
    *ok = true;

    FORMATETC fe = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stm = {};
    if (SUCCEEDED(pdtobj->GetData(&fe, &stm))) {
        auto hDrop = static_cast<HDROP>(GlobalLock(stm.hGlobal));
        if (hDrop == nullptr) {
            *ok = false;
            ReleaseStgMedium(&stm);
            return sel_files;
        }

        UINT cnt = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
        auto str = new WCHAR[MAX_PATH_LENGTH];
        try {
            for (UINT i = 0; i < cnt; i++) {
                memset(str, 0, MAX_PATH_LENGTH);
                int size = DragQueryFile(hDrop, i, str, MAX_PATH_LENGTH);
                if (size == 0) {
                    continue;
                }
                std::wstring name = str; // WCHAR * -> wstr
                if (name.empty()) {
                    continue;
                }
                // MessageBox(NULL, name.c_str(), _T("MyShell"), MB_OK);
                sel_files.push_back(name);
            }
        } catch (...) {
            *ok = false;
        }
        delete []str;
        GlobalUnlock(stm.hGlobal);
    }

    ReleaseStgMedium(&stm);
    return sel_files;
}

std::wstring catFilename(std::vector<std::wstring> vec, bool quote, bool only_name, std::wstring join) {
    auto func = [&](std::wstring name) -> std::wstring {
        if (only_name) {
            auto sp = Utils::wstring_split(name, _T("\\"));
            if (!sp.empty()) {
                return sp.at(sp.size() - 1);
            }
        }
        return name;
    };
    std::wstring out = func(vec.at(0));
    if (quote) {
        out = _T("\"") + out + _T("\"");
    }
    for (auto iter = vec.begin() + 1; iter != vec.end(); iter++) {
        if (quote) {
            out += join + _T("\"") + func(*iter) + _T("\"");
        } else {
            out += join + func(*iter);
        }
    }
    return out;
}

HRESULT STDMETHODCALLTYPE CCopyMenu::InvokeCommand( 
    __in CMINVOKECOMMANDINFO *pici
) {
    UINT idCmd = LOWORD(pici->lpVerb);
    if (HIWORD(pici->lpVerb) != 0) {
        return E_INVALIDARG;
    }

    // get
    std::vector<std::wstring> sel_files;
    if (this->pdtobj != nullptr) {
        bool ok;
        sel_files = getFilenameFromDataObject(this->pdtobj, &ok);
        if (!ok) {
            MessageBox(nullptr, _T("ファイルの名前のロードはエラーが発生しました。"), _T("エラー"), MB_OK);
            return S_OK;
        }
    } else if (this->pidlFolder != nullptr) {
        auto current_path = new WCHAR[MAX_PATH_LENGTH];
        if (!SHGetPathFromIDListW(this->pidlFolder, current_path)) {
            MessageBox(nullptr, L"ファイルの名前のロードはエラーが発生しました。",  _T("エラー"), MB_OK);
            return S_FALSE;
        }
        sel_files.emplace_back(current_path);
    } else {
        return S_OK;
    }

    // !!! actions
    std::wstring out;
    switch (idCmd) {
    case MENU_ID_NQ_BCK: // 引用符なし \ 
        out = catFilename(sel_files, false, false, _T(";"));
        break;
    case MENU_ID_Q_BCK: // 引用符付き "\"
        out = catFilename(sel_files, true, false, _T(";"));
        break;
    case MENU_ID_NQ_FNT: // 引用符なし /
        out = catFilename(sel_files, false, false, _T(";"));
        Utils::wstring_replace(out, _T("\\"), _T("/"));
        break;
    case MENU_ID_Q_FNT: // 引用符付き "/"
        out = catFilename(sel_files, true, false, _T(";"));
        Utils::wstring_replace(out, _T("\\"), _T("/"));
        break;
    case MENU_ID_NQ_DBL: // 引用符なし \ 
        out = catFilename(sel_files, false, false, _T(";"));
        Utils::wstring_replace(out, _T("\\"), _T("\\\\"));
        break;
    case MENU_ID_Q_DBL: // 引用符付き "\\"
        out = catFilename(sel_files, true, false, _T(";"));
        Utils::wstring_replace(out, _T("\\"), _T("\\\\"));
        break;
    case MENU_ID_NQ_NME:  // 引用符なし 名前のみ
        out = catFilename(sel_files, false, true, _T(";"));
        break;
    case MENU_ID_Q_NME:  // 引用符付き 名前のみ
        out = catFilename(sel_files, true, true, _T(";"));
        break;

    case MENU_ID_NQ_BCK_COMMA: // 引用符なし \ 
        out = catFilename(sel_files, false, false, _T(","));
        break;
    case MENU_ID_Q_BCK_COMMA: // 引用符付き "\"
        out = catFilename(sel_files, true, false, _T(","));
        break;
    case MENU_ID_NQ_FNT_COMMA: // 引用符なし /
        out = catFilename(sel_files, false, false, _T(","));
        Utils::wstring_replace(out, _T("\\"), _T("/"));
        break;
    case MENU_ID_Q_FNT_COMMA: // 引用符付き "/"
        out = catFilename(sel_files, true, false, _T(","));
        Utils::wstring_replace(out, _T("\\"), _T("/"));
        break;
    case MENU_ID_NQ_DBL_COMMA: // 引用符なし \ 
        out = catFilename(sel_files, false, false, _T(","));
        Utils::wstring_replace(out, _T("\\"), _T("\\\\"));
        break;
    case MENU_ID_Q_DBL_COMMA: // 引用符付き "\\"
        out = catFilename(sel_files, true, false, _T(","));
        Utils::wstring_replace(out, _T("\\"), _T("\\\\"));
        break;
    case MENU_ID_NQ_NME_COMMA:  // 引用符なし 名前のみ
        out = catFilename(sel_files, false, true, _T(","));
        break;
    case MENU_ID_Q_NME_COMMA:  // 引用符付き 名前のみ
        out = catFilename(sel_files, true, true, _T(","));
        break;
    }
    
    // copy
    try {
        if (OpenClipboard(nullptr)) {
            EmptyClipboard();
            HGLOBAL buf = GlobalAlloc(GHND | GMEM_SHARE, (out.length() + 1) * sizeof(WCHAR));
            auto strMem = (LPWSTR) GlobalLock(buf);
            wcscpy(strMem, out.data());
            GlobalUnlock(buf);

            SetClipboardData(CF_UNICODETEXT, buf);
            CloseClipboard();
        }
    } catch (...) { }

    return E_INVALIDARG;
}

HRESULT STDMETHODCALLTYPE CCopyMenu::GetCommandString( 
    __in UINT_PTR idCmd,
    __in UINT uType,
    __reserved UINT *pReserved,
    __out_awcount(!(uType & GCS_UNICODE), cchMax) LPSTR pszName,
    __in UINT cchMax
) {
    return S_OK;
}
