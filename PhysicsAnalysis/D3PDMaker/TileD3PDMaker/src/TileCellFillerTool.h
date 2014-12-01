/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileCellFillerTool.h
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Description: CaloCell filler.
 *
 * Created in September 2012
 */

/**
 * @file TileCellFillerTool.h
 * @author Marco van Woerden <mvanwoer@cern.ch>
 * @date September, 2012
 * @brief Block filler tool for CaloCells, used for muon studies in TileCal.
 */

#ifndef TILEMUONCALOD3PDMAKER_CELLFILLERTOOL_H
#define TILEMUONCALOD3PDMAKER_CELLFILLERTOOL_H

/// D3PD INCLUDES
#include "D3PDMakerUtils/BlockFillerTool.h"

/// TRACK TOOL INCLUDES
#include "TrackTools.h"

#include "CaloGeoHelpers/CaloPhiRange.h"

//xAOD tools
#include "xAODMuon/MuonContainer.h"

//Type definitions
typedef xAOD::Muon MUON;
typedef xAOD::MuonContainer     MUONCONTAINER;

using namespace std;

// ADD TILEMUONCELLFILLERTOOL TO D3PD NAMESPACE
namespace D3PD{

//============================================================
class TileCellFillerTool:public BlockFillerTool<CaloCell>{
//============================================================

    public:
        // STANDARD GAUDI CONSTRUCTOR
        TileCellFillerTool (const std::string& type, const std::string& name, const IInterface* parent);
        ~TileCellFillerTool(){;};

        // BOOK VARIABLES
        virtual StatusCode initialize();
        virtual StatusCode book();
        virtual StatusCode fill (const CaloCell& p);
        virtual StatusCode finalize();

    private:
        // LEVEL OF DETAILS
        short m_LevelOfDetails;
        std::string m_standalone;
        short m_trackType;
        std::string          m_muonContainerName;


        // DEPOSITED ENERGY
        float *m_cellEnergy;
        float *m_cellEt;

        // FILTER CONFIGURATION
        int m_filterConfiguration;

        // PMT INFORMATION
        float *m_pmt1Energy;
        float *m_pmt2Energy;
        float *m_pmt1Time;
        float *m_pmt2Time;
        unsigned short *m_pmt1Quality;
        unsigned short *m_pmt2Quality;
        unsigned short *m_pmt1Qbit;
        unsigned short *m_pmt2Qbit;
        bool  *m_pmt1Bad;
        bool  *m_pmt2Bad;
        short *m_pmt1Gain;
        short *m_pmt2Gain;

        short *m_pmt1Ros;
        short *m_pmt2Ros;
        short *m_pmt1Drawer;
        short *m_pmt2Drawer;
        short *m_pmt1Channel;
        short *m_pmt2Channel;

        // CELL POSITION
        float *m_cellEta;
        float *m_cellPhi;
        float *m_cellSinTh;
        float *m_cellCosTh;
        float *m_cellCotTh;

        // CELL DIMENSIONS
        float *m_cellX;
        float *m_cellY;
        float *m_cellZ;
        float *m_cellR;
        float *m_celldX;
        float *m_celldY;
        float *m_celldZ;
        float *m_celldR;
        float *m_cellV;
        float *m_celldPhi;
        float *m_celldEta;

        // ASSOCIATED MUON TO CELL VARIABLES
        unsigned short *m_muN;
        std::vector<float>* m_to_muon_dx;
        std::vector<float>* m_to_muon_dy;
        std::vector<float>* m_to_muon_dz;
        std::vector<float>* m_to_muon_dEta;
        std::vector<float>* m_to_muon_dPhi;

        std::vector<short>* m_mutype;
        std::vector<short>* m_muonAuthor;
        std::vector<float>* m_muptcone40;
        std::vector<float>* m_mularcone40;
        std::vector<float>* m_mup;
        std::vector<short>* m_muNsiliconhits;

        std::vector<float>* m_muonX;
        std::vector<float>* m_muonY;
        std::vector<float>* m_muonZ;
        std::vector<float>* m_muonEta;
        std::vector<float>* m_muonPhi;

        // PATH LENGTH AND CELL RESPONSE
        std::vector<float>* m_dx;
        std::vector<float>* m_dEdx;

        // CELL STATUS INFORMATION
        int *m_cellGain;
        bool *m_badCell;
        float *m_time;
        uint16_t *m_quality;

        unsigned int *m_samplingID;
        float *m_subcalo;

        // TOOLHANDLES
        ToolHandle<ITrackTools> m_trackInCalo;

        StoreGateSvc* m_storeGate;

        // CELL POSITIONING
        const TileID * m_tileID;
        const TileHWID * m_tileHWID;
        short *m_side;
        short *m_section;
        short *m_module;
        short *m_tower;
        short *m_sample;
        short *m_eta_index;

}; // TileCellFillerTool:BlockFillerTool<CaloCell>

} // namespace D3PD

#endif // not TILEMUONCALOD3PDMAKER_CELLFILLERTOOL_H
