/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_EFMissingETFromClustersPS_H
#define TRIGEFMISSINGET_EFMissingETFromClustersPS_H

/********************************************************************

NAME:     EFMissingETFromClustersPS.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Florian U. Bernlochner, Bertrand Chapleau, Bob Kowalewski
          Pile-up subtraction by Randall Sobie -- ported by Justin Chiu

CREATED:  Feb 7, 2012

PURPOSE:  Updates TrigMissingETHelper using info from topo. clusters
          A modified version of EFMissingETFromClusters with pile-up subtraction

 ********************************************************************/

#include "TrigEFMissingET/EFMissingETBaseTool.h"
#include "TrigMissingEtEvent/TrigMissingET.h"


/**
  $class EFMissingETFromClusters
  Updates transient helper object with topo. clusters
 **/

class EFMissingETFromClustersPS : public EFMissingETBaseTool
{
  public:

    EFMissingETFromClustersPS(const std::string& type,
                              const std::string& name,
                              const IInterface* parent);

    ~EFMissingETFromClustersPS();

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
    bool  m_saveuncalibrated;
    bool  m_subtractpileup;
    bool  m_pileupdebug;
    int   m_pileupnumrings;
    float m_pileupnumstddev;
    int   m_methelperposition;
    xAOD::CaloCluster_v1::State m_clusterstate;
};

#endif // TRIGEFMISSINGET_EFMissingETFromClustersPS_H
