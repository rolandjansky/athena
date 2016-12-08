# $Id: CMakeLists.txt 782671 2016-11-07 09:27:00Z elmsheus $

# The name of the package:
atlas_subdir( JetJvtEfficiency )

# Extra dependencies, based on the build environment:
set( extra_deps )
if( NOT XAOD_STANDALONE )
   set( extra_deps GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODJet
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PRIVATE
   Tools/PathResolver
   ${extra_deps} )

# External(s):
find_package( ROOT COMPONENTS Core Hist RIO )

# Libraries of the package:
atlas_add_library( JetJvtEfficiencyLib
   JetJvtEfficiency/*.h Root/*.cxx
   PUBLIC_HEADERS JetJvtEfficiency
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODJet PATInterfaces
   PRIVATE_LINK_LIBRARIES PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( JetJvtEfficiency
      src/components/*.cxx
      LINK_LIBRARIES JetJvtEfficiencyLib )
endif()

atlas_add_dictionary( JetJvtEfficiencyDict
   JetJvtEfficiency/JetJvtEfficiencyDict.h
   JetJvtEfficiency/selection.xml
   LINK_LIBRARIES JetJvtEfficiencyLib )

# Install files from the package:
atlas_install_data( share/*.root )
