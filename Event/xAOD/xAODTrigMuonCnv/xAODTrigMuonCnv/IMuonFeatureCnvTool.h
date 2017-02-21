/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// V1.0
#ifndef xAODTrigMuonCNV_IMuonFeatureCnvTool_H
#define xAODTrigMuonCNV_IMuonFeatureCnvTool_H


// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigMuon/L2StandAloneMuon.h"

// Forward declaration(s):
class MuonFeature;
class MuonFeatureDetails;
class MuonFeatureContainer;
class MuonFeatureDetailsContainer;

static const InterfaceID IID_IMuonFeatureCnvTool( "xAODMaker::IMuonFeatureCnvTool", 1, 0 );
  
class IMuonFeatureCnvTool : public virtual IAlgTool {
  
 public:
  /// Convert MuonFeature and MuonFeatureDetails objectes into xAOD::L2StandAloneMuon object
  virtual StatusCode convertMuonFeature( const MuonFeature* aod_mf,
					 xAOD::L2StandAloneMuon* xaod ) = 0;
  virtual StatusCode convertMuonFeature( const MuonFeatureDetails* aod_mfd,
					 xAOD::L2StandAloneMuon* xaod ) = 0;
  
  /// Convert MuonFeatureContainer and MuonFeatureDetailsContainer into xAOD::L2StandAloneMuonContainer
  virtual StatusCode convertMuonFeatureContainer( const MuonFeatureContainer* aod_mf,
						  const MuonFeatureDetailsContainer* aod_mfd,
						  xAOD::L2StandAloneMuonContainer* xaod ) = 0;
  
  /// Gaudi interface definition
  static const InterfaceID& interfaceID() {
    return IID_IMuonFeatureCnvTool;
  }
    
}; // class IMuonFeatureCnvTool

#endif // xAODTrigMuonCNV_IMuonFeatureCnvTool_H
