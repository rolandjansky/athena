///////////////////////// -*- C++ -*- ////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetVertexTaggerTool.cxx
// Implementation file for class JetVertexTaggerTool
// Author: James Frost <james.frost@cern.ch>
/////////////////////////////////////////////////////////////////// 

#include "JetMomentTools/JetVertexTaggerTool.h"
#include "PathResolver/PathResolver.h"

#include "xAODTracking/VertexContainer.h"

using std::string;
using xAOD::JetFourMom_t;

//**********************************************************************

JetVertexTaggerTool::JetVertexTaggerTool(const std::string& name)
: asg::AsgTool(name)
, m_jvtlikelihoodHistName("")
, m_jvtfileName("")
, m_htsel("")
{
    declareProperty("JVFCorrName", m_jvfCorrName="JVFCorr");
    declareProperty("SumPtTrkName", m_sumPtTrkName="SumPtTrkPt500");
    declareProperty("VertexContainer", m_verticesName="PrimaryVertices");

    declareProperty("JVTFileName",m_jvtfileName = "JetMomentTools/JVTlikelihood_20140805.root");
    declareProperty("JVTLikelihoodHistName",m_jvtlikelihoodHistName = "JVTRootCore_kNN100trim_pt20to50_Likelihood");
    declareProperty("TrackSelector", m_htsel);
    declareProperty("JVTName", m_jvtName ="Jvt");
}

//**********************************************************************

StatusCode JetVertexTaggerTool::initialize() {
  ATH_MSG_INFO("Initializing JetVertexTaggerTool " << name());

  if ( m_htsel.empty() ) {
    ATH_MSG_INFO("  No track selector.");
  } else {
    ATH_MSG_INFO("  Track selector: " << m_htsel->name());
  }

  // Use the Path Resolver to find the jvt file and retrieve the likelihood histogram
  m_fn =  PathResolverFindCalibFile(m_jvtfileName);	
  ATH_MSG_INFO("  Reading JVT file from:\n    " << m_jvtfileName << "\n");
  ATH_MSG_INFO("                     resolved in  :\n    " << m_fn << "\n\n");

  m_jvtfile = TFile::Open(m_fn);
  if ( !m_jvtfile ) { ATH_MSG_FATAL( "Cannot open JVTLikelihoodFile: " << m_fn ); return StatusCode::FAILURE; }

 ATH_MSG_VERBOSE("\n Reading JVT likelihood histogram from:\n    " << m_fn << "\n\n");

 m_jvthisto = (TH2F*)m_jvtfile->Get(m_jvtlikelihoodHistName.c_str() );
 if ( !m_jvthisto ) 
   {
     ATH_MSG_FATAL( "\n  Found JVT file, but JVT histogram missing. Aborting..." );
     return StatusCode::FAILURE;
   }

  return StatusCode::SUCCESS;
}

//**********************************************************************

int JetVertexTaggerTool::modify(xAOD::JetContainer& jetCont) const {

  const xAOD::Vertex* HSvertex = findHSVertex();
  if(!HSvertex) return 1;

  for(xAOD::Jet * jet : jetCont) 
    {      
      // Calculate RpT and JVFCorr 
      // Default JVFcorr to -1 when no tracks are associated.
      float jvfcorr = jet->getAttribute<float>(m_jvfCorrName);
      std::vector<float> sumpttrkpt500 = jet->getAttribute<std::vector<float> >(m_sumPtTrkName);
      const float rpt = sumpttrkpt500[HSvertex->index()]/jet->pt();
      float jvt = evaluateJvt(rpt, jvfcorr);
      
      jet->setAttribute(m_jvtName+"Rpt",rpt);
      jet->setAttribute(m_jvtName,jvt);

      // ATH_MSG_VERBOSE("JetVertexTaggerTool " << name()
      // 		   << ": Primary trk pT=" << tracksums.first
      // 		   << ", Pileup trk pT=" << tracksums.second 
      // 		   << ", Old JVF=" <<  tracksums.first/(tracksums.first+tracksums.second)   );

      ATH_MSG_VERBOSE("JetVertexTaggerTool " << name()
		   << ": JVT=" << jvt
		   << ", RpT=" << rpt
		   << ", JVFCorr=" << jvfcorr       );

      // Done
      
    } 

  return 0;
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
  string sjvfcorr = m_jvfCorrName;
  float jvfcorr = jet.getAttribute<float>(sjvfcorr);
  std::vector<float> sumpttrkpt500 = jet.getAttribute<std::vector<float> >(m_sumPtTrkName);
  const xAOD::Vertex* HSvertex = findHSVertex();
  if(!HSvertex) {
    ATH_MSG_ERROR("No hard scatter vertex found. Returning JVT=-1");
    return -1.;
  }
  const float rptnew = sumpttrkpt500[HSvertex->index()]/jet.pt();
  return evaluateJvt(rptnew, jvfcorr);
}

//**********************************************************************

StatusCode JetVertexTaggerTool::finalize() {
  ATH_MSG_INFO("Finalizing JetVertexTaggerTool " << name());
  m_jvtfile->Close();

  return StatusCode::SUCCESS;
}

const xAOD::Vertex* JetVertexTaggerTool::findHSVertex() const
{

  // Get the vertices container
  const xAOD::VertexContainer* vertices = NULL;
  if ( evtStore()->retrieve(vertices,m_verticesName).isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve the VertexContainer from evtStore: " << m_verticesName);
    return nullptr;
  }
  ATH_MSG_DEBUG("Successfully retrieved VertexContainer from evtStore: " << m_verticesName);

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

//**********************************************************************
