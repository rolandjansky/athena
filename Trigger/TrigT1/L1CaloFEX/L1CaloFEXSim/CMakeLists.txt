# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( L1CaloFEXSim )

# External dependencies:
find_package( Boost )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Hist Tree gPad )

# Component(s) in the package:
atlas_add_library( L1CaloFEXSimLib
                   L1CaloFEXSim/*.h src/*.cxx
                   PUBLIC_HEADERS L1CaloFEXSim
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks AthenaBaseComps AthenaKernel CaloEvent CaloIdentifier CxxUtils GaudiKernel Identifier L1CaloFEXToolInterfaces StoreGateLib xAODBase xAODCore xAODTrigL1Calo xAODTruth
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} SGTools xAODJet )

atlas_add_component( L1CaloFEXSim
                     src/components/*.cxx
                     LINK_LIBRARIES L1CaloFEXSimLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
