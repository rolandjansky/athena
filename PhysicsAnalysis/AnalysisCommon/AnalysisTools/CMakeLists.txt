# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AnalysisTools )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase )
find_package( Python COMPONENTS Development )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( AthAnalysisToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS AnalysisTools
                   INCLUDE_DIRS ${Python_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Python_LIBRARIES} ${ROOT_LIBRARIES} AnalysisUtilsLib AthContainers AthenaBaseComps GaudiKernel McParticleEvent NavFourMom
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaPoolUtilities EventInfo PersistentDataModel RootCollection RootUtils SGTools StoreGateLib xAODEventInfo )

atlas_add_component( AthAnalysisTools
                     src/components/*.cxx
                     LINK_LIBRARIES AthAnalysisToolsLib )

atlas_add_dictionary( AthAnalysisToolsDict
                      AnalysisTools/AnalysisToolsDict.h
                      AnalysisTools/selection.xml
                      LINK_LIBRARIES AthAnalysisToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

