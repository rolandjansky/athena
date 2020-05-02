################################################################################
# Package: MuonMDT_CnvTools
################################################################################

# Declare the package name:
atlas_subdir( MuonMDT_CnvTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/ByteStreamData
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/GeoModel/GeoModelUtilities
                          DetectorDescription/Identifier
                          Event/ByteStreamCnvSvcBase
                          MuonSpectrometer/MuonCablings/MuonMDT_Cabling
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibSvc
                          MuonSpectrometer/MuonCalib/MuonCalibEvent
                          MuonSpectrometer/MuonCnv/MuonCnvToolInterfaces
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonRDO
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData 
			  MuonSpectrometer/MuonCablings/MuonCablingData )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat_write DataWriter )

atlas_add_library( MuonMDT_CnvToolsLib
                   MuonMDT_CnvTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonMDT_CnvTools
                   LINK_LIBRARIES GaudiKernel ByteStreamData )

# Component(s) in the package:
atlas_add_component( MuonMDT_CnvTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ByteStreamData ByteStreamData_test GaudiKernel AthenaBaseComps StoreGateLib SGtests AtlasDetDescr Identifier ByteStreamCnvSvcBaseLib MuonMDT_CablingLib MdtCalibSvcLib MuonCalibEvent MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonRDO MuonPrepRawData MuonCablingData GeoModelUtilities MuonCnvToolInterfacesLib MuonMDT_CnvToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

