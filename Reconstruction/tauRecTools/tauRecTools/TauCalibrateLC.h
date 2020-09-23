/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUCALIBRATELC_H
#define TAUREC_TAUCALIBRATELC_H

#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgDataHandles/ReadDecorHandleKey.h"
#include "tauRecTools/TauRecToolBase.h"
#include "xAODEventInfo/EventInfo.h"

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

    virtual StatusCode initialize() override;
    virtual StatusCode execute(xAOD::TauJet& pTau) const override;


  private:
    static const int s_nProngBins = 2;

    std::vector<std::vector<std::unique_ptr<TF1>>> m_calibFunc;
    std::vector<std::unique_ptr<TH1>> m_slopeNPVHist; 
    std::unique_ptr<TH1> m_etaBinHist = nullptr; 
    std::unique_ptr<TH1> m_etaCorrectionHist = nullptr; 

    unsigned int m_minNTrackAtVertex=0;
    int    m_nEtaBins=0;
    double m_averageNPV=0;

    std::string m_calibrationFile; //!< energy calibration file
    bool m_doEnergyCorr; //!< switch for energy correction
    bool m_doPtResponse; //!< switch for pt response vs pt, if false, use E response vs E
    bool m_countOnlyPileupVertices; //!< switch for counting vertices by nTracks or VxType::PileUp
    bool m_doAxisCorr;   //!< switch for eta correction
    bool m_usePantauAxis; //!< switch for overwriting calo (eta,phi) with Pantau (eta,phi) 
    bool m_isCaloOnly;   //!< switch for CaloOnly corrections

    SG::ReadDecorHandleKey<xAOD::EventInfo> m_aveIntPerXKey {this, 
        "AveIntPerXKey", 
        "EventInfo.AveIntPerXDecor",
        "Decoration for Average Interaction Per Crossing"};
  
    SG::ReadHandleKey<xAOD::VertexContainer> m_vertexInputContainer {this,
        "Key_vertexInputContainer",
        "PrimaryVertices",
        "input vertex container key"};
};

#endif
