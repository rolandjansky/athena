# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_CalibAlgs )

# External dependencies:
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_component( TRT_CalibAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils StoreGateLib xAODEventInfo GaudiKernel TrkToolInterfaces CommissionEvent AthContainers Identifier xAODTracking TRT_ConditionsData TRT_ConditionsServicesLib InDetIdentifier InDetRawData InDetPrepRawData InDetRIO_OnTrack TrkParameters TrkTrack VxVertex TrkFitterInterfaces TRT_TrackHoleSearchLib TRT_ElectronPidToolsLib TRT_CalibData TRT_CalibToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
