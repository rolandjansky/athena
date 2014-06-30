/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileMBTSFillerTool.h
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Description: CaloCell filler.
 *
 * Created in September 2012
 */

/**
 * @file TileMBTSFillerTool.h
 * @author Marco van Woerden <mvanwoer@cern.ch>
 * @date September, 2012
 * @brief Block filler tool for CaloCells, used for muon studies in TileCal.
 */

#ifndef TILEMUONCALOD3PDMAKER_MBTSFILLERTOOL_H
#define TILEMUONCALOD3PDMAKER_MBTSFILLERTOOL_H

/// D3PD INCLUDES
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"

/// GENERIC INCLUDES
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"

/// PHYSICS INCLUDES
#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"

/// TRACK INCLUDES
#include "Particle/TrackParticle.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/PropDirection.h"

/// TRACK TOOL INCLUDES
#include "Extrapolator.h"
#include "IExtrapolator.h"
#include "ITrackTools.h"
#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"

/// CALORIMETER INCLUDES
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCell.h"
#include "CaloIdentifier/CaloID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileEvent/TileCell.h"

/// UTILITIES INCLUDES
#include "Scintillator.h"
#include "CaloGeoHelpers/CaloPhiRange.h"

/// C++ INCLUDES
#include <vector>
#include <algorithm>
#include <math.h>
#include <string>
#include <cmath>

class StoreGateSvc;
class TileID;
class TileTBID;
class TileHWID;
class HWIdentifier;
class TileCablingService;

using namespace TICT;
using namespace std;

// ADD TILEMUONCELLFILLERTOOL TO D3PD NAMESPACE
namespace D3PD{

//============================================================
class TileMBTSFillerTool:public BlockFillerTool<TileCell>{
//============================================================

    public:
        // STANDARD GAUDI CONSTRUCTOR
        TileMBTSFillerTool (const std::string& type, const std::string& name, const IInterface* parent);
        ~TileMBTSFillerTool(){;};

        // BOOK VARIABLES
        virtual StatusCode initialize();
        virtual StatusCode book();
        virtual StatusCode fill (const TileCell& p);
        virtual StatusCode finalize();

    private:
        // LEVEL OF DETAILS
        short m_LevelOfDetails;
        std::string m_standalone;
        short m_trackType;

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
        std::vector<float>* m_to_mu_dx;
        std::vector<float>* m_to_mu_dy;
        std::vector<float>* m_to_mu_dz;
        std::vector<float>* m_to_mu_dEta;
        std::vector<float>* m_to_mu_dPhi;

//         std::vector<short>* m_mutype;
        std::vector<float>* m_muonX;
        std::vector<float>* m_muonY;
        std::vector<float>* m_muonZ;
        std::vector<float>* m_muonEta;
        std::vector<float>* m_muonPhi;

        // PATH LENGTH AND CELL RESPONSE
        std::vector<float>* m_dx_mu;
        std::vector<float>* m_celldEdx_mu;

        // CELL STATUS INFORMATION
        int *m_cellGain;
        bool *m_badCell;
        float *m_time;
        uint16_t *m_quality;

        unsigned int *m_samplingID;
        float *m_subcalo;

        // TOOLHANDLES
        ToolHandle<ITrackTools> m_inCalo;
        IExtrapolator *m_exCalo;

        StoreGateSvc* m_storeGate;
        StoreGateSvc* m_detStore;

        // CELL POSITIONING
        const TileID * m_tileID;
        const TileHWID * m_tileHWID;
        short *m_side;
        short *m_section;
        short *m_module;
        short *m_tower;
        short *m_sample;
        short *m_eta_index;

        // UTILITY
        double phidiff(double phi1, double phi2);

}; // TileMBTSFillerTool:BlockFillerTool<TileCell>

} // namespace D3PD

#endif // not TILEMUONCALOD3PDMAKER_CELLFILLERTOOL_H
