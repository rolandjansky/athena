################################################################################
# Package: MuonCSC_CnvTools
################################################################################

# Declare the package name:
atlas_subdir( MuonCSC_CnvTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/ByteStreamData
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/ByteStreamCnvSvcBase
                          Event/EventInfo
                          Event/EventPrimitives
                          MuonSpectrometer/MuonCablings/CSCcabling
                          MuonSpectrometer/MuonCalib/CscCalib/CscCalibTools
                          MuonSpectrometer/MuonCnv/MuonCnvToolInterfaces
                          MuonSpectrometer/MuonCnv/MuonContainerManager
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonDigitization/MuonDigToolInterfaces
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonRDO
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          Simulation/Tools/AtlasCLHEP_RandomGenerators
                          Tracking/TrkDetDescr/TrkSurfaces )

# External dependencies:
find_package( CLHEP )
find_package( tdaq-common COMPONENTS eformat_write DataWriter )

# Component(s) in the package:
atlas_add_component( MuonCSC_CnvTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${CLHEP_LIBRARIES} ByteStreamData ByteStreamData_test GaudiKernel AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests Identifier ByteStreamCnvSvcBaseLib EventInfo EventPrimitives CSCcablingLib MuonContainerManager MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonRDO MuonPrepRawData AtlasCLHEP_RandomGenerators TrkSurfaces )

# Install files from the package:
atlas_install_headers( MuonCSC_CnvTools )
atlas_install_joboptions( share/*.py )

