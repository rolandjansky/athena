# $Id: CMakeLists.txt 769423 2016-08-23 09:45:44Z krasznaa $
################################################################################
# Package: ISF_FastCaloSimEvent
################################################################################

# Declare the package name:
atlas_subdir( ISF_FastCaloSimEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/SGTools
   Calorimeter/CaloGeoHelpers
   TileCalorimeter/TileSimEvent
   PRIVATE
   GaudiKernel )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO TMVA Matrix Physics )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_root_dictionary( ISF_FastCaloSimEvent _dictSource
   ROOT_HEADERS ISF_FastCaloSimEvent/IntArray.h )

atlas_add_library( ISF_FastCaloSimEvent
   ISF_FastCaloSimEvent/*.h ${_dictSource} src/*.cxx
   PUBLIC_HEADERS ISF_FastCaloSimEvent
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthContainers SGTools
   CaloGeoHelpers TileSimEvent
   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_dictionary( ISF_FastCaloSimEventDict
   ISF_FastCaloSimEvent/ISF_FastCaloSimEventDict.h
   ISF_FastCaloSimEvent/selection.xml
   LINK_LIBRARIES ISF_FastCaloSimEvent )
