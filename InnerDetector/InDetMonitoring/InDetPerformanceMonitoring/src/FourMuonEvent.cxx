/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//==================================================================================
// Include files...
//==================================================================================

// This files header
#include "InDetPerformanceMonitoring/FourMuonEvent.h"

// Standard headers

// Package Headers
#include "InDetPerformanceMonitoring/PerfMonServices.h"

// ATLAS headers
#include "StoreGate/StoreGateSvc.h"

//#include "muonEvent/MuonParamDefs.h"

#include "CLHEP/Random/RandFlat.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
//==================================================================================
// Public Methods
//==================================================================================

FourMuonEvent::FourMuonEvent()
{
  m_xSampleName = "FourMuon";

  m_container = PerfMonServices::MUID_COLLECTION;

  m_doDebug = false;
  m_workAsFourMuons = true;
  m_workAsFourElectrons = false;
  m_workAsFourLeptons = false;

  // Setup the muon tags
  m_uMuonTags   = 4;
  m_uTrackMatch = 0;
  m_bLooseMatch = true;  // will use combined fit otherwise.
  m_etaCut      = 1.05;
  m_LeadingMuonPtCut = 20.;   
  m_SecondMuonPtCut = 15.;
  m_MassWindowLow = 10.0;
  m_MassWindowHigh = 125.0;
  m_OpeningAngleCut = 0.2; // in radians
  m_Z0GapCut = 5.0; // in mm
  m_SelectMuonByIso = true;
  m_SelectMuonByIP = true;
  m_eventCount = 0;
  m_acceptedEventCount = 0;
}

//==================================================================================
FourMuonEvent::~FourMuonEvent()
{
}

//==================================================================================
void FourMuonEvent::Init()
{
  bool thisdebug = true;
  if(m_doDebug || thisdebug){ std::cout << " * FourMuonEvent::Init * START *" << std::endl; }
  
  m_xMuonID.Init();
  m_xElecID.Init();

  PARENT::Init();
  if(m_doDebug || thisdebug){ std::cout << " * FourMuonEvent::Init * Completed * " << std::endl; }
}

//==================================================================================
bool FourMuonEvent::Reco()
{
  bool thisdebug = false;
  m_eventCount++;
  if(m_doDebug || thisdebug){ std::cout << " * FourMuonEvent::Reco * starting * event count " << m_eventCount << std::endl; }

  // Clear out the previous events record.
  this->Clear();

  const xAOD::MuonContainer* pxMuonContainer = PerfMonServices::getContainer<xAOD::MuonContainer>( m_container );

  // if muons are requested 
  if (m_workAsFourMuons || m_workAsFourLeptons) {
    // check if muon container does exist
    if (pxMuonContainer != nullptr) { 
      if (m_doDebug || thisdebug) {
	std::cout << " * FourMuonEvent::Reco * eventCount " << m_eventCount 
					     << " track list has "<< pxMuonContainer->size() 
					     << " combined muons in container " << m_container 
					     << std::endl; 
      }
      xAOD::MuonContainer::const_iterator xMuonItr  = pxMuonContainer->begin();
      xAOD::MuonContainer::const_iterator xMuonItrE  = pxMuonContainer->end();
      int theCount = 0;
      while ( xMuonItr != xMuonItrE ){ // start loop on muons
	const xAOD::Muon* pxCMuon = *xMuonItr;
	theCount++;
	if (m_doDebug || thisdebug) std::cout << " * FourMuonEvent::Reco ** attempt on muon " << theCount << " with xMuonItr "<< *xMuonItr << std::endl;
	
	// Apply muon cuts
	if ( m_xMuonID.passSelection( pxCMuon)) {
	  RecordMuon( pxCMuon );
	  if (m_doDebug || thisdebug) std::cout << " * FourMuonEvent::Reco ** muon " << theCount << " is accepted " << std::endl;
	}
	xMuonItr++;
      } // end loop on muons
      
      // ordering of muons
      this->OrderMuonList();      
    } // end muon container exists
    if (!pxMuonContainer) {
      std::cout << " * FourMuonEvent::Reco * Can't retrieve combined muon collection (container: " << m_container <<") " << std::endl;
      return false;
    } // end muon container does not exist
  } // end requesting muons

  //  
  // here must go the electron selection
  //
  // if muons are requested 
  if (m_workAsFourElectrons || m_workAsFourLeptons) {
  }  

  // now check if the particles in the event make them to satisfy the event selection
  if (m_workAsFourMuons) {
    m_passedFourMuonSelection = false; // unless the event has 4 muons, assume it is not good

    if (m_numberOfFullPassMuons == 4) {
      m_passedFourMuonSelection = true;  // now that we know the event has 4 muons, assume the event satisfies the selection cuts
      if ( m_passedFourMuonSelection && thisdebug) std::cout << " * FourMuonEvent::Reco * This events has 4 muons. Let's check... " << std::endl;
      
      m_passedFourMuonSelection = this->ReconstructKinematics(); // try the event kinematics 
      
      if (m_passedFourMuonSelection) {   
	m_passedFourMuonSelection = EventSelection(ID);
	m_FourMuonInvMass = m_fInvariantMass[ID];
      }
      else {
	if (m_doDebug || thisdebug) std::cout << " * FourMuonEvent::Reco * This events has no 4 good muons :( " << std::endl;
      }          
      if (!m_passedFourMuonSelection && thisdebug) std::cout << " * FourMuonEvent::Reco * However the 4 muons are not good for the analysis :( " << std::endl;
    }  
  } // end of workAsFourMuons
  
  if (m_workAsFourElectrons) {
    std::cout << " * FourMuonEvent::Reco * work in progress to use 4 electrons as well " << std::endl;
  }
  
  if (m_workAsFourLeptons) {
    std::cout << " * FourMuonEvent::Reco * work in progress to use 4 leptons (electrons or muons) " << std::endl;
  }

  m_passedSelectionCuts = false; // assume event is not good, but check the selection according to the use case
  if (m_workAsFourMuons)     m_passedSelectionCuts = m_passedFourMuonSelection;
  if (m_workAsFourElectrons) m_passedSelectionCuts = m_passedFourElectronSelection;
  if (m_workAsFourLeptons)   m_passedSelectionCuts = m_passedFourLeptonSelection;

  if (m_doDebug) {
    if ( m_passedSelectionCuts) std::cout << " * FourMuonEvent::Reco * Selected event :) " << std::endl;
    if (!m_passedSelectionCuts) std::cout << " * FourMuonEvent::Reco * Rejected event :( " << std::endl;
  }
  
  if ( m_passedSelectionCuts) m_acceptedEventCount++;

  if (m_doDebug || thisdebug || m_passedSelectionCuts) {
    std::cout << " * FourMuonEvent::Reco * COMPLETED * Event has " << m_numberOfFullPassMuons 
	      << " muons. " << m_acceptedEventCount 
	      << " events accpeted out of " <<  m_eventCount 
	      << " tested ";
    if (m_passedSelectionCuts) std::cout << " This m= " << m_FourMuonInvMass;
    std::cout << " * return " << m_passedSelectionCuts << std::endl; 
  }
  
  return m_passedSelectionCuts;
}

//==================================================================================
// Protected Methods
//==================================================================================
void FourMuonEvent::BookHistograms()
{
}

//==================================================================================
// Private Methods
//==================================================================================
bool FourMuonEvent::EventSelection(ZTYPE eType)
{
  if(m_doDebug){  std::cout <<" * FourMuonEvent::EventSelection(" << eType << ") ** started ** " << std::endl 
			    << "                                            event count: " << m_eventCount
			    << "                                m_NumberOfFullPassMuons: " << m_numberOfFullPassMuons 
			    << std::endl;
  }

  // First require two muon-id's with cuts pre-applied.
  if ( m_numberOfFullPassMuons < 4 ) {
    if(m_doDebug) {std::cout <<" * FourMuonEvent::EventSelection(" << eType << ") * Failing number of good muons == 4 :( " <<  m_numberOfFullPassMuons << std::endl;}
    return false;
  }
  
  if ( m_numberOfFullPassMuons > 4 ) {
    if(m_doDebug) {std::cout <<" * FourMuonEvent::EventSelection(" << eType << ") * Failing number of good muons == 4 :( " <<  m_numberOfFullPassMuons << std::endl;}
    return false;
  }
  
  // momentum of the muons
  double leadingMuonPt, secondMuonPt, thirdMuonPt, fourthMuonPt;
  switch ( eType ) {
  case MS :
    {
      leadingMuonPt = m_pxMSTrack[m_muonpos1]->pt();
      secondMuonPt =  m_pxMSTrack[m_muonpos2]->pt();
      thirdMuonPt =   m_pxMSTrack[m_muonneg1]->pt();
      fourthMuonPt =  m_pxMSTrack[m_muonneg2]->pt();
      break;
    }
  case ME:
    {
      leadingMuonPt = m_pxMETrack[m_muonpos1]->pt();
      secondMuonPt =  m_pxMETrack[m_muonpos2]->pt();
      thirdMuonPt =   m_pxMETrack[m_muonneg1]->pt();
      fourthMuonPt =  m_pxMETrack[m_muonneg2]->pt();
      break;
    }
  case CB:
    {
      leadingMuonPt = m_pxRecMuon[m_muonpos1]->pt();
      secondMuonPt =  m_pxRecMuon[m_muonpos2]->pt();
      thirdMuonPt =   m_pxRecMuon[m_muonneg1]->pt();
      fourthMuonPt =  m_pxRecMuon[m_muonneg2]->pt();
      break;
    }
  case ID:
    {
      leadingMuonPt = m_pxIDTrack[m_muonpos1]->pt();
      secondMuonPt =  m_pxIDTrack[m_muonpos2]->pt();
      thirdMuonPt =   m_pxIDTrack[m_muonneg1]->pt();
      fourthMuonPt =  m_pxIDTrack[m_muonneg2]->pt();
      break;
    }
    
  default:
    leadingMuonPt = m_pxRecMuon[m_muonpos1]->pt();
    secondMuonPt =  m_pxRecMuon[m_muonpos2]->pt();
    thirdMuonPt =   m_pxRecMuon[m_muonneg1]->pt();
    fourthMuonPt =  m_pxRecMuon[m_muonneg2]->pt();
  } // end switch

  // up to here the leading and second pt are not really in the right order.
  // order the muon pt:
  double theLeadingPt = leadingMuonPt;
  if (secondMuonPt > theLeadingPt) { theLeadingPt = secondMuonPt;}
  if (thirdMuonPt  > theLeadingPt) { theLeadingPt = thirdMuonPt;}
  if (fourthMuonPt > theLeadingPt) { theLeadingPt = fourthMuonPt;}

  double theTrailingPt = leadingMuonPt;
  if (secondMuonPt < theTrailingPt) { theTrailingPt = secondMuonPt;}
  if (thirdMuonPt  < theTrailingPt) {theTrailingPt = thirdMuonPt;}
  if (fourthMuonPt < theTrailingPt) { theTrailingPt = fourthMuonPt;}
  
  if (m_doDebug) {
    std::cout << " * FourMuonEvent::EventSelection * muon pt selection -- cuts: Leading: " << m_LeadingMuonPtCut*CLHEP::GeV << std::endl
	      << "                                                                  2nd: " << m_SecondMuonPtCut*CLHEP::GeV << std::endl;
    std::cout << "                                          Pt of muons in this event 1: " << leadingMuonPt << std::endl
	      << "                                                                    2: " << secondMuonPt << std::endl
	      << "                                                                    3: " << thirdMuonPt << std::endl
	      << "                                                                    4: " << fourthMuonPt << std::endl
	      << "                                                           leading Pt: " << theLeadingPt << std::endl
	      << "                                                          trailing Pt: " << theTrailingPt << std::endl;
  }
  
  // muon pt cut
  if ( !(theLeadingPt > m_LeadingMuonPtCut*CLHEP::GeV && theTrailingPt >  m_SecondMuonPtCut*CLHEP::GeV ) ) {
    if(m_doDebug){  
      std::cout <<" * FourMuonEvent::EventSelection * Failing pt cut * Reco Pt:  leading " << theLeadingPt << "  --> trailing  " << theTrailingPt << std::endl;
    }
    return false;
  }
  if(m_doDebug){
    std::cout << " * FourMuonEvent::EventSelection * Event passed the pt cuts: leading muon pt: " << leadingMuonPt << std::endl;
    std::cout << "                                                            trailing muon pt: " << theTrailingPt << std::endl; 
  }
  
 
  // Invariant mass window
  if ( m_fInvariantMass[eType]  < m_MassWindowLow  ) {
    if(m_doDebug) {
      std::cout <<" * FourMuonEvent::EventSelection * Failing mass window low cut:  reco m= " << m_fInvariantMass[eType] << " > " <<  m_MassWindowLow << std::endl;
    }
    return false;
  }
  if ( m_fInvariantMass[eType]  > m_MassWindowHigh ) {
    if(m_doDebug) {
      std::cout <<" * FourMuonEvent * Failing mass window high cut:  reco m= " << m_fInvariantMass[eType] << " > " <<  m_MassWindowHigh << std::endl;
    }
    return false;
  }
  if(m_doDebug){
    std::cout <<" * FourMuonEvent::EventSelection * Event passed the mass window: " << m_fInvariantMass[eType] << std::endl;
  }
  

  // All muons should come from the same vertex
  // if the vertex information is used, that is already guaranteed, but if not, one has to check the z0
  
  if (eType == ID) {
    bool vertexstatus = true;
    if (m_pxIDTrack[m_muonpos1]->vertex() != nullptr) {
      if(m_doDebug) { 
	std::cout <<" * FourMuonEvent::EventSelection * vertex of the muons -- ID Tracks --" << std::endl; 
	std::cout << "                 vertex muonpos_1 (x,y,z): (" << m_pxIDTrack[m_muonpos1]->vertex()->x() 
		  << ", " << m_pxIDTrack[m_muonpos1]->vertex()->y() 
		  << ", " << m_pxIDTrack[m_muonpos1]->vertex()->z()
		  << ")  --> pt: " << m_pxIDTrack[m_muonpos1]->pt() 
		  << std::endl; 
      }
    }
    else {
      vertexstatus = false;
      if(m_doDebug) std::cout <<" * FourMuonEvent::EventSelection * WARNING muonpos_1 (" << m_muonpos1 << ") has no vertex " << std::endl; 
    }
    if (m_pxIDTrack[m_muonpos2]->vertex() != nullptr) {
      if(m_doDebug) { 
	std::cout << "                 vertex muonpos_2 (x,y,z): (" << m_pxIDTrack[m_muonpos2]->vertex()->x() 
		  << ", " << m_pxIDTrack[m_muonpos2]->vertex()->y() 
		  << ", " << m_pxIDTrack[m_muonpos2]->vertex()->z()
		  << ")  --> pt: " << m_pxIDTrack[m_muonpos2]->pt() 
		  << std::endl; 
      }
    }
    else{
      vertexstatus = false;
      if(m_doDebug) std::cout <<" * FourMuonEvent::EventSelection * WARNING muonpos_2 (" << m_muonpos2 << ") has no vertex " << std::endl; 
    }
    if (m_pxIDTrack[m_muonneg1]->vertex() != nullptr) {    
      if(m_doDebug) { 
	std::cout << "                 vertex muonneg_1 (x,y,z): (" << m_pxIDTrack[m_muonneg1]->vertex()->x() 
		  << ", " << m_pxIDTrack[m_muonneg1]->vertex()->y() 
		  << ", " << m_pxIDTrack[m_muonneg1]->vertex()->z()
		  << ")  --> pt: " << m_pxIDTrack[m_muonneg1]->pt() 
		  << std::endl; 
      }
    }
    else{
      vertexstatus = false;
      if(m_doDebug) std::cout <<" * FourMuonEvent::EventSelection * WARNING muonneg_1 (" << m_muonneg1 << ") has no vertex " << std::endl; 
    }
    
    if (m_pxIDTrack[m_muonneg2]->vertex() != nullptr) {
      if(m_doDebug) { 
	std::cout << "                 vertex muonneg_2 (x,y,z): (" << m_pxIDTrack[m_muonneg2]->vertex()->x() 
		  << ", " << m_pxIDTrack[m_muonneg2]->vertex()->y() 
		  << ", " << m_pxIDTrack[m_muonneg2]->vertex()->z()
		  << ")  --> pt: " << m_pxIDTrack[m_muonneg2]->pt() 
		  << std::endl; 
      }
    }
    else{
      vertexstatus = false;
      if(m_doDebug) std::cout <<" * FourMuonEvent::EventSelection * WARNING muonneg_2 (" << m_muonneg2 << ") has no vertex " << std::endl; 
    }
    
    if (vertexstatus) { // this means: all muons have vertex associated. Let's check it is the same.
      if(m_doDebug) std::cout << " -- debug -- * FourMuonEvent::EventSelection * let's find the number of muon-vertices... " << std::endl;
      
      std::vector <float> vtxListX;
      std::vector <float> vtxListZ;
      float rGapCut = 0.1;
      // add the vertex of the 1st in the list (the muonneg1)
      vtxListX.push_back(m_pxIDTrack[m_muonneg1]->vertex()->x());
      vtxListZ.push_back(m_pxIDTrack[m_muonneg1]->vertex()->z());
      m_nVertex = 1; // for the time being there is just one vertex
      m_muonneg1_vtx = m_nVertex; // and the muonneg1 is in that one
      if(m_doDebug) {
	std::cout << " * FourMuonEvent::EventSelection * muonneg1 in vertex: " << m_muonneg1_vtx << std::endl;
      }	
      
      // m_muonneg 2
      bool thisMuonIsInExistingVertex = false;
      for (int ivtx=0; ivtx < (int) vtxListX.size(); ivtx++) {
	if ( fabs(vtxListX.at(ivtx) - m_pxIDTrack[m_muonneg2]->vertex()->x()) < rGapCut &&
	     fabs(vtxListZ.at(ivtx) - m_pxIDTrack[m_muonneg2]->vertex()->z()) < m_Z0GapCut) {
	  // muonneg2 in an already listed vertex
	  m_muonneg2_vtx = ivtx+1;
	  thisMuonIsInExistingVertex = true;
	}
      }
      if (thisMuonIsInExistingVertex) {
	if(m_doDebug) {
	  std::cout << " * FourMuonEvent::EventSelection * muonneg2 in vertex: " << m_muonneg2_vtx << std::endl;
	}	
      }
      else {
	m_nVertex += 1; // add new vertex
	m_muonneg2_vtx = m_nVertex; 
	vtxListX.push_back(m_pxIDTrack[m_muonneg2]->vertex()->x());
	vtxListZ.push_back(m_pxIDTrack[m_muonneg2]->vertex()->z());
	if(m_doDebug) {
	  std::cout << " * FourMuonEvent::EventSelection * Add a new vertex to the list. Current size: " << vtxListX.size()
		    << "  vtx.x= " << m_pxIDTrack[m_muonneg2]->vertex()->x() 
		    << "  vtx.y= " << m_pxIDTrack[m_muonneg2]->vertex()->y() 
		    << "  vtx.z= " << m_pxIDTrack[m_muonneg2]->vertex()->z() 
		    << std::endl;
	  std::cout << " * FourMuonEvent::EventSelection * muonneg2 in vertex: " << m_muonneg2_vtx << std::endl;
	}
      }
      
      // m_muonpos 1
      thisMuonIsInExistingVertex = false;
      for (int ivtx=0; ivtx < (int) vtxListX.size(); ivtx++) {
	if ( fabs(vtxListX.at(ivtx) - m_pxIDTrack[m_muonpos1]->vertex()->x()) < rGapCut &&
	     fabs(vtxListZ.at(ivtx) - m_pxIDTrack[m_muonpos1]->vertex()->z()) < m_Z0GapCut) {
	  // muonneg2 in an already listed vertex
	  m_muonpos1_vtx = ivtx+1;
	  thisMuonIsInExistingVertex = true;
	}
      }
      if (thisMuonIsInExistingVertex) {
	if(m_doDebug) {
	  std::cout << " * FourMuonEvent::EventSelection * muonpos1 in vertex: " << m_muonpos1_vtx << std::endl;
	}	
      }
      else {
	m_nVertex += 1; // add new vertex
	m_muonpos1_vtx = m_nVertex; 
	vtxListX.push_back(m_pxIDTrack[m_muonpos1]->vertex()->x());
	vtxListZ.push_back(m_pxIDTrack[m_muonpos1]->vertex()->z());
	if(m_doDebug) {
	  std::cout << " * FourMuonEvent::EventSelection * Add a new vertex to the list. Current size: " << vtxListX.size()
		    << "  vtx.x= " << m_pxIDTrack[m_muonpos1]->vertex()->x() 
		    << "  vtx.y= " << m_pxIDTrack[m_muonpos1]->vertex()->y() 
		    << "  vtx.z= " << m_pxIDTrack[m_muonpos1]->vertex()->z() 
		    << std::endl;
	  std::cout << " * FourMuonEvent::EventSelection * muonpos1 in vertex: " << m_muonpos1_vtx << std::endl;
	}
      }
      
      // m_muonpos 2
      thisMuonIsInExistingVertex = false;
      for (int ivtx=0; ivtx < (int) vtxListX.size(); ivtx++) {
	if ( fabs(vtxListX.at(ivtx) - m_pxIDTrack[m_muonpos2]->vertex()->x()) < rGapCut &&
	     fabs(vtxListZ.at(ivtx) - m_pxIDTrack[m_muonpos2]->vertex()->z()) < m_Z0GapCut) {
	  // muonneg2 in an already listed vertex
	  m_muonpos2_vtx = ivtx+1;
	  thisMuonIsInExistingVertex = true;
	}
      }
      if (thisMuonIsInExistingVertex) {
	if(m_doDebug) {
	  std::cout << " * FourMuonEvent::EventSelection * muonpos2 in vertex: " << m_muonpos1_vtx << std::endl;
	}	
      }
      else {
	m_nVertex += 1; // add new vertex
	m_muonpos2_vtx = m_nVertex; 
	vtxListX.push_back(m_pxIDTrack[m_muonpos2]->vertex()->x());
	vtxListZ.push_back(m_pxIDTrack[m_muonpos2]->vertex()->z());
	if(m_doDebug) {
	  std::cout << " * FourMuonEvent::EventSelection * Add a new vertex to the list. Current size: " << vtxListX.size()
		    << "  vtx.x= " << m_pxIDTrack[m_muonpos2]->vertex()->x() 
		    << "  vtx.y= " << m_pxIDTrack[m_muonpos2]->vertex()->y() 
		    << "  vtx.z= " << m_pxIDTrack[m_muonpos2]->vertex()->z() 
		    << std::endl;
	  std::cout << " * FourMuonEvent::EventSelection * muonpos2 in vertex: " << m_muonpos2_vtx << std::endl;
	}
      }
    } 
    
    if (vertexstatus) {
      if(m_doDebug) std::cout << " * FourMuonEvent::EventSelection ** All muons come from some vertex. " << std::endl
			      << "                                    N listed vertex: " << m_nVertex << std::endl
			      << "                                    muon- 1: " << m_muonneg1_vtx << std::endl
			      << "                                    muon- 2: " << m_muonneg2_vtx << std::endl
			      << "                                    muon+ 1: " << m_muonpos1_vtx << std::endl
			      << "                                    muon+ 2: " << m_muonpos2_vtx << std::endl
			      << "                                    Cut passed :) " << std::endl;      
    }
    else { // vertex cut failed
      if (m_doDebug) std::cout <<" * FourMuonEvent::EventSelection ** Failing all muons coming from a primary vertex cut :(" << std::endl;
      return false;
    }
  }
  
  
  if(m_doDebug) {
    std::cout << " * FourMuonEvent::EventSelection(" << eType << ")*  Good 4-muon set: pt range from  " <<  leadingMuonPt/1000 
	      << " to " << secondMuonPt/1000 
	      << " GeV   4-muon invariant mass = " << m_fInvariantMass[eType] << " GeV " << std::endl;
    std::cout << " * FourMuonEvent::EventSelection(" << eType << ")*  completed * " << std::endl;
  }
  return true;
}

//==================================================================================
void FourMuonEvent::Clear()
{
  m_numberOfFullPassMuons = 0;
  m_passedSelectionCuts   = false;
  m_passedFourMuonSelection = false;
  m_passedFourElectronSelection = false;
  m_passedFourLeptonSelection = false;


  m_FourMuonInvMass = -1.; // flag as no reconstructed inv mass yet
  m_muon1 = MUON1; // point to the first two
  m_muon2 = MUON2;
  m_muonneg1 = -1;
  m_muonneg2 = -1;
  m_muonpos1 = -1;
  m_muonpos2 = -1;


  for ( unsigned int u = 0; u < NUM_MUONS; ++u ) {
      m_pxRecMuon[u] = nullptr;
      m_pxMSTrack[u] = nullptr;
      m_pxMETrack[u] = nullptr;
      m_pxIDTrack[u] = nullptr;
  }
  for ( unsigned int v = 0; v < NUM_TYPES; ++v ) {
    m_fZPt[v]            = -999.9f;
    m_fZEtaDir[v]        = -999.9f;
    m_fZPhiDir[v]        = -999.9f;
    m_fInvariantMass[v]  = -999.9f;
    m_fMuonDispersion[v] = -999.9f;
  }

  // tell us to which vertex the muons are associated 
  m_nVertex = 0; // reset vertex count
  m_muonneg1_vtx = 0;
  m_muonneg2_vtx = 0;
  m_muonpos1_vtx = 0;
  m_muonpos2_vtx = 0;

  return;
}

//==================================================================================
void FourMuonEvent::RecordMuon( const xAOD::Muon* pxMuon )
{
  if(m_doDebug){  std::cout <<" * FourMuonEvent * RecordMuon * started "<< std::endl;}
  // This shouldn't really ever happen but just in case.
  if ( !pxMuon ) {
    if(m_doDebug){  std::cout <<" * FourMuonEvent * RecordMuon * bad pxMuon --> EXIT "<< std::endl;}
    return;
  }

  if ( m_numberOfFullPassMuons < NUM_MUONS ) {
      // The main Muon
      m_pxRecMuon[m_numberOfFullPassMuons] = pxMuon;
      // Tracking Muon Spectrometer ( raw )
      const xAOD::TrackParticle* pxMSTrack   = pxMuon->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
      if (!pxMSTrack) {
	if(m_doDebug){  std::cout <<" * FourMuonEvent * RecordMuon * bad pxMSmuon --> EXIT "<< std::endl;}
	return;
      } 
      m_pxMSTrack[m_numberOfFullPassMuons] = pxMSTrack;

      // Tracking ID ( fix later to include loose match track conditions )
      const xAOD::TrackParticle*  pxIDTrack  = pxMuon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
      if (!pxIDTrack) {
	if(m_doDebug){  std::cout <<" * FourMuonEvent * RecordMuon * bad pxIDTrack for this muon--> EXIT "<< std::endl;}
	return;
      }      
      m_pxIDTrack[m_numberOfFullPassMuons] = pxIDTrack;
      //
      ++m_numberOfFullPassMuons;
  }
  if(m_doDebug){  std::cout <<" * FourMuonEvent * RecordMuon * completed -- return with a total of " << m_numberOfFullPassMuons << std::endl;}
  return;
}


//==================================================================================
bool FourMuonEvent::ReconstructKinematics()
{
  if(m_doDebug){ std::cout << " * FourMuonEvent * ReconstructKinematics * -- start -- " << std::endl; }
  bool kinematicscomputed = false;

  // Three ways. No checks here. Thus make sure the pointers are ok before this.
  if ( m_numberOfFullPassMuons == 4 ) {
    // crosscheck identifiers are good:
    bool goodidentifiers = true;
    if (m_muonneg1 < 0) goodidentifiers = false;
    if (m_muonneg2 < 0) goodidentifiers = false;
    if (m_muonpos1 < 0) goodidentifiers = false;
    if (m_muonpos2 < 0) goodidentifiers = false;

    if (goodidentifiers) {
      // before computing the kinematic parameters check track particles are ok
      bool goodtracks = true;
      if (m_pxIDTrack[m_muonneg1] == nullptr) goodtracks = false;
      if (m_pxIDTrack[m_muonneg2] == nullptr) goodtracks = false;
      if (m_pxIDTrack[m_muonpos1] == nullptr) goodtracks = false;
      if (m_pxIDTrack[m_muonpos2] == nullptr) goodtracks = false;
      
      if (goodtracks) { // Everything is ready
	// For the time being analysis is performed only with ID tracks
	m_fInvariantMass[ID]      = EvalFourMuInvMass( m_pxIDTrack[m_muonpos1], m_pxIDTrack[m_muonpos2], m_pxIDTrack[m_muonneg1], m_pxIDTrack[m_muonneg2] );
	m_fMuonDispersion[ID]     = EvaluateAngle(   m_pxIDTrack[m_muonpos1], m_pxIDTrack[m_muonneg1] );
	m_fZPt[ID]                = EvalPt(          m_pxIDTrack[m_muonpos1], m_pxIDTrack[m_muonneg1] );
	m_fZEtaDir[ID]            = EvalEta(         m_pxIDTrack[m_muonpos1], m_pxIDTrack[m_muonneg1] );
	m_fZPhiDir[ID]            = EvalPhi(         m_pxIDTrack[m_muonpos1], m_pxIDTrack[m_muonneg1] );
	kinematicscomputed = true;

	if(m_doDebug){ 
	  std::cout << " * FourMuonEvent * ReconstructKinematics * -- ID Tracks -- " << std::endl
		    << "                                           Pt(mu1+)["<< m_muonpos1<<"] = " << m_pxIDTrack[m_muonpos1]->pt() << std::endl
		    << "                                           Pt(mu2+)["<< m_muonpos2<<"] = " << m_pxIDTrack[m_muonpos2]->pt() << std::endl
		    << "                                           Pt(mu1-)["<< m_muonneg1<<"] = " << m_pxIDTrack[m_muonneg1]->pt() << std::endl
		    << "                                           Pt(mu2-)["<< m_muonneg2<<"] = " << m_pxIDTrack[m_muonneg2]->pt() << std::endl
		    << "                                           invariant mass (ID) = " << m_fInvariantMass[ID] << std::endl
		    << std::endl; 
	}
      } // good tracks  
    } // goodidentifiers
  } // goodmuons == 4
  
  if (!kinematicscomputed) {
    if(m_doDebug){  std::cout <<" * FourMuonEvent * ReconstructKinematics * -- FAILED -- " << std::endl; }
  }

  if(m_doDebug){  std::cout <<" * FourMuonEvent * ReconstructKinematics * -- completed -- status: " << kinematicscomputed << std::endl; }
  return kinematicscomputed;
}

//==================================================================================
float FourMuonEvent::getPtImbalance( ZTYPE eType )
{
  // First determine what's positive
  if ( m_numberOfFullPassMuons == 2 )
    {
      switch ( eType )
	{
	case MS :
	  {
	    return EvalPtDiff( m_pxMSTrack[m_muon1], m_pxMSTrack[m_muon2] );
	  }
	case ME:
	  {
	    return EvalPtDiff( m_pxMETrack[m_muon1], m_pxMETrack[m_muon2] );
	  }
	case CB:
	  {
	    return EvalPtDiff( m_pxRecMuon[m_muon1], m_pxRecMuon[m_muon2] );
	  }
	case ID:
	  {
	    return EvalPtDiff( m_pxIDTrack[m_muon1], m_pxIDTrack[m_muon2] );
	  }
	default:
	  return -999.0;
	}
    }
  else
    {
      return -999.0;
    }
}

//==================================================================================
int FourMuonEvent::getZCharge( ZTYPE eType )
{
  switch ( eType )
    {
    case MS :
      {
	return ( static_cast<int>( EvalCharge( m_pxMSTrack[m_muon1], m_pxMSTrack[m_muon2] ) ) );
      }
    case ME:
      {
	return ( static_cast<int>( EvalCharge( m_pxMETrack[m_muon1], m_pxMETrack[m_muon2] ) ) );
      }
    case CB:
      {
	return ( static_cast<int>( EvalCharge( m_pxRecMuon[m_muon1], m_pxRecMuon[m_muon2] ) ) );
      }
    case ID:
      {
	return ( static_cast<int>( EvalCharge( m_pxIDTrack[m_muon1], m_pxIDTrack[m_muon2] ) ) );
      }
    default:
      return -999;
    }
}

//==================================================================================
unsigned int FourMuonEvent::getPosMuon( int eType )
{
  //if ( getNumberOfTaggedMuons() != 2 ) return 999;
  //if ( getZCharge(eType) != 0        ) return 999;

  unsigned int muid = m_muonpos1;
  if (eType==2) muid = m_muonpos2;
  return muid;
}

//==================================================================================
unsigned int FourMuonEvent::getNegMuon( int eType )
{
  unsigned int muid = m_muonneg1;
  if (eType==2) muid = m_muonneg2;
  return muid;
}

//==================================================================================
const xAOD::TrackParticle*  FourMuonEvent::getLooseIDTk( unsigned int /*uPart*/ )
{
  const xAOD::TrackParticleContainer*  pxTrackContainer =
    PerfMonServices::getContainer<xAOD::TrackParticleContainer>( PerfMonServices::TRK_COLLECTION );

  if ( pxTrackContainer )
    {
      xAOD::TrackParticleContainer::const_iterator xTrkItr  = pxTrackContainer->begin();
      xAOD::TrackParticleContainer::const_iterator xTrkItrE  = pxTrackContainer->end();
      while ( xTrkItr != xTrkItrE )
	{
	  const xAOD::TrackParticle* pxTrack = *xTrkItr;
	  if(!(pxTrack->track())) continue;
	  const Trk::Track* pxTrkTrack = pxTrack->track();
	  if ( !pxTrack ) continue;
	  const Trk::Perigee* pxPerigee = pxTrkTrack->perigeeParameters() ;
	  if ( !pxPerigee ) continue;

	  // const float fTrkPt    = pxPerigee->pT()*1.0e-3;
	  // const float fTrkPtErr = fabs( pxPerigee->localErrorMatrix().error(Trk::qOverP) );
	  // const float fPtSig    = ( 1.0f / pxPerigee->pT() ) /  fTrkPtErr;  // Potential problem.
	  const float fTrkPhi   = pxPerigee->parameters()[Trk::phi];
	  const float fTrkEta   = pxPerigee->eta();

	  float fDPhi = fabs( fTrkPhi -  m_pxMETrack[m_muon1]->phi() );
	  float fDEta = fabs( fTrkEta -  m_pxMETrack[m_muon2]->eta() );
	  float fDR = sqrt( fDPhi*fDPhi + fDEta*fDEta );

	  if ( fDR < 0.3f )
	    {
	      return pxTrack;
	    }

	  xTrkItr++;
	}
    }
  // if ()
  return nullptr;
}

//==================================================================================
void FourMuonEvent::SetLeadingMuonPtCut (double newvalue)
{
  // first set the new pt cut value
  m_LeadingMuonPtCut = newvalue;
  
  // the second muon pt cut can not be higher than the leading muon pt cut:
  if (m_LeadingMuonPtCut < m_SecondMuonPtCut) this->SetSecondMuonPtCut(m_LeadingMuonPtCut);

  // this has to be translated to the MuonSelector
  // but there one has to use the minimum momentum --> second muon
  //this->SetMuonPtCut(m_SecondMuonPtCut);
  if(m_doDebug){ 
    std::cout <<" * FourMuonEvent * SetLeadingMuonPtCut * new Pt cuts:  " << m_LeadingMuonPtCut << " & " 
	      << m_SecondMuonPtCut 
	      << "  MuonSelector: " << m_xMuonID.GetPtCut() << std::endl;
  }
  return;
}

//==================================================================================
void FourMuonEvent::SetSecondMuonPtCut (double newvalue) 
{
  m_SecondMuonPtCut = newvalue;

  // second muon pt shouldn't be higher than the leading muon pt
  if (m_LeadingMuonPtCut < m_SecondMuonPtCut) this->SetLeadingMuonPtCut(m_LeadingMuonPtCut);

  // this has to be translated to the MuonSelector
  this->SetMuonPtCut(m_SecondMuonPtCut);

  if(m_doDebug) {
    std::cout <<" * FourMuonEvent * SetSecondMuonPtCut * new Pt cuts:  " << m_LeadingMuonPtCut 
	      << " & " << m_SecondMuonPtCut 
	      << "  MuonSelector: " << m_xMuonID.GetPtCut() << std::endl;
  }

  return;
}

//==================================================================================
void FourMuonEvent::OrderMuonList()
{
  if(m_doDebug){ std::cout << " * FourMuonEvent::OrderMuonList * -- start -- " << std::endl 
			   << "                                  #muons: " << m_numberOfFullPassMuons<< std::endl;}

  int muPlus1Id = -9;
  int muPlus2Id = -9;
  int muMinus1Id = -9;
  int muMinus2Id = -9;
  double muPlus1Pt = 0.;
  double muPlus2Pt = 0.;
  double muMinus1Pt = 0.;
  double muMinus2Pt = 0.;

  int muposcount = 0;
  int munegcount = 0;

  int nMuonsAtEntry = m_numberOfFullPassMuons;
  m_numberOfFullPassMuons = 0; // reset the number of full pass muons

  if (nMuonsAtEntry >= 2) { // we need at least 2 muons
    for (int imuon=0; imuon < (int) nMuonsAtEntry; imuon++) {
      if(m_doDebug){ std::cout << " * FourMuonEvent::OrderMuonList * testing imuon= " << imuon 
			       << "   with charge= " << m_pxRecMuon[imuon]->charge()
			       << "   and pt= " << m_pxRecMuon[imuon]->pt()
			       << std::endl;
      }
      if (m_pxRecMuon[imuon] != nullptr) {
	
	if (m_pxRecMuon[imuon]->charge()==1) { // positive muon
	  muposcount++;
	  if (m_pxRecMuon[imuon]->pt()> muPlus1Pt) {
	    // store 1st in 2nd
	    muPlus2Pt = muPlus1Pt;
	    muPlus2Id = muPlus1Id;
	    // now store the new one in 1st place
	    muPlus1Pt = m_pxRecMuon[imuon]->pt();
	    muPlus1Id = imuon;	
	  } 
	  else if (m_pxRecMuon[imuon]->pt()> muPlus2Pt) {
	    // store the new one in 2nd place
	    muPlus2Pt = m_pxRecMuon[imuon]->pt();
	    muPlus2Id = imuon;
	  }
	}
	// Negative muons
	if (m_pxRecMuon[imuon]->charge()==-1) {
	  munegcount++;
	  if(m_pxRecMuon[imuon]->pt()> muMinus1Pt) {
	    // store 1st in 2nd
	    muMinus2Pt = muMinus1Pt;
	    muMinus2Id = muMinus1Id;
	    muMinus1Pt = m_pxRecMuon[imuon]->pt();
	    muMinus1Id = imuon;
	  } 
	  else if(m_pxRecMuon[imuon]->pt()> muMinus2Pt) {
	    muMinus2Pt = m_pxRecMuon[imuon]->pt();
	    muMinus2Id = imuon;
	  }
	}
      } // muon exist
    } // for (int imuon)
  } // if (nMuonsAtEntry >= 2)

  if (muPlus1Id>=0) {m_muonpos1 = muPlus1Id; m_numberOfFullPassMuons++;}
  if (muPlus2Id>=0) {m_muonpos2 = muPlus2Id; m_numberOfFullPassMuons++;}
  if (muMinus1Id>=0) {m_muonneg1 = muMinus1Id; m_numberOfFullPassMuons++;}
  if (muMinus2Id>=0) {m_muonneg2 = muMinus2Id; m_numberOfFullPassMuons++;}

  m_muon1 = m_muonpos1; // to be deleted when no more m_muon is left
  m_muon2 = m_muonneg1; // to be deleted when no more m_muon is left

  if (m_doDebug && m_numberOfFullPassMuons >= 2){ 
    std::cout << " * FourMuonEvent::OrderMuonList * taking " << m_numberOfFullPassMuons << "  muons from the input list of " << nMuonsAtEntry << " muons: " << std::endl;
    if (muMinus1Id >= 0) std::cout << "                                  leading mu-: " << muMinus1Id << "   Pt = " << muMinus1Pt << std::endl;
    if (muMinus2Id >= 0) std::cout << "                                  second  mu-: " << muMinus2Id << "   Pt = " << muMinus2Pt << std::endl;
    if (muPlus1Id >= 0)  std::cout << "                                  leading mu+: " << muPlus1Id  << "   Pt = " << muPlus1Pt << std::endl;
    if (muPlus2Id >= 0)  std::cout << "                                  second  mu+: " << muPlus2Id  << "   Pt = " << muPlus2Pt << std::endl;
  }
  else {
    if(m_doDebug) std::cout << " * FourMuonEvent::OrderMuonList * This event has less than 2 muons :("  << std::endl;
  }
  
  if(m_doDebug) std::cout << " * FourMuonEvent::OrderMuonList * m_numberOfFullPassMuons= " << m_numberOfFullPassMuons << std::endl;
  if(m_doDebug) std::cout << " * FourMuonEvent::OrderMuonList * completed * " << std::endl;
  return;
}
