# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_ConditionsData )

# Component(s) in the package:
atlas_add_library( TRT_ConditionsData
                   src/*.cxx
                   PUBLIC_HEADERS TRT_ConditionsData
                   LINK_LIBRARIES AthenaKernel AthenaPoolUtilities Identifier GaudiKernel TrkEventPrimitives )

atlas_add_dictionary( TRT_ConditionsDataDict
                      TRT_ConditionsData/TRT_ConditionsDataDict.h
                      TRT_ConditionsData/selection.xml
                      LINK_LIBRARIES TRT_ConditionsData )
