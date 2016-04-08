################################################################################
# Package: MdtVsRpcRawDataMonitoring
################################################################################

# Declare the package name:
atlas_subdir( MdtVsRpcRawDataMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaMonitoring
                          Control/DataModel
                          Control/StoreGate
                          GaudiKernel
                          MuonSpectrometer/MuonCablings/RPCcablingInterface
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonValidation/MuonDQA/MuonDQAUtils
                          PRIVATE
                          DetectorDescription/Identifier
                          MuonSpectrometer/MuonRDO
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData )

# tag ROOTGraphicsLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( MdtVsRpcRawDataMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaMonitoringLib DataModel StoreGateLib SGtests GaudiKernel RPCcablingInterfaceLib MuonReadoutGeometry MuonIdHelpersLib MuonDQAUtilsLib Identifier MuonRDO MuonPrepRawData )

# Install files from the package:
atlas_install_headers( MdtVsRpcRawDataMonitoring )
atlas_install_joboptions( share/*.py )

