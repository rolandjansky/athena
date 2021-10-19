# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetPrepRawDataToxAOD )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core MathCore )

# Component(s) in the package:
atlas_add_component( InDetPrepRawDataToxAOD
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib GaudiKernel AthenaBaseComps StoreGateLib Identifier PixelConditionsData xAODTracking TRT_ConditionsServicesLib InDetIdentifier InDetReadoutGeometry PixelReadoutGeometryLib TRT_ReadoutGeometry InDetCondTools InDetConditionsSummaryService InDetRawData InDetSimData InDetPrepRawData InDetSimEvent TrkSurfaces TrkTruthData TrkEventUtils TRT_DriftFunctionToolLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

