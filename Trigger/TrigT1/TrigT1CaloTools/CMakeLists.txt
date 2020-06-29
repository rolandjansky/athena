# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1CaloTools )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigT1CaloTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AthenaBaseComps CaloIdentifier CaloTriggerToolLib CxxUtils EventInfo GaudiKernel PathResolver StoreGateLib TrigAnalysisInterfaces TrigBunchCrossingToolLib TrigConfInterfaces TrigConfL1Data TrigT1CaloCalibConditions TrigT1CaloCalibToolInterfaces TrigT1CaloCondSvcLib TrigT1CaloEventLib TrigT1CaloToolInterfaces TrigT1CaloUtilsLib TrigT1Interfaces xAODEventInfo xAODTrigL1Calo )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.root share/*.txt )

