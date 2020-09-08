################################################################################
# Package: MuonMM_CnvTools
################################################################################

# Declare the package name:
atlas_subdir( MuonMM_CnvTools )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat_write DataWriter )

atlas_add_library( MuonMM_CnvToolsLib
                   MuonMM_CnvTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonMM_CnvTools
                   LINK_LIBRARIES GaudiKernel )

# Component(s) in the package:
atlas_add_component( MuonMM_CnvTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ByteStreamData ByteStreamData_test GaudiKernel AthenaBaseComps SGTools StoreGateLib SGtests AtlasDetDescr Identifier ByteStreamCnvSvcBaseLib MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonRDO MuonPrepRawData MMClusterizationLib NSWCalibToolsLib MuonCnvToolInterfacesLib MuonMM_CnvToolsLib )

