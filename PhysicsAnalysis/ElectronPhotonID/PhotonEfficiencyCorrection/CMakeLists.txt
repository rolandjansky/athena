# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# The name of the package:
atlas_subdir( PhotonEfficiencyCorrection )

# External(s) for the package:
find_package( Boost )
find_package( ROOT COMPONENTS Core )

# Libraries in the package:
atlas_add_library( PhotonEfficiencyCorrectionLib
   PhotonEfficiencyCorrection/*.h Root/*.cxx
   PUBLIC_HEADERS PhotonEfficiencyCorrection
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} EgammaAnalysisInterfacesLib AsgTools xAODEgamma
   PATCoreAcceptLib PATInterfaces ElectronEfficiencyCorrectionLib
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} xAODEventInfo PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( PhotonEfficiencyCorrection
      src/*.h src/*.cxx src/components/*.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${Boost_LIBRARIES} AsgTools AthenaBaseComps EgammaAnalysisInterfacesLib
      ElectronEfficiencyCorrectionLib GaudiKernel PATCoreAcceptLib PATInterfaces
      xAODCore xAODEgamma xAODEventInfo PhotonEfficiencyCorrectionLib )
endif()

atlas_add_dictionary( PhotonEfficiencyCorrectionDict
   PhotonEfficiencyCorrection/PhotonEfficiencyCorrectionDict.h
   PhotonEfficiencyCorrection/selection.xml
   LINK_LIBRARIES PhotonEfficiencyCorrectionLib )

# Executable(s) in the package:
atlas_add_executable( PrintPhotonSF
   util/PrintPhotonSF.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ElectronEfficiencyCorrectionLib PATCoreAcceptLib
   PhotonEfficiencyCorrectionLib )

if( XAOD_STANDALONE )
   atlas_add_executable( TestxAODPhotonAlg
      util/TestxAODPhotonAlg.cxx
      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AsgTools EgammaAnalysisInterfacesLib
      ElectronEfficiencyCorrectionLib PATCoreAcceptLib PATInterfaces PathResolver
      xAODCore xAODEgamma xAODEventInfo PhotonEfficiencyCorrectionLib )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_data( data/*.root data/*.txt )
