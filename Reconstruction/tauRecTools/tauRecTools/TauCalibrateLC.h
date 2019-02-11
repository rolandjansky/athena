/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUCALIBRATELC_H
#define TAUREC_TAUCALIBRATELC_H

#include "tauRecTools/TauRecToolBase.h"

class TH1;
class TF1;

/**
 * @brief Implementation of tau energy scale (TES) with eta and pile-up correction.
 * 
 *  The energy and eta (direction) correction are done separatly and steered by flags.
 * 
 * @author Margar Simonyan
 * @author Felix Friedrich
 *                                                                              
 */

class TauCalibrateLC : public TauRecToolBase {
public:

  ASG_TOOL_CLASS2( TauCalibrateLC, TauRecToolBase, ITauToolBase )

  TauCalibrateLC(const std::string& name="TauCalibrateLC");
    ~TauCalibrateLC();

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode execute(xAOD::TauJet& pTau);
    virtual StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode eventInitialize() { return StatusCode::SUCCESS; }
    virtual StatusCode eventFinalize() { return StatusCode::SUCCESS; }

    virtual void print() const { }


private:
    std::string m_configPath;
    std::string m_calibrationFile; //!< energy calibration file

    static const int s_nProngBins = 2;

    const TF1 * m_calibFunc[s_nProngBins][10]; //maximum 10 eta bins; might not be used on the whole 
    const TH1 * m_slopeNPVHist[s_nProngBins]={0};
    const TH1 * m_etaBinHist=0;
    const TH1 * m_etaCorrectionHist=0;

    unsigned int m_minNTrackAtVertex=0;
    int    m_nEtaBins=0;
    double m_averageNPV=0;

    bool m_doEnergyCorr; //!< switch for energy correction
    bool m_doPtResponse; //!< switch for pt response vs pt, if false, use E response vs E
    bool m_countOnlyPileupVertices; //!< switch for counting vertices by nTracks or VxType::PileUp
    bool m_doAxisCorr;   //!< switch for eta correction
    bool m_usePantauAxis; //!< switch for overwriting calo (eta,phi) with Pantau (eta,phi) 
    bool m_isCaloOnly;   //!< switch for CaloOnly corrections

    SG::ReadHandleKey<xAOD::VertexContainer> m_vertexInputContainer{this,"Key_vertexInputContainer", "PrimaryVertices", "input vertex container key"};

};

#endif
