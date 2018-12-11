/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_EFMISSINGETFROMJETS_H
#define TRIGEFMISSINGET_EFMISSINGETFROMJETS_H

/********************************************************************

NAME:     EFMissingETFromJets.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Florian U. Bernlochner, Doug Schaefer, Justin Chiu
CREATED:  May 20, 2014

PURPOSE:  Updates TrigMissingETHelper using info from jets

 ********************************************************************/

#include "TrigEFMissingET/EFMissingETBaseTool.h"
#include "TrigMissingEtEvent/TrigMissingET.h"


/**
  $class EFMissingETFromJets
  Updates transient helper object with jets
 **/

class EFMissingETFromJets : public EFMissingETBaseTool
{
  public:

    EFMissingETFromJets(const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

    ~EFMissingETFromJets();

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
    float m_etacut;
    float m_forward_ptcut;
    float m_central_ptcut;
    int   m_methelperposition;
    bool  m_applyTileGap3Correction;
};

#endif // TRIGEFMISSINGET_EFMISSINGETFROMJETS_H
