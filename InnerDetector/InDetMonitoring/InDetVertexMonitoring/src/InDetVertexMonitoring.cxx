/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <map>
#include <string>
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "TMath.h"

#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "InDetVertexMonitoring.h"

//////////////////////////////////////////////////////////////////////////////////////

InDetVertexMonitoring::InDetVertexMonitoring( const std::string & type, const std::string & name, const IInterface* parent ) 
  : ManagedMonitorToolBase( type, name, parent )
{
  m_h_vxdist = 0;
  m_h_Ntracks = 0;
  m_h_sumpt = 0;
  m_h_pvChiSq = 0;
  m_h_pvDof = 0;
  m_h_pvChiSqovDof = 0;
  m_h_pvChiSqProb = 0;
  m_h_cumulVx = 0;
  m_h_deltaZpairs = 0;
}

/////////////////////////////////////////////////////////////////////////////////////

InDetVertexMonitoring::~InDetVertexMonitoring() {}

////////////////////////////////////////////////////////////////////////////////////

StatusCode InDetVertexMonitoring::initialize(){

  ATH_CHECK( ManagedMonitorToolBase::initialize() );
  ATH_CHECK( m_eventInfo.initialize() );
  ATH_CHECK( m_vertexContainer.initialize() );

  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////

StatusCode InDetVertexMonitoring::bookHistograms()
{
  if(m_doControlPlots){
    m_h_cumulVx = TH1F_LW::create("nVx","N_{Vx}",70,0,70.);
    m_h_Ntracks = TH1F_LW::create("nTracks","N_{tracks}",50,0,50.);
    m_h_sumpt = TH1F_LW::create("sumpt","sumpt",50,0,25.);
    m_h_pvChiSq = TH1F_LW::create("pvChiSq","Primary vertex: #Chi^{2}",100,0,100.);
    m_h_pvDof = TH1F_LW::create("pvDof","Primary vertex: Dof",100,0,100.);
    m_h_pvChiSqovDof = TH1F_LW::create("pvChiSqovDof","Primary vertex: #Chi^{2}/DoF",100,0,5.);
    m_h_pvChiSqProb = TH1F_LW::create("pvChiSqProb","Primary vertex: #Chi^{2} Probability",50,0,1.);
    m_h_deltaZpairs = TH2F_LW::create("deltaZpairs","#Deltaz of vertices pairs",40,0,40.,20000,-300.,300);
    
    regHist( m_h_cumulVx, m_histFolder, run, ATTRIB_MANAGED).ignore();
    regHist( m_h_Ntracks, m_histFolder, run, ATTRIB_MANAGED).ignore();
    regHist( m_h_sumpt, m_histFolder, run, ATTRIB_MANAGED).ignore();
    regHist( m_h_pvChiSq, m_histFolder, run, ATTRIB_MANAGED).ignore();
    regHist( m_h_pvDof, m_histFolder, run, ATTRIB_MANAGED).ignore();
    regHist( m_h_pvChiSqovDof, m_histFolder, run, ATTRIB_MANAGED).ignore();
    regHist( m_h_pvChiSqProb, m_histFolder, run, ATTRIB_MANAGED).ignore(); 
    regHist( m_h_deltaZpairs, m_histFolder, run, ATTRIB_MANAGED).ignore(); 
  }

  m_h_vxdist = TH1F_LW::create("nVx","Vx number with EF_rd0_filled_NoAlg",70,0.,70.);
  regHist( m_h_vxdist, m_histFolder, lumiBlock, ATTRIB_MANAGED).ignore();

  return StatusCode::SUCCESS;

}

StatusCode InDetVertexMonitoring::fillHistograms()
{

  SG::ReadHandle<xAOD::VertexContainer> vertexContainer(m_vertexContainer);
  if ( !vertexContainer.isValid() )
  {
    ATH_MSG_WARNING ("No xAOD::VertexContainer found");
    return StatusCode::FAILURE;
  }

  // Event info is never actually used
  // Leaving in the call since perhaps the check for presence is necessary
  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfo);
  if ( !eventInfo.isValid() )
  {
     ATH_MSG_WARNING ("No Event Info found");
     return StatusCode::FAILURE;
  }

  int n_vtx = vertexContainer->size()-1;

  for ( const xAOD::Vertex* vtx : (*vertexContainer) ) {
    if (vtx->vertexType() == xAOD::VxType::NoVtx) continue;
  
    int ntrk(-1);
    ntrk = vtx->nTrackParticles();
  
    if(m_doControlPlots){
      m_h_Ntracks->Fill(ntrk);
      double spt = 0.;
      if(ntrk>0){
	for (const ElementLink<xAOD::TrackParticleContainer>& trk : vtx->trackParticleLinks()) {
	  spt += (*trk)->pt()/1e6;
        }
      }
      spt = TMath::Sqrt(spt);
      m_h_sumpt->Fill(spt);
      double chi2 = vtx->chiSquared();
      m_h_pvChiSq->Fill(chi2);
      double dof = vtx->numberDoF();
      m_h_pvDof->Fill(dof);
      double chi2ovdof = chi2/dof;
      m_h_pvChiSqovDof->Fill(chi2ovdof);
      double chi2prob = TMath::Prob(chi2,dof);
      m_h_pvChiSqProb->Fill(chi2prob);
    } 
  }

  if(m_doControlPlots){
   m_h_cumulVx->Fill(n_vtx);
   //DeltaZ Plot
   double deltaZ = 0.;
   double zit1 = 0.;
   double zit2 = 0.;
   for ( const xAOD::Vertex *vtx : (*vertexContainer) ) {
     //Check vertex type (e.g. no dummy vertex)
     if ( ! ( vtx->vertexType() == xAOD::VxType::PriVtx or vtx->vertexType() == xAOD::VxType::PileUp ) )
       continue;
     zit1 = vtx->z();
     // Loop over vertices again (starting from vxIter + 1) to get all distinct pairs
     for ( const xAOD::Vertex *vtx2 : (*vertexContainer) ) {
       //Check vertex type (e.g. no dummy vertex)
       if ( ! ( vtx2->vertexType() == xAOD::VxType::PriVtx or vtx->vertexType() == xAOD::VxType::PileUp ) )
	 continue;
       zit2 = vtx2->z();
       deltaZ = (zit1-zit2);
       //Weight events with the total number of pairs (no compare distributions from different n_vtx bins)
       m_h_deltaZpairs->Fill(n_vtx,deltaZ, 1./TMath::Binomial(n_vtx,2));
     }
   }
 }
 
 m_h_vxdist->Fill(n_vtx);

 return StatusCode::SUCCESS;
}
