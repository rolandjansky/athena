################################################################################
# Package: EgammaAnalysisHelpers
################################################################################

# Declare the package name:
atlas_subdir(EgammaAnalysisHelpers )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
  Event/xAOD/xAODEgamma
  PRIVATE
  Control/AthToolSupport/AsgTools
  Event/xAOD/xAODCaloEvent
  ${extra_dep} )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist MathMore )

# Component(s) in the package:
atlas_add_library( EgammaAnalysisHelpersLib
  EgammaAnalysisHelpers/*.h Root/*.cxx
  PUBLIC_HEADERS EgammaAnalysisHelpers
  PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES xAODEgamma 
  PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODCaloEvent)

atlas_add_dictionary( EgammaAnalysisHelpersDict
  EgammaAnalysisHelpers/EgammaAnalysisHelpersDict.h
  EgammaAnalysisHelpers/selection.xml
  LINK_LIBRARIES EgammaAnalysisHelpersLib )


