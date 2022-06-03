# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DetDescrCondTools )

# External dependencies:
find_package( ROOT COMPONENTS Hist Tree RIO Core MathCore pthread )

atlas_add_library( DetDescrCondToolsLib
                   INTERFACE
                   PUBLIC_HEADERS DetDescrCondTools
                   LINK_LIBRARIES CxxUtils GaudiKernel )

# Component(s) in the package:
atlas_add_component( DetDescrCondTools
                     src/CoolStrFileSvc.cxx
                     src/CoolHistSvc.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps CxxUtils StoreGateLib GaudiKernel FileCatalog AthenaPoolUtilities DetDescrCondToolsLib PoolSvcLib )

# Install files from the package:
atlas_install_scripts( share/coolHist_setFileIdentifier.sh share/coolHist_extractFileIdentifier.sh share/coolHist_setReference.py share/coolHist_insertFileToCatalog.py share/coolStrFileSvc_set.py )

