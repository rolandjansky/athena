# $Id: CMakeLists.txt 796691 2017-02-11 10:15:48Z ludovica $

# The name of the package:
atlas_subdir( PhotonEfficiencyCorrection )

# Extra dependencies based on the build environment:
set( extra_deps )
if( XAOD_STANDALONE )
   set( extra_deps Control/xAODRootAccess )
else()
   set( extra_deps Control/AthenaBaseComps GaudiKernel )
endif()

# Dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODEgamma
   PhysicsAnalysis/AnalysisCommon/PATCore
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/ElectronPhotonID/ElectronEfficiencyCorrection
   PRIVATE
   Event/xAOD/xAODCore
   Event/xAOD/xAODEventInfo
   Tools/PathResolver
   ${extra_deps} )

# External(s) for the package:
find_package( Boost COMPONENTS filesystem )
find_package( ROOT COMPONENTS Core Hist RIO MathCore )

# Libraries in the package:
atlas_add_library( PhotonEfficiencyCorrectionLib
   PhotonEfficiencyCorrection/*.h Root/*.cxx
   PUBLIC_HEADERS PhotonEfficiencyCorrection
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AsgTools xAODEgamma
   PATCoreLib PATInterfaces ElectronEfficiencyCorrectionLib
   PRIVATE_LINK_LIBRARIES xAODEventInfo PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( PhotonEfficiencyCorrection
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AthenaBaseComps GaudiKernel xAODEventInfo xAODEgamma
      xAODCore PhotonEfficiencyCorrectionLib )
endif()

atlas_add_dictionary( PhotonEfficiencyCorrectionDict
   PhotonEfficiencyCorrection/PhotonEfficiencyCorrectionDict.h
   PhotonEfficiencyCorrection/selection.xml
   LINK_LIBRARIES PhotonEfficiencyCorrectionLib )

# Executable(s) in the package:
atlas_add_executable( PrintPhotonSF
   util/PrintPhotonSF.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES}
   PhotonEfficiencyCorrectionLib )

if( XAOD_STANDALONE )
   atlas_add_executable( TestxAODPhotonAlg
      util/TestxAODPhotonAlg.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEventInfo xAODEgamma
      xAODCore PathResolver PATInterfaces PhotonEfficiencyCorrectionLib )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
#atlas_install_data( data/*.root )
atlas_install_data( data/*.root data/*.txt )
