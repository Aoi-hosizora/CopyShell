#include "stdafx.h"
#include "CopyMenu.h"
#include "Utils.hpp"

#include <atlconv.h>
#include <string>

#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

const UINT MAX_PATH_LENGTH = 2048;

// Context menu popuped.
HRESULT STDMETHODCALLTYPE CCopyMenu::Initialize(
    __in_opt PCIDLIST_ABSOLUTE pidlFolder,
    __in_opt IDataObject *pdtobj,
    __in_opt HKEY hkeyProgID
) {
    this->pdtobj = pdtobj;
    this->pidlFolder = pidlFolder;
    this->selected_count = 0;

    FORMATETC fe = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stm = {};
    if (pdtobj != nullptr && SUCCEEDED(pdtobj->GetData(&fe, &stm))) {
        auto hDrop = static_cast<HDROP>(GlobalLock(stm.hGlobal));
        if (hDrop != nullptr) {
            this->selected_count = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
        }
    }
    return S_OK;
}

// Naming
// N: No quote; Q: quote
// BCK: back slash; FNT: front slash; DBL: double bask slash; NME: name only
const UINT MENUID_N_BCK = 0;
const UINT MENUID_Q_BCK = 1;
const UINT MENUID_N_FNT = 2;
const UINT MENUID_Q_FNT = 3;
const UINT MENUID_N_DBL = 4;
const UINT MENUID_Q_DBL = 5;
const UINT MENUID_N_NME = 6;
const UINT MENUID_Q_NME = 7;
const UINT MENUID_N_BCK_COMMA = 8;
const UINT MENUID_Q_BCK_COMMA = 9;
const UINT MENUID_N_FNT_COMMA = 10;
const UINT MENUID_Q_FNT_COMMA = 11;
const UINT MENUID_N_DBL_COMMA = 12;
const UINT MENUID_Q_DBL_COMMA = 13;
const UINT MENUID_N_NME_COMMA = 14;
const UINT MENUID_Q_NME_COMMA = 15;

// Create menu item.
HRESULT STDMETHODCALLTYPE CCopyMenu::QueryContextMenu(
    __in HMENU hmenu,
    __in UINT indexMenu,
    __in UINT idCmdFirst,
    __in UINT idCmdLast,
    __in UINT uFlags
) {
    if (uFlags & CMF_VERBSONLY) {
        return S_OK;
    }
    if (this->pdtobj == nullptr && this->pidlFolder == nullptr) { // error
        return S_OK;
    }
    if (uFlags & CMF_DEFAULTONLY) {
        return MAKE_SCODE(SEVERITY_SUCCESS, FACILITY_NULL, 0);
    }

    // out layer menu
    MENUITEMINFO mii_out;
    mii_out.cbSize = sizeof(MENUITEMINFO);
    mii_out.fMask = MIIM_STRING | MIIM_SUBMENU;
    mii_out.dwTypeData = _T("パスのコピー");
    // sub menu
    HMENU mii_sub = CreateMenu();
    mii_out.hSubMenu = mii_sub;
    // splitter mii
    MENUITEMINFO mii_split;
    mii_split.cbSize = sizeof(MENUITEMINFO);
    mii_split.fMask = MIIM_FTYPE;
    mii_split.fType = MFT_SEPARATOR;

    InsertMenuItem(hmenu, indexMenu + 0, true, &mii_split);
    InsertMenuItem(hmenu, indexMenu + 1, true, &mii_out);
    InsertMenuItem(hmenu, indexMenu + 2, true, &mii_split);

    // temp mii
    MENUITEMINFO mii;
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_STRING | MIIM_ID;
    // menu item count
    int command_count;

    if (this->selected_count <= 1) { // directory background or single file
        // \ -> F:\Projects\CopyShell\.gitignore
        mii.dwTypeData = _T("引用符なし バックスラッシュ (\\)");
        mii.wID = idCmdFirst + MENUID_N_BCK;
        InsertMenuItem(mii_sub, 0, true, &mii);

        // "\" -> "F:\Projects\CopyShell\.gitignore"
        mii.dwTypeData = _T("引用符付き バックスラッシュ (\"\\\")");
        mii.wID = idCmdFirst + MENUID_Q_BCK;
        InsertMenuItem(mii_sub, 1, true, &mii);

        // / -> F:/Projects/CopyShell/.gitignore
        mii.dwTypeData = _T("引用符なし フロントスラッシュ (/)");
        mii.wID = idCmdFirst + MENUID_N_FNT;
        InsertMenuItem(mii_sub, 2, true, &mii);

        // "/" -> "F:/Projects/CopyShell/.gitignore"
        mii.dwTypeData = _T("引用符付き フロントスラッシュ (\"/\")");
        mii.wID = idCmdFirst + MENUID_Q_FNT;
        InsertMenuItem(mii_sub, 3, true, &mii);

        // \\ -> F:\\Projects\\CopyShell\\.gitignore
        mii.dwTypeData = _T("引用符なし ダブルバックスラッシュ (\\\\)");
        mii.wID = idCmdFirst + MENUID_N_DBL;
        InsertMenuItem(mii_sub, 4, true, &mii);

        // "\\" -> "F:\\Projects\\CopyShell\\.gitignore"
        mii.dwTypeData = _T("引用符付き ダブルバックスラッシュ (\"\\\\\")");
        mii.wID = idCmdFirst + MENUID_Q_DBL;
        InsertMenuItem(mii_sub, 5, true, &mii);

        // splitter
        InsertMenuItem(mii_sub, 6, true, &mii_split);

        // -> .gitignore
        mii.dwTypeData = _T("引用符なし 名前のみ (x)");
        mii.wID = idCmdFirst + MENUID_N_NME;
        InsertMenuItem(mii_sub, 7, true, &mii);

        // -> ".gitignore"
        mii.dwTypeData = _T("引用符付き 名前のみ (\"x\")");
        mii.wID = idCmdFirst + MENUID_Q_NME;
        InsertMenuItem(mii_sub, 8, true, &mii);

        command_count = 8;
    } else { // multiple files
        // \ -> F:\Projects\CopyShell\.gitignore
        mii.dwTypeData = _T("引用符なし バックスラッシュ セミコロン (\\;\\)");
        mii.wID = idCmdFirst + MENUID_N_BCK;
        InsertMenuItem(mii_sub, 0, true, &mii);

        // "\" -> "F:\Projects\CopyShell\.gitignore"
        mii.dwTypeData = _T("引用符付き バックスラッシュ セミコロン (\"\\\";\"\\\")");
        mii.wID = idCmdFirst + MENUID_Q_BCK;
        InsertMenuItem(mii_sub, 1, true, &mii);

        // / -> F:/Projects/CopyShell/.gitignore
        mii.dwTypeData = _T("引用符なし フロントスラッシュ セミコロン (/;/)");
        mii.wID = idCmdFirst + MENUID_N_FNT;
        InsertMenuItem(mii_sub, 2, true, &mii);

        // "/" -> "F:/Projects/CopyShell/.gitignore"
        mii.dwTypeData = _T("引用符付き フロントスラッシュ セミコロン (\"/\";\"/\")");
        mii.wID = idCmdFirst + MENUID_Q_FNT;
        InsertMenuItem(mii_sub, 3, true, &mii);

        // \\ -> F:\\Projects\\CopyShell\\.gitignore
        mii.dwTypeData = _T("引用符なし ダブルバックスラッシュ セミコロン (\\\\;\\\\)");
        mii.wID = idCmdFirst + MENUID_N_DBL;
        InsertMenuItem(mii_sub, 4, true, &mii);

        // "\\" -> "F:\\Projects\\CopyShell\\.gitignore"
        mii.dwTypeData = _T("引用符付き ダブルバックスラッシュ セミコロン (\"\\\\\";\"\\\\\")");
        mii.wID = idCmdFirst + MENUID_Q_DBL;
        InsertMenuItem(mii_sub, 5, true, &mii);

        // splitter
        InsertMenuItem(mii_sub, 6, true, &mii_split);

        // -> .gitignore
        mii.dwTypeData = _T("引用符なし 名前のみ セミコロン (;)");
        mii.wID = idCmdFirst + MENUID_N_NME;
        InsertMenuItem(mii_sub, 7, true, &mii);

        // -> ".gitignore"
        mii.dwTypeData = _T("引用符付き 名前のみ セミコロン (;)");
        mii.wID = idCmdFirst + MENUID_Q_NME;
        InsertMenuItem(mii_sub, 8, true, &mii);

        // splitter
        InsertMenuItem(mii_sub, 9, true, &mii_split);

        // \ -> F:\Projects\CopyShell\.gitignore
        mii.dwTypeData = _T("引用符なし バックスラッシュ セミコロン (\\,\\)");
        mii.wID = idCmdFirst + MENUID_N_BCK_COMMA;
        InsertMenuItem(mii_sub, 10, true, &mii);

        // "\" -> "F:\Projects\CopyShell\.gitignore"
        mii.dwTypeData = _T("引用符付き バックスラッシュ コンマ (\"\\\",\"\\\")");
        mii.wID = idCmdFirst + MENUID_Q_BCK_COMMA;
        InsertMenuItem(mii_sub, 11, true, &mii);

        // / -> F:/Projects/CopyShell/.gitignore
        mii.dwTypeData = _T("引用符なし フロントスラッシュ コンマ (/,/)");
        mii.wID = idCmdFirst + MENUID_N_FNT_COMMA;
        InsertMenuItem(mii_sub, 12, true, &mii);

        // "/" -> "F:/Projects/CopyShell/.gitignore"
        mii.dwTypeData = _T("引用符付き フロントスラッシュ コンマ (\"/\",\"/\")");
        mii.wID = idCmdFirst + MENUID_Q_FNT_COMMA;
        InsertMenuItem(mii_sub, 13, true, &mii);

        // \\ -> F:\\Projects\\CopyShell\\.gitignore
        mii.dwTypeData = _T("引用符なし ダブルバックスラッシュ コンマ (\\\\,\\\\)");
        mii.wID = idCmdFirst + MENUID_N_DBL_COMMA;
        InsertMenuItem(mii_sub, 14, true, &mii);

        // "\\" -> "F:\\Projects\\CopyShell\\.gitignore"
        mii.dwTypeData = _T("引用符付き ダブルバックスラッシュ コンマ (\"\\\\\",\"\\\\\")");
        mii.wID = idCmdFirst + MENUID_Q_DBL_COMMA;
        InsertMenuItem(mii_sub, 15, true, &mii);

        // splitter
        InsertMenuItem(mii_sub, 16, true, &mii_split);

        // -> .gitignore
        mii.dwTypeData = _T("引用符なし 名前のみ コンマ (x,y)");
        mii.wID = idCmdFirst + MENUID_N_NME_COMMA;
        InsertMenuItem(mii_sub, 17, true, &mii);

        // -> ".gitignore"
        mii.dwTypeData = _T("引用符付き 名前のみ コンマ (\"x\",\"y\")");
        mii.wID = idCmdFirst + MENUID_Q_NME_COMMA;
        InsertMenuItem(mii_sub, 18, true, &mii);

        command_count = 16;
    }

    return MAKE_SCODE(SEVERITY_SUCCESS, FACILITY_NULL, command_count);
}

std::vector<std::wstring> getFilenamesFromDataObject(IDataObject *pdtobj, bool *ok) {
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
                sel_files.push_back(name);
            }
        } catch (...) {
            *ok = false;
        }
        delete[] str;
        GlobalUnlock(stm.hGlobal);
    }

    ReleaseStgMedium(&stm);
    return sel_files;
}

std::wstring joinFilenames(std::vector<std::wstring> vec, bool quote, bool only_name, bool use_comma) {
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
    std::wstring join = _T(";");
    if (use_comma) {
        join = _T(",");
    }
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

// Invoke menu item command.
HRESULT STDMETHODCALLTYPE CCopyMenu::InvokeCommand(
    __in CMINVOKECOMMANDINFO *pici
) {
    UINT idCmd = LOWORD(pici->lpVerb);
    if (HIWORD(pici->lpVerb) != 0) {
        return E_INVALIDARG;
    }
    if (this->pidlFolder == nullptr && this->pdtobj == nullptr) { // error
        return S_FALSE;
    }

    // get selected filename
    std::vector<std::wstring> sel_names;
    if (this->pidlFolder != nullptr) { // directory background
        auto current_path = new WCHAR[MAX_PATH_LENGTH];
        if (!SHGetPathFromIDListW(this->pidlFolder, current_path)) {
            MessageBox(nullptr, L"ファイルネームの取得にエラーが発生しました。", _T("エラー"), MB_OK);
            return S_FALSE;
        }
        sel_names.emplace_back(current_path);
    } else { // file
        bool ok;
        sel_names = getFilenamesFromDataObject(this->pdtobj, &ok);
        if (!ok) {
            MessageBox(nullptr, _T("ファイルネームの取得にエラーが発生しました。"), _T("エラー"), MB_OK);
            return S_FALSE;
        }
    }

    // menu item action
    std::wstring out;
    switch (idCmd) {
    case MENUID_N_BCK:
        out = joinFilenames(sel_names, false, false, false);
        break;
    case MENUID_Q_BCK:
        out = joinFilenames(sel_names, true, false, false);
        break;
    case MENUID_N_FNT:
        out = joinFilenames(sel_names, false, false, false);
        Utils::wstring_replace(out, _T("\\"), _T("/"));
        break;
    case MENUID_Q_FNT:
        out = joinFilenames(sel_names, true, false, false);
        Utils::wstring_replace(out, _T("\\"), _T("/"));
        break;
    case MENUID_N_DBL:
        out = joinFilenames(sel_names, false, false, false);
        Utils::wstring_replace(out, _T("\\"), _T("\\\\"));
        break;
    case MENUID_Q_DBL:
        out = joinFilenames(sel_names, true, false, false);
        Utils::wstring_replace(out, _T("\\"), _T("\\\\"));
        break;
    case MENUID_N_NME:
        out = joinFilenames(sel_names, false, true, false);
        break;
    case MENUID_Q_NME:
        out = joinFilenames(sel_names, true, true, false);
        break;
    case MENUID_N_BCK_COMMA:
        out = joinFilenames(sel_names, false, false, true);
        break;
    case MENUID_Q_BCK_COMMA:
        out = joinFilenames(sel_names, true, false, true);
        break;
    case MENUID_N_FNT_COMMA:
        out = joinFilenames(sel_names, false, false, true);
        Utils::wstring_replace(out, _T("\\"), _T("/"));
        break;
    case MENUID_Q_FNT_COMMA:
        out = joinFilenames(sel_names, true, false, true);
        Utils::wstring_replace(out, _T("\\"), _T("/"));
        break;
    case MENUID_N_DBL_COMMA:
        out = joinFilenames(sel_names, false, false, true);
        Utils::wstring_replace(out, _T("\\"), _T("\\\\"));
        break;
    case MENUID_Q_DBL_COMMA:
        out = joinFilenames(sel_names, true, false, true);
        Utils::wstring_replace(out, _T("\\"), _T("\\\\"));
        break;
    case MENUID_N_NME_COMMA:
        out = joinFilenames(sel_names, false, true, true);
        break;
    case MENUID_Q_NME_COMMA:
        out = joinFilenames(sel_names, true, true, true);
        break;
    }

    // copy to clipboard
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

    return S_OK;
}
