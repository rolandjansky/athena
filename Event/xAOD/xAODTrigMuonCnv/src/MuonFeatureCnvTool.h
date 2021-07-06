// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMuonCnv_MuonFeatureCnvTool_h
#define xAODTrigMuonCnv_MuonFeatureCnvTool_h

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigMuonCnv/IMuonFeatureCnvTool.h"

class MuonFeatureCnvTool : public AthAlgTool, public virtual IMuonFeatureCnvTool {
  
public:
  /// Regular AlgTool constructor
  MuonFeatureCnvTool( const std::string& type, const std::string& name,
		      const IInterface* parent );
  
  /// Convert MuonFeature object into xAOD::L2StandAloneMuon object
  virtual StatusCode convertMuonFeature( const MuonFeature* aod_mf,
					 xAOD::L2StandAloneMuon* xaod ) override;

  /// Convert MuonFeatureDetails object into xAOD::L2StandAloneMuon object
  virtual StatusCode convertMuonFeature( const MuonFeatureDetails* aod_mfd,
					 xAOD::L2StandAloneMuon* xaod ) override;
  
  /// Conver MuonDeatureContainer and MuonFeatureDetailsContainer into xAOD::L2StandAloneMuonContainer
  virtual StatusCode convertMuonFeatureContainer( const MuonFeatureContainer* aod_mf_container,
						  const MuonFeatureDetailsContainer* aod_mfd_container,
						  xAOD::L2StandAloneMuonContainer* xaod_container ) override;
  
}; // end calss definition for MuonFeatureCnvTool

#endif
