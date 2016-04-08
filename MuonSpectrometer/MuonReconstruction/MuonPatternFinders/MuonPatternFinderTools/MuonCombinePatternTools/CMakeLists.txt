################################################################################
# Package: MuonCombinePatternTools
################################################################################

# Declare the package name:
atlas_subdir( MuonCombinePatternTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonHoughPatternEvent
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          PRIVATE
                          Control/CxxUtils
                          Control/StoreGate
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPattern
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParameters )

# Component(s) in the package:
atlas_add_library( MuonCombinePatternToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonCombinePatternTools
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonHoughPatternEvent MuonRecToolInterfaces MuonIdHelpersLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES CxxUtils MuonReadoutGeometry MuonPattern MuonPrepRawData TrkSurfaces TrkParameters )

atlas_add_component( MuonCombinePatternTools
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonIdHelpersLib MuonHoughPatternEvent MuonRecToolInterfaces CxxUtils StoreGateLib SGtests MuonReadoutGeometry MuonPattern MuonPrepRawData TrkSurfaces TrkParameters MuonCombinePatternToolsLib )

