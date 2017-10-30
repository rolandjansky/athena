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


DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagBTagWeightsFillerTool)
//DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagJetTrackAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagJetNavigable4MomentumAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagMuonInJetGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagInfoBaseFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagIPInfoBaseFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagJetProbInfoBaseFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagSVInfoBaseFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagIPInfoPlusTrackAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagSVInfoPlusFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagSVInfoPlusTrackAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagJetMuonAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagJetFitterTagInfoFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagJetFitterGenericTagInfoFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagSoftMuonInfoMuonAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagJetGenSoftLeptonAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagElectronInJetGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagJetElectronAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagSoftElecInfoegammaAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagPhotonInJetGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagJetPhotonAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagVxOnJetAxisFillerTool)
//DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagJetJFTwoTrackVertexAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagTwoTrackVertexFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagVxOnJetAxisTrackAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagTwoTrackVertexTrackAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagVKalVxInJetBadTrackAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagTruthInfoFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagGbbNNInfoFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagHadFlavorTagInfoFillerTool)

DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagPixelModuleGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagPixelModCondFillerTool)

DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagMSVVtxInfoGetterTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagMultiSVInfoPlusFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagMSVVtxInfoFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagMSVVtxInfoTrackAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagMultiSVInfoMSVVtxInfoAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, JetTagQGPartonTruthInfoFillerTool)

DECLARE_NAMESPACE_ALGORITHM_FACTORY (D3PD, JetTagJetFilterAlg) 

