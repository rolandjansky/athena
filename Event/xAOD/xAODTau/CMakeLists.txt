# $Id: CMakeLists.txt 775886 2016-09-29 16:03:02Z griffith $ 
################################################################################
# Package: xAODTau
################################################################################

# Declare the package name:
atlas_subdir( xAODTau )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthLinks
   Event/xAOD/xAODBase
   Event/xAOD/xAODCore
   Event/xAOD/xAODJet
   Event/xAOD/xAODPFlow
   Event/xAOD/xAODTracking
   Event/xAOD/xAODTruth )

# External dependencies:
find_package( ROOT COMPONENTS Core GenVector )

# Component(s) in the package:
atlas_add_library( xAODTau
   xAODTau/*.h xAODTau/*.icc xAODTau/versions/*.h
   Root/*.h Root/*.icc Root/*.cxx
   PUBLIC_HEADERS xAODTau
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks xAODBase xAODCore
   xAODJet xAODPFlow xAODTracking xAODTruth )

atlas_add_dictionary( xAODTauDict
   xAODTau/xAODTauDict.h
   xAODTau/selection.xml
   LINK_LIBRARIES xAODTau
   EXTRA_FILES Root/dict/*.cxx )

atlas_add_dictionary( xAODTau_cDict
   xAODTau/xAODTau_cDict.h
   xAODTau/selection_c.xml
   LINK_LIBRARIES xAODTau )


# Test(s) in the package:
atlas_add_test( xAODTau_TauJet_test
   SOURCES test/xAODTau_TauJet_test.cxx
   LINK_LIBRARIES xAODTau )
