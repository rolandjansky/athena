################################################################################
# Package: SiSpacePointsSeedTool_xk
################################################################################

# Declare the package name:
atlas_subdir( SiSpacePointsSeedTool_xk )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/SiSpacePointsSeed
                          InnerDetector/InDetRecEvent/SiSPSeededTrackFinderData
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkSpacePoint
                          PRIVATE
                          Tracking/TrkTools/TrkToolInterfaces
                          MagneticField/MagFieldElements
                          MagneticField/MagFieldConditions)

# Component(s) in the package:
atlas_add_component( SiSpacePointsSeedTool_xk
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetReadoutGeometry InDetPrepRawData SiSpacePointsSeed SiSPSeededTrackFinderData InDetRecToolInterfaces TrkSurfaces TrkSpacePoint TrkToolInterfaces MagFieldElements MagFieldConditions BeamSpotConditionsData )

# Install files from the package:
atlas_install_headers( SiSpacePointsSeedTool_xk )

