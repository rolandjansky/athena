#include "tauRec/TauBuilder.h"
#include "tauRec/JetSeedBuilder.h"
#include "tauRec/LockTauContainers.h"
#include "tauRec/TauAxisSetter.h"
#include "tauRec/TauCalibrateEM.h"
#include "tauRec/TauCalibrateLC.h"
#include "tauRec/TauCellVariables.h"
//#include "tauRec/TauOriginCorrectionTool.h"
#include "tauRec/TauProcessor.h"
#include "tauRec/TauTrackFinder.h"
#include "tauRec/TauVertexFinder.h"
#include "tauRec/TauElectronVetoVariables.h"
//#include "tauRec/TauPi0EflowCreateROI.h"
#include "tauRec/TauCommonCalcVars.h"
//#include "tauRec/TauEflowAddCaloInfo.h"
//#include "tauRec/TauEflowTrackMatchCells.h"
//#include "tauRec/TauEflowVariables.h"
#include "tauRec/TauShotFinder.h"
#include "tauRec/TauPi0BonnClusterCreator.h"
#include "tauRec/TauPi0BonnCreateROI.h"
#include "tauRec/TauPi0BonnScoreCalculator.h"
#include "tauRec/TauPi0BonnSelector.h"
//#include "tauRec/TauPi0CrakowClusterCreator.h"
//#include "tauRec/TauPi0CreatorChooser.h"
#include "tauRec/TauSubstructureVariables.h"
#include "tauRec/TauConversionFinder.h"
#include "tauRec/PhotonConversionPID.h"
#include "tauRec/PhotonConversionVertex.h"
#include "tauRec/TauConversionTagger.h"
#include "tauRec/TauVertexVariables.h"
#include "tauRec/tauCalibrateWeightTool.h"  //for trigger
#include "tauRec/TauTrackSlimmer.h"
#include "tauRec/TauTrackFilter.h"
#include "tauRec/TauGenericPi0Cone.h"
#include "tauRec/TauTestDump.h"



#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TauBuilder   )
DECLARE_ALGORITHM_FACTORY( TauProcessor )
DECLARE_TOOL_FACTORY( JetSeedBuilder             )
DECLARE_TOOL_FACTORY( LockTauContainers          )
DECLARE_TOOL_FACTORY( TauAxisSetter         )
DECLARE_TOOL_FACTORY( TauCalibrateEM             )
DECLARE_TOOL_FACTORY( TauCalibrateLC             )
DECLARE_TOOL_FACTORY( TauCellVariables           )
//DECLARE_TOOL_FACTORY( TauOriginCorrectionTool    )
DECLARE_TOOL_FACTORY( TauTrackFinder             )
DECLARE_TOOL_FACTORY( TauVertexFinder            )
DECLARE_TOOL_FACTORY( TauElectronVetoVariables             )
//DECLARE_TOOL_FACTORY( TauPi0EflowCreateROI      )
DECLARE_TOOL_FACTORY( TauCommonCalcVars          )
//DECLARE_TOOL_FACTORY( TauEflowAddCaloInfo        )
//DECLARE_TOOL_FACTORY( TauEflowTrackMatchCells    )
//DECLARE_TOOL_FACTORY( TauEflowVariables          )
DECLARE_TOOL_FACTORY( TauShotFinder              )
DECLARE_TOOL_FACTORY( TauPi0BonnClusterCreator   )
DECLARE_TOOL_FACTORY( TauPi0BonnCreateROI        )
DECLARE_TOOL_FACTORY( TauPi0BonnScoreCalculator  )
DECLARE_TOOL_FACTORY( TauPi0BonnSelector        )
//DECLARE_TOOL_FACTORY( TauPi0CrakowClusterCreator )
//DECLARE_TOOL_FACTORY( TauPi0CreatorChooser       )
DECLARE_TOOL_FACTORY( TauSubstructureVariables     )
DECLARE_TOOL_FACTORY( PhotonConversionPID )
DECLARE_TOOL_FACTORY( PhotonConversionVertex )
DECLARE_TOOL_FACTORY( TauConversionFinder )
DECLARE_TOOL_FACTORY( TauConversionTagger )
DECLARE_TOOL_FACTORY( TauVertexVariables )
DECLARE_TOOL_FACTORY( tauCalibrateWeightTool )
DECLARE_TOOL_FACTORY( TauTrackFilter )
DECLARE_TOOL_FACTORY( TauGenericPi0Cone )
DECLARE_TOOL_FACTORY( TauTestDump )
DECLARE_ALGORITHM_FACTORY( TauTrackSlimmer )


DECLARE_FACTORY_ENTRIES(tauRec) {
    DECLARE_ALGORITHM(TauBuilder)
    DECLARE_ALGORITHM(TauProcessor)
    DECLARE_TOOL(JetSeedBuilder)
    DECLARE_TOOL(LockTauContainers)
    DECLARE_TOOL(TauAxisSetter)
    DECLARE_TOOL(TauCalibrateEM)
    DECLARE_TOOL(TauCalibrateLC)
    DECLARE_TOOL(TauCellVariables)
    //DECLARE_TOOL(TauOriginCorrectionTool)
    DECLARE_TOOL(TauTrackFinder)
    DECLARE_TOOL(TauVertexFinder)
    DECLARE_TOOL( TauElectronVetoVariables )
    //DECLARE_TOOL( TauPi0EflowCreateROI      )
    DECLARE_TOOL( TauCommonCalcVars          )
    //DECLARE_TOOL( TauEflowAddCaloInfo        )
    //DECLARE_TOOL( TauEflowTrackMatchCells    )
    //DECLARE_TOOL( TauEflowVariables          )
    DECLARE_TOOL( TauShotFinder              )
    DECLARE_TOOL( TauPi0BonnClusterCreator   )
    DECLARE_TOOL( TauPi0BonnCreateROI        )
    DECLARE_TOOL( TauPi0BonnScoreCalculator  )
    DECLARE_TOOL( TauPi0BonnSelector         )
    //DECLARE_TOOL( TauPi0CrakowClusterCreator )
    //DECLARE_TOOL( TauPi0CreatorChooser       )
    DECLARE_TOOL( TauSubstructureVariables     )
    DECLARE_TOOL( PhotonConversionPID )
    DECLARE_TOOL( PhotonConversionVertex )
    DECLARE_TOOL( TauConversionFinder )
    DECLARE_TOOL( TauConversionTagger )
    DECLARE_TOOL( TauVertexVariables  )   
    DECLARE_TOOL( tauCalibrateWeightTool )         
      DECLARE_TOOL( TauTestDump )
    DECLARE_ALGORITHM( TauTrackSlimmer )
/*
      DECLARE_ALGORITHM( TauTrackSlimmer )
      DECLARE_ALGORITHM( TauAODDetailsCleaner )
         */
}
