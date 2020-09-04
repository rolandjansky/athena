################################################################################
# Package: CscCalibEvent
################################################################################

# Declare the package name:
atlas_subdir( CscCalibEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( CscCalibEvent
                   src/*.cxx
                   PUBLIC_HEADERS CscCalibEvent
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers Identifier MuonIdHelpersLib EventContainers
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( CscCalibEventDict
                      CscCalibEvent/CscCalibEventDict.h
                      CscCalibEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers Identifier MuonIdHelpersLib CscCalibEvent EventContainers )

