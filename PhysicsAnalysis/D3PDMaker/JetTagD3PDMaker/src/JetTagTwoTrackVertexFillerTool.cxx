/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTwoTrackVertexFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "JetTagTwoTrackVertexCandidateHolder.h"

#include "VxVertex/VxCandidate.h"
#include "VxJetVertex/TwoTrackVerticesInJet.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"



using CLHEP::HepLorentzVector;
using CLHEP::Hep3Vector;


namespace D3PD {
  
  

JetTagTwoTrackVertexFillerTool::JetTagTwoTrackVertexFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<JetTagTwoTrackVertexCandidateHolder> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetTagTwoTrackVertexFillerTool::book()
{

  CHECK( addVariable ("isNeutral", m_neutral) );
  CHECK( addVariable ("chi2", m_chi2) );
  CHECK( addVariable ("ndof", m_ndof) );

  CHECK( addVariable ("x", m_x) );
  CHECK( addVariable ("y", m_y) );
  CHECK( addVariable ("z", m_z) );

  CHECK( addVariable ("errx", m_errx) );
  CHECK( addVariable ("erry", m_erry) );
  CHECK( addVariable ("errz", m_errz) );

  CHECK( addVariable ("mass", m_mass) );

  return StatusCode::SUCCESS;
}


StatusCode JetTagTwoTrackVertexFillerTool::fill(const JetTagTwoTrackVertexCandidateHolder& p)
{

  *m_neutral=false;
  *m_chi2 = -999;
  *m_ndof = -999;

  *m_x = -999;
  *m_y = -999;
  *m_z = -999;

  *m_errx = -999;
  *m_erry = -999;
  *m_errz = -999;

  *m_mass=-999;

  const Trk::VxCandidate* vx =  p.getVxCandidate();

  //// can create association if needed. For now just filling if isNeutal or not.
  const Trk::TrackParticleBase* neutral =  p.getNeutralTrack();


  if(!vx){
    ATH_MSG_WARNING("VxCandidate pointer null");
    return StatusCode::SUCCESS;
  }

  if(neutral)*m_neutral=true;

  const double s_pion=139.57018;
  const Trk::RecVertex& recoVertex=vx->recVertex();
   
  *m_chi2 = recoVertex.fitQuality().chiSquared();
  *m_ndof = recoVertex.fitQuality().numberDoF();

  *m_x = recoVertex.position().x();
  *m_y = recoVertex.position().y();
  *m_z = recoVertex.position().z();

  const AmgSymMatrix(3)& err = recoVertex.covariancePosition();
   
  *m_errx = Amg::error(err, Trk::x);
  *m_erry = Amg::error(err, Trk::y);
  *m_errz = Amg::error(err, Trk::z);


  const std::vector<Trk::VxTrackAtVertex*>* tracksOfVertex=vx->vxTrackAtVertex();

  const Trk::VxTrackAtVertex* track1=0;
  const Trk::VxTrackAtVertex* track2=0;
   
  if(tracksOfVertex->size()==2){
    track1=(*tracksOfVertex)[0];
    track2=(*tracksOfVertex)[1];

    HepLorentzVector massVector(0,0,0,0);
    const Amg::Vector3D& momentum1 = track1->perigeeAtVertex()->momentum();
    const Amg::Vector3D& momentum2 = track2->perigeeAtVertex()->momentum();

    massVector+=HepLorentzVector(momentum1[0], momentum1[1], momentum1[2],
                                 sqrt(s_pion*s_pion+momentum1.norm()*momentum1.norm()));
    massVector+=HepLorentzVector(momentum2[0], momentum2[1], momentum2[2],
                                 sqrt(s_pion*s_pion+momentum2.norm()*momentum2.norm()));

    *m_mass = massVector.mag();

  }
  else{
    ATH_MSG_WARNING("Two track vertex have " << tracksOfVertex->size() << " tracks!");
  }


  return StatusCode::SUCCESS;
}


}// namespace D3PD
