################################################################################
# Package: MuonTGC_CnvTools
################################################################################

# Declare the package name:
atlas_subdir( MuonTGC_CnvTools )

# External dependencies:
find_package( Eigen )
find_package( tdaq-common COMPONENTS eformat_write DataWriter )

atlas_add_library( MuonTGC_CnvToolsLib
                   MuonTGC_CnvTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonTGC_CnvTools
                   LINK_LIBRARIES GaudiKernel ByteStreamData )

# Component(s) in the package:
atlas_add_component( MuonTGC_CnvTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${EIGEN_LIBRARIES} ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test GaudiKernel AthenaBaseComps Identifier EventPrimitives TGCcablingInterfaceLib MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonRDO MuonPrepRawData MuonTrigCoinData TrkSurfaces MuonCnvToolInterfacesLib MuonTGC_CnvToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

