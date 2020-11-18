# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AnalysisTriggerEvent )

# Component(s) in the package:
atlas_add_library( AnalysisTriggerEvent
                   src/*.cxx
                   PUBLIC_HEADERS AnalysisTriggerEvent
                   LINK_LIBRARIES Navigation AthenaKernel EventKernel FourMom )

atlas_add_dictionary( AnalysisTriggerEventDict
                      AnalysisTriggerEvent/AnalysisTriggerEventDict.h
                      AnalysisTriggerEvent/selection.xml
                      LINK_LIBRARIES AnalysisTriggerEvent )
