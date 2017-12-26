################################################################################
# Package: xAODTruthCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODTruthCnv )

# Extra package dependencies, based on the build environment:
set( extra_dep )
if( NOT XAOD_STANDALONE )
   set( extra_dep Database/AthenaPOOL/AthenaPoolKernel
      PRIVATE
      Control/AthLinks
      Control/AthenaBaseComps
      Control/StoreGate
      Event/EventInfo
      Event/xAOD/xAODEventInfo
      Event/xAOD/xAODTruth
      GaudiKernel
      Generators/GeneratorObjects )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODTruth
   ${extra_dep} )

# Component(s) in the package:
atlas_add_library( xAODTruthCnvLib
   xAODTruthCnv/*.h Root/*.cxx
   PUBLIC_HEADERS xAODTruthCnv
   LINK_LIBRARIES AsgTools xAODTruth )

if( NOT XAOD_STANDALONE )
   atlas_add_component( xAODTruthCnv
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AthLinks AthenaBaseComps StoreGateLib EventInfo
      xAODEventInfo xAODTruth GaudiKernel GeneratorObjects
      xAODTruthCnvLib )
endif()

atlas_add_dictionary( xAODTruthCnvDict
   xAODTruthCnv/xAODTruthCnvDict.h
   xAODTruthCnv/selection.xml
   LINK_LIBRARIES xAODTruthCnvLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
