#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigHLTJetHypo/TrigHLTJetHypo2.h"
#include "TrigHLTJetHypo/TrigEFRazorAllTE.h"
#include "TrigHLTJetHypo/TrigEFDPhiMetJetAllTE.h"
// #include "TrigHLTJetHypo/TrigHLTJetHypo_HT.h"

#include "../TrigHLTJetHypo_EtaEtTool.h"
#include "../TrigHLTJetHypo_DijetMassDEtaTool.h"
#include "../TrigHLTJetHypo_SMCTool.h"
#include "../TrigHLTJetHypo_TLATool.h"
#include "../TrigJetHypo.h"

DECLARE_ALGORITHM_FACTORY(TrigHLTJetHypo2)
DECLARE_ALGORITHM_FACTORY(TrigJetHypo)

DECLARE_ALGORITHM_FACTORY(TrigEFRazorAllTE)
DECLARE_ALGORITHM_FACTORY(TrigEFDPhiMetJetAllTE)

DECLARE_FACTORY_ENTRIES(TrigHLTJetHypo) {
  DECLARE_ALGORITHM(TrigHLTJetHypo2)
    DECLARE_ALGORITHM(TrigJetHypo)
    // DECLARE_ALGORITHM(TrigHLTJetHypo_EtaEt)
    // DECLARE_ALGORITHM(TrigHLTJetHypo_HT)
    DECLARE_ALGORITHM(TrigEFRazorAllTE)
    DECLARE_ALGORITHM(TrigEFDPhiMetJetAllTE)
    DECLARE_TOOL(TrigHLTJetHypo_EtaEtTool)
    DECLARE_TOOL(TrigHLTJetHypo_DijetDEtaMassTool)
    DECLARE_TOOL(TrigHLTJetHypo_SMCTool)
    DECLARE_TOOL(TrigHLTJetHypo_TLATool)
}

DECLARE_TOOL_FACTORY(TrigHLTJetHypo_EtaEtTool)
DECLARE_TOOL_FACTORY(TrigHLTJetHypo_DijetMassDEtaTool)
DECLARE_TOOL_FACTORY(TrigHLTJetHypo_SMCTool)
DECLARE_TOOL_FACTORY(TrigHLTJetHypo_TLATool)

