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

//#include "JetTagTools/SharedHitMapper.h"
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

