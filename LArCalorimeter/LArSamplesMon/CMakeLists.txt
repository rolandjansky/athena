# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArSamplesMon )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO MathMore Minuit Gpad )

# Component(s) in the package:
atlas_add_library( LArSamplesMonLib
                   src/*.cxx
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PUBLIC_HEADERS LArSamplesMon
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaMonitoringKernelLib AthenaMonitoringLib CaloIdentifier GaudiKernel LArCablingLib LArCafJobsLib LArRawEvent StoreGateLib
                   PRIVATE_LINK_LIBRARIES Identifier LArIdentifier LArRawConditions )

atlas_add_component( LArSamplesMon
                     src/components/*.cxx
                     LINK_LIBRARIES LArSamplesMonLib )

atlas_add_dictionary( LArSamplesMonDict
                      LArSamplesMon/LArSamplesMonDict.h
                      LArSamplesMon/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} LArCafJobsLib LArSamplesMonLib )

atlas_add_executable( LCE_CellList
                      src/LCE_CellList.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} LArCafJobsLib LArRawEvent LArSamplesMonLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/LCE_postprocessing.py )

