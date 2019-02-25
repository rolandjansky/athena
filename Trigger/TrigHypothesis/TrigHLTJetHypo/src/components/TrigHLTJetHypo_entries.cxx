#include "GaudiKernel/DeclareFactoryEntries.h"

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
#include "TrigHLTJetHypo/TrigHLTJetHypo_JetAttrs.h"

DECLARE_ALGORITHM_FACTORY(TrigHLTJetHypo2)

DECLARE_ALGORITHM_FACTORY(TrigEFRazorAllTE)
DECLARE_ALGORITHM_FACTORY(TrigEFDPhiMetJetAllTE)
DECLARE_ALGORITHM_FACTORY(TrigHLTJetHypo_Dijet)
DECLARE_ALGORITHM_FACTORY(TrigHLTJetHypo_DijetMassDEta)
DECLARE_ALGORITHM_FACTORY(TrigHLTJetHypo_DijetMassDEtaDPhi)
DECLARE_ALGORITHM_FACTORY(TrigHLTJetHypo_EtaEt)
DECLARE_ALGORITHM_FACTORY(TrigHLTJetHypo_SMC)
DECLARE_ALGORITHM_FACTORY(TrigHLTJetHypo_HT)
DECLARE_ALGORITHM_FACTORY(TrigHLTJetHypo_TLA)
DECLARE_ALGORITHM_FACTORY(TrigHLTJetHypo_JetAttrs)

DECLARE_FACTORY_ENTRIES(TrigHLTJetHypo) {
    DECLARE_ALGORITHM(TrigHLTJetHypo2)
    DECLARE_ALGORITHM(TrigEFRazorAllTE)
    DECLARE_ALGORITHM(TrigEFDPhiMetJetAllTE)

    DECLARE_ALGORITHM(TrigHLTJetHypo_Dijet)
    DECLARE_ALGORITHM(TrigHLTJetHypo_DijetMassDEta)
    DECLARE_ALGORITHM(TrigHLTJetHypo_DijetMassDEtaDPhi)
    DECLARE_ALGORITHM(TrigHLTJetHypo_EtaEt)
    DECLARE_ALGORITHM(TrigHLTJetHypo_SMC)
    DECLARE_ALGORITHM(TrigHLTJetHypo_HT)
    DECLARE_ALGORITHM(TrigHLTJetHypo_TLA)
    DECLARE_ALGORITHM(TrigHLTJetHypo_JetAttrs)
}

