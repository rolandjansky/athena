################################################################################
# Package: TRT_TrackExtensionTool_xk
################################################################################

# Declare the package name:
atlas_subdir( TRT_TrackExtensionTool_xk )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
			  Control/StoreGate
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkPatternParameters
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkEvent/TrkEventUtils
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          MagneticField/MagFieldElements
                          MagneticField/MagFieldConditions)

# Component(s) in the package:
atlas_add_component( TRT_TrackExtensionTool_xk
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetReadoutGeometry TRT_ReadoutGeometry InDetPrepRawData InDetRecToolInterfaces MagFieldInterfaces TrkGeometry TrkEventPrimitives TrkPatternParameters TrkPseudoMeasurementOnTrack TrkSegment TrkExInterfaces TrkToolInterfaces AthenaPoolUtilities InDetRIO_OnTrack TrkSurfaces TrkRIO_OnTrack TrkTrack MagFieldElements MagFieldConditions)

# Install files from the package:
atlas_install_headers( TRT_TrackExtensionTool_xk )

