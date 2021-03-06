/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 1992-2019 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */


#ifndef _DIALOG_EDIT_COMPONENT_IN_SCHEMATIC_H_
#define _DIALOG_EDIT_COMPONENT_IN_SCHEMATIC_H_

#include <dialog_edit_component_in_schematic_base.h>
#include <fields_grid_table.h>
#include <sch_pin.h>


class LIB_PART;
class SCH_PIN_TABLE_DATA_MODEL;
class SCH_EDIT_FRAME;


// The dialog can be closed for several reasons.
enum SYMBOL_PROPS_RETVALUE
{
    SYMBOL_PROPS_WANT_UPDATE_SYMBOL,
    SYMBOL_PROPS_WANT_EXCHANGE_SYMBOL,
    SYMBOL_PROPS_EDIT_OK,
    SYMBOL_PROPS_EDIT_SCHEMATIC_SYMBOL,
    SYMBOL_PROPS_EDIT_LIBRARY_SYMBOL
};


/**
 * Dialog used to edit #SCH_COMPONENT objects in a schematic.
 *
 * This is derived from DIALOG_EDIT_COMPONENT_IN_SCHEMATIC_BASE which is maintained by
 * wxFormBuilder.
 */
class DIALOG_EDIT_COMPONENT_IN_SCHEMATIC : public DIALOG_EDIT_COMPONENT_IN_SCHEMATIC_BASE
{
public:
    DIALOG_EDIT_COMPONENT_IN_SCHEMATIC( SCH_EDIT_FRAME* aParent, SCH_COMPONENT* aComponent );
    ~DIALOG_EDIT_COMPONENT_IN_SCHEMATIC() override;

    SCH_EDIT_FRAME* GetParent();

private:
    bool TransferDataToWindow() override;
    bool TransferDataFromWindow() override;

    bool Validate() override;

    // event handlers
    void OnAddField( wxCommandEvent& event ) override;
    void OnDeleteField( wxCommandEvent& event ) override;
    void OnMoveUp( wxCommandEvent& event ) override;
    void OnMoveDown( wxCommandEvent& event ) override;
    void OnEditSpiceModel( wxCommandEvent& event ) override;
    void OnPinTableColSort( wxGridEvent& aEvent );
    void OnPinTableCellEdited( wxGridEvent& event ) override;
    void OnSizeGrid( wxSizeEvent& event ) override;
    void OnGridCellChanging( wxGridEvent& event );
    void OnUpdateUI( wxUpdateUIEvent& event ) override;
    void OnCancelButtonClick( wxCommandEvent& event ) override;
    void OnInitDlg( wxInitDialogEvent& event ) override;

    void OnEditSymbol( wxCommandEvent&  ) override;
    void OnEditLibrarySymbol( wxCommandEvent&  ) override;
    void OnUpdateSymbol( wxCommandEvent&  ) override;
    void OnExchangeSymbol( wxCommandEvent&  ) override;

    void AdjustGridColumns( int aWidth );

private:
    SCH_COMPONENT* m_comp;
    LIB_PART*      m_part;

    int            m_width;
    int            m_delayedFocusRow;
    int            m_delayedFocusColumn;
    wxString       m_shownColumns;

    FIELDS_GRID_TABLE<SCH_FIELD>* m_fields;
    SCH_PIN_TABLE_DATA_MODEL*     m_dataModel;
};

#endif // _DIALOG_EDIT_COMPONENT_IN_SCHEMATIC_H_
