# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HIGlobal )

# Component(s) in the package:
atlas_add_library( HIGlobalLib
                   HIGlobal/*.h
                   INTERFACE
                   PUBLIC_HEADERS HIGlobal
                   LINK_LIBRARIES AsgTools AthenaBaseComps CaloEvent GaudiKernel HIEventUtilsLib NavFourMom StoreGateLib xAODCaloEvent xAODHIEvent )

atlas_add_component( HIGlobal
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloEvent GaudiKernel HIEventUtilsLib HIGlobalLib Navigation StoreGateLib xAODHIEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

atlas_add_test( HIRec_test
   SCRIPT python -m HIRecConfig.HIRecConfig
   PROPERTIES TIMEOUT 600
   POST_EXEC_SCRIPT nopost.sh )
