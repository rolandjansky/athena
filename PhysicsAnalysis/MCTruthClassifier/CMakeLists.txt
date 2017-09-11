# $Id: CMakeLists.txt 779176 2016-10-19 10:15:04Z krasznaa $
################################################################################
# Package: MCTruthClassifier
################################################################################

# Declare the package name:
atlas_subdir( MCTruthClassifier )

# Extra dependencies based on the environment:
set( extra_deps )
set( extra_libs )
if( XAOD_STANDALONE )
   set( extra_deps PRIVATE Control/xAODRootAccess )
elseif( XAOD_ANALYSIS )
   set( extra_deps GaudiKernel )
   set( extra_libs GaudiKernel )
else()
   set( extra_deps GaudiKernel
      Reconstruction/RecoTools/ParticlesInConeTools
      PRIVATE
      Generators/GeneratorObjects
      Reconstruction/RecoTools/RecoToolInterfaces
      Tracking/TrkEvent/TrkEventPrimitives
      Tracking/TrkEvent/TrkParametersIdentificationHelpers
      Control/AthenaKernel )
   set( extra_libs GaudiKernel
      PRIVATE_LINK_LIBRARIES GeneratorObjects RecoToolInterfaces
      TrkEventPrimitives TrkParametersIdentificationHelpers )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODCaloEvent
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODJet
   Event/xAOD/xAODMuon
   Event/xAOD/xAODTracking
   Event/xAOD/xAODTruth
   Generators/TruthUtils
   ${extra_deps} )

# External dependencies:
find_package( HepMC )
find_package( ROOT COMPONENTS Core RIO )

# Libraries in the package:
if( HEPMC_FOUND )
   atlas_add_library( MCTruthClassifierLib
      MCTruthClassifier/*.h src/*.cxx Root/*.cxx
      PUBLIC_HEADERS MCTruthClassifier
      INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
      LINK_LIBRARIES ${HEPMC_LIBRARIES} AsgTools xAODCaloEvent xAODEgamma
      xAODJet xAODMuon xAODTracking xAODTruth ${extra_libs} )
else()
   atlas_add_library( MCTruthClassifierLib
      MCTruthClassifier/*.h src/*.cxx Root/*.cxx
      PUBLIC_HEADERS MCTruthClassifier
      LINK_LIBRARIES AsgTools xAODCaloEvent xAODEgamma
      xAODJet xAODMuon xAODTracking xAODTruth ${extra_libs} )
endif()

if( NOT XAOD_STANDALONE )
   atlas_add_component( MCTruthClassifier
      src/components/*.cxx
      LINK_LIBRARIES GaudiKernel MCTruthClassifierLib )
endif()

atlas_add_dictionary( MCTruthClassifierDict
   MCTruthClassifier/MCTruthClassifierDict.h
   MCTruthClassifier/selection.xml
   LINK_LIBRARIES MCTruthClassifierLib )

# Executable(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( testClassifier
      util/testClassifier.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEgamma
      MCTruthClassifierLib )
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py )
