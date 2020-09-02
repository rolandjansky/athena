################################################################################
# Package: InDetTestBLayer
################################################################################

# Declare the package name:
atlas_subdir( InDetTestBLayer )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkTools/TrkToolInterfaces
			  Tracking/TrkExtrapolation/TrkExInterfaces 
                          PRIVATE
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/IdDictDetDescr
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/PixelReadoutGeometry
                          Reconstruction/Particle
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkTrack
                          )

# External dependencies:
find_package( Eigen )

# tag ROOTRooFitLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTMathLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( InDetTestBLayerLib
                   src/*.cxx
                   PUBLIC_HEADERS InDetTestBLayer
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives Identifier EventPrimitives GaudiKernel InDetRecToolInterfaces 
		   TrkEventPrimitives TrkParameters TrkToolInterfaces TrkExInterfaces 
                   InDetConditionsSummaryService
                   PRIVATE_LINK_LIBRARIES AtlasDetDescr IdDictDetDescr InDetIdentifier InDetReadoutGeometry PixelReadoutGeometry Particle 
		   TrkGeometry TrkSurfaces TrkMeasurementBase TrkTrack )

atlas_add_component( InDetTestBLayer
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives Identifier EventPrimitives 
		     GaudiKernel InDetRecToolInterfaces TrkEventPrimitives TrkParameters TrkToolInterfaces AtlasDetDescr IdDictDetDescr InDetIdentifier 
		     InDetReadoutGeometry Particle TrkGeometry TrkSurfaces TrkMeasurementBase TrkTrack TrkExInterfaces InDetTestBLayerLib )

