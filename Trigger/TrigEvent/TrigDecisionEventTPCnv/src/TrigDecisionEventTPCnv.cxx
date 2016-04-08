/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "TrigDecisionEvent/TrigDecision.h"
#include "TrigDecisionEventTPCnv/TrigDecision_p1.h"
#include "TrigDecisionEventTPCnv/TrigDecision_p2.h"
#include "TrigDecisionEventTPCnv/TrigDecision_p3.h"
#include "TrigDecisionEventTPCnv/TrigDecision_p4.h"
#include "TrigDecisionEventTPCnv/TrigDecision_p5.h"
#include "TrigDecisionEventTPCnv/TrigDecisionCnv_p1.h"
#include "TrigDecisionEventTPCnv/TrigDecisionCnv_p2.h"
#include "TrigDecisionEventTPCnv/TrigDecisionCnv_p3.h"
#include "TrigDecisionEventTPCnv/TrigDecisionCnv_p4.h"
#include "TrigDecisionEventTPCnv/TrigDecisionCnv_p5.h"

using namespace TrigDec;

DECLARE_TPCNV_FACTORY(TrigDecisionCnv_p1,
                      TrigDec::TrigDecision,
                      TrigDec::TrigDecision_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigDecisionCnv_p2,
                      TrigDec::TrigDecision,
                      TrigDec::TrigDecision_p2,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigDecisionCnv_p3,
                      TrigDec::TrigDecision,
                      TrigDec::TrigDecision_p3,
                      Athena::TPCnvVers::Old)
                      
DECLARE_TPCNV_FACTORY(TrigDecisionCnv_p4,
                      TrigDec::TrigDecision,
                      TrigDec::TrigDecision_p4,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrigDecisionCnv_p5,
                      TrigDec::TrigDecision,
                      TrigDec::TrigDecision_p5,
                      Athena::TPCnvVers::Current)
                      
