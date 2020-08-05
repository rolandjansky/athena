################################################################################
# Package: MuonByteStream
################################################################################

# Declare the package name:
atlas_subdir( MuonByteStream )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/IRegionSelector
			  Database/AthenaPOOL/AthenaPoolUtilities
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamData
                          GaudiKernel
                          MuonSpectrometer/MuonCnv/MuonCSC_CnvTools
                          MuonSpectrometer/MuonCnv/MuonCnvToolInterfaces
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Control/StoreGate
                          Control/AthViews
                          Trigger/TrigSteer/ViewAlgs
                          MuonSpectrometer/MuonCablings/CSCcabling
                          MuonSpectrometer/MuonCablings/RPCcablingInterface
                          MuonSpectrometer/MuonCnv/MuonMDT_CnvTools
                          MuonSpectrometer/MuonCnv/MuonRPC_CnvTools
                          MuonSpectrometer/MuonIdHelpers
			  MuonSpectrometer/MuonAlignment/MuonAlignmentData
                          MuonSpectrometer/MuonRDO )

# External dependencies:
find_package( tdaq-common )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( MuonByteStream
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
		     DEFINITIONS ${CLHEP_DEFINITIONS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test GaudiKernel StoreGateLib SGtests AthenaPoolUtilities CSCcablingLib RPCcablingInterfaceLib MuonIdHelpersLib MuonRDO TrigSteeringEvent AthViews MuonMDT_CnvToolsLib MuonCSC_CnvToolsLib MuonRPC_CnvToolsLib MuonCnvToolInterfacesLib ViewAlgsLib)

# Install files from the package:
atlas_install_headers( MuonByteStream )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

