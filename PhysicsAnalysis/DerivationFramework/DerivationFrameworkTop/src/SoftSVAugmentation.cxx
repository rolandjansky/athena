#include "DerivationFrameworkTop/SoftSVAugmentation.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"


namespace DerivationFramework {

SoftSVAugmentation::SoftSVAugmentation(const std::string& t, const std::string& n, const IInterface* p):
  AthAlgTool(t,n,p),
  secvtx_lxy("lxy"),
  secvtx_lxysig("lxysig"),
  secvtx_lz("lz"),
  secvtx_lzsig("lzsig"),
  secvtx_l3d("l3d"),
  secvtx_l3dsig("l3dsig")
{

  declareInterface<DerivationFramework::IAugmentationTool>(this);

  declareProperty("EventInfoName",m_eventInfoName="EventInfo", "event info name");
  declareProperty("SoftSVCollectionName_tc_lvt_Loose",m_SoftSVCollectionName_tc_lvt_Loose="SoftBVrtClusterTool_Loose_Vertices", "name of the tc-lvt soft secondary vertex collection at the Loose WP");
  declareProperty("SoftSVCollectionName_tc_lvt_Medium",m_SoftSVCollectionName_tc_lvt_Medium="SoftBVrtClusterTool_Medium_Vertices", "name of the tc-lvt soft secondary vertex collection at the Medium WP");
  declareProperty("SoftSVCollectionName_tc_lvt_Tight",m_SoftSVCollectionName_tc_lvt_Tight="SoftBVrtClusterTool_Tight_Vertices", "name of the tc-lvt soft secondary vertex collection at the Tight WP");
  declareProperty("SoftSVCollectionName_t_lvt",m_SoftSVCollectionName_t_lvt="VrtSecInclusive_SoftBtagCandidateVertices", "name of the t-lvt soft secondary vertex collection");
  declareProperty("PrimaryVerticesCollectionName",m_primaryVerticesCollectionName="PrimaryVertices", "name of the primary vertex collection");

  declareProperty("isMC",m_isMC=false, "Data/MC flag");

}



StatusCode SoftSVAugmentation::initialize(){

  ATH_MSG_INFO("Initializing SoftSVAugmentation tool... " );

  return StatusCode::SUCCESS;

}



StatusCode SoftSVAugmentation::addBranches() const{

  //// retrieve objects ////

  // event info
  const xAOD::EventInfo* eventInfo;
  ATH_CHECK( evtStore()->retrieve(eventInfo, m_eventInfoName) );

  // primary vertex
  const xAOD::VertexContainer *mprimvtx;
  ATH_CHECK( evtStore()->retrieve(mprimvtx, m_primaryVerticesCollectionName) );

  const xAOD::Vertex* primVtx = nullptr;
  for(const auto vtx : *mprimvtx) {
    if (vtx->vertexType() != xAOD::VxType::VertexType::PriVtx) continue;
    primVtx = vtx;
  }
  
  // SV collections
  const xAOD::VertexContainer* tc_lvt_Loose;
  ATH_CHECK( evtStore()->retrieve(tc_lvt_Loose, m_SoftSVCollectionName_tc_lvt_Loose) );

  const xAOD::VertexContainer* tc_lvt_Medium;
  ATH_CHECK( evtStore()->retrieve(tc_lvt_Medium, m_SoftSVCollectionName_tc_lvt_Medium) );

  const xAOD::VertexContainer* tc_lvt_Tight;
  ATH_CHECK( evtStore()->retrieve(tc_lvt_Tight, m_SoftSVCollectionName_tc_lvt_Tight) );

  const xAOD::VertexContainer* t_lvt;
  ATH_CHECK( evtStore()->retrieve(t_lvt, m_SoftSVCollectionName_t_lvt) );

  std::vector<const xAOD::VertexContainer*> SVCollections = {tc_lvt_Loose, tc_lvt_Medium, tc_lvt_Tight, t_lvt};

  //// calculate decay lenghts ////

  // loop over the SV collections
  for (const auto collection : SVCollections) {

    // loop over the SVs
    for(xAOD::Vertex *secVtx: *collection) { 

      // calculate the decay length variables
      float Lx = secVtx->position().x()-primVtx->position().x();
      float Ly = secVtx->position().y()-primVtx->position().y();
      float Lz = secVtx->position().z()-primVtx->position().z();

      Amg::Vector3D dL(Lx,Ly,Lz);

      AmgSymMatrix(3) covariance = secVtx->covariancePosition() + primVtx->covariancePosition(); 
      AmgSymMatrix(3) invert_covariance = covariance.inverse();

      const float Lxy = std::hypot(Lx,Ly);
      const float Lxy_sig = std::sqrt(Lx*Lx*invert_covariance(0,0) + 2*Lx*Ly*invert_covariance(0,1) + Ly*Ly*invert_covariance(1,1));
      const float Lz_sig = std::abs(Lz)/std::sqrt(covariance(2,2));
  
      const double decaylength = dL.norm();
      double decaylength_err = dL.dot(covariance*dL);
      decaylength_err /= dL.squaredNorm();
      double decaylength_significance = 0.;
      if (decaylength_err != 0.) decaylength_significance = decaylength/decaylength_err;

      // decorate the vertex
      secvtx_lxy(*secVtx) = std::abs(Lxy);
      secvtx_lxysig(*secVtx) = Lxy_sig;
      secvtx_lz(*secVtx) = std::abs(Lz);
      secvtx_lzsig(*secVtx) = Lz_sig;
      secvtx_l3d(*secVtx) = decaylength;
      secvtx_l3dsig(*secVtx) = decaylength_significance;

    }
  } 

  return StatusCode::SUCCESS;

}


} /// namespace
