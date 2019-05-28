################################################################################
# Package: MuonMM_CnvTools
################################################################################

# Declare the package name:
atlas_subdir( MuonMM_CnvTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/ByteStreamData
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/CLIDSvc
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/Identifier
                          Event/ByteStreamCnvSvcBase
                          MuonSpectrometer/MuonCnv/MuonCnvToolInterfaces
                          MuonSpectrometer/MuonCnv/MuonContainerManager
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonRDO
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData 
			  MuonSpectrometer/MuonReconstruction/MuonDataPrep/MMClusterization )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat_write DataWriter )

# Component(s) in the package:
atlas_add_component( MuonMM_CnvTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ByteStreamData ByteStreamData_test GaudiKernel AthenaBaseComps SGTools StoreGateLib SGtests AtlasDetDescr Identifier ByteStreamCnvSvcBaseLib MuonContainerManager MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonRDO MuonPrepRawData MMClusterization )

# Install files from the package:
atlas_install_headers( MuonMM_CnvTools )

