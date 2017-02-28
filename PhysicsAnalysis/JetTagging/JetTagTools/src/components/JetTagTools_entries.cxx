#include "GaudiKernel/DeclareFactoryEntries.h"

#include "JetTagTools/IPTag.h"
#include "JetTagTools/RNNIPTag.h"
#include "JetTagTools/SVTag.h"
#include "JetTagTools/MultiSVTag.h"

#include "JetTagTools/SoftMuonTag.h"

#include "JetTagTools/NewLikelihoodTool.h"
#include "JetTagTools/TrackSelector.h"
#include "JetTagTools/JetFitterTag.h"

#include "JetTagTools/JetFitterNtupleWriter.h"
#include "JetTagTools/JetFitterVariablesFactory.h"
#include "JetTagTools/MSVVariablesFactory.h"

#include "JetTagTools/MultivariateTagManager.h"
#include "JetTagTools/DL1Tag.h"
#include "JetTagTools/TagNtupleDumper.h"
#include "JetTagTools/JetFitterNNTool.h"
#include "JetTagTools/JetFitterDummyClassifier.h"
#include "JetTagTools/JetFitterInputWriter.h"

#include "JetTagTools/SVForIPTool.h"
#include "JetTagTools/BasicTrackGradeFactory.h"
#include "JetTagTools/DetailedTrackGradeFactory.h"

#include "JetTagTools/MV1Tag.h"
#include "JetTagTools/MV2Tag.h"
#include "JetTagTools/MyTag.h"

#include "JetTagTools/JetVertexCharge.h"

#include "JetTagTools/ExKtbbTag.h"
#include "JetTagTools/ExKtbbTagTool.h"

//
//#include "JetTagTools/SharedHitMapper.h"
//
//#include "JetTagTools/EmulTag.h"
#include "JetTagTools/DummyTag.h"

//DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, LifetimeTag )
//DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, SecVtxTag )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, IPTag )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, RNNIPTag )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, SVTag )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, MultiSVTag )
//DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, JetProbTag )
//DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, SoftElectronTag )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, SoftMuonTag )
//DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, CombinerTool )
//DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, CombinerToolTag )
//DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, LikelihoodTool )
//DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, LikelihoodMultiDTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, NewLikelihoodTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, TrackSelector )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, JetFitterTag )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, JetFitterNtupleWriter )
// DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, JetFitterLikelihood )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, JetFitterVariablesFactory )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, MSVVariablesFactory )
//DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, JetFitterCharmVariablesFactory )
//DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, GaiaVariablesFactory )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, DL1Tag )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, TagNtupleDumper )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, MultivariateTagManager )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, JetFitterNNTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, JetFitterDummyClassifier )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, JetFitterInputWriter )
//DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, JetFitterCharmNNTool )
//DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, GaiaNNTool );
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, SVForIPTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, BasicTrackGradeFactory )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, DetailedTrackGradeFactory )
//DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, TrackCounting )
//DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, GbbNNTag )
//DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, HadFlavorTag )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, MV1Tag )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, MV2Tag )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, MyTag )
//DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, MV3Tag )



DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, JetVertexCharge )

DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, ExKtbbTag )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, ExKtbbTagTool )

// Until we put it elsewhere
//DECLARE_ALGORITHM_FACTORY( SharedHitMapper )

//DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, EmulTag )
DECLARE_NAMESPACE_TOOL_FACTORY( Analysis, DummyTag )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( JetTagTools )
{
  // DECLARE_NAMESPACE_TOOL( Analysis, LifetimeTag );
  // DECLARE_NAMESPACE_TOOL( Analysis, SecVtxTag );
  DECLARE_NAMESPACE_TOOL( Analysis, IPTag );
  DECLARE_NAMESPACE_TOOL( Analysis, SVTag );
  DECLARE_NAMESPACE_TOOL( Analysis, MultiSVTag );
  // DECLARE_NAMESPACE_TOOL( Analysis, JetProbTag );
  // DECLARE_NAMESPACE_TOOL( Analysis, SoftElectronTag );
  DECLARE_NAMESPACE_TOOL( Analysis, SoftMuonTag );
  // DECLARE_NAMESPACE_TOOL( Analysis, CombinerTool );
  // DECLARE_NAMESPACE_TOOL( Analysis, LikelihoodTool );
  // DECLARE_NAMESPACE_TOOL( Analysis, CombinerToolTag );
  // DECLARE_NAMESPACE_TOOL( Analysis, LikelihoodMultiDTool );
  DECLARE_NAMESPACE_TOOL( Analysis, NewLikelihoodTool );
  DECLARE_NAMESPACE_TOOL( Analysis, TrackSelector );
  DECLARE_NAMESPACE_TOOL( Analysis, JetFitterTag );
  DECLARE_NAMESPACE_TOOL( Analysis, JetFitterNtupleWriter );
  // DECLARE_NAMESPACE_TOOL( Analysis, JetFitterLikelihood );
  DECLARE_NAMESPACE_TOOL( Analysis, JetFitterVariablesFactory );
  DECLARE_NAMESPACE_TOOL( Analysis, MSVVariablesFactory );
  // DECLARE_NAMESPACE_TOOL( Analysis, JetFitterCharmVariablesFactory );
  // DECLARE_NAMESPACE_TOOL( Analysis, GaiaVariablesFactory );
  DECLARE_NAMESPACE_TOOL( Analysis, JetFitterNNTool );
  // DECLARE_NAMESPACE_TOOL( Analysis, JetFitterCharmNNTool );
  //DECLARE_NAMESPACE_TOOL( Analysis, GaiaNNTool );
  DECLARE_NAMESPACE_TOOL( Analysis, DL1Tag );
  DECLARE_NAMESPACE_TOOL( Analysis, TagNtupleDumper );
  DECLARE_NAMESPACE_TOOL( Analysis, MultivariateTagManager );
  DECLARE_NAMESPACE_TOOL( Analysis, SVForIPTool );
  DECLARE_NAMESPACE_TOOL( Analysis, BasicTrackGradeFactory );
  DECLARE_NAMESPACE_TOOL( Analysis, DetailedTrackGradeFactory );
  // DECLARE_NAMESPACE_TOOL( Analysis, TrackCounting );
  // DECLARE_NAMESPACE_TOOL( Analysis, GbbNNTag );
  // DECLARE_NAMESPACE_TOOL( Analysis, HadFlavorTag )
  DECLARE_NAMESPACE_TOOL( Analysis, MV1Tag );
  DECLARE_NAMESPACE_TOOL( Analysis, MV2Tag );
  DECLARE_NAMESPACE_TOOL( Analysis, MyTag );
  // DECLARE_NAMESPACE_TOOL( Analysis, MV3Tag );
  DECLARE_NAMESPACE_TOOL( Analysis, JetVertexCharge );
  DECLARE_NAMESPACE_TOOL( Analysis, ExKtbbTag );
  DECLARE_NAMESPACE_TOOL( Analysis, ExKtbbTagTool );
  // //
  // DECLARE_ALGORITHM( SharedHitMapper );
  // //
  // DECLARE_NAMESPACE_TOOL( Analysis, EmulTag );
  DECLARE_NAMESPACE_TOOL( Analysis, DummyTag );
}
