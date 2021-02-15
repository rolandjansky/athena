# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_Digitization )

# External dependencies:
find_package( CLHEP )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( TRT_Digitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel AthenaBaseComps AthenaKernel CxxUtils PileUpToolsLib AthenaPoolUtilities GeoModelInterfaces GeoModelUtilities GeoPrimitives Identifier GeneratorObjects TRT_ConditionsData TRT_ConditionsServicesLib InDetIdentifier TRT_ReadoutGeometry InDetRawData InDetSimData InDetSimEvent HitManagement MagFieldElements MagFieldConditions TRT_PAI_ProcessLib StoreGateLib xAODEventInfo )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

