################################################################################
# Package: TrigConfInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrigConfInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_dictionary( TrigConfInterfacesDict
                      TrigConfInterfaces/TrigConfInterfacesDict.h
                      TrigConfInterfaces/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools GaudiKernel )

# Install files from the package:
atlas_install_headers( TrigConfInterfaces )

