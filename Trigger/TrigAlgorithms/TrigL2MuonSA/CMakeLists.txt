################################################################################
# Package: TrigL2MuonSA
################################################################################

# Declare the package name:
atlas_subdir( TrigL2MuonSA )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          DetectorDescription/RegionSelector
                          Event/ByteStreamCnvSvcBase
                          Event/xAOD/xAODTrigMuon
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          MuonSpectrometer/MuonCablings/MuonMDT_Cabling
                          MuonSpectrometer/MuonCablings/MuonRPC_Cabling
                          MuonSpectrometer/MuonCablings/MuonTGC_Cabling
                          MuonSpectrometer/MuonCablings/RPCcablingInterface
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibSvc
                          MuonSpectrometer/MuonCnv/MuonCnvToolInterfaces
                          MuonSpectrometer/MuonRDO
                          MuonSpectrometer/MuonReconstruction/MuonDataPrep/CscClusterization
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigT1/TrigT1RPCRecRoiSvc
                          Trigger/TrigTools/TrigMuonBackExtrapolator
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          Control/CxxUtils
                          Control/StoreGate
                          Event/EventInfo
                          MuonSpectrometer/MuonCablings/MuonCablingData
                          MuonSpectrometer/MuonCalib/MuonCalibEvent
                          MuonSpectrometer/MuonCnv/MuonContainerManager
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( GSL )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common COMPONENTS MuCalDecode circ_proc )

# Component(s) in the package:
atlas_add_library( TrigL2MuonSALib
                     src/*.cxx
                     PUBLIC_HEADERS TrigL2MuonSA
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GSL_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${GSL_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${EIGEN_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps GeoPrimitives Identifier RegionSelectorLib ByteStreamCnvSvcBaseLib xAODTrigMuon xAODTrigger GaudiKernel MuonMDT_CablingLib MuonTGC_CablingLib RPCcablingInterfaceLib MdtCalibSvcLib MuonRDO CscClusterizationLib MuonPrepRawData MuonRecToolInterfaces TrigMuonEvent TrigSteeringEvent TrigInterfacesLib TrigT1Interfaces TrigT1RPCRecRoiSvcLib TrigTimeAlgsLib StoreGateLib SGtests EventInfo MuonCablingData MuonCalibEvent MuonContainerManager MuonReadoutGeometry MuonIdHelpersLib PathResolver )

atlas_add_component( TrigL2MuonSA
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES TrigL2MuonSALib )

# Install files from the package:
atlas_install_headers( TrigL2MuonSA )
atlas_install_python_modules( python/*.py )
atlas_install_runtime( share/pt_barrel.lut share/pt_barrelSP.lut share/pt_barrelSP_new.lut share/pt_barrel.mc10.lut share/pt_endcap.lut share/pt_endcap.mc10.lut share/dZ_barrel.lut share/pt_endcap_small_large.lut share/pt_endcap_run2.lut share/pt_comb_sigma.lut share/pt_comb_mean.lut)

