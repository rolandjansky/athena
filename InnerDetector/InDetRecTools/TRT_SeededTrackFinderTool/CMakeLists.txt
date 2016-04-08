################################################################################
# Package: TRT_SeededTrackFinderTool
################################################################################

# Declare the package name:
atlas_subdir( TRT_SeededTrackFinderTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParameters
                          PRIVATE
                          Control/StoreGate
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkEvent/TrkCaloClusterROI
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TRT_SeededTrackFinderTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel InDetRecToolInterfaces MagFieldInterfaces TrkGeometry TrkSurfaces TrkParameters StoreGateLib SGtests InDetIdentifier InDetPrepRawData InDetRIO_OnTrack TrkCaloClusterROI TrkEventPrimitives TrkPseudoMeasurementOnTrack TrkTrack TrkExInterfaces TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( TRT_SeededTrackFinderTool )

