################################################################################
# Package: MuonTGC_CnvTools
################################################################################

# Declare the package name:
atlas_subdir( MuonSTGC_CnvTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamData
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          MuonSpectrometer/MuonCablings/TGCcablingInterface
                          MuonSpectrometer/MuonCnv/MuonCnvToolInterfaces
                          MuonSpectrometer/MuonCnv/MuonContainerManager
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonRDO
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonTrigCoinData
                          Tracking/TrkDetDescr/TrkSurfaces )

# External dependencies (some of these aren't currently needed, but they will once we add BS etc)
find_package( Eigen )
find_package( tdaq-common COMPONENTS eformat_write DataWriter )

# Component(s) in the package:
atlas_add_component( MuonSTGC_CnvTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${EIGEN_LIBRARIES} ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test GaudiKernel AthenaBaseComps Identifier EventPrimitives TGCcablingInterfaceLib MuonContainerManager MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonRDO MuonPrepRawData MuonTrigCoinData TrkSurfaces )

# Install files from the package:
atlas_install_headers( MuonTGC_CnvTools )
atlas_install_joboptions( share/*.py )

