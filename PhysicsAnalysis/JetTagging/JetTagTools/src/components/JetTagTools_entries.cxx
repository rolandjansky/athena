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

//DECLARE_COMPONENT( Analysis::LifetimeTag )
//DECLARE_COMPONENT( Analysis::SecVtxTag )
DECLARE_COMPONENT( Analysis::IPTag )
DECLARE_COMPONENT( Analysis::RNNIPTag )
DECLARE_COMPONENT( Analysis::SVTag )
DECLARE_COMPONENT( Analysis::MultiSVTag )
//DECLARE_COMPONENT( Analysis::JetProbTag )
//DECLARE_COMPONENT( Analysis::SoftElectronTag )
DECLARE_COMPONENT( Analysis::SoftMuonTag )
//DECLARE_COMPONENT( Analysis::CombinerTool )
//DECLARE_COMPONENT( Analysis::CombinerToolTag )
//DECLARE_COMPONENT( Analysis::LikelihoodTool )
//DECLARE_COMPONENT( Analysis::LikelihoodMultiDTool )
DECLARE_COMPONENT( Analysis::NewLikelihoodTool )
DECLARE_COMPONENT( Analysis::TrackSelector )
DECLARE_COMPONENT( Analysis::JetFitterTag )
DECLARE_COMPONENT( Analysis::JetFitterNtupleWriter )
// DECLARE_COMPONENT( Analysis::JetFitterLikelihood )
DECLARE_COMPONENT( Analysis::JetFitterVariablesFactory )
DECLARE_COMPONENT( Analysis::MSVVariablesFactory )
//DECLARE_COMPONENT( Analysis::JetFitterCharmVariablesFactory )
//DECLARE_COMPONENT( Analysis::GaiaVariablesFactory )
DECLARE_COMPONENT( Analysis::DL1Tag )
DECLARE_COMPONENT( Analysis::TagNtupleDumper )
DECLARE_COMPONENT( Analysis::MultivariateTagManager )
DECLARE_COMPONENT( Analysis::JetFitterNNTool )
DECLARE_COMPONENT( Analysis::JetFitterDummyClassifier )
DECLARE_COMPONENT( Analysis::JetFitterInputWriter )
//DECLARE_COMPONENT( Analysis::JetFitterCharmNNTool )
//DECLARE_COMPONENT( Analysis::GaiaNNTool );
DECLARE_COMPONENT( Analysis::SVForIPTool )
DECLARE_COMPONENT( Analysis::BasicTrackGradeFactory )
DECLARE_COMPONENT( Analysis::DetailedTrackGradeFactory )
//DECLARE_COMPONENT( Analysis::TrackCounting )
//DECLARE_COMPONENT( Analysis::GbbNNTag )
//DECLARE_COMPONENT( Analysis::HadFlavorTag )
DECLARE_COMPONENT( Analysis::MV1Tag )
DECLARE_COMPONENT( Analysis::MV2Tag )
DECLARE_COMPONENT( Analysis::MyTag )
//DECLARE_COMPONENT( Analysis::MV3Tag )



DECLARE_COMPONENT( Analysis::JetVertexCharge )

DECLARE_COMPONENT( Analysis::ExKtbbTag )
DECLARE_COMPONENT( Analysis::ExKtbbTagTool )

// Until we put it elsewhere
//DECLARE_COMPONENT( SharedHitMapper )

//DECLARE_COMPONENT( Analysis::EmulTag )
DECLARE_COMPONENT( Analysis::DummyTag )

