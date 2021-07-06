///////////////////////// -*- C++ -*- ////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetVertexTaggerTool.cxx
// Implementation file for class JetVertexTaggerTool
// Author: James Frost <james.frost@cern.ch>
///////////////////////////////////////////////////////////////////

#include "JetMomentTools/JetVertexTaggerTool.h"
#include "PathResolver/PathResolver.h"
#include "AsgDataHandles/ReadDecorHandle.h"
#include "AsgDataHandles/WriteDecorHandle.h"

using std::string;
using xAOD::JetFourMom_t;

//**********************************************************************

JetVertexTaggerTool::JetVertexTaggerTool(const std::string& name)
: asg::AsgTool(name),
  m_jvthisto(nullptr)
{
}

//**********************************************************************

JetVertexTaggerTool::~JetVertexTaggerTool(){
  if(m_jvthisto) delete m_jvthisto;
}

//**********************************************************************

StatusCode JetVertexTaggerTool::initialize() {
  ATH_MSG_INFO("Initializing JetVertexTaggerTool " << name());

  if(m_jetContainerName.empty()){
    ATH_MSG_ERROR("JetVertexTaggerTool needs to have its input jet container configured!");
    return StatusCode::FAILURE;
  }

  // Use the Path Resolver to find the jvt file and retrieve the likelihood histogram
  m_fn =  PathResolverFindCalibFile(m_jvtfileName);
  ATH_MSG_INFO("  Reading JVT file from:\n    " << m_jvtfileName << "\n");
  ATH_MSG_INFO("                     resolved in  :\n    " << m_fn << "\n\n");

  std::unique_ptr<TFile> jvtfile {TFile::Open(m_fn)};
  if(!jvtfile){
    ATH_MSG_FATAL("Cannot open JVTLikelihoodFile: " << m_fn);
    return StatusCode::FAILURE;
  }

  ATH_MSG_VERBOSE("\n Reading JVT likelihood histogram from:\n    " << m_fn << "\n\n");

  m_jvthisto = (TH2F*)jvtfile->Get(std::string(m_jvtlikelihoodHistName).c_str() );
  if(!m_jvthisto){
    ATH_MSG_FATAL( "\n  Found JVT file, but JVT histogram missing. Aborting..." );
    return StatusCode::FAILURE;
  }
  m_jvthisto->SetDirectory (nullptr);

  m_jvfCorrKey = m_jetContainerName + "." + m_jvfCorrKey.key();
  m_sumPtTrkKey = m_jetContainerName + "." + m_sumPtTrkKey.key();
  m_jvtKey = m_jetContainerName + "." + m_jvtKey.key();
  m_rptKey = m_jetContainerName + "." + m_rptKey.key();

#ifndef XAOD_STANDALONE
  if(m_suppressInputDeps){
    // The user has promised that these will be produced by the same alg running JVT.
    // Tell the scheduler to ignore them to avoid circular dependencies.
    renounce(m_jvfCorrKey);
    renounce(m_sumPtTrkKey);
  }
#endif

  ATH_CHECK(m_vertexContainer_key.initialize());
  ATH_CHECK(m_jvfCorrKey.initialize());
  ATH_CHECK(m_sumPtTrkKey.initialize());
  ATH_CHECK(m_jvtKey.initialize());
  ATH_CHECK(m_rptKey.initialize());

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetVertexTaggerTool::decorate(const xAOD::JetContainer& jetCont) const {

  const xAOD::Vertex* HSvertex = findHSVertex();
  // findHSVertex will provide an error message if this comes back null
  if(HSvertex == nullptr) return StatusCode::FAILURE;

  // Grab vertices for index bookkeeping
  SG::ReadHandle<xAOD::VertexContainer> vertexHandle = SG::makeHandle (m_vertexContainer_key);
  const xAOD::VertexContainer* vertices = vertexHandle.cptr();

  SG::ReadDecorHandle<xAOD::JetContainer, float> jvfCorrHandle(m_jvfCorrKey);
  SG::ReadDecorHandle<xAOD::JetContainer, std::vector<float> > sumPtTrkHandle(m_sumPtTrkKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> jvtHandle(m_jvtKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> rptHandle(m_rptKey);

  for(const xAOD::Jet* jet : jetCont){
    // Calculate RpT and JVFCorr
    // Default JVFcorr to -1 when no tracks are associated.
    float jvfcorr = jvfCorrHandle(*jet);
    std::vector<float> sumpttrk = sumPtTrkHandle(*jet);
    const float rpt = sumpttrk[HSvertex->index() - (*vertices)[0]->index()]/jet->pt();
    float jvt = evaluateJvt(rpt, jvfcorr);

    rptHandle(*jet) = rpt;
    jvtHandle(*jet) = jvt;

    ATH_MSG_VERBOSE("JetVertexTaggerTool " << name() << ": JVT=" << jvt << ", RpT=" << rpt << ", JVFCorr=" << jvfcorr);
  }

  return StatusCode::SUCCESS;
}

//**********************************************************************

float JetVertexTaggerTool::evaluateJvt(float rpt, float jvfcorr) const {
  // Look up JVT value
  float jvt = -999.;
  if ( jvfcorr == -1.0 ) {
    jvt = -0.1;
  } else {
    float rpt_inputtojvt = std::min(rpt, (float) 1. );
    int bin = m_jvthisto->FindBin(jvfcorr, rpt_inputtojvt);
    jvt = m_jvthisto->GetBinContent(bin);
    jvt = m_jvthisto->Interpolate(jvfcorr, rpt_inputtojvt);
  }
  return jvt;
}

//**********************************************************************

float JetVertexTaggerTool::updateJvt(const xAOD::Jet& jet) const {

  SG::ReadDecorHandle<xAOD::JetContainer, float> jvfCorrHandle(m_jvfCorrKey);
  SG::ReadDecorHandle<xAOD::JetContainer, std::vector<float> > sumPtTrkHandle(m_sumPtTrkKey);

  float jvfcorr = jvfCorrHandle(jet);
  std::vector<float> sumpttrk = sumPtTrkHandle(jet);

  const xAOD::Vertex* HSvertex = findHSVertex();
  if(!HSvertex) {
    ATH_MSG_ERROR("No hard scatter vertex found. Returning JVT=-1");
    return -1.;
  }

  // Grab vertices for index bookkeeping
  SG::ReadHandle<xAOD::VertexContainer> vertexHandle = SG::makeHandle (m_vertexContainer_key);
  const xAOD::VertexContainer* vertices = vertexHandle.cptr();

  const float rptnew = sumpttrk[HSvertex->index() - (*vertices)[0]->index()]/jet.pt();

  return evaluateJvt(rptnew, jvfcorr);
}

//**********************************************************************

const xAOD::Vertex* JetVertexTaggerTool::findHSVertex() const
{
  // Get input vertex collection
  SG::ReadHandle<xAOD::VertexContainer> vertexHandle = SG::makeHandle (m_vertexContainer_key);
  if (!vertexHandle.isValid()){
    ATH_MSG_ERROR("Invalid VertexContainer datahandle: " << m_vertexContainer_key.key());
    return nullptr;
  }
  const xAOD::VertexContainer* vertices = vertexHandle.cptr();
  ATH_MSG_DEBUG("Successfully retrieved VertexContainer: " << m_vertexContainer_key.key());

  if (vertices->size() == 0 ) {
    ATH_MSG_WARNING("There are no vertices in the container. Exiting");
    return nullptr;
  }

  for ( size_t iVertex = 0; iVertex < vertices->size(); ++iVertex ) {
    if(vertices->at(iVertex)->vertexType() == xAOD::VxType::PriVtx) {

      ATH_MSG_VERBOSE("JetVertexTaggerTool " << name() << " Found HS vertex at index: "<< iVertex);
      return vertices->at(iVertex);
    }
  }
  ATH_MSG_VERBOSE("There is no vertex of type PriVx. Taking default vertex.");
  return vertices->at(0);
}
