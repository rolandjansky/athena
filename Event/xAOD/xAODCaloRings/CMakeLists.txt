################################################################################
# Package: xAODCaloRings
################################################################################

# Declare the package name:
atlas_subdir( xAODCaloRings )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Calorimeter/CaloGeoHelpers
   Control/AthContainers
   Control/AthLinks
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODCore )

# Component(s) in the package:
atlas_add_library( xAODCaloRings
   xAODCaloRings/*.h xAODCaloRings/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODCaloRings
   LINK_LIBRARIES CaloGeoHelpers AthContainers AthLinks AsgTools xAODCore )

atlas_add_dictionary( xAODCaloRingsDict
   xAODCaloRings/xAODCaloRingsDict.h
   xAODCaloRings/selection.xml
   LINK_LIBRARIES xAODCaloRings
   EXTRA_FILES Root/dict/*.cxx )
