################################################################################
# Package: SiSpacePointsSeedTool_xk
################################################################################

# Declare the package name:
atlas_subdir( SiSpacePointsSeedTool_xk )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/SiSpacePointsSeed
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkSpacePoint			  
			  Control/AthenaServices
                          PRIVATE
                          Tracking/TrkTools/TrkToolInterfaces 
			  Tracking/TrkEvent/TrkTrack
                          Event/xAOD/xAODEventInfo
			  Event/xAOD/xAODTruth )

find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( SiSpacePointsSeedTool_xk
                     src/*.cxx
                     src/components/*.cxx
		     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel InDetReadoutGeometry InDetPrepRawData SiSpacePointsSeed 
                     InDetRecToolInterfaces MagFieldInterfaces TrkSurfaces TrkPseudoMeasurementOnTrack TrkSpacePoint 
		     TrkToolInterfaces xAODTruth ITrackToVertex xAODEventInfo )

# Install files from the package:
atlas_install_headers( SiSpacePointsSeedTool_xk )

