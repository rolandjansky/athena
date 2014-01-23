/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <map>
#include <string>
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "TMath.h"

#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "VxVertex/VxContainer.h"
#include "VxVertex/VxTrackAtVertex.h"

#include "InDetVertexMonitoring/InDetVertexMonitoring.h"

//////////////////////////////////////////////////////////////////////////////////////

InDetVertexMonitoring::InDetVertexMonitoring( const std::string & type, const std::string & name, const IInterface* parent ) : ManagedMonitorToolBase( type, name, parent ),
  m_doControlPlots(false), m_histFolder("InDetGlobal/PrimaryVertexMultiplicity")
{
  declareProperty( "VertexContainer", m_VxContainerName = "VxPrimaryCandidate" );
    /** sumpt ntrack etc **/
  declareProperty("DoControlPlots", m_doControlPlots, "enable control plots");
  declareProperty("histFolder", m_histFolder);
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
  StatusCode sc;
  sc = ManagedMonitorToolBase::initialize();
  if(!sc.isSuccess()) return sc;
  
  sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ("Unable to access StoreGateSvc");
    return StatusCode::FAILURE;
  }
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

//  std::map<long int,TH1F*>::iterator it;
  
  const VxContainer* Vertexes = 0;
  StatusCode sc=m_storeGate->retrieve( Vertexes, m_VxContainerName);
  if( sc.isFailure()  ||  !Vertexes ) {
    ATH_MSG_WARNING ("No AOD vertex container found");
    return StatusCode::FAILURE;
  }  
  
  int lumiBlock=0;

  const EventInfo*  p_evt = 0;
  sc = m_storeGate->retrieve(p_evt);
  if( sc.isFailure()) {
     ATH_MSG_WARNING ("No Event Info found");
     return StatusCode::FAILURE;
  }   

  lumiBlock = p_evt->event_ID()->lumi_block(); 
  int n_vtx = Vertexes->size()-1;

  for (VxContainer::const_iterator vxIter = Vertexes->begin(); vxIter != Vertexes->end(); ++vxIter){
    if ((*vxIter)->vertexType() == Trk::NoVtx) continue;
    std::vector<Trk::VxTrackAtVertex*>* vxTrackAtVertex = (*vxIter)->vxTrackAtVertex();  
  
    int ntrk;
    if(vxTrackAtVertex->size()!=0) ntrk = vxTrackAtVertex->size();
    else ntrk = -1;
  
    if(m_doControlPlots){
      m_h_Ntracks->Fill(ntrk);
      double spt = 0.;
      if(ntrk>0){
        for (std::vector<Trk::VxTrackAtVertex*>::iterator trkIter  = vxTrackAtVertex->begin(); trkIter != vxTrackAtVertex->end()  ; ++trkIter){
        // pT at initial and perigeeAtVertex should be similar if not equal.
        // measured perigee is stored in vertex (the other has to be retrieved from the linked Trk::Track(Particle))
        const Trk::Perigee* measuredPerigee = dynamic_cast<const Trk::Perigee*>((*trkIter)->perigeeAtVertex());
	if (measuredPerigee == 0) measuredPerigee = dynamic_cast<const Trk::Perigee*>((*trkIter)->initialPerigee());
        if (measuredPerigee!=0) spt += (measuredPerigee->pT()*measuredPerigee->pT())/1e6;
        }
      }
    spt = TMath::Sqrt(spt);
    m_h_sumpt->Fill(spt);
    double chi2 = (*vxIter)->recVertex().fitQuality().chiSquared();
    m_h_pvChiSq->Fill(chi2);
    double dof = (*vxIter)->recVertex().fitQuality().numberDoF();
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
   for(VxContainer::const_iterator vxIter = Vertexes->begin(); vxIter != Vertexes->end(); ++vxIter){
     //Check vertex type (e.g. no dummy vertex)
     if ( ! ( (*vxIter)->vertexType() == Trk::PriVtx or (*vxIter)->vertexType() == Trk::PileUp ) )
       continue;
     zit1 = (*vxIter)->recVertex().position().z();
     // Loop over vertices again (starting from vxIter + 1) to get all distinct pairs
     for(VxContainer::const_iterator vxIter2 = vxIter+1; vxIter2 != Vertexes->end(); ++vxIter2){
       //Check vertex type (e.g. no dummy vertex)
       if ( ! ( (*vxIter2)->vertexType() == Trk::PriVtx or (*vxIter2)->vertexType() == Trk::PileUp ) )
	 continue;
       zit2 = (*vxIter2)->recVertex().position().z();
       deltaZ = (zit1-zit2);
       //Weight events with the total number of pairs (no compare distributions from different n_vtx bins)
       m_h_deltaZpairs->Fill(n_vtx,deltaZ, 1./TMath::Binomial(n_vtx,2));
     }
   }
 }
 
 m_h_vxdist->Fill(n_vtx);

 return StatusCode::SUCCESS;
}
