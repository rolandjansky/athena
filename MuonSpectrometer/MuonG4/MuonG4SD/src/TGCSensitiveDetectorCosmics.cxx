/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonG4SD/TGCSensitiveDetectorCosmics.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include <iostream>
#include <string>
#include "MuonSimEvent/TgcHitIdHelper.h"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

using namespace FADS;

//comment the following line if you want hit information print out
//#define TGCG4_DEBUG


// Intitialization of static data
static SensitiveDetectorEntryT<TGCSensitiveDetectorCosmics> tgcsd("TGCSens_Cosmics");


// construction/destruction
TGCSensitiveDetectorCosmics::TGCSensitiveDetectorCosmics(std::string s) : 
  FadsSensitiveDetector(s),
  momMag(0), 
  m_globalTime(0),
  myTGCHitColl(0) 
{
	muonHelper = TgcHitIdHelper::GetHelper();
}

// Implemenation of member functions
void TGCSensitiveDetectorCosmics::Initialize(G4HCofThisEvent*) 
{
	myTGCHitColl = m_hitCollHelp.RetrieveNonconstCollection<TGCSimHitCollection>("TGC_Hits");
	mom = Amg::Vector3D(0.,0.,0.);
	globH = Amg::Vector3D(0.,0.,0.);
}


G4bool TGCSensitiveDetectorCosmics::ProcessHits(G4Step* aStep,G4TouchableHistory*) {
    
    G4Track* track = aStep->GetTrack();
    
    if (track->GetDefinition()->GetPDGCharge() == 0.0) {
	 if (track->GetDefinition()!=G4Geantino::GeantinoDefinition()) return true;
	 else if (track->GetDefinition()==G4ChargedGeantino::ChargedGeantinoDefinition()) return true;
    }
    G4TouchableHistory*     touchHist = (G4TouchableHistory*)aStep->GetPreStepPoint()->GetTouchable();	
    G4ThreeVector           position  = aStep->GetPreStepPoint()->GetPosition();
    const G4AffineTransform trans     = track->GetTouchable()->GetHistory()->GetTopTransform();
    
    
    // helps computing 'gasGap' FIXME
    //    std::string technology = "";

    //
    // davide costanzo 01/05/05 Get geometry info from the volume name
    //
    if(m_layout == "") {
       m_layout="P03"; // default layout
         for (int i=touchHist->GetHistoryDepth();i>=0;i--) {
           std::string volName = touchHist->GetVolume(i)->GetName();
             if ((volName.find("Q02")) != std::string::npos) m_layout="Q02";
         }
//       ATH_MSG_INFO("TGCSensitiveDetectorCosmics: Layout set to: " << m_layout );
    }
    
    // fields for the TGC identifier construction
    std::string stationName;
    int stationEta(0);
    int stationPhi(0);
    int gasGap = 0;
    
    // TGC hit information
    double globalTime        = aStep->GetPreStepPoint()->GetGlobalTime();
    Amg::Vector3D localPosition = Amg::Hep3VectorToEigen(trans.TransformPoint(position));
    G4ThreeVector direcos    = aStep->GetPreStepPoint()->GetMomentumDirection();
    Amg::Vector3D localDireCos  = Amg::Hep3VectorToEigen(trans.TransformAxis(direcos));

    // global coordinates
    G4ThreeVector globVrtx = aStep->GetPreStepPoint()->GetPosition();

    // distance of the hit from (0,0,0) vertex - calculated from the PreStepPoint (approximation)
    double dist = globVrtx.mag();
    double lightspeed = 299.792458; /* in vacuo speed of light [mm/ns] */
    double tOrigin = dist / lightspeed;

    G4int trackid = aStep->GetTrack()->GetTrackID();
    currVertex = Amg::Hep3VectorToEigen(aStep->GetTrack()->GetVertexPosition());
    
    // for cosmics: only primary muon tracks - track momentum when first entering the spectrometer (one muon per event)
    if ((currVertex != vertex) && (trackid == 1)) {
       // after calculationg the momentum magnidude, normalize it 
       mom = Amg::Hep3VectorToEigen(track->GetMomentum());
       momMag = mom.mag();
       mom.normalize();
       // the direction of the primary mu is used to calculate the t0, the position ot the t0, globH, is ONE for a track
       Amg::Vector3D globVrtxFix = Amg::Hep3VectorToEigen( globVrtx );
       double AlphaGlobal = -1*(globVrtxFix[0]*mom[0] + globVrtxFix[1]*mom[1] + globVrtxFix[2]*mom[2])/(mom[0]*mom[0] + mom[1]*mom[1] + mom[2]*mom[2]);   
       globH = globVrtxFix + AlphaGlobal*mom;     
       ATH_MSG_INFO("COSMICS MAIN TRACK IN THE MDT!"); 
       }  
    double globalDist = sqrt((globH[0] - globVrtx[0])*(globH[0] - globVrtx[0]) +
  			   (globH[1] - globVrtx[1])*(globH[1] - globVrtx[1]) +
  			   (globH[2] - globVrtx[2])*(globH[2] - globVrtx[2]));
    double tof = globalDist / lightspeed;

    // scan geometry tree to identify hit channel
    int zside(0);
    for (int i=touchHist->GetHistoryDepth();i>=0;i--) {
	
      std::string::size_type npos;
	std::string volName = touchHist->GetVolume(i)->GetName();

	// stationName and stationPhi	
	if ((npos = volName.find("station")) != std::string::npos) {
	    
	    stationName   = volName.substr(0,npos-2);
	    int volCopyNo = touchHist->GetVolume(i)->GetCopyNo();
	    if (volCopyNo > 0) {
	        zside = 1;
	    } else {
	        zside = -1;
	    }
	    if (stationName.substr(2,1) == "F") { 

	      stationPhi    = (abs(volCopyNo%100)-1)*3;
	      if (abs(volCopyNo/100) > 3) {
		stationPhi += abs(volCopyNo/100)-3;
	      } else {
		stationPhi += abs(volCopyNo/100);
	      }

	      if (m_layout == "Q02") {
		stationPhi -= 1;
		if (stationPhi <= 0) {
		  stationPhi = 24 - stationPhi;
		}
	      }

	    } else if (stationName.substr(2,1) == "E") {
	      if (stationName.substr(1,1) == "4") {

		stationPhi = (abs(volCopyNo%100)-1)*3+abs(volCopyNo/100);

		if (m_layout == "Q02") { // layout Q02
		  if (abs(volCopyNo%100) < 4) {
		    stationPhi = stationPhi - 1;
		    if (stationPhi <= 0) {
		      stationPhi = 21 - stationPhi;
		    }
		  }
		  else if(abs(volCopyNo%100) < 7) {
		    stationPhi = stationPhi - 1 - 1;
		  }
		  else {
		    stationPhi = stationPhi - 2 - 1;
		  }
		}

	      }
	      else {

		stationPhi = (abs(volCopyNo%100)-1)*6+abs(volCopyNo/100);

		if (m_layout == "Q02") {
		  stationPhi -= 2;
		  if (stationPhi <= 0) {
		    stationPhi = 48 - stationPhi;
		  }

		}
	      }
	    }

	}
	
	// stationEta
	else if ((npos = volName.find("tgccomponent")) != std::string::npos) {
            int volCopyNo = abs(touchHist->GetVolume(i)->GetCopyNo());
	    stationEta = zside*volCopyNo%100;
	    if (volCopyNo > 1000) { // stationPhi overridden by the number assigned by MuonGeoModel
	      stationPhi = volCopyNo/1000;
	    }
	}

	// gasGap
	else if ((npos = volName.find("Gas Volume Layer")) != std::string::npos) {
            int volCopyNo = touchHist->GetVolume(i)->GetCopyNo();
	    int iStation = atoi(stationName.substr(1,1).c_str());
	    if (zside < 0) {
                if (iStation == 1)
                    gasGap = 3-(volCopyNo-3)/4;
		else
                    gasGap = 2-(volCopyNo-3)/4;
	    }
	    else {
                if (iStation == 1)
                    gasGap = (volCopyNo-3)/4+1;
		else
                    gasGap = (volCopyNo-3)/4+1;
	    }
	}
	else if ((npos = volName.find("TGCGas")) != std::string::npos) {
	    
	    std::string currentTech = volName.substr(0,4);
	    int volCopyNo = touchHist->GetVolume(i)->GetCopyNo();

	    if (volCopyNo != 0)
	        gasGap = volCopyNo;
	    //	    technology = currentTech;
	}		

    }

    //                ATH_MSG_INFO("TGCSensitiveDetectorCosmics : stationName/stationEta/stationPhi/gasGap : "
    //            	      << stationName << "/"
    //            	      << stationEta << "/"
    //            	      << stationPhi << "/"
    //            	      << gasGap << " "
    //            	      << position[0] << "/"
    //            	      << position[1] << "/"
    //            	      << position[2] << " "
    //            	      << localPosition[0] << "/"
    //            	      << localPosition[1] << "/"
    //            	      << localPosition[2] << " "
    //            	      );
//construct the hit identifier	    
    HitID TGCid = muonHelper->BuildTgcHitId(stationName,
						stationPhi, 
						stationEta,
						gasGap);
    //muonHelper->Print(TGCid);
    vertex = Amg::Hep3VectorToEigen(aStep->GetTrack()->GetVertexPosition());
    // if the track vertex is far from (0,0,0), takes the tof, otherwise take the "usual" g4 globalTime
    ((((vertex.mag()) < 100) || ((fabs(globalTime - tOrigin)) < 0.1) ) ? (m_globalTime  = globalTime) : (m_globalTime = tof));
    // if m_globalTime  != globalTime and m_globalTime != tof in the output, this is due to multiple hits
    // before founding the good one (small approximation)

	
// construct new mdt hit

     TGCSimHit* newHit = new TGCSimHit(TGCid,
				       m_globalTime,
				       localPosition,
				       localDireCos,
				       trackid,
				       aStep->GetTotalEnergyDeposit(),
				       aStep->GetStepLength());

    myTGCHitColl->Insert(*newHit );
	delete newHit;

//  #ifndef TGCG4_DEBUG
// 
// // printouts for cosmics
//         ATH_MSG_INFO("------------------TGC------------------");
// 	ATH_MSG_INFO("Track "<<trackid);
// 	ATH_MSG_INFO("Track vertex "<<vertex[0]<<" " <<vertex[1]<<" " <<vertex[2]);
// 	ATH_MSG_INFO("Global position of the hit " << globVrtx[0] <<" " << globVrtx[1] <<" " << globVrtx[2] );
//         ATH_MSG_INFO("Distance from (0,0,0) and time " << dist << " " <<tOrigin );
//         ATH_MSG_INFO("Momentum "<<momMag);
// 	ATH_MSG_INFO("Momentum director cosines " <<mom[0]<<" " <<mom[1]<<" " <<mom[2]); 
// 	ATH_MSG_INFO("Eta and phi "<<mom.eta()<<" " <<mom.phi());
//         ATH_MSG_INFO("Closest approach position and distance from (0,0,0) " 
// 	          << globH[0] <<" "<<globH[1]<<" "<<globH[2]<<" "
// 	          << sqrt(globH[0]*globH[0] + globH[1]*globH[1] + globH[2]*globH[2]) ); 
//         ATH_MSG_INFO("Distance from t0 and tof " << globalDist <<" " <<tof ); 
//         ATH_MSG_INFO("g4 globalTime " << globalTime );
//         ATH_MSG_INFO("Time " << m_globalTime );
// 
// 
//          ATH_MSG_INFO("TRI "<<muonHelper->GetStationName(TGCid)
//                    << " "<<muonHelper->GetFieldValue("StationEta")
//                    << " "<<muonHelper->GetFieldValue("StationPhi")
//                    << " "<<muonHelper->GetFieldValue("GasGap") );
//  	
//  	//ATH_MSG_INFO(stationName<<" "<<newHit->print());
//  #endif

    return true;
}


void TGCSensitiveDetectorCosmics::EndOfEvent(G4HCofThisEvent* /*HCE*/) {
  if (!m_allowMods)
	m_hitCollHelp.SetConstCollection(myTGCHitColl);
} 
