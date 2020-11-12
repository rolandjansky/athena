# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AnalysisTriggerEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( AnalysisTriggerEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES AnalysisTriggerEvent/CTP_Decision.h AnalysisTriggerEvent/LVL1_ROI.h
                           LINK_LIBRARIES AthenaPoolUtilities GaudiKernel AthenaPoolCnvSvcLib AnalysisTriggerEvent AnalysisTriggerEventTPCnv )
