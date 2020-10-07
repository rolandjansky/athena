# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigLongLivedParticlesHypo )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat )

# Component(s) in the package:
atlas_add_component( TrigLongLivedParticlesHypo
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} CaloEvent CxxUtils GaudiKernel IRegionSelector RoiDescriptor InDetIdentifier InDetPrepRawData TrigCaloEvent TrigInDetEvent TrigInterfacesLib TrigParticle TrigSteeringEvent TrigTimeAlgsLib TrkSpacePoint xAODEventInfo xAODJet xAODTracking xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )
