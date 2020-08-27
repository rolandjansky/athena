# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthAnalysisBaseComps )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( AthAnalysisBaseCompsLib
                   src/*.cxx
                   PUBLIC_HEADERS AthAnalysisBaseComps
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps IOVDbDataModel GaudiKernel StoreGateLib SGtests AthContainers
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_component( AthAnalysisBaseComps
                     src/components/*.cxx
                     LINK_LIBRARIES AthAnalysisBaseCompsLib )

atlas_add_dictionary( AthAnalysisBaseCompsDict
                      AthAnalysisBaseComps/AthAnalysisBaseCompsDict.h
                      AthAnalysisBaseComps/selection.xml
                      LINK_LIBRARIES AthAnalysisBaseCompsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

