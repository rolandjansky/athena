///////////////////////// -*- C++ -*- ////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetVertexTaggerTool.cxx
// Implementation file for class JetVertexTaggerTool
// Author: James Frost <james.frost@cern.ch>
///////////////////////////////////////////////////////////////////

#include "JetMomentTools/JetVertexTaggerTool.h"
#include "PathResolver/PathResolver.h"

using std::string;
using xAOD::JetFourMom_t;

//**********************************************************************

JetVertexTaggerTool::JetVertexTaggerTool(const std::string& name)
: asg::AsgTool(name)
, m_jvtlikelihoodHistName("")
, m_jvtfileName("")
{
    declareProperty("JVFCorrName", m_jvfCorrName="JVFCorr");
    declareProperty("SumPtTrkName", m_sumPtTrkName="SumPtTrkPt500");

    declareProperty("JVTFileName",m_jvtfileName = "JVTlikelihood_20140805.root");
    declareProperty("JVTLikelihoodHistName",m_jvtlikelihoodHistName = "JVTRootCore_kNN100trim_pt20to50_Likelihood");
    declareProperty("JVTName", m_jvtName ="Jvt");

    declareProperty("VertexContainer", m_vertexContainer_key="PrimaryVertices");

}

//**********************************************************************

StatusCode JetVertexTaggerTool::initialize() {
  ATH_MSG_DEBUG("initializing version with data handles");
  ATH_MSG_INFO("Initializing JetVertexTaggerTool " << name());

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

  ATH_CHECK(m_vertexContainer_key.initialize());

  return StatusCode::SUCCESS;
}

//**********************************************************************

int JetVertexTaggerTool::modify(xAOD::JetContainer& jetCont) const {

  // Get input vertex collection
  auto vertexContainer = SG::makeHandle (m_vertexContainer_key);
  if (!vertexContainer.isValid()){
    ATH_MSG_ERROR("Invalid VertexContainer datahandle: " 
                  << m_vertexContainer_key.key());
    return 1;
  }

  auto vertices = vertexContainer.cptr();

  ATH_MSG_DEBUG("Successfully retrieved VertexContainer: " 
                //                << m_verticesName);
                << m_vertexContainer_key.key());


  if (vertices->size() == 0 ) {
    ATH_MSG_WARNING("There are no vertices in the container. Exiting");
    return 2;
  }

  const xAOD::Vertex* HSvertex = findHSVertex(vertices);

  for(xAOD::Jet * jet : jetCont)
    {
      // Calculate RpT and JVFCorr
      // Default JVFcorr to -1 when no tracks are associated.
      float jvfcorr = jet->getAttribute<float>(m_jvfCorrName);
      std::vector<float> sumpttrkpt500 = jet->getAttribute<std::vector<float> >(m_sumPtTrkName);
      const float rpt = sumpttrkpt500[HSvertex->index() - (*vertices)[0]->index()]/jet->pt();
      float jvt = evaluateJvt(rpt, jvfcorr);

      jet->setAttribute(m_jvtName+"Rpt",rpt);
      jet->setAttribute(m_jvtName,jvt);

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

float JetVertexTaggerTool::updateJvt(const xAOD::Jet& jet, std::string sjvt, std::string scale) const {
  string sjvfcorr = m_jvfCorrName;
  string srpt = sjvt + "Rpt";
  JetFourMom_t p4old = jet.jetP4(scale);
  float ptold = p4old.pt();
  float ptnew = jet.pt();
  float jvfcorr = jet.getAttribute<float>(sjvfcorr);
  float rptold = jet.getAttribute<float>(srpt);
  //float jvtold = jet.getAttribute<float>(sjvt);
  float rptnew = rptold*ptold/ptnew;
  return evaluateJvt(rptnew, jvfcorr);
}

//**********************************************************************

StatusCode JetVertexTaggerTool::finalize() {
  ATH_MSG_INFO("Finalizing JetVertexTaggerTool " << name());
  m_jvtfile->Close();

  return StatusCode::SUCCESS;
}

const xAOD::Vertex* JetVertexTaggerTool::findHSVertex(const xAOD::VertexContainer*& vertices) const
{
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
