# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigIDtrkMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist )

# Component(s) in the package:
atlas_add_component( TrigIDtrkMonitoring
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib CaloEvent CaloGeoHelpers EventInfo GaudiKernel Particle RecoToolInterfaces StoreGateLib TrigCaloEvent TrigDecisionToolLib TrigHLTMonitoringLib TrigInDetEvent TrigInDetToolInterfacesLib TrigParticle TrigSteeringEvent egammaEvent egammaInterfacesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

