################################################################################
# Package: MuGirlGlobalFit
################################################################################

# Declare the package name:
atlas_subdir( MuGirlGlobalFit )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MuonSpectrometer/MuonIdHelpers
                          Reconstruction/MuonIdentification/MuGirlInterfaces
                          Reconstruction/MuonIdentification/MuidInterfaces
                          Tracking/TrkFitter/TrkFitterUtils
                          PRIVATE
                          Control/DataModel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonCompetingRIOsOnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          Reconstruction/iPat/iPatTrackParameters
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( MuGirlGlobalFit
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel MuonIdHelpersLib MuGirlInterfaces MuidInterfaces TrkFitterUtils DataModel MuonCompetingRIOsOnTrack MuonRIO_OnTrack MuonSegment MuonRecToolInterfaces iPatTrackParameters TrkSurfaces TrkEventPrimitives TrkRIO_OnTrack TrkTrack TrkExInterfaces TrkExUtils )

# Install files from the package:
atlas_install_headers( MuGirlGlobalFit )

