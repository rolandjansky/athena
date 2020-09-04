# $Id: CMakeLists.txt 781052 2016-10-28 16:02:10Z krasznaa $

# The name of the package:
atlas_subdir( ZMassConstraint )

# Extra dependencies, based on the build environment:
set( extra_deps )
if( NOT XAOD_STANDALONE )
   set( extra_deps Control/AthenaBaseComps GaudiKernel )
endif()

# The dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/EventPrimitives
   Event/xAOD/xAODBase
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODMuon
   PRIVATE
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODTracking
   PhysicsAnalysis/ElectronPhotonID/ElectronPhotonFourMomentumCorrection
   PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonMomentumCorrections
   PhysicsAnalysis/Interfaces/EgammaAnalysisInterfaces
   ${extra_deps} )

# Externals used by the package:
find_package( ROOT COMPONENTS Core Physics )

# Libraries in the package:
atlas_add_library( ZMassConstraintLib
   ZMassConstraint/*.h Root/*.cxx
   PUBLIC_HEADERS ZMassConstraint
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools EventPrimitives xAODBase xAODEgamma
   xAODMuon
   PRIVATE_LINK_LIBRARIES MuonMomentumCorrectionsLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( ZMassConstaint
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AthenaBaseComps AsgTools xAODEgamma xAODEventInfo
      xAODTracking GaudiKernel ZMassConstraintLib )
endif()

atlas_add_dictionary( ZMassConstraintDict
   ZMassConstraint/ZMassConstraintDict.h
   ZMassConstraint/selection.xml
   LINK_LIBRARIES ZMassConstraintLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
