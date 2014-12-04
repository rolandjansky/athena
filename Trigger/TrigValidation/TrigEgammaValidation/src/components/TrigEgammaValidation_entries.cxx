#include "TrigEgammaValidation/ZeeValidation.h"
#include "TrigEgammaValidation/JpsiValidation.h"
#include "TrigEgammaValidation/ZeeOfflineValidation.h"
#include "TrigEgammaValidation/SingleElectronValidation.h"
#include "TrigEgammaValidation/SingleElectronOfflineValidation.h"
#include "TrigEgammaValidation/JetValidation.h"
#include "TrigEgammaValidation/HggValidation.h"
#include "TrigEgammaValidation/NavOfflineValidation.h"
#include "TrigEgammaValidation/NavSEValidation.h"
#include "TrigEgammaValidation/NavRingerValidation.h"
#include "TrigEgammaValidation/NavZeeValidation.h"
#include "TrigEgammaValidation/NavZeeTPxAODValidation.h"
#include "TrigEgammaValidation/NavRateValidation.h"
#include "TrigEgammaValidation/NavRateExpert.h"
#include "TrigEgammaValidation/NavZeeTPCounts.h"
#include "TrigEgammaValidation/NavZeeTPEff.h"
#include "TrigEgammaValidation/NavZeeTPRes.h"
#include "TrigEgammaValidation/NavZeeTPIneff.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( ZeeValidation )
DECLARE_ALGORITHM_FACTORY( JpsiValidation )
DECLARE_ALGORITHM_FACTORY( ZeeOfflineValidation )
DECLARE_ALGORITHM_FACTORY( SingleElectronValidation )
DECLARE_ALGORITHM_FACTORY( SingleElectronOfflineValidation )
DECLARE_ALGORITHM_FACTORY( JetValidation )
DECLARE_ALGORITHM_FACTORY( HggValidation )
DECLARE_ALGORITHM_FACTORY( NavOfflineValidation)
DECLARE_ALGORITHM_FACTORY( NavSEValidation)
DECLARE_ALGORITHM_FACTORY( NavRingerValidation)
DECLARE_ALGORITHM_FACTORY( NavZeeValidation)
DECLARE_ALGORITHM_FACTORY( NavZeeTPxAODValidation )
DECLARE_ALGORITHM_FACTORY( NavRateValidation)
DECLARE_ALGORITHM_FACTORY( NavRateExpert)
DECLARE_ALGORITHM_FACTORY( NavZeeTPCounts)
DECLARE_ALGORITHM_FACTORY( NavZeeTPEff)
DECLARE_ALGORITHM_FACTORY( NavZeeTPRes)
DECLARE_ALGORITHM_FACTORY( NavZeeTPIneff)

DECLARE_FACTORY_ENTRIES( TrigEgammaValidation ) {
  DECLARE_ALGORITHM( ZeeValidation )
  DECLARE_ALGORITHM( JpsiValidation )
  DECLARE_ALGORITHM( ZeeOfflineValidation )
  DECLARE_ALGORITHM( SingleElectronValidation )
  DECLARE_ALGORITHM( SingleElectronOfflineValidation )
  DECLARE_ALGORITHM( JetValidation )
  DECLARE_ALGORITHM( HggValidation )
  DECLARE_ALGORITHM( NavOfflineValidation)
  DECLARE_ALGORITHM( NavSEValidation)
  DECLARE_ALGORITHM( NavRingerValidation)
  DECLARE_ALGORITHM( NavZeeValidation)
  DECLARE_ALGORITHM( NavZeeTPxAODValidation )
  DECLARE_ALGORITHM( NavRateValidation)
  DECLARE_ALGORITHM( NavRateExpert)
  DECLARE_ALGORITHM( NavZeeTPCounts)
  DECLARE_ALGORITHM( NavZeeTPEff)
  DECLARE_ALGORITHM( NavZeeTPRes)
  DECLARE_ALGORITHM( NavZeeTPIneff)
}

