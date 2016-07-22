
################################################################################
# Package: ISF_FastCaloSimEvent
################################################################################

# Declare the package name:
atlas_subdir( ISF_FastCaloSimEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          GaudiKernel
                          Calorimeter/CaloGeoHelpers
                          TileCalorimeter/TileSimEvent )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Physics Hist RIO pthread )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_root_dictionary( ISF_FastCaloSimEvent _dictSource
                           ROOT_HEADERS ISF_FastCaloSimEvent/IntArray.h )

atlas_add_library( ISF_FastCaloSimEvent ${_dictSource}
                   src/*.cxx
                   PUBLIC_HEADERS ISF_FastCaloSimEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} DataModel SGTools GaudiKernel CaloGeoHelpers TileSimEvent )

atlas_add_dictionary( ISF_FastCaloSimEventDict
                      ISF_FastCaloSimEvent/ISF_FastCaloSimEventDict.h
                      ISF_FastCaloSimEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModel SGTools GaudiKernel CaloGeoHelpers TileSimEvent ISF_FastCaloSimEvent )
