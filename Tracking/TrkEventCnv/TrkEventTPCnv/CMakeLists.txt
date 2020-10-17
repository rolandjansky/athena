# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkEventTPCnv )

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
   TrkEventTPCnv/TrkClusterSplitProbabilityContainer/*.h src/TrkClusterSplitProbabilityContainer/*.cxx
   PUBLIC_HEADERS TrkEventTPCnv
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthContainers AthLinks DataModelAthenaPoolLib
   StoreGateLib AthenaPoolCnvSvcLib AthenaPoolUtilities CxxUtils GeoPrimitives Identifier
   EventPrimitives GaudiKernel InDetIdentifier TrkDistortedSurfaces TrkSurfaces
   TrkCompetingRIOsOnTrack TrkEventPrimitives TrkMaterialOnTrack
   TrkMeasurementBase TrkNeutralParameters TrkParameters TrkParametersBase
   TrkParticleBase TrkPrepRawData TrkPseudoMeasurementOnTrack TrkRIO_OnTrack
   TrkSegment TrkTrack TrkTrackSummary TrkV0Vertex TrkVertexOnTrack TrkEventCnvToolsLib TrkEventUtils
   VxMultiVertex VxVertex 
   PRIVATE_LINK_LIBRARIES AthenaKernel TrkDetDescrUtils TrkTrackLink
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
