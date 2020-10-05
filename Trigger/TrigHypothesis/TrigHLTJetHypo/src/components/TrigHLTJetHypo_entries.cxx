/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypo2.h"
#include "TrigHLTJetHypo/TrigEFRazorAllTE.h"
#include "TrigHLTJetHypo/TrigEFDPhiMetJetAllTE.h"
#include "TrigHLTJetHypo/TrigHLTJetHypo_Dijet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypo_DijetMassDEta.h"
#include "TrigHLTJetHypo/TrigHLTJetHypo_DijetMassDEtaDPhi.h"
#include "TrigHLTJetHypo/TrigHLTJetHypo_EtaEt.h"
#include "TrigHLTJetHypo/TrigHLTJetHypo_HT.h"
#include "TrigHLTJetHypo/TrigHLTJetHypo_TLA.h"
#include "TrigHLTJetHypo/TrigHLTJetHypo_SMC.h"
#include "../TrigJetHypoAlgMT.h"
#include "../TrigJetHypoToolConfig_simple.h"
#include "../TrigJetHypoToolConfig_simple_partition.h"
#include "../TrigJetHypoToolConfig_dijet.h"
#include "../TrigJetHypoToolConfig_combgen.h"
#include "../TrigJetHypoToolConfig_partgen.h"
#include "../TrigJetHypoToolConfig_fastreduction.h"
#include "../TrigJetHypoToolConfig_leaf.h"
#include "../TrigJetHypoToolConfig_ht.h"
//
#include "../TrigJetConditionConfig_abs_eta.h"
#include "../TrigJetConditionConfig_signed_eta.h"
#include "../TrigJetConditionConfig_et.h"
#include "../TrigJetConditionConfig_ht.h"
#include "../TrigJetConditionConfig_dijet_mass.h"
#include "../TrigJetConditionConfig_dijet_dphi.h"
#include "../TrigJetConditionConfig_dijet_deta.h"
#include "../TrigJetConditionConfig_qjet_mass.h"
#include "../TrigJetConditionConfig_smc.h"
#include "../TrigJetConditionConfig_jvt.h"
#include "../TrigJetConditionConfig_acceptAll.h"
#include "../TrigJetConditionConfig_moment.h"
#include "../TrigJetConditionConfig_compound.h"

//
#include "../NotHelperTool.h"
#include "../AndHelperTool.h"
#include "../OrHelperTool.h"
#include "../TrigJetHypoToolMT.h"
#include "../TrigJetHypoToolHelperMT.h"
#include "../TrigJetTLAHypoAlgMT.h"
#include "../TrigJetTLAHypoToolMT.h"
#include "../CombinationsHelperTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/BasicCleanerTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/AntiCleanerTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/EtaEtCleanerTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/LlpCleanerTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/LooseCleanerTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/NullCleanerTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/TightCleanerTool.h"


DECLARE_COMPONENT(TrigHLTJetHypo2)

DECLARE_COMPONENT(TrigEFRazorAllTE)
DECLARE_COMPONENT(TrigEFDPhiMetJetAllTE)
DECLARE_COMPONENT(TrigHLTJetHypo_Dijet)
DECLARE_COMPONENT(TrigHLTJetHypo_DijetMassDEta)
DECLARE_COMPONENT(TrigHLTJetHypo_DijetMassDEtaDPhi)
DECLARE_COMPONENT(TrigJetHypoToolConfig_simple)
DECLARE_COMPONENT(TrigJetHypoToolConfig_simple_partition)
DECLARE_COMPONENT(TrigJetHypoToolConfig_dijet)
DECLARE_COMPONENT(TrigJetHypoToolConfig_combgen)
DECLARE_COMPONENT(TrigJetHypoToolConfig_partgen)
DECLARE_COMPONENT(TrigJetHypoToolConfig_fastreduction)
DECLARE_COMPONENT(TrigJetHypoToolConfig_leaf)
DECLARE_COMPONENT(TrigJetHypoToolConfig_ht)
DECLARE_COMPONENT(NotHelperTool)
DECLARE_COMPONENT(AndHelperTool)
 DECLARE_COMPONENT(OrHelperTool)
DECLARE_COMPONENT(TrigHLTJetHypo_SMC)
DECLARE_COMPONENT(TrigHLTJetHypo_HT)
DECLARE_COMPONENT(TrigHLTJetHypo_TLA)
DECLARE_COMPONENT(TrigHLTJetHypo_EtaEt)


DECLARE_COMPONENT(TrigJetConditionConfig_signed_eta)
DECLARE_COMPONENT(TrigJetConditionConfig_abs_eta)
DECLARE_COMPONENT(TrigJetConditionConfig_et)
DECLARE_COMPONENT(TrigJetConditionConfig_ht)
DECLARE_COMPONENT(TrigJetConditionConfig_dijet_mass)
DECLARE_COMPONENT(TrigJetConditionConfig_dijet_deta)
DECLARE_COMPONENT(TrigJetConditionConfig_dijet_dphi)
DECLARE_COMPONENT(TrigJetConditionConfig_smc)
DECLARE_COMPONENT(TrigJetConditionConfig_jvt)
DECLARE_COMPONENT(TrigJetConditionConfig_acceptAll)
DECLARE_COMPONENT(TrigJetConditionConfig_moment)
DECLARE_COMPONENT(TrigJetConditionConfig_compound)
DECLARE_COMPONENT(TrigJetConditionConfig_qjet_mass)

DECLARE_COMPONENT(TrigJetHypoAlgMT)
DECLARE_COMPONENT(TrigJetHypoToolMT)
DECLARE_COMPONENT(TrigJetTLAHypoAlgMT)
DECLARE_COMPONENT(TrigJetTLAHypoToolMT)
DECLARE_COMPONENT(TrigJetHypoToolHelperMT)
DECLARE_COMPONENT(CombinationsHelperTool)

DECLARE_COMPONENT(BasicCleanerTool)
DECLARE_COMPONENT(AntiCleanerTool)
DECLARE_COMPONENT(EtaEtCleanerTool)
DECLARE_COMPONENT(LlpCleanerTool)
DECLARE_COMPONENT(LooseCleanerTool)
DECLARE_COMPONENT(NullCleanerTool)
DECLARE_COMPONENT(TightCleanerTool)

