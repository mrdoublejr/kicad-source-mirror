/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2011-2018 Jean-Pierre Charras  jp.charras at wanadoo.fr
 * Copyright (C) 1992-2018 KiCad Developers, see AUTHORS.txt for contributors.
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

/**
 * @file select_layers_to_pcb.h
 */

#ifndef _SELECT_LAYERS_TO_PCB_H_
#define _SELECT_LAYERS_TO_PCB_H_

#include <dialogs/dialog_layers_select_to_pcb_base.h>

/*
 * This dialog shows the gerber files loaded, and allows user to choose
 * equivalence between gerber layers and pcb layers
 */
class LAYERS_MAP_DIALOG : public LAYERS_MAP_DIALOG_BASE
{
private:
    GERBVIEW_FRAME* m_Parent;
    int m_gerberActiveLayersCount;                  // Number of initialized gerber layers
    static int m_exportBoardCopperLayersCount;
    LAYER_NUM m_layersLookUpTable[GERBER_DRAWLAYERS_COUNT]; // Indexes Gerber layers to PCB file layers
                                                    // the last value in table is the number of copper layers
    int    m_buttonTable[int(GERBER_DRAWLAYERS_COUNT)+1];       // Indexes buttons to Gerber layers
    wxStaticText* m_layersList[int(GERBER_DRAWLAYERS_COUNT)+1]; // Indexes text strings to buttons

public: LAYERS_MAP_DIALOG( GERBVIEW_FRAME* parent );
    ~LAYERS_MAP_DIALOG() {};

    LAYER_NUM * GetLayersLookUpTable() { return m_layersLookUpTable; }
    static int GetCopperLayersCount() { return m_exportBoardCopperLayersCount; }

private:
    void initDialog();
    void normalizeBrdLayersCount();
    void OnBrdLayersCountSelection( wxCommandEvent& event ) override;
    void OnSelectLayer( wxCommandEvent& event );
    void OnOkClick( wxCommandEvent& event ) override;

    void OnStoreSetup( wxCommandEvent& event ) override;
    void OnGetSetup( wxCommandEvent& event ) override;
    void OnResetClick( wxCommandEvent& event ) override;

    /**
     * Finds number of loaded Gerbers where the matching KiCad layer can be identified
     *
     * The passed vector<int> will be returned with the same number of elements
     * as there are Gerber files.  The indices into it are 1:1 with the loaded Gerber
     * files.  Any Gerber that maps will have it's entry set to the KiCad PCB layer
     * number.  Gerbers that can be identified or which don't map to an
     * equivalent KiCad PCB layer will be set to UNSELECTED_LAYER.
     *
     * @param aGerber2KicadMapping passed to return KiCad PCB layer number for each Gerber
     *
     * @return int - The number of loaded Gerbers that have Altium extensions
     */
    int findKnownGerbersLoaded( std::vector<int>& aGerber2KicadMapping );

    /**
     * Finds number of loaded Gerbers using Altium file extensions
     *
     * The passed vector<int> will be returned with the same number of elements
     * as there are Gerber files.  The indices into it are 1:1 with the loaded Gerber
     * files.  Any Gerber that maps will have it's entry set to the KiCad PCB layer
     * number.  Gerbers that aren't using Altium extensions or which don't map to an
     * equivalent KiCad PCB layer will be set to UNSELECTED_LAYER.
     *
     * @param aGerber2KicadMapping passed to return KiCad PCB layer number for each Gerber
     *
     * @return int - The number of loaded Gerbers that have Altium extensions
     */
    int findNumAltiumGerbersLoaded( std::vector<int>& aGerber2KicadMapping );

    /**
     * Finds number of loaded Gerbers using KiCad naming convention
     *
     * The passed vector<int> will be returned with the same number of elements
     * as there are Gerber files.  The indices into it are 1:1 with the loaded Gerber
     * files.  Any Gerber that maps will have it's entry set to the KiCad PCB layer
     * number.  Gerbers that aren't using KiCad naming or which don't map to an
     * equivalent KiCad PCB layer will be set to UNSELECTED_LAYER.
     *
     * @param aGerber2KicadMapping passed to return KiCad PCB layer number for each Gerber
     *
     * @return int - The number of loaded Gerbers using KiCad naming conventions
     */
    int findNumKiCadGerbersLoaded( std::vector<int>& aGerber2KicadMapping );

    /**
     * Finds number of loaded Gerbers using X2 File Functions to define layers
     *
     * The passed vector<int> will be returned with the same number of elements
     * as there are Gerber files.  The indices into it are 1:1 with the loaded Gerber
     * files.  Any Gerber that maps will have it's entry set to the KiCad PCB layer
     * number.  Gerbers that aren't using X2 File functions or which don't map to an
     * equivalent KiCad PCB layer will be set to UNSELECTED_LAYER.
     *
     * @param aGerber2KicadMapping passed to return KiCad PCB layer number for each Gerber
     *
     * @return int - The number of loaded Gerbers with X2 File Functions
     */
    int findNumX2GerbersLoaded( std::vector<int>& aGerber2KicadMapping );


    DECLARE_EVENT_TABLE()
};

#endif      // _SELECT_LAYERS_TO_PCB_H_
