################################################################################
# Package: SeedToTrackConversionTool
################################################################################

# Declare the package name:
atlas_subdir( SeedToTrackConversionTool )
# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack )
# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
# Component(s) in the package:
atlas_add_component( SeedToTrackConversionTool
                     src/*.cxx
                     src/components/*.cxx 
		     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
		     LINK_LIBRARIES ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel InDetRecToolInterfaces TrkTrack TrkExInterfaces TrkToolInterfaces AthenaPoolUtilities InDetPrepRawData TrkRIO_OnTrack )
# Install files from the package:
atlas_install_headers( SeedToTrackConversionTool )
