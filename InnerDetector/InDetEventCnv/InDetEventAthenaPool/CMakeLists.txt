# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( InDetEventAthenaPoolPoolCnv
   InDetEventAthenaPool/*.h src/*.h src/*.cxx
   FILES InDetRawData/PixelRDO_Container.h InDetRawData/SCT_RDO_Container.h
   InDetRawData/TRT_RDO_Container.h InDetPrepRawData/TRT_DriftCircleContainer.h
   InDetPrepRawData/PixelClusterContainer.h
   InDetPrepRawData/SCT_ClusterContainer.h
   InDetPrepRawData/PixelGangedClusterAmbiguities.h src/InDetTrack.h
   InDetSimData/InDetSimDataCollection.h
   InDetLowBetaInfo/InDetLowBetaCandidate.h
   InDetLowBetaInfo/InDetLowBetaContainer.h
   SCT_ConditionsData/SCT_FlaggedCondData.h
   TYPES_WITH_NAMESPACE InDet::InDetLowBetaCandidate
   LINK_LIBRARIES Identifier GeneratorObjectsTPCnv AthAllocators AthContainers
   AthenaBaseComps AthenaKernel SGTools StoreGateLib AthenaPoolCnvSvcLib
   AthenaPoolUtilities AtlasSealCLHEP GaudiKernel InDetIdentifier
   InDetReadoutGeometry TRT_ReadoutGeometry InDetEventTPCnv InDetRawData InDetSimData
   InDetLowBetaInfo InDetPrepRawData SCT_ConditionsData TrkTrack )

atlas_add_dictionary( InDetEventAthenaPoolCnvDict
   InDetEventAthenaPool/InDetEventAthenaPoolCnvDict.h
   InDetEventAthenaPool/selection.xml
   LINK_LIBRARIES Identifier GeneratorObjectsTPCnv )

# Install files from the package:
atlas_install_headers( InDetEventAthenaPool )
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )
if( ATHENAPOOLUTILITIESTEST_FOUND )
   set( INDETEVENTATHENAPOOL_REFERENCE_TAG
        InDetEventAthenaPoolReference-01-00-00 )
   run_tpcnv_legacy_test( InDetEventTPCnv_16.6.2.1 ESD-16.6.2.1
                   REFERENCE_TAG ${INDETEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
