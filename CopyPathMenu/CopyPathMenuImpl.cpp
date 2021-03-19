#include "pch.h"
#include "CopyPathMenuImpl.h"

HRESULT STDMETHODCALLTYPE CCopyPathMenuImpl::Initialize(
    __in_opt PCIDLIST_ABSOLUTE pidlFolder,
    __in_opt IDataObject *pdtobj,
    __in_opt HKEY hkeyProgID
) {
    this->curr_folder = pidlFolder;
    this->sel_object = pdtobj;
    this->sel_count = 0;

    FORMATETC fe = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stm = {};
    if (pdtobj != nullptr && SUCCEEDED(pdtobj->GetData(&fe, &stm))) {
        auto hDrop = static_cast<HDROP>(GlobalLock(stm.hGlobal));
        if (hDrop != nullptr) {
            this->sel_count = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
        }
    }

    return S_OK;
}

// Naming
// N: No quote; Q: quote
// BCK: back slash; FNT: front slash; DBL: double bask slash; NME: name only
const UINT MENUID_RESTART_EXPLORER = 0;
const UINT MENUID_N_BCK = 1;
const UINT MENUID_Q_BCK = 2;
const UINT MENUID_N_FNT = 3;
const UINT MENUID_Q_FNT = 4;
const UINT MENUID_N_DBL = 5;
const UINT MENUID_Q_DBL = 6;
const UINT MENUID_N_NME = 7;
const UINT MENUID_Q_NME = 8;
const UINT MENUID_N_BCK_COMMA = 9;
const UINT MENUID_Q_BCK_COMMA = 10;
const UINT MENUID_N_FNT_COMMA = 11;
const UINT MENUID_Q_FNT_COMMA = 12;
const UINT MENUID_N_DBL_COMMA = 13;
const UINT MENUID_Q_DBL_COMMA = 14;
const UINT MENUID_N_NME_COMMA = 15;
const UINT MENUID_Q_NME_COMMA = 16;

/**
 * @brief Represents a menu action.
 */
struct MenuAction {
    LPWSTR title;
    UINT id;
    UINT pos;
};

HRESULT STDMETHODCALLTYPE CCopyPathMenuImpl::QueryContextMenu(
    __in HMENU hmenu,
    __in UINT indexMenu,
    __in UINT idCmdFirst,
    __in UINT idCmdLast,
    __in UINT uFlags
) {
    if (uFlags & CMF_VERBSONLY) {
        return S_OK;
    }
    if (uFlags & CMF_DEFAULTONLY) {
        return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
    }
    if (this->curr_folder == nullptr && this->sel_object == nullptr) {
        return S_FALSE;
    }

    // menu actions
    std::vector<MenuAction> actions;
    if (this->sel_count <= 1) { // directory background or single file
        actions = {
            {L"���p���Ȃ� �o�b�N�X���b�V�� (\\)", MENUID_N_BCK}, // |\|
            {L"���p���t�� �o�b�N�X���b�V�� (\"\\\")", MENUID_Q_BCK}, // |"\"|
            {L"���p���Ȃ� �t�����g�X���b�V�� (/)", MENUID_N_FNT}, // |/|
            {L"���p���t�� �t�����g�X���b�V�� (\"/\")", MENUID_Q_FNT}, // |"/"|
            {L"���p���Ȃ� �_�u���o�b�N�X���b�V�� (\\\\)", MENUID_N_DBL}, // |\\|
            {L"���p���t�� �_�u���o�b�N�X���b�V�� (\"\\\\\")", MENUID_Q_DBL}, // |"\\"|
            {L"", 0}, // splitter
            {L"���p���Ȃ� ���O�̂� (x)", MENUID_N_NME}, // |x|
            {L"���p���t�� ���O�̂� (\"x\")", MENUID_Q_NME}, // |"x"|
        };
    } else { // multiple files
        actions = {
            {L"���p���Ȃ� �o�b�N�X���b�V�� �Z�~�R���� (\\;\\)", MENUID_N_BCK}, // |\|
            {L"���p���t�� �o�b�N�X���b�V�� �Z�~�R���� (\"\\\";\"\\\")", MENUID_Q_BCK}, // |"\"|
            {L"���p���Ȃ� �t�����g�X���b�V�� �Z�~�R���� (/;/)", MENUID_N_FNT}, // |/|
            {L"���p���t�� �t�����g�X���b�V�� �Z�~�R���� (\"/\";\"/\")", MENUID_Q_FNT}, // |"/"|
            {L"���p���Ȃ� �_�u���o�b�N�X���b�V�� �Z�~�R���� (\\\\;\\\\)", MENUID_N_DBL}, // |\\|
            {L"���p���t�� �_�u���o�b�N�X���b�V�� �Z�~�R���� (\"\\\\\";\"\\\\\")", MENUID_Q_DBL}, // |"\\"|
            {L"", 0}, // splitter
            {L"���p���Ȃ� ���O�̂� �Z�~�R���� (;)", MENUID_N_NME}, // |x|
            {L"���p���t�� ���O�̂� �Z�~�R���� (;)", MENUID_Q_NME}, // |"x"|
            {L"", 0}, // splitter
            {L"���p���Ȃ� �o�b�N�X���b�V�� �R���} (\\,\\)", MENUID_N_BCK_COMMA},  // |\|
            {L"���p���t�� �o�b�N�X���b�V�� �R���} (\"\\\",\"\\\")", MENUID_Q_BCK_COMMA}, // |"\"|
            {L"���p���Ȃ� �t�����g�X���b�V�� �R���} (/,/)", MENUID_N_FNT_COMMA}, // |/|
            {L"���p���t�� �t�����g�X���b�V�� �R���} (\"/\",\"/\")", MENUID_Q_FNT_COMMA}, // |"/"|
            {L"���p���Ȃ� �_�u���o�b�N�X���b�V�� �R���} (\\\\,\\\\)", MENUID_N_DBL_COMMA}, // |\\|
            {L"���p���t�� �_�u���o�b�N�X���b�V�� �R���} (\"\\\\\",\"\\\\\")", MENUID_Q_DBL_COMMA}, // |"\\"|
            {L"", 0}, // splitter
            {L"���p���Ȃ� ���O�̂� �R���} (,)", MENUID_N_NME_COMMA,17}, // |x|
            {L"���p���t�� ���O�̂� �R���} (,)", MENUID_Q_NME_COMMA, 18}, // |"x"|
        };
    }

    // copy sub menu
    HMENU copy_menu = CreateMenu();

    // temp mii
    MENUITEMINFO mii;
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_STRING | MIIM_ID;

    // splitter mii
    MENUITEMINFO mii_splitter;
    mii_splitter.cbSize = sizeof(MENUITEMINFO);
    mii_splitter.fMask = MIIM_FTYPE;
    mii_splitter.fType = MFT_SEPARATOR;

    // command count
    int cmd_count = 0;

    // insert to sub menu
    for (int i = 0; i < actions.size(); i++) {
        auto action = actions.at(i);
        if (action.id == 0) {
            InsertMenuItem(copy_menu, i, true, &mii_splitter);
        } else {
            mii.dwTypeData = action.title;
            mii.wID = idCmdFirst + action.id;
            InsertMenuItem(copy_menu, i, true, &mii);
            cmd_count++;
        }
    }

    // path copy menu
    MENUITEMINFO mii_copy;
    mii_copy.cbSize = sizeof(MENUITEMINFO);
    mii_copy.fMask = MIIM_STRING | MIIM_SUBMENU;
    mii_copy.dwTypeData = _T("�p�X�̃R�s�[(&P)");
    mii_copy.hSubMenu = copy_menu;

    // restart explorer
    MENUITEMINFO mii_re;
    mii_re.cbSize = sizeof(MENUITEMINFO);
    mii_re.fMask = MIIM_STRING | MIIM_ID;
    mii_re.dwTypeData = _T("�G�N�X�v���[���[���ċN��(&R)");
    mii_re.wID = idCmdFirst + MENUID_RESTART_EXPLORER;

    // insert to hmenu
    if (this->sel_count == 0) { // folder background
        cmd_count++;
        InsertMenuItem(hmenu, indexMenu + 0, true, &mii_splitter);
        InsertMenuItem(hmenu, indexMenu + 1, true, &mii_copy);
        InsertMenuItem(hmenu, indexMenu + 2, true, &mii_re);
        InsertMenuItem(hmenu, indexMenu + 3, true, &mii_splitter);
    } else {
        InsertMenuItem(hmenu, indexMenu + 0, true, &mii_splitter);
        InsertMenuItem(hmenu, indexMenu + 1, true, &mii_copy);
        InsertMenuItem(hmenu, indexMenu + 2, true, &mii_splitter);
    }

    // make hResult
    return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, (USHORT) cmd_count);
}

HRESULT STDMETHODCALLTYPE CCopyPathMenuImpl::InvokeCommand(
    __in CMINVOKECOMMANDINFO *pici
) {
    UINT idCmd = LOWORD(pici->lpVerb);
    if (HIWORD(pici->lpVerb) != 0) {
        return E_INVALIDARG;
    }
    if (this->curr_folder == nullptr && this->sel_object == nullptr) {
        return S_FALSE;
    }

    // restart explorer
    if (idCmd == MENUID_RESTART_EXPLORER) {
        system("taskkill /f /im explorer.exe && start explorer.exe");
        return S_OK;
    }

    // other commands
    switch (idCmd) {
        // TODO
    }

    return S_OK;
}
