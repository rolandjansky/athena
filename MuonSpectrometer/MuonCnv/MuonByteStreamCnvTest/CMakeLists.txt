################################################################################
# Package: MuonByteStreamCnvTest
################################################################################

# Declare the package name:
atlas_subdir( MuonByteStreamCnvTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          MuonSpectrometer/MuonCablings/RPCcablingInterface
                          MuonSpectrometer/MuonCalib/CscCalib/CscCalibTools
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonDigitization/MuonDigToolInterfaces
                          MuonSpectrometer/MuonRDO
                          Trigger/TrigT1/TrigT1RPChardware
                          Trigger/TrigT1/TrigT1RPClogic
                          PRIVATE
                          Event/EventInfoMgt
                          MuonSpectrometer/MuonCablings/MuonMDT_Cabling
                          MuonSpectrometer/MuonCablings/TGCcablingInterface
                          MuonSpectrometer/MuonCnv/MuonCSC_CnvTools
                          MuonSpectrometer/MuonCnv/MuonMDT_CnvTools
                          MuonSpectrometer/MuonCnv/MuonRPC_CnvTools
                          MuonSpectrometer/MuonCnv/MuonTGC_CnvTools
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData 
			  MuonSpectrometer/MuonCablings/MuonCablingData )

# Component(s) in the package:
atlas_add_library( MuonByteStreamCnvTestLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonByteStreamCnvTest
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonReadoutGeometry MuonRDO StoreGateLib SGtests RPCcablingInterfaceLib TrigT1RPChardwareLib RPChardware TrigT1RPClogicLib MuonMDT_CablingLib TGCcablingInterfaceLib MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES EventInfo MuonDigitContainer MuonPrepRawData MuonCablingData )

atlas_add_component( MuonByteStreamCnvTest
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests GaudiKernel RPCcablingInterfaceLib MuonReadoutGeometry MuonRDO TrigT1RPChardwareLib RPChardware TrigT1RPClogicLib EventInfo MuonMDT_CablingLib TGCcablingInterfaceLib MuonDigitContainer MuonIdHelpersLib MuonPrepRawData MuonByteStreamCnvTestLib MuonCablingData )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

