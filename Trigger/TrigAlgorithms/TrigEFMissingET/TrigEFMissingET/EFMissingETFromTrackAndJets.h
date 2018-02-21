/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_EFMISSINGETFROMTRACKANDJETS_H
#define TRIGEFMISSINGET_EFMISSINGETFROMTRACKANDJETS_H

/********************************************************************

NAME:     EFMissingETFromTrackAndJets.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Renjie Wang (renjie.wang@cern.ch)
CREATED:  Nov 18, 2016

PURPOSE:  Updates TrigMissingETHelper using info from tracks and jets

 ********************************************************************/

#include "TrigEFMissingET/EFMissingETBaseTool.h"
#include "TrigMissingEtEvent/TrigMissingET.h"


#include "TH2.h"
#include "TH1.h"
#include "TFile.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "PathResolver/PathResolver.h"

#include "GaudiKernel/ToolHandle.h"

/**
  $class EFMissingETFromTrackAndJets
  Updates transient helper object with jets
 **/

class EFMissingETFromTrackAndJets : public EFMissingETBaseTool
{
  public:

    EFMissingETFromTrackAndJets(const std::string& type,
                                const std::string& name,
                                const IInterface* parent);

    ~EFMissingETFromTrackAndJets();

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

    TH1* getHistogramFromFile(TString hname, TString fname);

  private:
    float m_etacut;
    float m_forward_ptcut;
    float m_central_ptcut;
    int   m_methelperposition;
    float m_track_ptcut;
    float m_central_jvtcut;
    TH2F* m_jvtLikelihood;

    ToolHandle<InDet::IInDetTrackSelectionTool> m_trackselTool;
};

#endif // TRIGEFMISSINGET_EFMISSINGETFROMTRACKANDJETS_H
