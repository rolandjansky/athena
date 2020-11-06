# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( tauRec )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_component( tauRec
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps CaloEvent CaloInterfaceLib CaloUtilsLib GaudiKernel InDetReadoutGeometry NavFourMom StoreGateLib TRT_ReadoutGeometry tauRecToolsLib xAODCaloEvent xAODCore xAODJet xAODPFlow xAODParticleEvent xAODTau xAODTracking )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
