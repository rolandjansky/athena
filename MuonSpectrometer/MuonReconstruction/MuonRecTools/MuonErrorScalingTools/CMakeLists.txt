################################################################################
# Package: MuonErrorScalingTools
################################################################################

# Declare the package name:
atlas_subdir( MuonErrorScalingTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          DetectorDescription/Identifier
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonCompetingRIOsOnTrack )

# Component(s) in the package:
atlas_add_component( MuonErrorScalingTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel Identifier GaudiKernel MuonRecToolInterfaces TrkEventPrimitives AthenaPoolUtilities MuonIdHelpersLib MuonCompetingRIOsOnTrack )

# Install files from the package:
atlas_install_headers( MuonErrorScalingTools )
atlas_install_joboptions( share/*.py )

