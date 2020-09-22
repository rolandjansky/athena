/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RPC_PrepDataToxAOD.h
//   Header file for class RPC_PrepDataToxAOD
///////////////////////////////////////////////////////////////////

#ifndef RPC_PREPDATATOXAOD_H
#define RPC_PREPDATATOXAOD_H

#include "MuonPrepDataToxAOD.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"

class RPC_PrepDataToxAOD : public MuonPrepDataToxAOD<Muon::RpcPrepDataContainer,MuonSimDataCollection> {
public:
  // Constructor with parameters:
  RPC_PrepDataToxAOD(const std::string &name,ISvcLocator *pSvcLocator);

  // algorithm execute:
  StatusCode execute();
  StatusCode initialize();

  // overload base class function to add technology specific information
  void addPRD_TechnologyInformation( xAOD::TrackMeasurementValidation& xprd, const Muon::RpcPrepData& prd ) const;

  // overload base class function to add technology specific information
  void addSDO_TechnologyInformation( xAOD::TrackMeasurementValidation& xprd, const Muon::RpcPrepData& prd, const MuonSimData* sdo ) const;
private:
  ToolHandle<Muon::IMuonClusterOnTrackCreator> m_clusterCreator{this,"ClusterOnTrackCreator","Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator"};
  ToolHandle<Trk::IResidualPullCalculator> m_pullCalculator{this,"PullCalculator","Trk::ResidualPullCalculator/ResidualPullCalculator"};
  double m_inversePropagationSpeed;
};


#endif 
