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
    int  m_methelperposition;

    // Put internals here
};

#endif // TRIGEFMISSINGET_EFMissingETFromClustersTracksPUC_H
