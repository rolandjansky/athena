################################################################################
# Package: MuonRDO
################################################################################

# Declare the package name:
atlas_subdir( MuonRDO )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          DetectorDescription/Identifier
                          Event/EventContainers
                          MuonSpectrometer/MuonCablings/CSCcabling
                          MuonSpectrometer/MuonIdHelpers
                          PRIVATE
                          Control/StoreGate
                          GaudiKernel
                          MuonSpectrometer/MuonCablings/TGCcablingInterface )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonRDO
                   src/*.cxx
                   PUBLIC_HEADERS MuonRDO
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools Identifier CSCcablingLib MuonIdHelpersLib StoreGateLib SGtests TGCcablingInterfaceLib EventContainers
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_dictionary( MuonRDODict
                      MuonRDO/MuonRDODict.h
                      MuonRDO/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools Identifier CSCcablingLib MuonIdHelpersLib StoreGateLib SGtests GaudiKernel TGCcablingInterfaceLib MuonRDO EventContainers)

