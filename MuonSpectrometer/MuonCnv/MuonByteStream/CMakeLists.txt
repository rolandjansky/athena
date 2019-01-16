################################################################################
# Package: MuonByteStream
################################################################################

# Declare the package name:
atlas_subdir( MuonByteStream )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/IRegionSelector
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamData
                          GaudiKernel
                          MuonSpectrometer/MuonCnv/MuonCSC_CnvTools
                          MuonSpectrometer/MuonCnv/MuonCnvToolInterfaces
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Control/StoreGate
                          Control/AthViews
                          MuonSpectrometer/MuonCablings/CSCcabling
                          MuonSpectrometer/MuonCablings/RPCcablingInterface
                          MuonSpectrometer/MuonCnv/MuonMDT_CnvTools
                          MuonSpectrometer/MuonCnv/MuonRPC_CnvTools
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonRDO )

# External dependencies:
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( MuonByteStream
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test GaudiKernel StoreGateLib SGtests CSCcablingLib RPCcablingInterfaceLib MuonIdHelpersLib MuonRDO TrigSteeringEvent AthViews)

# Install files from the package:
atlas_install_headers( MuonByteStream )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

