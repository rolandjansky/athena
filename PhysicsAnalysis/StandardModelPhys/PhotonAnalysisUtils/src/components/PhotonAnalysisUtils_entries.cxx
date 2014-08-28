#include "GaudiKernel/DeclareFactoryEntries.h"

#include "PhotonAnalysisUtils/DiphotonAnalysis.h"

#include "PhotonAnalysisUtils/PAUprimaryVertexFinder.h"
#include "PhotonAnalysisUtils/PAUhggFitter.h"
#include "PhotonAnalysisUtils/PAUcaloIsolationTool.h"
#include "PhotonAnalysisUtils/PAUconversionFlagTool.h"
#include "PhotonAnalysisUtils/PAUphotonCorrectionTool.h"
#include "PhotonAnalysisUtils/PAUphotonIdentificationTool.h"
#include "PhotonAnalysisUtils/PAUhggUserDataTool.h"
//#include "PhotonAnalysisUtils/PAUtruthTool.h"

#include "PhotonAnalysisUtils/PAUtruthMatchingTool.h"
#include "PhotonAnalysisUtils/PAUtrigRecoMatch.h"
//#include "PhotonAnalysisUtils/PAUtruthTrackClassifier.h"
#include "PhotonAnalysisUtils/PAUconvIPTool.h"
#include "PhotonAnalysisUtils/PAUconvGeoTool.h"

#include "PhotonAnalysisUtils/PAUtopoClusterTool.h"
#include "PhotonAnalysisUtils/PAU_ToT_tool.h"

#include "PhotonAnalysisUtils/ElectronToPhotonAlg.h"
#include "PhotonAnalysisUtils/ElectronToPhotonTool.h"

#include "PhotonAnalysisUtils/PAUprobeStoreGate.h"

#include "../PhotonNeuralNetworkTool.h"

#include "PhotonAnalysisUtils/DiphotonVertexID.h"

//FOR EGAMMA :
#include "PhotonAnalysisUtils/ExtrapolCaloConversion.h"

#include "PhotonAnalysisUtils/ComputePointingQuantities.h"
#include "PhotonAnalysisUtils/ComputeClusterQuantities.h"


DECLARE_ALGORITHM_FACTORY( DiphotonAnalysis )
DECLARE_ALGORITHM_FACTORY( PAUprobeStoreGate )
DECLARE_ALGORITHM_FACTORY( ElectronToPhotonAlg )

DECLARE_TOOL_FACTORY( PAUprimaryVertexFinder )
DECLARE_TOOL_FACTORY( PAUhggFitter )
DECLARE_TOOL_FACTORY( PAUcaloIsolationTool )
DECLARE_TOOL_FACTORY( PAUconversionFlagTool )
DECLARE_TOOL_FACTORY( PAUphotonCorrectionTool ) 
DECLARE_TOOL_FACTORY( PAUphotonIdentificationTool )
DECLARE_TOOL_FACTORY( PAUhggUserDataTool )
//DECLARE_TOOL_FACTORY( PAUtruthTool )

DECLARE_TOOL_FACTORY( PAUtruthMatchingTool )
DECLARE_TOOL_FACTORY( PAUtrigRecoMatch )
//DECLARE_TOOL_FACTORY( PAUtruthTrackClassifier )
DECLARE_TOOL_FACTORY( PAUconvIPTool )
DECLARE_TOOL_FACTORY( PAUconvGeoTool )
DECLARE_TOOL_FACTORY( PAUtopoClusterTool )
DECLARE_TOOL_FACTORY( PAU_ToT_tool )
DECLARE_TOOL_FACTORY( ElectronToPhotonTool )

DECLARE_TOOL_FACTORY( PhotonNeuralNetworkTool )
DECLARE_TOOL_FACTORY( DiphotonVertexID )

//for EGAMMA
DECLARE_TOOL_FACTORY( ExtrapolCaloConversion )

DECLARE_TOOL_FACTORY( ComputePointingQuantities )
DECLARE_TOOL_FACTORY( ComputeClusterQuantities )


DECLARE_FACTORY_ENTRIES( PhotonAnalysisUtils ) {
  DECLARE_ALGORITHM( DiphotonAnalysis )
  DECLARE_ALGORITHM( PAUprobeStoreGate )
  DECLARE_ALGORITHM( ElectronToPhotonAlg )
  DECLARE_TOOL( PAUprimaryVertexFinder )
  DECLARE_TOOL( PAUhggFitter )
  DECLARE_TOOL( PAUcaloIsolationTool )
  DECLARE_TOOL( PAUconversionFlagTool )
  DECLARE_TOOL( PAUphotonCorrectionTool ) 
  DECLARE_TOOL( PAUphotonIdentificationTool )
  DECLARE_TOOL( PAUhggUserDataTool )
  //DECLARE_TOOL( PAUtruthTool )
  DECLARE_TOOL( PhotonNeuralNetworkTool )
  DECLARE_TOOL( DiphotonVertexID )

  DECLARE_TOOL( PAUtruthMatchingTool )
  DECLARE_TOOL( PAUtrigRecoMatch )
  //DECLARE_TOOL( PAUtruthTrackClassifier )
  DECLARE_TOOL( PAUconvIPTool )
  DECLARE_TOOL( PAUconvGeoTool )
  DECLARE_TOOL( PAUtopoClusterTool )
  DECLARE_TOOL( PAU_ToT_tool )
  DECLARE_TOOL( ElectronToPhotonTool )
  //FOR EGAMMA :
  DECLARE_TOOL( ExtrapolCaloConversion )

  DECLARE_TOOL( ComputePointingQuantities )
  DECLARE_TOOL( ComputeClusterQuantities )

}
