// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
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
  
  /// Function initialising the tool
  virtual StatusCode initialize();
  
  /// Convert MuonFeature object into xAOD::L2StandAloneMuon object
  virtual StatusCode convertMuonFeature( const MuonFeature* aod_mf,
					 xAOD::L2StandAloneMuon* xaod );

  /// Convert MuonFeatureDetails object into xAOD::L2StandAloneMuon object
  virtual StatusCode convertMuonFeature( const MuonFeatureDetails* aod_mfd,
					 xAOD::L2StandAloneMuon* xaod );
  
  /// Conver MuonDeatureContainer and MuonFeatureDetailsContainer into xAOD::L2StandAloneMuonContainer
  virtual StatusCode convertMuonFeatureContainer( const MuonFeatureContainer* aod_mf_container,
						  const MuonFeatureDetailsContainer* aod_mfd_container,
						  xAOD::L2StandAloneMuonContainer* xaod_container );
  
}; // end calss definition for MuonFeatureCnvTool

#endif

