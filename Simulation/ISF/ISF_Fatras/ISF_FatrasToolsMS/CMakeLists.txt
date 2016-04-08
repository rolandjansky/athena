################################################################################
# Package: ISF_FatrasToolsMS
################################################################################

# Declare the package name:
atlas_subdir( ISF_FatrasToolsMS )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          DetectorDescription/Identifier
                          GaudiKernel
                          MuonSpectrometer/MuonSimEvent
                          Simulation/ISF/ISF_Fatras/ISF_FatrasInterfaces
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          PRIVATE
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonTGRecTools
                          Simulation/ISF/ISF_Core/ISF_Event
                          Simulation/ISF/ISF_Core/ISF_Interfaces
                          Tracking/TrkEvent/TrkTrack )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( ISF_FatrasToolsMS
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel Identifier GaudiKernel MuonSimEvent ISF_FatrasInterfaces TrkParameters TrkExInterfaces MuonReadoutGeometry MuonIdHelpersLib ISF_Event ISF_Interfaces TrkTrack )

# Install files from the package:
atlas_install_headers( ISF_FatrasToolsMS )

