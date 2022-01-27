/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "../TrigJetHypoAlg.h"
#include "../TrigJetHypoToolConfig_fastreduction.h"
#include "../TrigJetHypoToolConfig_conditionfilter.h"
#include "../TrigJetHypoToolConfig_passthroughfilter.h"
#include "../TrigJetHypoToolConfig_rangefilter.h"
//
#include "../TrigJetConditionConfig_abs_eta.h"
#include "../TrigJetConditionConfig_signed_eta.h"
#include "../TrigJetConditionConfig_phi.h"
#include "../TrigJetConditionConfig_et.h"
#include "../TrigJetConditionConfig_pt.h"
#include "../TrigJetConditionConfig_htfr.h"
#include "../TrigJetConditionConfig_dijet_mass.h"
#include "../TrigJetConditionConfig_dijet_dphi.h"
#include "../TrigJetConditionConfig_dijet_deta.h"
#include "../TrigJetConditionConfig_qjet_mass.h"
#include "../TrigJetConditionConfig_smc.h"
#include "../TrigJetConditionConfig_jvt.h"
#include "../TrigJetConditionConfig_clean.h"
#include "../TrigJetConditionConfig_acceptAll.h"
#include "../TrigJetConditionConfig_moment.h"
#include "../TrigJetConditionConfig_repeated.h"

//
#include "../TrigJetHypoTool.h"
#include "../TrigJetHypoToolHelperNoGrouper.h"
#include "../TrigJetTLAHypoAlg.h"
#include "../TrigJetTLAHypoTool.h"
#include "../TrigBtagTLATool.h"
#include "../TrigJetEJsHypoAlg.h"
#include "../TrigJetEJsHypoTool.h"

DECLARE_COMPONENT(TrigJetHypoToolConfig_fastreduction)
DECLARE_COMPONENT(TrigJetHypoToolConfig_conditionfilter)
DECLARE_COMPONENT(TrigJetHypoToolConfig_passthroughfilter)
DECLARE_COMPONENT(TrigJetHypoToolConfig_rangefilter)
DECLARE_COMPONENT(TrigJetConditionConfig_phi)
DECLARE_COMPONENT(TrigJetConditionConfig_signed_eta)
DECLARE_COMPONENT(TrigJetConditionConfig_abs_eta)
DECLARE_COMPONENT(TrigJetConditionConfig_et)
DECLARE_COMPONENT(TrigJetConditionConfig_pt)
DECLARE_COMPONENT(TrigJetConditionConfig_htfr)
DECLARE_COMPONENT(TrigJetConditionConfig_dijet_mass)
DECLARE_COMPONENT(TrigJetConditionConfig_dijet_deta)
DECLARE_COMPONENT(TrigJetConditionConfig_dijet_dphi)
DECLARE_COMPONENT(TrigJetConditionConfig_smc)
DECLARE_COMPONENT(TrigJetConditionConfig_jvt)
DECLARE_COMPONENT(TrigJetConditionConfig_clean)
DECLARE_COMPONENT(TrigJetConditionConfig_acceptAll)
DECLARE_COMPONENT(TrigJetConditionConfig_moment)
DECLARE_COMPONENT(TrigJetConditionConfig_repeated)
DECLARE_COMPONENT(TrigJetConditionConfig_qjet_mass)

DECLARE_COMPONENT(TrigJetHypoAlg)
DECLARE_COMPONENT(TrigJetHypoTool)
DECLARE_COMPONENT(TrigJetTLAHypoAlg)
DECLARE_COMPONENT(TrigJetTLAHypoTool)
DECLARE_COMPONENT(TrigBtagTLATool)
DECLARE_COMPONENT(TrigJetHypoToolHelperNoGrouper)

DECLARE_COMPONENT(TrigJetEJsHypoAlg)
DECLARE_COMPONENT(TrigJetEJsHypoTool)
