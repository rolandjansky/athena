/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonG4SD/MDTSensitiveDetectorCosmics.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "MuonSimEvent/MdtHitIdHelper.h"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"

#include <string>
#include <iostream>
#include <limits>

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

using namespace FADS;

typedef std::istringstream my_isstream;

//comment the following line if you want hit information print out
//#define MDTG4_DEBUG


double MDTSensitiveDetectorCosmics::DEFAULT_TUBE_RADIUS = std::numeric_limits<double>::max();
// Initialize static data
static SensitiveDetectorEntryT<MDTSensitiveDetectorCosmics> mdtsd("MDTSens_Cosmics");

// construction/destruction
MDTSensitiveDetectorCosmics::MDTSensitiveDetectorCosmics(std::string name)
  : FadsSensitiveDetector(name),
    momMag(0.), 
    m_MDTHitColl(0),
    m_driftRadius(0.),
    m_globalTime(0.)
{
	m_muonHelper = MdtHitIdHelper::GetHelper();
	//m_muonHelper->PrintFields();
}


// Implemenation of memebr functions
void MDTSensitiveDetectorCosmics::Initialize(G4HCofThisEvent*) 
{
    //m_MDTHitColl = new MDTSimHitCollection("MDT_Hits");
    m_MDTHitColl = m_hitCollHelp.RetrieveNonconstCollection<MDTSimHitCollection>("MDT_Hits");
    m_driftRadius = DEFAULT_TUBE_RADIUS;
    mom = Amg::Vector3D(0.,0.,0.);
    globH = Amg::Vector3D(0.,0.,0.);
}

G4bool MDTSensitiveDetectorCosmics::ProcessHits(G4Step* aStep,G4TouchableHistory* /*ROHist*/) {
  
  G4Track* currentTrack = aStep->GetTrack(); 
    
  // MDTs sensitive to charged particle only
     if (currentTrack->GetDefinition()->GetPDGCharge() == 0.0) {
	 if (currentTrack->GetDefinition()!=G4Geantino::GeantinoDefinition()) return true;
	 else if (currentTrack->GetDefinition()==G4ChargedGeantino::ChargedGeantinoDefinition()) return true;
     }
   
  G4VPhysicalVolume*  physVolPostStep = aStep->GetPostStepPoint()->GetPhysicalVolume();
  if (0 == physVolPostStep)  return true;

  // hit information to be recorded
  double     globalTime;
  double     driftRadius;
  Amg::Vector3D localPosition;
  localPosition.setZero();
  	    
  // get top transformation
  const G4AffineTransform trans = currentTrack->GetTouchable()->GetHistory()->GetTopTransform();

  // global coordinates
  G4ThreeVector globVrtx = aStep->GetPreStepPoint()->GetPosition();

  // distance of the hit from (0,0,0) vertex - calculated from the PreStepPoint (approximation)
  double dist = globVrtx.mag();
  double lightspeed = 299.792458; /* in vacuo speed of light [mm/ns] */
  double tOrigin = dist / lightspeed;

  G4int trackid = aStep->GetTrack()->GetTrackID();
  currVertex = Amg::Hep3VectorToEigen( aStep->GetTrack()->GetVertexPosition() );
    
  // for cosmics: only primary muon tracks - track momentum when first entering the spectrometer (one muon per event)
  if ((currVertex != vertex) && (trackid == 1)) {
     // after calculationg the momentum magnidude, normalize it 
     mom = Amg::Hep3VectorToEigen( currentTrack->GetMomentum() );
     momMag = mom.mag();
     mom.normalize();
     // the direction of the primary mu is used to calculate the t0, the position ot the t0, globH, is ONE for a track
     Amg::Vector3D globVrtxFix = Amg::Hep3VectorToEigen( globVrtx );
     double AlphaGlobal = -1*(globVrtxFix[0]*mom[0] + globVrtxFix[1]*mom[1] + globVrtxFix[2]*mom[2])/(mom[0]*mom[0] + mom[1]*mom[1] + mom[2]*mom[2]);   
     globH = globVrtxFix + AlphaGlobal*mom;     
//     std::cout << "COSMICS MAIN TRACK IN THE MDT!"<<std::endl; 
     }  
  double globalDist = sqrt((globH[0] - globVrtx[0])*(globH[0] - globVrtx[0]) +
  			   (globH[1] - globVrtx[1])*(globH[1] - globVrtx[1]) +
  			   (globH[2] - globVrtx[2])*(globH[2] - globVrtx[2]));
  double tof = globalDist / lightspeed;
  
  // transform pre and post step positions to local positions
  Amg::Vector3D localVertex1( Amg::Hep3VectorToEigen( trans.TransformPoint(aStep->GetPreStepPoint()->GetPosition()) ) );
  Amg::Vector3D localVertex2( Amg::Hep3VectorToEigen( trans.TransformPoint(aStep->GetPostStepPoint()->GetPosition()) ) );
  
  // calculate local direction from begin- and end-point of the step
  Amg::Vector3D localDirection( (localVertex2 - localVertex1) );  // normalized
  localDirection.z() = 0.;       // look in xy-plane 


  // See if particle passed wire by projecting begin- and end-point on the step direction
  if( (localVertex2.dot(localDirection)) * (localVertex1.dot(localDirection)) < 0 ) { // particle passed wire

    // compute drift radius ( = impact parameter)
    double Xpos = localVertex1[0];
    double Ypos = localVertex1[1];
    double Xdir = localDirection[0];
    double Ydir = localDirection[1];

    double Alpha = -1*(Xpos*Xdir + Ypos*Ydir)/(Xdir*Xdir + Ydir*Ydir);   // localPosition*localDirection
    
    localPosition = localVertex1 + Alpha*(localVertex2-localVertex1);
    driftRadius = localPosition.perp();
    globalTime = aStep->GetPreStepPoint()->GetGlobalTime();  // take pre step time

  }else{   // particle didn't pass wire, one of the end-points is the shortest distance to the wire

    // calculate which of the two end-points is closer to the wire
    double dist1 = localVertex1.perp();
    double dist2 = localVertex2.perp();
    if( dist1 < dist2 ){  // first is closer
      driftRadius = dist1;
      localPosition = localVertex1;
      globalTime = aStep->GetPreStepPoint()->GetGlobalTime();   
    }else{                // second is closer
      driftRadius = dist2;
      localPosition = localVertex2;
      globalTime = aStep->GetPostStepPoint()->GetGlobalTime();   
    }
  }
  vertex = Amg::Hep3VectorToEigen( aStep->GetTrack()->GetVertexPosition() );

  if( driftRadius < m_driftRadius ){  // check if current step came closer to the wire than the previous ones
    m_driftRadius = driftRadius;
    // if the track vertex is far from (0,0,0), takes the tof, otherwise take the "usual" g4 globalTime
    ((((vertex.mag()) < 100) || ((fabs(globalTime - tOrigin)) < 0.1) ) ? (m_globalTime  = globalTime) : (m_globalTime = tof));
    // if m_globalTime  != globalTime and m_globalTime != tof in the output, this is due to multiple hits
    // before founding the good one (small approximation)
    m_localPosition = localPosition;
  }
  
  // check if particle left tube or stopped in tube
  G4String  namePreStepMat  = aStep->GetPreStepPoint()->GetMaterial()->GetName();
  G4String  namePostStepMat = aStep->GetPostStepPoint()->GetMaterial()->GetName();
  G4String  nameSD          = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetSensitiveDetector()->GetName();	  

  // see if we were in the sensitive volume and left it, or the particle was killed
  if( ((nameSD) && (namePreStepMat != namePostStepMat)) || (currentTrack->GetTrackStatus() ==  fStopAndKill)){

    // get identifier
    G4TouchableHistory* touchHist = (G4TouchableHistory*)aStep->GetPreStepPoint()->GetTouchable();
    int MDTid = GetIdentifier(touchHist);

    // construct new mdt hit
    MDTSimHit* nHit = new MDTSimHit(MDTid, m_globalTime, m_driftRadius, m_localPosition, trackid, 
				    aStep->GetStepLength(),
				    aStep->GetTotalEnergyDeposit(), 
				    currentTrack->GetDefinition()->GetPDGEncoding(), 
				    aStep->GetPreStepPoint()->GetKineticEnergy());
    m_MDTHitColl->Insert(*nHit );
	delete nHit;
    
// #ifndef MDTG4_DEBUG
// 
// // printouts for cosmics
//         std::cout<<"------------------MDT------------------"<<std::endl;
// 		  std::cout << "Track "<<trackid<<std::endl;
// 	     std::cout << "Track vertex "<<vertex[0]<<" " <<vertex[1]<<" " <<vertex[2]<<std::endl;
// 	     std::cout << "Global position of the hit " << globVrtx[0] <<" " << globVrtx[1] <<" " << globVrtx[2] <<std::endl;
//         std::cout << "Distance from (0,0,0) and time " << dist << " " <<tOrigin <<std::endl;
//         std::cout << "Momentum "<<momMag<<std::endl;
// 	     std::cout << "Momentum director cosines " <<mom[0]<<" " <<mom[1]<<" " <<mom[2]<<std::endl; 
// 	     std::cout << "Eta and phi "<<mom.eta()<<" " <<mom.phi()<<std::endl;
//         std::cout << "Closest approach position and distance from (0,0,0) " 
// 	          << globH[0] <<" "<<globH[1]<<" "<<globH[2]<<" "
// 	          << sqrt(globH[0]*globH[0] + globH[1]*globH[1] + globH[2]*globH[2]) << std::endl; 
//         std::cout << "Distance from t0 and tof " << globalDist <<" " <<tof <<std::endl; 
//         std::cout << "g4 globalTime " << globalTime <<std::endl;
//         std::cout << "Time " << m_globalTime <<std::endl;
// 
//    	  std::cout<<"TUB "<<m_muonHelper->GetStationName(MDTid);	
//  	     std::cout<<" "<<m_muonHelper->GetFieldValue("PhiSector");
//  	     std::cout<<" "<<m_muonHelper->GetFieldValue("ZSector");
//  	     std::cout<<" "<<m_muonHelper->GetFieldValue("MultiLayer");
//  	     std::cout<<" "<<m_muonHelper->GetFieldValue("Layer");
//  	     std::cout<<" "<<m_muonHelper->GetFieldValue("Tube")<<std::endl;
//   	    
//    	  std::cout<<m_muonHelper->GetStationName(MDTid)<<" "<<nHit->print()<<std::endl;
//  	
//  #endif

    m_driftRadius = DEFAULT_TUBE_RADIUS; // reset start value of driftRadius
  }
  return true;
}


void MDTSensitiveDetectorCosmics::EndOfEvent(G4HCofThisEvent* /*HCE*/) {
  if (!m_allowMods)
    m_hitCollHelp.SetConstCollection(m_MDTHitColl);
} 

int MDTSensitiveDetectorCosmics::GetIdentifier(G4TouchableHistory* touchHist)
{
  // attributes of the MDT identifier construction
  std::string stationName;
  int         stationEta=0;
  int         stationPhi=0;
  int         multilayer=0;
  int         tubeLayer=0;
  int         tube=0;
  
  bool isAssembly = false;
  // scan geometry tree to identify the tube
  for (int i = touchHist->GetHistoryDepth(); i>=0; i--) {

    std::string::size_type npos;
    std::string::size_type loc1;
    std::string::size_type loc2;
    std::string volName = touchHist->GetVolume(i)->GetName();

    // check if this station is an assembly 
    if ((npos = volName.find("av_")) != std::string::npos &&
        (npos = volName.find("impr_")) != std::string::npos)  isAssembly = true;
    
    
    // station: name, eta and phi (-> chamber!)
    if ((npos = volName.find("station")) != std::string::npos && (!isAssembly)) {

      volName       = volName.substr(0,npos-2);
      int volCopyNo = touchHist->GetVolume(i)->GetCopyNo();
      volCopyNo=volCopyNo%1000; 
      stationName   = volName;
      stationEta    = volCopyNo/100;
      stationPhi    = abs(volCopyNo%100);

    }
    else if ((npos = volName.find("MDT")) != std::string::npos && isAssembly) {
        
        // vol name for Assembly components are
        // av_WWW_impr_XXX_Muon::BMSxMDTxx_pv_ZZZ_NAME
        //                                               where WWW is ass. istance nr.
        //                                                     XXX is comp. imprint nr.
        //                                                     BMSxMDTxx is the name of the comp. log.Vol.
        //                                                        x station sub-type; xx technology subtype
        //                                                     ZZZ is the comp. number of order
        //                                                     NAME is the comp. tag (geoIdentifierTag)
        //                                                          for MDTs NAME is ml[1] or ml[2]
        // copy numbers for Ass.components are =
        //                CopyNoBase(= geoIdentifierTag of the assembly) + geoIdentifierTag of the component
        //                geoIdentifierTag of the component = Job
        //                geoIdentifierTag of the assembly = (sideC*10000 +
        //                             mirsign*1000 + abs(zi)*100 + fi+1)*100000;
        //                             mirsign*1000 + abs(zi)*100 + fi+1)*100000;
        // 
        if ((loc1 = volName.find("Muon::")) != std::string::npos) 
        {
            stationName = volName.substr(loc1+6,3);
        }

        int copyNr = touchHist->GetVolume(i)->GetCopyNo();
        int copyNrBase = int(copyNr/100000);
        int sideC  = int(copyNrBase/10000);
        int zi     = int((copyNrBase%1000)/100);
        int fi     = int(copyNrBase%100);
        if (sideC == 1) zi = -zi;
        stationEta = zi;
        stationPhi = fi;
        
        int gmID = 0;
        if ((loc1 = volName.find("[")) != std::string::npos) 
        {
            if ((loc2 = volName.find("]", loc1+1)) != std::string::npos) 
            {
                my_isstream istrvar(volName.substr(loc1+1,loc2-loc1-1));
                istrvar>>gmID;
            }
        }
        multilayer = gmID;
//         std::cout<<"Mdt SD: swimming: found <MDT>"<<volName
//                  <<" copy n="<<copyNr<<" base n="<<copyNrBase<<std::endl;
//         std::cout<<"Mdt SD: swimming: stName/Eta/Phi/multilayer="
//                  <<stationName<<"/"<<stationEta<<"/"<<stationPhi<<"/"<<multilayer<<std::endl;
    }
    else if ((npos = volName.find("component")) != std::string::npos && (!isAssembly)) {     // multilayer

        int gmID = 0;
        if ((loc1 = volName.find("[")) != std::string::npos) 
        {
            if ((loc2 = volName.find("]", loc1+1)) != std::string::npos) 
            {
                my_isstream istrvar(volName.substr(loc1+1,loc2-loc1-1));
                istrvar>>gmID;
            }
        }
        multilayer = gmID;
// ss - this can be done more easily - see above 
//       int copyNo = touchHist->GetVolume(i)->GetCopyNo();
//       if (copyNo == 1 
//           || copyNo == 5  
// 	  || copyNo == 8 )   
// 	  {
// 	  multilayer = 1;
// 	} else if (copyNo == 3 
//           || copyNo == 7  
// 	  || copyNo == 10 
// 	  || copyNo == 11  
// 	  || copyNo == 9
// 	  || copyNo == 14) 
// 	  {
// 	multilayer = 2;
// 	}
    }else if ((npos = volName.find("Drift")) != std::string::npos) {         // layer and tube

      tubeLayer = touchHist->GetVolume(i)->GetCopyNo()/100;
      tube      = touchHist->GetVolume(i)->GetCopyNo()%100;
//       G4ThreeVector postube=touchHist->GetTranslation();
// 	std::cout<<"StationName "<< stationName<<" Tube "<<tube<<" layer "<<tubeLayer<<" position "<<postube[0]<<" "<<postube[1]<<" "<<postube[2]<<" "<<std::endl;   

    }
  }
  //construct the hit identifier	    
  return m_muonHelper->BuildMdtHitId(stationName, stationPhi, stationEta, multilayer,tubeLayer, tube);
}
