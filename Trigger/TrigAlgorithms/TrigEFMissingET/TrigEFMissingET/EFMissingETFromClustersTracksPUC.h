/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_EFMissingETFromClustersTracksPUC_H
#define TRIGEFMISSINGET_EFMissingETFromClustersTracksPUC_H



/********************************************************************

This code should be edited as necessary and then placed in the TrigEFMissingET/TrigEFMissingET directory

 ********************************************************************/

#include "TrigEFMissingET/EFMissingETBaseTool.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "GaudiKernel/ToolHandle.h"

class EFMissingETFromClustersTracksPUC : public EFMissingETBaseTool
{
  public:

    EFMissingETFromClustersTracksPUC(const std::string& type,
                               const std::string& name,
                               const IInterface* parent);

    ~EFMissingETFromClustersTracksPUC();

    virtual StatusCode initialize(); 
    virtual StatusCode finalize();
    virtual StatusCode execute();

    virtual StatusCode execute(xAOD::TrigMissingET *met,
                               TrigEFMissingEtHelper *metHelper,
                               const xAOD::CaloClusterContainer *caloCluster,
                               const xAOD::JetContainer *jets,
                               const xAOD::TrackParticleContainer *track,
                               const xAOD::VertexContainer *vertex,
                               const xAOD::MuonContainer *muon);

  private:
    //internal
    unsigned int m_nPhiBins;
    unsigned int m_nEtaBins;
    unsigned int m_nTowers;
    double m_towerEtaWidth;
    double m_towerPhiWidth;

    int  m_methelperposition;
    // Tower
    double m_targetTowerWidth;
    double m_maxEta;

    // For jet selection
    double m_forward_ptcut;
    double m_track_ptcut;
    double m_minJetPtJvt;
    double m_maxJetPtJvt;
    double m_jetRpTCut;
    ToolHandle<InDet::IInDetTrackSelectionTool> m_trackselTool;

    //Covariance variables
    double m_caloResSqrtTerm;
    double m_caloResFloor;

    // Pufit variables
    double m_constraintWeight;
};

#endif // TRIGEFMISSINGET_EFMissingETFromClustersTracksPUC_H
