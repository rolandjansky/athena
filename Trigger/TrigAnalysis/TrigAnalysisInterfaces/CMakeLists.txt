################################################################################
# Package: TrigAnalysisInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrigAnalysisInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_dictionary( TrigAnalysisInterfacesDict
                      TrigAnalysisInterfaces/TrigAnalysisInterfacesDict.h
                      TrigAnalysisInterfaces/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools )

# Install files from the package:
atlas_install_headers( TrigAnalysisInterfaces )

