/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_EFMISSINGETFROMCLUSTERS_H
#define TRIGEFMISSINGET_EFMISSINGETFROMCLUSTERS_H

/********************************************************************

NAME:     EFMissingETFromClusters.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Florian U. Bernlochner, Bertrand Chapleau, Bob Kowalewski
CREATED:  Feb 7, 2012

PURPOSE:  Updates TrigMissingETHelper using info from topo. clusters

 ********************************************************************/

#include "TrigEFMissingET/EFMissingETBaseTool.h"
#include "TrigMissingEtEvent/TrigMissingET.h"


/**
  $class EFMissingETFromClusters
  Updates transient helper object with topo. clusters
 **/

class EFMissingETFromClusters : public EFMissingETBaseTool
{
  public:

    EFMissingETFromClusters(const std::string& type,
                            const std::string& name,
                            const IInterface* parent);

    ~EFMissingETFromClusters();

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
    bool m_saveuncalibrated;
    int  m_methelperposition;
    xAOD::CaloCluster_v1::State m_clusterstate;
};

#endif // TRIGEFMISSINGET_EFMISSINGETFROMCLUSTERS_H
