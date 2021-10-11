################################################################################
# Package: MuonByteStream
################################################################################

# Declare the package name:
atlas_subdir( MuonByteStream )

# External dependencies:
find_package( tdaq-common )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( MuonByteStream
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     DEFINITIONS ${CLHEP_DEFINITIONS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test GaudiKernel StoreGateLib AthenaPoolUtilities CSCcablingLib MuonIdHelpersLib MuonRDO TrigSteeringEvent AthViews MuonMDT_CnvToolsLib MuonCSC_CnvToolsLib MuonRPC_CnvToolsLib MuonSTGC_CnvToolsLib MuonCnvToolInterfacesLib ViewAlgsLib MuonAlignmentData )

# Install files from the package:
atlas_install_headers( MuonByteStream )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

