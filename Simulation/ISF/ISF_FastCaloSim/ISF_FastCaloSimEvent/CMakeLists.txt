# $Id: CMakeLists.txt 781795 2016-11-02 06:28:22Z vpascuzz $
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
  ROOT_HEADERS ISF_FastCaloSimEvent/IntArray.h ISF_FastCaloSimEvent/TFCS1DFunction.h ISF_FastCaloSimEvent/TFCSEnergyParametrization.h ISF_FastCaloSimEvent/TFCSExtrapolationState.h ISF_FastCaloSimEvent/TFCSParametrization.h ISF_FastCaloSimEvent/TFCSParametrizationBase.h ISF_FastCaloSimEvent/TFCSPCAEnergyParametrization.h ISF_FastCaloSimEvent/TFCSSimulationState.h ISF_FastCaloSimEvent/TFCSTruthState.h ISF_FastCaloSimEvent/LinkDef.h )

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
