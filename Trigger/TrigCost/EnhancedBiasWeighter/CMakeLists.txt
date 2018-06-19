################################################################################
# Package: EnhancedBiasWeighter
################################################################################

# Declare the package name: 
atlas_subdir( EnhancedBiasWeighter )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODEventInfo
   GaudiKernel
   PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
   Trigger/TrigAnalysis/TrigAnalysisInterfaces
   PRIVATE
   Tools/PathResolver )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( CORAL COMPONENTS CoralKernel )
find_package( ROOT COMPONENTS XMLIO XMLParser Core )

# Component(s) in the package:
atlas_add_library( EnhancedBiasWeighterLib
   EnhancedBiasWeighter/*.h Root/*.cxx
   PUBLIC_HEADERS EnhancedBiasWeighter
   INCLUDE_DIRS ${COOL_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
   LINK_LIBRARIES ${COOL_LIBRARIES} AsgTools xAODEventInfo GaudiKernel
   DerivationFrameworkInterfaces TrigAnalysisInterfaces
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} PathResolver )

atlas_add_component( EnhancedBiasWeighter
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel EnhancedBiasWeighterLib )

atlas_add_dictionary( EnhancedBiasWeighterDict
   EnhancedBiasWeighter/EnhancedBiasWeighterDict.h
   EnhancedBiasWeighter/selection.xml
   LINK_LIBRARIES EnhancedBiasWeighterLib )
