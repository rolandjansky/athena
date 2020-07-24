# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMultiVarHypo )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree )

# Component(s) in the package:
atlas_add_library( TrigMultiVarHypoLib
                   src/*.cxx
                   src/tools/*.cxx
                   src/tools/procedures/*.cxx
                   src/tools/common/*.cxx
                   PUBLIC_HEADERS TrigMultiVarHypo
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools AthenaMonitoringKernelLib GaudiKernel LumiBlockCompsLib PathResolver TrigCaloEvent TrigInterfacesLib TrigTimeAlgsLib xAODTrigCalo xAODTrigRinger )

atlas_add_component( TrigMultiVarHypo
                     src/components/*.cxx
                     LINK_LIBRARIES TrigMultiVarHypoLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )
