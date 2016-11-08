#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigHLTJetHypo/TrigHLTJetHypo2.h"
#include "TrigHLTJetHypo/TrigEFRazorAllTE.h"
#include "TrigHLTJetHypo/TrigEFDPhiMetJetAllTE.h"

DECLARE_ALGORITHM_FACTORY(TrigHLTJetHypo2)

DECLARE_ALGORITHM_FACTORY(TrigEFRazorAllTE)
DECLARE_ALGORITHM_FACTORY(TrigEFDPhiMetJetAllTE)

DECLARE_FACTORY_ENTRIES(TrigHLTJetHypo) {
  DECLARE_ALGORITHM(TrigHLTJetHypo2)
    DECLARE_ALGORITHM(TrigEFRazorAllTE)
    DECLARE_ALGORITHM(TrigEFDPhiMetJetAllTE)
}

