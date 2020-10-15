# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ZdcRec )

# External dependencies:
find_package( GSL )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ZdcRecLib
                   src/*.cxx
                   PUBLIC_HEADERS ZdcRec
                   INCLUDE_DIRS ${GSL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GSL_LIBRARIES} AsgTools AthenaBaseComps xAODForward xAODTrigL1Calo ZdcEvent GaudiKernel StoreGateLib ZdcAnalysisLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ZdcByteStreamLib ZdcConditions ZdcIdentifier )

atlas_add_component( ZdcRec
                     src/components/*.cxx
                     LINK_LIBRARIES ZdcRecLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )
