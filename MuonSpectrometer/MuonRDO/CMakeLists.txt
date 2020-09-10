################################################################################
# Package: MuonRDO
################################################################################

# Declare the package name:
atlas_subdir( MuonRDO )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonRDO
                   src/*.cxx
                   PUBLIC_HEADERS MuonRDO
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel AthContainers Identifier CSCcablingLib MuonIdHelpersLib StoreGateLib SGtests TGCcablingInterfaceLib EventContainers
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_dictionary( MuonRDODict
                      MuonRDO/MuonRDODict.h
                      MuonRDO/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers Identifier CSCcablingLib MuonIdHelpersLib StoreGateLib SGtests GaudiKernel TGCcablingInterfaceLib MuonRDO EventContainers)

