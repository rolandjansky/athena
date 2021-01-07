/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

    unsigned int m_minNTrackAtVertex=0;
    int    m_nEtaBins=0;
    double m_averageNPV=0;

    std::string m_calibrationFile; //!< energy calibration file
    bool m_doPtResponse; //!< switch for pt response vs pt, if false, use E response vs E
    bool m_isCaloOnly;   //!< switch for CaloOnly corrections

    SG::ReadDecorHandleKey<xAOD::EventInfo> m_aveIntPerXKey {this, 
        "averageInteractionsPerCrossingKey", 
        "EventInfo.averageInteractionsPerCrossing",
        "Decoration for Average Interaction Per Crossing"};
  
    SG::ReadHandleKey<xAOD::VertexContainer> m_vertexInputContainer {this,
        "Key_vertexInputContainer",
        "PrimaryVertices",
        "input vertex container key"};
};

#endif
