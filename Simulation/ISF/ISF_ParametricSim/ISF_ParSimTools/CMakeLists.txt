################################################################################
# Package: ISF_ParSimTools
################################################################################

# Declare the package name:
atlas_subdir( ISF_ParSimTools )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Matrix )

# Component(s) in the package:
atlas_add_component( ISF_ParSimTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel BeamSpotConditionsData xAODTracking GaudiKernel ISF_InterfacesLib TrkExInterfaces ISF_Event ISF_ParSimInterfacesLib PathResolver TrkParameters )

# Install files from the package:
atlas_install_runtime( Data/*.txt Data/*.root )

