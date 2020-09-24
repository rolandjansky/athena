################################################################################
# Package: MuonCalibIdentifier
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibIdentifier )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonCalibIdentifier
                   src/*.cxx
                   PUBLIC_HEADERS MuonCalibIdentifier
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES Identifier MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} PathResolver )

atlas_add_dictionary( MuonCalibIdentifierDict
                      MuonCalibIdentifier/DictHeaders.h
                      MuonCalibIdentifier/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} Identifier MuonIdHelpersLib PathResolver MuonCalibIdentifier )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_runtime( share/mdt_gas_distribution.txt )

