################################################################################
# Package: SiTrackMakerTool_xk
################################################################################

# Declare the package name:
atlas_subdir( SiTrackMakerTool_xk )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkEvent/TrkCaloClusterROI
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack )

# Component(s) in the package:
atlas_add_component( SiTrackMakerTool_xk
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetRecToolInterfaces MagFieldInterfaces TrkGeometry TrkCaloClusterROI AthenaPoolUtilities InDetPrepRawData TrkRIO_OnTrack )

# Install files from the package:
atlas_install_headers( SiTrackMakerTool_xk )

