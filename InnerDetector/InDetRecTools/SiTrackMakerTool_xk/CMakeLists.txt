################################################################################
# Package: SiTrackMakerTool_xk
################################################################################

# Declare the package name:
atlas_subdir( SiTrackMakerTool_xk )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkEvent/TrkCaloClusterROI
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/SiSPSeededTrackFinderData
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          MagneticField/MagFieldElements
                          MagneticField/MagFieldConditions)

# Component(s) in the package:
atlas_add_component( SiTrackMakerTool_xk
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetRecToolInterfaces TrkGeometry TrkCaloClusterROI AthenaPoolUtilities InDetPrepRawData SiSPSeededTrackFinderData TrkRIO_OnTrack MagFieldElements MagFieldConditions BeamSpotConditionsData )

# Install files from the package:
atlas_install_headers( SiTrackMakerTool_xk )

