# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AnalysisTriggerEventTPCnv )

# External dependencies:
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_tpcnv_library( AnalysisTriggerEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS AnalysisTriggerEventTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib AnalysisTriggerEvent
                         PRIVATE_LINK_LIBRARIES AthenaKernel GaudiKernel TrigT1Interfaces )

atlas_add_dictionary( AnalysisTriggerEventTPCnvDict
                      AnalysisTriggerEventTPCnv/AnalysisTriggerEventTPCnvDict.h
                      AnalysisTriggerEventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AnalysisTriggerEventTPCnv )
