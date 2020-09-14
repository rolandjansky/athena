################################################################################
# Package: MuonJiveXML
################################################################################

# Declare the package name:
atlas_subdir( MuonJiveXML )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( MuonJiveXML
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps MuonRPC_CnvToolsLib MuonReadoutGeometry JiveXMLLib GaudiKernel RPCcablingInterfaceLib MuonIdHelpersLib MuonRDO MuonPrepRawData AnalysisTriggerEvent RPC_CondCablingLib)

# Install files from the package:
atlas_install_headers( MuonJiveXML )
atlas_install_joboptions( share/*.py )

