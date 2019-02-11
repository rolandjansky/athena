/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

    virtual StatusCode initialize();
    virtual StatusCode execute(xAOD::TauJet& pTau);
    virtual StatusCode executeShotFinder(xAOD::TauJet&, xAOD::CaloClusterContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0CreateROI(xAOD::TauJet&, CaloCellContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterCreator(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&, xAOD::CaloClusterContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executeVertexVariables(xAOD::TauJet&, xAOD::VertexContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePi0ClusterScaler(xAOD::TauJet&, xAOD::PFOContainer&, xAOD::PFOContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode executePanTau(xAOD::TauJet&, xAOD::ParticleContainer&) { return StatusCode::SUCCESS; }
    virtual StatusCode finalize();
    virtual StatusCode eventInitialize() { return StatusCode::SUCCESS; }
    virtual StatusCode eventFinalize() { return StatusCode::SUCCESS; }

    virtual void print() const { }

private:
    
    std::string m_trackContainerName;

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
