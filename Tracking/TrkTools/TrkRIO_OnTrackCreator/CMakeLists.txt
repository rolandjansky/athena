################################################################################
# Package: TrkRIO_OnTrackCreator
################################################################################

# Declare the package name:
atlas_subdir( TrkRIO_OnTrackCreator )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/EventPrimitives
                          GaudiKernel
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkTools/TrkToolInterfaces
			  PRIVATE
			  Control/CxxUtils
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/Identifier
                          Event/xAOD/xAODEventInfo
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack )

# Component(s) in the package:
atlas_add_component( TrkRIO_OnTrackCreator
                     src/*.cxx
                     src/components/*.cxx
		     LINK_LIBRARIES AthenaBaseComps AthenaKernel EventPrimitives GaudiKernel TrkParameters TrkToolInterfaces CxxUtils AthenaPoolUtilities AtlasDetDescr Identifier xAODEventInfo InDetIdentifier InDetRIO_OnTrack MuonRIO_OnTrack TrkPrepRawData TrkRIO_OnTrack )

# Install files from the package:
atlas_install_headers( TrkRIO_OnTrackCreator )
atlas_install_joboptions( share/*.py )

