/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_EFMISSINGETFROMCLUSTERSMT_H
#define TRIGEFMISSINGET_EFMISSINGETFROMCLUSTERSMT_H

/********************************************************************

NAME:     EFMissingETFromClustersMT.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Kenji Hamano
CREATED:  Feb 8, 2019

PURPOSE:  athenaMT migration

 ********************************************************************/

// Framework includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// TrigEFMissingET included
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "TrigEFMissingET/IMissingETTool.h"
#include "TrigEFMissingET/EFMissingETHelper.h"

/**
  @class EFMissingETFromClustersMT
  \brief Updates metHelper object with topoclusters
  \author Kenji Hamano
  \author Gabriel Gallardo
  \date Feb 8, 2019
 **/
class EFMissingETFromClustersMT : public extends<AthAlgTool, IMissingETTool>
{
  public:

    EFMissingETFromClustersMT(const std::string& type,
                            const std::string& name,
                            const IInterface* parent);

    virtual ~EFMissingETFromClustersMT();

    virtual StatusCode initialize() override;

    /**
    This function does two things:
    1. It initializes the `met` object so that `EFMissingETFromHelper` knows that it will receive input from MetFromClusters 
    2. It fills in the topocluster components of the `metHelper` object 
    It is meant to be called by the `EFMissingETAlgMT` class 

    If `m_saveuncalibrated==true`, then EM calibrated TC are saved, else the LCW calibrated TC are saved.
    **/
    virtual StatusCode update(xAOD::TrigMissingET *met,
                               TrigEFMissingEtHelper *metHelper,
                               const EventContext& ctx) const override;

  private:
    Gaudi::Property<bool> m_saveuncalibrated {this, "SaveUncalibrated", false ,"save uncalibrated topo. clusters"};
    SG::ReadHandleKey<xAOD::CaloClusterContainer> m_clustersKey { this, "ClustersCollection", "CaloClusters", "Collection containg all clusters" };

    xAOD::CaloCluster_v1::State m_clusterstate;
    TrigEFMissingEtComponent::Component m_metHelperComp;

};

#endif // TRIGEFMISSINGET_EFMISSINGETFROMCLUSTERSMT_H
