# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SCT_ConditionsData )

atlas_add_dictionary( SCT_ConditionsDataDict
                      SCT_ConditionsData/SCT_ConditionsDataDict.h
                      SCT_ConditionsData/selection.xml
                      LINK_LIBRARIES SCT_ConditionsData )

# Component(s) in the package:
atlas_add_library( SCT_ConditionsData
                   src/*.cxx
                   PUBLIC_HEADERS SCT_ConditionsData
                   LINK_LIBRARIES AthenaKernel AthenaPoolUtilities Identifier
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps )
