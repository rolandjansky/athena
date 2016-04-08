################################################################################
# Package: MuonCalibStreamCnvSvc
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibStreamCnvSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          GaudiKernel
                          MuonSpectrometer/MuonCablings/CSCcabling
                          MuonSpectrometer/MuonCablings/RPCcablingInterface
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonRDO
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          PRIVATE
                          Control/CLIDSvc
                          Control/SGTools
                          Database/CoraCool
                          Event/EventInfo
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibSvc
                          MuonSpectrometer/MuonCalib/MuonCalibEvent
                          MuonSpectrometer/MuonCnv/MuonCSC_CnvTools )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common COMPONENTS MuCalDecode )

# Component(s) in the package:
atlas_add_component( MuonCalibStreamCnvSvc
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${COOL_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests GaudiKernel CSCcablingLib RPCcablingInterfaceLib MuonReadoutGeometry MuonIdHelpersLib MuonRDO MuonPrepRawData SGTools CoraCool EventInfo MdtCalibSvcLib MuonCalibEvent )

# Install files from the package:
atlas_install_headers( MuonCalibStreamCnvSvc )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/calibtest.dat )

