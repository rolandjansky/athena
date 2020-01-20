/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
#ifndef TAUREC_TAUCONVERSIONTAGGER_H
#define	TAUREC_TAUCONVERSIONTAGGER_H

#include "tauRecTools/TauRecToolBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "ITrackToVertex/ITrackToVertex.h"

/**
 * @brief This tool identifies if a tau track originates from a photon conversion track.
 * 
 * @author D. Varouchas
 */


class TauConversionTagger : public TauRecToolBase {
public:
    //-------------------------------------------------------------
    //! Constructor
    //-------------------------------------------------------------
    TauConversionTagger( const std::string& name );
    ASG_TOOL_CLASS2(TauConversionTagger, TauRecToolBase, ITauToolBase);

    //-------------------------------------------------------------
    //! Destructor
    //-------------------------------------------------------------
    ~TauConversionTagger();

    virtual StatusCode initialize() override;
    virtual StatusCode execute(xAOD::TauJet& pTau) override;
    virtual StatusCode finalize() override;
    virtual StatusCode eventInitialize() override { return StatusCode::SUCCESS; }
    virtual StatusCode eventFinalize() override { return StatusCode::SUCCESS; }


private:
    int m_ConvTaggerVer; 
    bool m_TrkIsConv;
    bool m_storeFullSummary;
    bool m_doTRTRatio;
    float m_a_cut[2][2], m_b_cut[2][2];	    
    float m_TRTHighTOutliersRatio;
    ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;
};

#endif //TAUREC_TAUCONVERSIONTAGGER_H
#endif //XAOD_ANALYSIS
