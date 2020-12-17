# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( FastTRT_Digitization )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( FastTRT_Digitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel PileUpToolsLib StoreGateLib Identifier EventInfo xAODEventInfo GaudiKernel TRT_ConditionsServicesLib InDetPrepRawData InDetSimEvent HitManagement TrkParameters TrkTruthData TrkToolInterfaces GeoPrimitives GeneratorObjects InDetIdentifier TRT_ReadoutGeometry InDetSimData TrkDetElementBase TRT_DriftFunctionToolLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

