# $Id: CMakeLists.txt 730480 2016-03-16 15:59:44Z krasznaa $
################################################################################
# Package: TrkEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TrkEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthLinks
   Control/DataModelAthenaPool
   Control/StoreGate
   Database/AthenaPOOL/AthenaPoolCnvSvc
   Database/AthenaPOOL/AthenaPoolUtilities
   DetectorDescription/GeoPrimitives
   DetectorDescription/Identifier
   Event/EventPrimitives
   GaudiKernel
   InnerDetector/InDetDetDescr/InDetIdentifier
   Tracking/TrkDetDescr/TrkDistortedSurfaces
   Tracking/TrkDetDescr/TrkSurfaces
   Tracking/TrkEvent/TrkCompetingRIOsOnTrack
   Tracking/TrkEvent/TrkEventPrimitives
   Tracking/TrkEvent/TrkMaterialOnTrack
   Tracking/TrkEvent/TrkMeasurementBase
   Tracking/TrkEvent/TrkNeutralParameters
   Tracking/TrkEvent/TrkParameters
   Tracking/TrkEvent/TrkParametersBase
   Tracking/TrkEvent/TrkParticleBase
   Tracking/TrkEvent/TrkPrepRawData
   Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
   Tracking/TrkEvent/TrkRIO_OnTrack
   Tracking/TrkEvent/TrkSegment
   Tracking/TrkEvent/TrkTrack
   Tracking/TrkEvent/TrkTrackSummary
   Tracking/TrkEvent/TrkV0Vertex
   Tracking/TrkEvent/TrkVertexOnTrack
   Tracking/TrkEvent/VxMultiVertex
   Tracking/TrkEvent/VxVertex
   Tracking/TrkEventCnv/TrkEventCnvTools
   PRIVATE
   AtlasTest/TestTools
   Control/AthenaBaseComps
   Control/AthenaKernel
   Control/CxxUtils
   Tracking/TrkDetDescr/TrkDetDescrUtils
   Tracking/TrkEvent/TrkTrackLink
   )

# External dependencies:
find_package( CLHEP )

# Definitions used to build TrkEventTPCnv:
set( _definitions
   -DUseFloatsIn_TrackParameters
   -DUseFloatsIn_HepSymMatrix
   -DUseFloatsIn_LocalDirection
   -DUseFloatsIn_LocalParameters
   -DUseFloatsIn_LocalPosition
   -DUseFloatsIn_CylinderBounds
   -DUseFloatsIn_DiamondBounds
   -DUseFloatsIn_DiscBounds
   -DUseFloatsIn_RectangleBounds
   -DUseFloatsIn_Surface
   -DUseFloatsIn_TrapezoidBounds
   -DUseFloatsIn_CompetingRIOsOnTrack )

# Component(s) in the package:
atlas_add_tpcnv_library( TrkEventTPCnv
   TrkEventTPCnv/*.h src/*.cxx
   TrkEventTPCnv/TrkEventPrimitives/*.h src/TrkEventPrimitives/*.cxx
   TrkEventTPCnv/TrkMaterialOnTrack/*.h src/TrkMaterialOnTrack/*.cxx
   TrkEventTPCnv/TrkParameters/*.h TrkEventTPCnv/TrkParameters/*.icc
   src/TrkParameters/*.cxx
   TrkEventTPCnv/TrkNeutralParameters/*.h
   TrkEventTPCnv/TrkNeutralParameters/*.icc
   src/TrkNeutralParameters/*.cxx
   TrkEventTPCnv/TrkPseudoMeasurementOnTrack/*.h
   src/TrkPseudoMeasurementOnTrack/*.cxx
   TrkEventTPCnv/TrkVertexOnTrack/*.h src/TrkVertexOnTrack/*.cxx
   TrkEventTPCnv/TrkSurfaces/*.h src/TrkSurfaces/*.cxx
   TrkEventTPCnv/TrkSegment/*.h src/TrkSegment/*.cxx
   TrkEventTPCnv/TrkTrack/*.h src/TrkTrack/*.cxx
   TrkEventTPCnv/TrkRIO_OnTrack/*.h src/TrkRIO_OnTrack/*.cxx
   TrkEventTPCnv/TrkPrepRawData/*.h src/TrkPrepRawData/*.cxx
   TrkEventTPCnv/TrkTrackSummary/*.h src/TrkTrackSummary/*.cxx
   TrkEventTPCnv/VxVertex/*.h src/VxVertex/*.cxx
   TrkEventTPCnv/VxMultiVertex/*.h src/VxMultiVertex/*.cxx
   TrkEventTPCnv/TrkV0Vertex/*.h src/TrkV0Vertex/*.cxx
   TrkEventTPCnv/TrkParticleBase/*.h src/TrkParticleBase/*.cxx
   TrkEventTPCnv/TrkCompetingRIOsOnTrack/*.h src/TrkCompetingRIOsOnTrack/*.cxx
   TrkEventTPCnv/TrkDetElementSurface/*.h src/TrkDetElementSurface/*.cxx
   PUBLIC_HEADERS TrkEventTPCnv
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthContainers AthLinks DataModelAthenaPoolLib
   StoreGateLib AthenaPoolCnvSvcLib AthenaPoolUtilities GeoPrimitives Identifier
   EventPrimitives GaudiKernel InDetIdentifier TrkDistortedSurfaces TrkSurfaces
   TrkCompetingRIOsOnTrack TrkEventPrimitives TrkMaterialOnTrack
   TrkMeasurementBase TrkNeutralParameters TrkParameters TrkParametersBase
   TrkParticleBase TrkPrepRawData TrkPseudoMeasurementOnTrack TrkRIO_OnTrack
   TrkSegment TrkTrack TrkTrackSummary TrkV0Vertex TrkVertexOnTrack TrkEventCnvToolsLib
   VxMultiVertex VxVertex
   PRIVATE_LINK_LIBRARIES AthenaKernel CxxUtils TrkDetDescrUtils
   DEFINITIONS ${_definitions} )

atlas_add_dictionary( TrkEventTPCnvDict
   TrkEventTPCnv/TrkEventTPCnvDict.h TrkEventTPCnv/selection.xml
   LINK_LIBRARIES TrkEventTPCnv )

atlas_add_dictionary( OLD_TrkEventTPCnvDict
   TrkEventTPCnv/TrkEventTPCnvOLDDict.h TrkEventTPCnv/OLD_selection.xml
   LINK_LIBRARIES TrkEventTPCnv )

# Add all the test(s) of the package:
foreach( name FitQualityCnv_p1_test LocalDirectionCnv_p1_test
      LocalPositionCnv_p1_test LocalParametersCnv_p1_test ErrorMatrixCnv_p1_test
      TrackParametersCnv_p2_test SurfaceCnv_p2_test
      PseudoMeasurementOnTrackCnv_p2_test CompetingRIOsOnTrackCnv_p1_test
      TrackStateOnSurfaceCnv_p3_test EnergyLossCnv_p1_test
      MaterialEffectsBaseCnv_p2_test MaterialEffectsOnTrackCnv_p2_test
      TrackCnv_p4_test SegmentCnv_p1_test )

   atlas_add_test( ${name}
      SOURCES test/${name}.cxx
      LINK_LIBRARIES GeoPrimitives GaudiKernel TrkSurfaces TrkEventPrimitives
      TrkMaterialOnTrack TrkMeasurementBase TrkPseudoMeasurementOnTrack
      TrkTrackSummary TestTools AthenaBaseComps CxxUtils TrkEventTPCnv )

endforeach()
# Needed for the plugin service to see the test components
# defined in the test binary.
set_target_properties( TrkEventTPCnv_SurfaceCnv_p2_test PROPERTIES ENABLE_EXPORTS True )
