# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloCondBlobAlgs )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( CaloCondBlobAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} CaloCondBlobObjs CaloDetDescrLib CaloIdentifier AthenaBaseComps AthenaKernel StoreGateLib AthenaPoolUtilities Identifier xAODEventInfo GaudiKernel )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )
