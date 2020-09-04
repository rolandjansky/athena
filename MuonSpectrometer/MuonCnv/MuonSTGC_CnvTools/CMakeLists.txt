################################################################################
# Package: MuonTGC_CnvTools
################################################################################

# Declare the package name:
atlas_subdir( MuonSTGC_CnvTools )

# External dependencies (some of these aren't currently needed, but they will once we add BS etc)
find_package( Eigen )
find_package( tdaq-common COMPONENTS eformat_write DataWriter )

atlas_add_library( MuonSTGC_CnvToolsLib
                   MuonSTGC_CnvTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonSTGC_CnvTools
                   LINK_LIBRARIES GaudiKernel )

# Component(s) in the package:
atlas_add_component( MuonSTGC_CnvTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${EIGEN_LIBRARIES} ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test GaudiKernel AthenaBaseComps Identifier EventPrimitives TGCcablingInterfaceLib MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonRDO MuonPrepRawData MuonTrigCoinData TrkSurfaces STgcClusterizationLib MuonCnvToolInterfacesLib MuonSTGC_CnvToolsLib )
