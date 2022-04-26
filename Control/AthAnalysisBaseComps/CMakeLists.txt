# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthAnalysisBaseComps )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core RIO )

# Component(s) in the package:
atlas_add_library( AthAnalysisBaseCompsLib
                   src/*.cxx
                   PUBLIC_HEADERS AthAnalysisBaseComps
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps IOVDbDataModel GaudiKernel StoreGateLib AthContainers
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} CxxUtils )

atlas_add_component( AthAnalysisBaseComps
                     src/components/*.cxx
                     LINK_LIBRARIES AthAnalysisBaseCompsLib )

atlas_add_dictionary( AthAnalysisBaseCompsDict
                      AthAnalysisBaseComps/AthAnalysisBaseCompsDict.h
                      AthAnalysisBaseComps/selection.xml
                      LINK_LIBRARIES AthAnalysisBaseCompsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

