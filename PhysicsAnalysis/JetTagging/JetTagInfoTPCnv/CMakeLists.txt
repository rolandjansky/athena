# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetTagInfoTPCnv )

# Component(s) in the package:
atlas_add_library( JetTagInfoTPCnv
                   src/*.cxx
                   PUBLIC_HEADERS JetTagInfoTPCnv
                   LINK_LIBRARIES AthenaPoolUtilities CxxUtils EventPrimitives JetTagEvent JetTagInfo MuonIDEvent JetEvent JetEventTPCnv Particle egammaEvent VxVertex TrkEventTPCnv DataModelAthenaPoolLib AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_dictionary( JetTagInfoTPCnvDict
                      JetTagInfoTPCnv/JetTagInfoTPCnvDict.h
                      JetTagInfoTPCnv/selection.xml
                      LINK_LIBRARIES JetTagInfoTPCnv )

atlas_add_dictionary( ARA_JetTagInfoTPCnvDict
                      JetTagInfoTPCnv/JetTagInfoTPCnvDict.h
                      JetTagInfoTPCnv/ARA_selection.xml
                      LINK_LIBRARIES JetTagInfoTPCnv )

# Test(s) in the package:
set( _jobOPath "${CMAKE_CURRENT_SOURCE_DIR}/share" )
set( _jobOPath "${_jobOPath}:${CMAKE_JOBOPT_OUTPUT_DIRECTORY}" )
set( _jobOPath "${_jobOPath}:$ENV{JOBOPTSEARCHPATH}" )
                    
foreach( test AtlfInfoCnv_p1 BaseTagInfoCnv_p1 ElectronAssociationCnv_p1 
              GbbNNTagInfoCnv_p1 IPInfoBaseCnv_p1 IPInfoPlusCnv_p1
              IPTrackInfoCnv_p1 ISvxAssociationCnv_p1 JetFitterTagInfoCnv_p1
              JetProbInfoBaseCnv_p1 LifetimeInfoCnv_p1
              MSVVtxInfoCnv_p1 MSVVtxInfoCnv_p2 MultiSVInfoPlusCnv_p1
              MuonAssociationCnv_p1 PhotonAssociationCnv_p1
              SecVtxInfoCnv_p1 SETrackInfoCnv_p1 SLTrueInfoCnv_p1
              SMTrackInfoCnv_p1 SoftElectronInfoCnv_p1
              SoftLeptonTruthInfoCnv_p1 SoftMuonInfoCnv_p1
              SVInfoBaseCnv_p1 SVInfoPlusCnv_p1 SvxSummaryCnv_p1
              TrackAssociationCnv_p1 TrackCountingInfoCnv_p1 TruthInfoCnv_p1 )
  atlas_add_test( ${test}_test
                  SOURCES
                  test/${test}_test.cxx
                  LINK_LIBRARIES GaudiKernel JetTagInfoTPCnv SGTools TestTools CxxUtils
                  ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )
endforeach()
              
