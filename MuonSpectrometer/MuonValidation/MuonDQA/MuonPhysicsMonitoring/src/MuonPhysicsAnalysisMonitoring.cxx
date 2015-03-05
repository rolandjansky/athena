/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////
// Authors:  I.Nomidis (AUTh) - N. Benekos(Illinois)
// MuonDQA
// Feb. 2009
//
// DESCRIPTION:
// Subject: 	This class is the basis for all Physics-Analysis-Classes
///////////////////////////////////////////////////////////////////////////

#include "MuonPhysicsMonitoring/MuonPhysicsAnalysisMonitoring.h"
//#include "egammaEvent/ElectronContainer.h"

using CLHEP::m2;

MuonPhysicsAnalysisMonitoring::MuonPhysicsAnalysisMonitoring()
{
  cutD.Pt_loose	= 15000.;  // in MeV
  cutD.Pt_tight	= 20000.;  // in MeV
  cutD.TrigMu_Pt = 20000.; // in MeV

  cutD.diMuonMass	= 30000.; // in MeV
  cutD.ZInvmMassWnd     = 12000.; // in MeV
  cutD.deltaPhi	        = 0.1;    // the distance in phi space between two items
  
  // Jet(loose identified) Energy, NTrack, PTTracks, ET in Halo around Muon [GeV]
  cutD.Jet_InHalo_Loose		= 25000.;
  cutD.NTracks_InHalo_Loose	= 6.;
  cutD.PTTracks_InHalo_Loose	= 15000.;
  cutD.ET_InHalo_Loose		= 20000.;
  
  // Jet(tight identified) Energy, NTrack, PTTracks, ET in Halo around Muon [GeV]
  cutD.Jet_InHalo_Tight		= 15000.;
  cutD.NTracks_InHalo_Tight	= 4.;
  cutD.PTTracks_InHalo_Tight	= 8000.;
  cutD.ET_InHalo_Tight		= 6000.;
}


MuonPhysicsAnalysisMonitoring::~MuonPhysicsAnalysisMonitoring()
{
}

void	MuonPhysicsAnalysisMonitoring::setup(std::string outputname, std::string dirname, cutDefinitions cuts)
{
  cutD		= cuts;
  m_Outputfile	= outputname;
  m_RootDirName	= dirname;
  //  msRootInterface.createDirectory(m_Outputfile, m_RootDirName);
}


double	MuonPhysicsAnalysisMonitoring::dRand(double min, double max)
{
  return (((max-min)*((double)(rand()))/((double)(RAND_MAX)))+min);
}

double MuonPhysicsAnalysisMonitoring::CorrectInvMassTwoMuons(const Rec::TrackParticle *track1, const Rec::TrackParticle *track2)
{
  double Mmu2=105.66*105.66;
  double e  =  sqrt( Mmu2 + track1->e()*track1->e() - track1->hlv().m()* track1->hlv().m() ) ;
  e +=  sqrt( Mmu2 + track2->e()*track2->e() - track2->hlv().m()* track2->hlv().m() ) ;  
  double px =  track1->px() + track2->px();
  double py =  track1->py() + track2->py();
  double pz =  track1->pz() + track2->pz();
    
  double m2 = e*e-px*px-py*py-pz*pz;
  if( m2< 0 ) return -1;
   
  double mass = sqrt(fabs(m2)) ;
        
  return mass ;
}

Trk::ExtendedVxCandidate* MuonPhysicsAnalysisMonitoring::doVertexing(const Rec::TrackParticle *theTag, const Rec::TrackParticle *theProbe, ToolHandle<InDet::VertexPointEstimator> m_vertexEstimator,ToolHandle<InDet::ConversionFinderUtils> m_helpertool, ToolHandle<Trk::IVertexFitter> m_iVertexFitter, const Trk::TrackParticleBaseCollection* importedTrackCollection,bool &deleteExtVxCandidate)
{
  std::vector<const Trk::TrackParticleBase*> inputTracks;
  inputTracks.push_back(static_cast<const Trk::TrackParticleBase*>(theTag));
  inputTracks.push_back(static_cast<const Trk::TrackParticleBase*>(theProbe));

  const Trk::Perigee * aPerigee1 = dynamic_cast<const Trk::Perigee*>(&(inputTracks[0])->definingParameters());
  const Trk::Perigee * aPerigee2 = dynamic_cast<const Trk::Perigee*>(&(inputTracks[1])->definingParameters());
  int sflag = 0;
  int errorcode = 0;
  Amg::Vector3D startingPoint = m_vertexEstimator->getCirclesIntersectionPoint(aPerigee1,aPerigee2,sflag,errorcode);
  if (errorcode != 0) {startingPoint(0) = 0.0; startingPoint(1) = 0.0; startingPoint(2) = 0.0;}
  Trk::Vertex vertex(startingPoint);
  const Trk::ParametersBase<5,Trk::Charged>* track1 = m_helpertool->getTrkParticleParameters((inputTracks[0]));
  const Trk::ParametersBase<5,Trk::Charged>* track2 = m_helpertool->getTrkParticleParameters((inputTracks[1]));
  std::vector<const Trk::ParametersBase<5,Trk::Charged>*> vertexTracks; vertexTracks.clear();
  vertexTracks.push_back(track1);
  vertexTracks.push_back(track2);
  Trk::VxCandidate* myVxCandidate = m_iVertexFitter->fit(vertexTracks, vertex);
  if (myVxCandidate==0) {
    delete myVxCandidate;
    return 0;
  }
  
  const std::vector<Trk::VxTrackAtVertex*>& trackAtVertexVec = *((myVxCandidate)->vxTrackAtVertex());
  for (unsigned int i = 0; i < trackAtVertexVec.size(); i++) {
    Trk::LinkToTrackParticleBase* newLinkToTrackPB = new Trk::LinkToTrackParticleBase();
    if(i == 0) newLinkToTrackPB->setElement(inputTracks[0]);
    if(i == 1) newLinkToTrackPB->setElement(inputTracks[1]);
    newLinkToTrackPB->setStorableObject(*importedTrackCollection);
    trackAtVertexVec[i]->setOrigTrack(newLinkToTrackPB);
  }
      
  Trk::ExtendedVxCandidate* jpsiCandidate = dynamic_cast<Trk::ExtendedVxCandidate* >(myVxCandidate);
  if (!jpsiCandidate) {
    jpsiCandidate = new Trk::ExtendedVxCandidate(myVxCandidate->recVertex(), *(myVxCandidate->vxTrackAtVertex()), 0);
    deleteExtVxCandidate=true;
    //if (jpsiCandidate==NULL) {
    //if (myVxCandidate->vxTrackAtVertex()) myVxCandidate->vxTrackAtVertex()->clear();
      //delete myVxCandidate;
      //delete jpsiCandidate;
      //return 0;
    //}
  }
  if (jpsiCandidate==NULL) {
    if (myVxCandidate->vxTrackAtVertex()) myVxCandidate->vxTrackAtVertex()->clear();
  }
  delete myVxCandidate;  
  return jpsiCandidate;
}
