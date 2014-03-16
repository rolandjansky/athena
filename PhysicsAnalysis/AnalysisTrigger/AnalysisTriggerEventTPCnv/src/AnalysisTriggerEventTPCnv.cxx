/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

// Persistent representation of the LVL1 RoIs (version 1):
#include "AnalysisTriggerEventTPCnv/LVL1_ROI_p1.h"
#include "AnalysisTriggerEventTPCnv/LVL1_ROICnv_p1.h"
#include "AnalysisTriggerEventTPCnv/Muon_ROI_p1.h"
#include "AnalysisTriggerEventTPCnv/Jet_ROI_p1.h"
#include "AnalysisTriggerEventTPCnv/JetET_ROI_p1.h"
#include "AnalysisTriggerEventTPCnv/EmTau_ROI_p1.h"
#include "AnalysisTriggerEventTPCnv/EnergySum_ROI_p1.h"

// Persistent representation of CTP_Decision (version 1):
#include "AnalysisTriggerEventTPCnv/CTP_Decision_p1.h"
#include "AnalysisTriggerEventTPCnv/CTP_DecisionCnv_p1.h"
// Persistent representation of CTP_Decision (version 2):
#include "AnalysisTriggerEventTPCnv/CTP_Decision_p2.h"
#include "AnalysisTriggerEventTPCnv/CTP_DecisionCnv_p2.h"

DECLARE_TPCNV_FACTORY(CTP_DecisionCnv_p1,
                      CTP_Decision,
                      CTP_Decision_p1,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(CTP_DecisionCnv_p2,
                      CTP_Decision,
                      CTP_Decision_p2,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(LVL1_ROICnv_p1,
                      LVL1_ROI,
                      LVL1_ROI_p1,
                      Athena::TPCnvVers::Current)
                      
