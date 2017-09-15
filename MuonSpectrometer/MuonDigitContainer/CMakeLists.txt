################################################################################
# Package: MuonDigitContainer
################################################################################

# Declare the package name:
atlas_subdir( MuonDigitContainer )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          DetectorDescription/Identifier
                          Event/EventContainers
                          MuonSpectrometer/MuonIdHelpers
                          PRIVATE
                          TestPolicy )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonDigitContainer
                   src/*.cxx
                   PUBLIC_HEADERS MuonDigitContainer
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES DataModel SGTools Identifier MuonIdHelpersLib EventContainers
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( MuonDigitContainerDict
                      MuonDigitContainer/MuonDigitContainerDict.h
                      MuonDigitContainer/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools Identifier MuonIdHelpersLib MuonDigitContainer EventContainers)

