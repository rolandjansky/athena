/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonG4SD/MDTSensitiveDetector.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "MuonSimEvent/MdtHitIdHelper.h"
#include "MCTruth/TrackHelper.h"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"

#include <string>
#include <iostream>
#include <limits>

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

typedef std::istringstream my_isstream;

//comment the following line if you want hit information print out
//#define MDTG4_DEBUG


double MDTSensitiveDetector::DEFAULT_TUBE_RADIUS = std::numeric_limits<double>::max();
// Initialize static data
static FADS::SensitiveDetectorEntryT<MDTSensitiveDetector> mdtsd("MDTSens");



// construction/destruction
MDTSensitiveDetector::MDTSensitiveDetector(std::string name)
  : FadsSensitiveDetector(name),
    m_MDTHitColl(0),
    m_driftRadius(0.),
    m_globalTime(0.)		
{
	m_muonHelper = MdtHitIdHelper::GetHelper();
	//m_muonHelper->PrintFields();
}


// Implemenation of memebr functions
void MDTSensitiveDetector::Initialize(G4HCofThisEvent*) 
{
    //m_MDTHitColl = new MDTSimHitCollection("MDT_Hits");
    m_MDTHitColl = m_hitCollHelp.RetrieveNonconstCollection<MDTSimHitCollection>("MDT_Hits");
    m_driftRadius = DEFAULT_TUBE_RADIUS;
}

G4bool MDTSensitiveDetector::ProcessHits(G4Step* aStep,G4TouchableHistory* /*ROHist*/) {
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

  if( driftRadius < m_driftRadius ){  // check if current step came closer to the wire than the previous ones
    m_driftRadius = driftRadius;
    m_globalTime  = globalTime;
    m_localPosition = localPosition;
  }
 
  // check if particle left tube or stopped in tube
  G4String  namePreStepMat  = aStep->GetPreStepPoint()->GetMaterial()->GetName();
  G4String  namePostStepMat = aStep->GetPostStepPoint()->GetMaterial()->GetName();
  G4String  nameSD          = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetSensitiveDetector()->GetName();	  
  // G4int     trackid         = aStep->GetTrack()->GetTrackID();
  // see if we were in the sensitive volume and left it, or the particle was killed
  if( ((nameSD) && (namePreStepMat != namePostStepMat)) || (currentTrack->GetTrackStatus() ==  fStopAndKill)){

    // get identifier
    G4TouchableHistory* touchHist = (G4TouchableHistory*)aStep->GetPreStepPoint()->GetTouchable();
    int MDTid = GetIdentifier(touchHist);

    TrackHelper trHelp(aStep->GetTrack());
    int barcode = trHelp.GetBarcode();

    // construct new mdt hit
    MDTSimHit* nHit = new MDTSimHit(MDTid, m_globalTime, m_driftRadius, m_localPosition, barcode,
				    aStep->GetStepLength(),
				    aStep->GetTotalEnergyDeposit(),
				    currentTrack->GetDefinition()->GetPDGEncoding(), 
				    aStep->GetPreStepPoint()->GetKineticEnergy());
    m_MDTHitColl->Insert(*nHit );
	delete nHit;

// #ifndef MDTG4_DEBUG
//   	ATH_MSG_INFO("TUB "<<m_muonHelper->GetStationName(MDTid));	
// 	ATH_MSG_INFO(" "<<m_muonHelper->GetFieldValue("PhiSector"));
// 	ATH_MSG_INFO(" "<<m_muonHelper->GetFieldValue("ZSector"));
// 	ATH_MSG_INFO(" "<<m_muonHelper->GetFieldValue("MultiLayer"));
// 	ATH_MSG_INFO(" "<<m_muonHelper->GetFieldValue("Layer"));
// 	ATH_MSG_INFO(" "<<m_muonHelper->GetFieldValue("Tube"));
//  	    
   	//ATH_MSG_INFO(m_muonHelper->GetStationName(MDTid)<<" "<<nHit->print());
// #endif

    m_driftRadius = DEFAULT_TUBE_RADIUS; // reset start value of driftRadius
  }
  return true;
}


void MDTSensitiveDetector::EndOfEvent(G4HCofThisEvent* /*HCE*/) {
  if (!m_allowMods)
    m_hitCollHelp.SetConstCollection(m_MDTHitColl);

} 

int MDTSensitiveDetector::GetIdentifier(G4TouchableHistory* touchHist)
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
      //ATH_MSG_INFO("Mdt SD: swimming: found <station>"<<volName<<" copy n="<<volCopyNo);
      //ATH_MSG_INFO("Mdt SD: swimming: stationName="<<stationName<<" stationEta/Phi="<<stationEta<<"/"<<stationPhi);

    }
    else if ((npos = volName.find("component")) != std::string::npos  && (!isAssembly)) {     // multilayer
        
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
//         ATH_MSG_INFO("Mdt SD: swimming: found <component>"<<volName
//                  <<" copy n="<<touchHist->GetVolume(i)->GetCopyNo());
//         ATH_MSG_INFO("Mdt SD: swimming: multilayer="<<multilayer);

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
//         ATH_MSG_INFO("Mdt SD: swimming: found <MDT>"<<volName
//                  <<" copy n="<<copyNr<<" base n="<<copyNrBase);
//         ATH_MSG_INFO("Mdt SD: swimming: stName/Eta/Phi/multilayer="
//                  <<stationName<<"/"<<stationEta<<"/"<<stationPhi<<"/"<<multilayer);
    }
    else if ((npos = volName.find("Drift")) != std::string::npos) {         // layer and tube

      tubeLayer = touchHist->GetVolume(i)->GetCopyNo()/100;
      tube      = touchHist->GetVolume(i)->GetCopyNo()%100;
//       ATH_MSG_INFO("Mdt SD: swimming: found <Drift>"<<volName
//                <<" copy n="<<touchHist->GetVolume(i)->GetCopyNo());
//       ATH_MSG_INFO("Mdt SD: swimming: tubeLayer/tube="
//                <<tubeLayer<<"/"<<tube);

    }
  }
  //construct the hit identifier	    
  return m_muonHelper->BuildMdtHitId(stationName, stationPhi, stationEta, multilayer,tubeLayer, tube);
}
