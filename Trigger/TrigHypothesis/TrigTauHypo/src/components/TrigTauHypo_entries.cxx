#include "TrigTauHypo/T2CaloTauHypo.h"
#include "TrigTauHypo/T2IDTauHypo.h"
#include "TrigTauHypo/T2IDCoreTauHypo.h"
#include "TrigTauHypo/T2IDIsoTauHypo.h"
#include "TrigTauHypo/T2TauHypo.h"
#include "TrigTauHypo/EFTauInvHypo.h"
#include "TrigTauHypo/EFTauMVHypo.h"
#include "TrigTauHypo/EFTauDiKaonHypo.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigTauHypo/EFHadCalibHypo.h"
#include "TrigTauHypo/T2TauTauCombHypo.h"
#include "TrigTauHypo/T2TauTauCombFexAlgo.h"
#include "TrigTauHypo/EFTauTauCombHypo.h"
#include "TrigTauHypo/EFTauTauCombFexAlgo.h"
#include "TrigTauHypo/HLTCaloPreSelHypo.h"
#include "TrigTauHypo/HLTTrackPreSelHypo.h"
#include "TrigTauHypo/HLTTauGenericHypo.h"
#include "TrigTauHypo/HLTTauTrackRoiUpdater.h"
#include "TrigTauHypo/HLTTauCaloRoiUpdater.h"
#include "TrigTauHypo/HLTVertexPreSelHypo.h"
#include "TrigTauHypo/HLTVertexCombo.h"
#include "TrigTauHypo/L2TauTopoFex.h"
#include "TrigTauHypo/L2TauTopoHypo.h"
#include "TrigTauHypo/EFTauTopoFex.h"
#include "TrigTauHypo/EFTauTopoHypo.h"
#include "TrigTauHypo/EFPhotonTauFex.h"
#include "TrigTauHypo/EFPhotonTauHypo.h"


DECLARE_ALGORITHM_FACTORY( T2CaloTauHypo )
DECLARE_ALGORITHM_FACTORY( T2IDTauHypo )
DECLARE_ALGORITHM_FACTORY( T2IDCoreTauHypo )
DECLARE_ALGORITHM_FACTORY( T2IDIsoTauHypo )
DECLARE_ALGORITHM_FACTORY( T2TauHypo )
DECLARE_ALGORITHM_FACTORY( EFTauInvHypo )
DECLARE_ALGORITHM_FACTORY( EFTauMVHypo )
DECLARE_ALGORITHM_FACTORY( EFTauDiKaonHypo )
DECLARE_ALGORITHM_FACTORY( EFHadCalibHypo )
DECLARE_ALGORITHM_FACTORY( T2TauTauCombHypo )
DECLARE_ALGORITHM_FACTORY( T2TauTauCombFexAlgo )
DECLARE_ALGORITHM_FACTORY( EFTauTauCombHypo )
DECLARE_ALGORITHM_FACTORY( EFTauTauCombFexAlgo )
DECLARE_ALGORITHM_FACTORY( HLTCaloPreSelHypo )
DECLARE_ALGORITHM_FACTORY( HLTTrackPreSelHypo )
DECLARE_ALGORITHM_FACTORY( HLTTauGenericHypo )
DECLARE_ALGORITHM_FACTORY( HLTTauTrackRoiUpdater )
DECLARE_ALGORITHM_FACTORY( HLTTauCaloRoiUpdater )
DECLARE_ALGORITHM_FACTORY( HLTVertexPreSelHypo )
DECLARE_ALGORITHM_FACTORY( HLTVertexCombo )
DECLARE_ALGORITHM_FACTORY( L2TauTopoFex )
DECLARE_ALGORITHM_FACTORY( L2TauTopoHypo )
DECLARE_ALGORITHM_FACTORY( EFTauTopoFex )
DECLARE_ALGORITHM_FACTORY( EFTauTopoHypo )
DECLARE_ALGORITHM_FACTORY( EFPhotonTauFex )
DECLARE_ALGORITHM_FACTORY( EFPhotonTauHypo )


DECLARE_FACTORY_ENTRIES( TrigTauHypo ) {
  DECLARE_ALGORITHM( T2CaloTauHypo );
  DECLARE_ALGORITHM( T2IDTauHypo );
  DECLARE_ALGORITHM( T2IDCoreTauHypo );
  DECLARE_ALGORITHM( T2IDIsoTauHypo );
  DECLARE_ALGORITHM( T2TauHypo );
  DECLARE_ALGORITHM( EFTauInvHypo );
  DECLARE_ALGORITHM( EFTauMVHypo );
  DECLARE_ALGORITHM( EFTauDiKaonHypo );
  DECLARE_ALGORITHM( EFHadCalibHypo );      
  DECLARE_ALGORITHM( T2TauTauCombHypo );      
  DECLARE_ALGORITHM( T2TauTauCombFexAlgo );      
  DECLARE_ALGORITHM( EFTauTauCombHypo );
  DECLARE_ALGORITHM( EFTauTauCombFexAlgo );
  DECLARE_ALGORITHM( HLTCaloPreSelHypo );
  DECLARE_ALGORITHM( HLTTrackPreSelHypo );
  DECLARE_ALGORITHM( HLTTauGenericHypo );
  DECLARE_ALGORITHM( HLTTauTrackRoiUpdater );
  DECLARE_ALGORITHM( HLTTauCaloRoiUpdater );
  DECLARE_ALGORITHM( HLTVertexPreSelHypo );
  DECLARE_ALGORITHM( HLTVertexCombo );
  DECLARE_ALGORITHM( L2TauTopoFex );
  DECLARE_ALGORITHM( L2TauTopoHypo );
  DECLARE_ALGORITHM( EFTauTopoFex );
  DECLARE_ALGORITHM( EFTauTopoHypo );
  DECLARE_ALGORITHM( EFPhotonTauFex );
  DECLARE_ALGORITHM( EFPhotonTauHypo );

}
