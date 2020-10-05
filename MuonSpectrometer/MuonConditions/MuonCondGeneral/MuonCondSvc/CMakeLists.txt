################################################################################
# Package: MuonCondSvc
################################################################################

# Declare the package name:
atlas_subdir( MuonCondSvc )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonCondSvcLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonCondSvc
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities Identifier GaudiKernel MuonCondData MuonCondInterface StoreGateLib SGtests MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_component( MuonCondSvc
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests AthenaPoolUtilities Identifier GaudiKernel MuonCondData MuonCondInterface MuonIdHelpersLib MuonCondSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
