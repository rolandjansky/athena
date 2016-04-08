################################################################################
# Package: MuonJiveXML
################################################################################

# Declare the package name:
atlas_subdir( MuonJiveXML )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          graphics/JiveXML
                          PRIVATE
                          GaudiKernel
                          MuonSpectrometer/MuonCablings/RPCcablingInterface
                          MuonSpectrometer/MuonCnv/MuonRPC_CnvTools
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonRDO
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( MuonJiveXML
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps MuonReadoutGeometry JiveXMLLib GaudiKernel RPCcablingInterfaceLib MuonIdHelpersLib MuonRDO MuonPrepRawData AnalysisTriggerEvent )

# Install files from the package:
atlas_install_headers( MuonJiveXML )
atlas_install_joboptions( share/*.py )

