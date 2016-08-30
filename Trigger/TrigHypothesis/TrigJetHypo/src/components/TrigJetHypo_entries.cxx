#include "TrigJetHypo/TrigL2JetHypo.h"
#include "TrigJetHypo/TrigL2JetHypoNoise.h"
#include "TrigJetHypo/TrigEFJetHypo.h"
//#include "TrigJetHypo/TrigEFJetHypo_RF.h"
#include "TrigJetHypo/TrigEFJetHypoNoise.h"
#include "TrigJetHypo/TrigL2DEtaFex.h"
#include "TrigJetHypo/TrigEFDEtaFex.h"
#include "TrigJetHypo/TrigL2DEtaHypo.h"
#include "TrigJetHypo/TrigEFDEtaHypo.h"
#include "TrigJetHypo/TrigL2DijetCombo.h"
#include "TrigJetHypo/TrigL2JetDphiHypo.h"
#include "TrigJetHypo/TrigL2JetMctHypo.h"
#include "TrigJetHypo/TrigL2MultiJetHypo.h"
#include "TrigJetHypo/TrigL2MultiJetAllTE.h"
#include "TrigJetHypo/TrigL2HTFullScanHypo.h"
#include "TrigJetHypo/TrigL2HTAllTE.h"
#include "TrigJetHypo/TrigL2DijetAllTE.h"
#include "TrigJetHypo/TrigEFDPhiMetJetAllTE.h"
#include "TrigJetHypo/TrigEFRazorAllTE.h"
#include "TrigJetHypo/TrigEFJetMctHypo.h"
#include "TrigJetHypo/TrigEFJetDphiHypo.h"
#include "TrigJetHypo/TrigEFHTHypo.h"
#include "TrigJetHypo/TrigEFJetMassY.h"
#include "TrigJetHypo/TrigEFJetMassDEta.h"
#include "TrigJetHypo/TrigL2DEtaMjjAllTE.h"
#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_ALGORITHM_FACTORY( TrigEFJetHypo )
//DECLARE_ALGORITHM_FACTORY( TrigEFJetHypo_RF )
DECLARE_ALGORITHM_FACTORY( TrigEFJetHypoNoise )
DECLARE_ALGORITHM_FACTORY( TrigL2JetHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2JetHypoNoise )
DECLARE_ALGORITHM_FACTORY( TrigL2DEtaFex )
DECLARE_ALGORITHM_FACTORY( TrigEFDEtaFex )
DECLARE_ALGORITHM_FACTORY( TrigL2DEtaHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFDEtaHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2DijetCombo )
DECLARE_ALGORITHM_FACTORY( TrigL2JetDphiHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2JetMctHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2MultiJetHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2MultiJetAllTE )
DECLARE_ALGORITHM_FACTORY( TrigL2HTFullScanHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2HTAllTE )
DECLARE_ALGORITHM_FACTORY( TrigL2DijetAllTE )
DECLARE_ALGORITHM_FACTORY( TrigEFDPhiMetJetAllTE )
DECLARE_ALGORITHM_FACTORY( TrigEFRazorAllTE )
DECLARE_ALGORITHM_FACTORY( TrigEFJetMctHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFJetDphiHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFHTHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFJetMassY )
DECLARE_ALGORITHM_FACTORY( TrigEFJetMassDEta )
DECLARE_ALGORITHM_FACTORY( TrigL2DEtaMjjAllTE )

DECLARE_FACTORY_ENTRIES( TrigJetHypo ) {
  DECLARE_ALGORITHM( TrigEFJetHypo )
//  DECLARE_ALGORITHM( TrigEFJetHypo_RF)
  DECLARE_ALGORITHM( TrigEFJetHypoNoise )
  DECLARE_ALGORITHM( TrigL2JetHypo )
  DECLARE_ALGORITHM( TrigL2JetHypoNoise )
  DECLARE_ALGORITHM( TrigL2DEtaFex )
  DECLARE_ALGORITHM( TrigEFDEtaFex )
  DECLARE_ALGORITHM( TrigL2DEtaHypo )
  DECLARE_ALGORITHM( TrigEFDEtaHypo )
  DECLARE_ALGORITHM( TrigL2DijetCombo )
  DECLARE_ALGORITHM( TrigL2JetDphiHypo )
  DECLARE_ALGORITHM( TrigL2JetMctHypo )
  DECLARE_ALGORITHM( TrigL2MultiJetHypo )
  DECLARE_ALGORITHM( TrigL2MultiJetAllTE )
  DECLARE_ALGORITHM( TrigL2HTFullScanHypo )
  DECLARE_ALGORITHM( TrigL2HTAllTE )
  DECLARE_ALGORITHM( TrigL2DijetAllTE )
  DECLARE_ALGORITHM( TrigEFDPhiMetJetAllTE )
  DECLARE_ALGORITHM( TrigEFRazorAllTE )
  DECLARE_ALGORITHM( TrigEFJetMctHypo )
  DECLARE_ALGORITHM( TrigEFJetDphiHypo )
  DECLARE_ALGORITHM( TrigEFHTHypo )
  DECLARE_ALGORITHM( TrigEFJetMassY )
  DECLARE_ALGORITHM( TrigEFJetMassDEta )
  DECLARE_ALGORITHM( TrigL2DEtaMjjAllTE )
}

