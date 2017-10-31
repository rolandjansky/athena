#include "../JetTagBTagWeightsFillerTool.h"
//#include "../JetTagJetTrackAssociationTool.h"
#include "../JetTagJetNavigable4MomentumAssociationTool.h"
#include "../JetTagMuonInJetGetterTool.h"
#include "../JetTagInfoBaseFillerTool.h"
#include "../JetTagIPInfoBaseFillerTool.h"
#include "../JetTagJetProbInfoBaseFillerTool.h"
#include "../JetTagSVInfoBaseFillerTool.h"
#include "../JetTagIPInfoPlusTrackAssociationTool.h"
#include "../JetTagSVInfoPlusFillerTool.h"
#include "../JetTagSVInfoPlusTrackAssociationTool.h"
#include "../JetTagJetMuonAssociationTool.h"
#include "../JetTagJetFitterTagInfoFillerTool.h"
#include "../JetTagJetFitterGenericTagInfoFillerTool.h"
#include "../JetTagSoftMuonInfoMuonAssociationTool.h"
#include "../JetTagJetFilterAlg.h"
#include "../JetTagJetGenSoftLeptonAssociationTool.h"
#include "../JetTagElectronInJetGetterTool.h"
#include "../JetTagJetElectronAssociationTool.h"
#include "../JetTagSoftElecInfoegammaAssociationTool.h"
#include "../JetTagPhotonInJetGetterTool.h"
#include "../JetTagJetPhotonAssociationTool.h"
#include "../JetTagVxOnJetAxisFillerTool.h"
//#include "../JetTagJetJFTwoTrackVertexAssociationTool.h"
#include "../JetTagTwoTrackVertexFillerTool.h"
#include "../JetTagVxOnJetAxisTrackAssociationTool.h"
#include "../JetTagTwoTrackVertexTrackAssociationTool.h"
#include "../JetTagVKalVxInJetBadTrackAssociationTool.h"
#include "../JetTagTruthInfoFillerTool.h"
#include "../JetTagMSVVtxInfoGetterTool.h"
#include "../JetTagMultiSVInfoPlusFillerTool.h"
#include "../JetTagMSVVtxInfoFillerTool.h"
#include "../JetTagMSVVtxInfoTrackAssociationTool.h"
#include "../JetTagMultiSVInfoMSVVtxInfoAssociationTool.h"
#include "../JetTagGbbNNInfoFillerTool.h"
#include "../JetTagHadFlavorTagInfoFillerTool.h"
#include "../JetTagPixelModuleGetterTool.h"
#include "../JetTagPixelModCondFillerTool.h"
#include "../JetTagQGPartonTruthInfoFillerTool.h"


DECLARE_COMPONENT( D3PD::JetTagBTagWeightsFillerTool )
//DECLARE_COMPONENT( D3PD::JetTagJetTrackAssociationTool )
DECLARE_COMPONENT( D3PD::JetTagJetNavigable4MomentumAssociationTool )
DECLARE_COMPONENT( D3PD::JetTagMuonInJetGetterTool )
DECLARE_COMPONENT( D3PD::JetTagInfoBaseFillerTool )
DECLARE_COMPONENT( D3PD::JetTagIPInfoBaseFillerTool )
DECLARE_COMPONENT( D3PD::JetTagJetProbInfoBaseFillerTool )
DECLARE_COMPONENT( D3PD::JetTagSVInfoBaseFillerTool )
DECLARE_COMPONENT( D3PD::JetTagIPInfoPlusTrackAssociationTool )
DECLARE_COMPONENT( D3PD::JetTagSVInfoPlusFillerTool )
DECLARE_COMPONENT( D3PD::JetTagSVInfoPlusTrackAssociationTool )
DECLARE_COMPONENT( D3PD::JetTagJetMuonAssociationTool )
DECLARE_COMPONENT( D3PD::JetTagJetFitterTagInfoFillerTool )
DECLARE_COMPONENT( D3PD::JetTagJetFitterGenericTagInfoFillerTool )
DECLARE_COMPONENT( D3PD::JetTagSoftMuonInfoMuonAssociationTool )
DECLARE_COMPONENT( D3PD::JetTagJetGenSoftLeptonAssociationTool )
DECLARE_COMPONENT( D3PD::JetTagElectronInJetGetterTool )
DECLARE_COMPONENT( D3PD::JetTagJetElectronAssociationTool )
DECLARE_COMPONENT( D3PD::JetTagSoftElecInfoegammaAssociationTool )
DECLARE_COMPONENT( D3PD::JetTagPhotonInJetGetterTool )
DECLARE_COMPONENT( D3PD::JetTagJetPhotonAssociationTool )
DECLARE_COMPONENT( D3PD::JetTagVxOnJetAxisFillerTool )
//DECLARE_COMPONENT( D3PD::JetTagJetJFTwoTrackVertexAssociationTool )
DECLARE_COMPONENT( D3PD::JetTagTwoTrackVertexFillerTool )
DECLARE_COMPONENT( D3PD::JetTagVxOnJetAxisTrackAssociationTool )
DECLARE_COMPONENT( D3PD::JetTagTwoTrackVertexTrackAssociationTool )
DECLARE_COMPONENT( D3PD::JetTagVKalVxInJetBadTrackAssociationTool )
DECLARE_COMPONENT( D3PD::JetTagTruthInfoFillerTool )
DECLARE_COMPONENT( D3PD::JetTagGbbNNInfoFillerTool )
DECLARE_COMPONENT( D3PD::JetTagHadFlavorTagInfoFillerTool )

DECLARE_COMPONENT( D3PD::JetTagPixelModuleGetterTool )
DECLARE_COMPONENT( D3PD::JetTagPixelModCondFillerTool )

DECLARE_COMPONENT( D3PD::JetTagMSVVtxInfoGetterTool )
DECLARE_COMPONENT( D3PD::JetTagMultiSVInfoPlusFillerTool )
DECLARE_COMPONENT( D3PD::JetTagMSVVtxInfoFillerTool )
DECLARE_COMPONENT( D3PD::JetTagMSVVtxInfoTrackAssociationTool )
DECLARE_COMPONENT( D3PD::JetTagMultiSVInfoMSVVtxInfoAssociationTool )
DECLARE_COMPONENT( D3PD::JetTagQGPartonTruthInfoFillerTool )

DECLARE_COMPONENT( D3PD::JetTagJetFilterAlg ) 

