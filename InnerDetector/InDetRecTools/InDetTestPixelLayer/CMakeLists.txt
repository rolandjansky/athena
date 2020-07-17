################################################################################
# Package: InDetTestPixelLayer
################################################################################

# Declare the package name:
atlas_subdir( InDetTestPixelLayer )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/IdDictDetDescr
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/PixelReadoutGeometry
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# External dependencies:
find_package( Eigen )

# tag ROOTRooFitLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTMathLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( InDetTestPixelLayerLib
                   src/*.cxx
                   PUBLIC_HEADERS InDetTestPixelLayer
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives Identifier GaudiKernel InDetIdentifier InDetRecToolInterfaces TrkEventPrimitives TrkParameters TrkToolInterfaces InDetConditionsSummaryService
                   PRIVATE_LINK_LIBRARIES AtlasDetDescr IdDictDetDescr InDetReadoutGeometry PixelReadoutGeometry TrkGeometry TrkSurfaces TrkMeasurementBase TrkParticleBase TrkTrack TrkTrackSummary TrkExInterfaces )

atlas_add_component( InDetTestPixelLayer
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives Identifier GaudiKernel InDetIdentifier InDetRecToolInterfaces TrkEventPrimitives TrkParameters TrkToolInterfaces AtlasDetDescr IdDictDetDescr InDetReadoutGeometry TrkGeometry TrkSurfaces TrkMeasurementBase TrkParticleBase TrkTrack TrkTrackSummary TrkExInterfaces InDetTestPixelLayerLib )

