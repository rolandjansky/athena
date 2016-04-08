################################################################################
# Package: RpcCalibTools
################################################################################

# Declare the package name:
atlas_subdir( RpcCalibTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/DataModel
                          Control/StoreGate
                          GaudiKernel
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonTGRecTools
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          PRIVATE
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkRIO_OnTrack )

# Component(s) in the package:
atlas_add_library( RpcCalibToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS RpcCalibTools
                   LINK_LIBRARIES AthenaBaseComps DataModel GaudiKernel MuonReadoutGeometry TrkGeometry TrkParameters TrkTrack TrkExInterfaces StoreGateLib SGtests MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES MuonPrepRawData TrkMeasurementBase TrkRIO_OnTrack )

atlas_add_component( RpcCalibTools
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps DataModel StoreGateLib SGtests GaudiKernel MuonReadoutGeometry TrkGeometry TrkParameters TrkTrack TrkExInterfaces MuonIdHelpersLib MuonPrepRawData TrkMeasurementBase TrkRIO_OnTrack RpcCalibToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

