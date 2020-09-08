################################################################################
# Package: MuonDigitContainer
################################################################################

# Declare the package name:
atlas_subdir( MuonDigitContainer )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonDigitContainer
                   src/*.cxx
                   PUBLIC_HEADERS MuonDigitContainer
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers Identifier MuonIdHelpersLib EventContainers
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( MuonDigitContainerDict
                      MuonDigitContainer/MuonDigitContainerDict.h
                      MuonDigitContainer/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers Identifier MuonIdHelpersLib MuonDigitContainer EventContainers)

