# $Id: CMakeLists.txt 730483 2016-03-16 16:03:14Z harkusha $
################################################################################
# Package: MuonEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( MuonEventTPCnv )

find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_tpcnv_library( MuonEventTPCnv
   MuonEventTPCnv/*.h src/*.cxx
   src/MuonRIO_OnTrack/*.cxx
   MuonEventTPCnv/MuonCompetingRIOsOnTrack/*.h
   src/MuonCompetingRIOsOnTrack/*.cxx
   MuonEventTPCnv/MuonPrepRawData/*.h src/MuonPrepRawData/*.cxx
   MuonEventTPCnv/MuonTrigCoinData/*.h src/MuonTrigCoinData/*.cxx
   MuonEventTPCnv/MuonSegment/*.h src/MuonSegment/*.cxx
   MuonEventTPCnv/MuonChamberT0s/*.h src/MuonChamberT0s/*.cxx
   MuonEventTPCnv/MuonDigitContainer/*.h src/MuonDigitContainer/*.cxx src/MuonRDO/*.cxx
   PUBLIC_HEADERS MuonEventTPCnv
   LINK_LIBRARIES AthAllocators AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib
   AthenaPoolUtilities Identifier GaudiKernel GeneratorObjectsTPCnv
   MuonDigitContainer MuonChamberT0s MuonCompetingRIOsOnTrack MuonPrepRawData
   MuonRIO_OnTrack MuonSegment MuonTrigCoinData MuonSimData MuonRDO TrkSurfaces
   TrkRIO_OnTrack TrkEventTPCnv
   PRIVATE_LINK_LIBRARIES AthenaKernel StoreGateLib GeoPrimitives
   EventPrimitives MuonReadoutGeometry MuonIdHelpersLib TrkEventPrimitives
   TrkMeasurementBase )

atlas_add_dictionary( MuonEventTPCnvDict
   MuonEventTPCnv/MuonEventTPCnvDict.h MuonEventTPCnv/selection.xml
   LINK_LIBRARIES MuonEventTPCnv )

atlas_add_dictionary( OLD_MuonEventTPCnvDict
   MuonEventTPCnv/OLD_MuonEventTPCnvDict.h MuonEventTPCnv/OLD_selection.xml
   LINK_LIBRARIES MuonEventTPCnv )

atlas_install_joboptions( share/*.txt )


# Set up the tests of the package:
foreach( name CscMcDataCnv_p1_test CscSimDataCnv_p1_test
      CscSimDataCollectionCnv_p1_test CscSimDataCnv_p2_test
      CscSimDataCollectionCnv_p2_test MuonMCDataCnv_p1_test
      MuonSimDataCnv_p1_test MuonSimDataCollectionCnv_p1_test
      MuonSimDataCnv_p2_test MuonSimDataCollectionCnv_p2_test
      CscClusterOnTrackCnv_p2_test RpcClusterOnTrackCnv_p3_test
      TgcClusterOnTrackCnv_p2_test MM_ClusterOnTrackCnv_p1_test
      STGC_ClusterOnTrackCnv_p1_test MdtDriftCircleOnTrackCnv_p2_test
      CompetingMuonClustersOnTrackCnv_p2_test MuonSegmentQualityCnv_p1_test
      MuonSegmentCnv_p2_test CscPrepDataCnv_p1_test CscPrepDataCnv_p2_test
      CscStripPrepDataCnv_p1_test MdtPrepDataCnv_p1_test
      MdtPrepDataCnv_p2_test RpcPrepDataCnv_p1_test RpcPrepDataCnv_p2_test
      RpcPrepDataCnv_p3_test TgcPrepDataCnv_p1_test TgcPrepDataCnv_p2_test
      TgcPrepDataCnv_p3_test MMPrepDataCnv_p1_test sTgcPrepDataCnv_p1_test
      CscPrepDataContainerCnv_p1_test CscPrepDataContainerCnv_p2_test
      CscStripPrepDataContainerCnv_p1_test MdtPrepDataContainerCnv_p1_test
      MdtPrepDataContainerCnv_p2_test RpcPrepDataContainerCnv_p1_test
      RpcPrepDataContainerCnv_p2_test RpcPrepDataContainerCnv_p3_test
      TgcPrepDataContainerCnv_p1_test TgcPrepDataContainerCnv_p2_test
      TgcPrepDataContainerCnv_p3_test sTgcPrepDataContainerCnv_p1_test
      MMPrepDataContainerCnv_p1_test RpcCoinDataCnv_p1_test
      TgcCoinDataCnv_p1_test TgcCoinDataCnv_p2_test TgcCoinDataCnv_p3_test
      RpcCoinDataContainerCnv_p1_test TgcCoinDataContainerCnv_p1_test
      TgcCoinDataContainerCnv_p2_test TgcCoinDataContainerCnv_p3_test
      MM_DigitCnv_p1_test
      MM_DigitCnv_p2_test
      MM_DigitContainerCnv_p1_test
      MM_DigitContainerCnv_p2_test
      STGC_DigitCnv_p1_test
      STGC_DigitContainerCnv_p1_test
      STGC_RawDataContainerCnv_p1_test
      )

   atlas_add_test( ${name}
      SOURCES test/${name}.cxx
      LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} GaudiKernel TrkSurfaces TrkEventTPCnv TestTools
      StoreGateLib TPTools AGDDKernel AtlasDetDescr 
      IdDict IdDictDetDescr IdDictParser InDetIdentifier InDetReadoutGeometry PixelReadoutGeometry SCT_ReadoutGeometry TRT_ReadoutGeometry
      MuonAGDDDescription MuonReadoutGeometry MuonIdHelpersLib
      TrkEventPrimitives TrkPseudoMeasurementOnTrack MuonEventTPCnv AthenaKernel
      LOG_IGNORE_PATTERN "MultiRange built successfully|element hash max|DEBUG|Strip range"
      PROPERTIES TIMEOUT 300 )
   set_target_properties ( MuonEventTPCnv_${name} PROPERTIES ENABLE_EXPORTS True )

endforeach()

