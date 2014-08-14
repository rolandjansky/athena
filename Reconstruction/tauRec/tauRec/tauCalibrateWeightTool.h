/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUCALIBRATEWEIGHTTOOL_H
#define TAUREC_TAUCALIBRATEWEIGHTTOOL_H
/********************************************************************
(depreciated!)

NAME:     tauCalibrateWeightTool.h
PACKAGE:  offline/Reconstruction/tauRec
AUTHORS:  M.Heldmann
CREATED:  March 15, 2005

See comments in tauCalibrateWeightTool.cxx.

13/12/2008 - (AK) change to extrapolation of TP instead of Track+code cleaning
 ********************************************************************/

#include "tauRec/TauToolBase.h"
#include "GaudiKernel/ToolHandle.h"


class TauCandidateData;
class LArEM_ID;
class TileID;
class IHistogram1D;
class IHadronicCalibrationTool;

//-------------------------------------------------------------
//! Class for applying H1-weighting on calorimeter cells and fudge factor for tau energy
//-------------------------------------------------------------
class tauCalibrateWeightTool : public TauToolBase
{
    public:
        enum calibrateType { calCells = 0, calJets, calTracks, calCluster, calTopocluster };

        tauCalibrateWeightTool(const std::string& type,
                const std::string& name,
                const IInterface* parent);
        ~tauCalibrateWeightTool();

        virtual StatusCode initialize();
        virtual StatusCode finalize();
        virtual StatusCode execute( TauCandidateData *data );

        // Accessor methods:

    private:
        int m_calibrateType;
        ToolHandle<IHadronicCalibrationTool> m_caloWeightTool;

        const LArEM_ID* m_emid;
        const TileID* m_tileid;

        std::string m_cellWeightTool;
        std::vector<float> m_poly;

        bool m_applyCellWeightEM;
        bool m_applyCellWeightHad;
        bool m_applyPtEtaCorrFactors;
        bool m_validCaloWeightTool;
        bool m_doEtaInterpolation;

        float m_fudge;
        double m_cellCone;

        int m_nptbins;
        int m_netabins;

        std::vector<float> m_ptpoints;
        std::vector<float> m_etapoints;
        std::vector<float> m_ptetacorrectionsntr1;
        std::vector<float> m_ptetacorrectionsntr23;
};

#endif
