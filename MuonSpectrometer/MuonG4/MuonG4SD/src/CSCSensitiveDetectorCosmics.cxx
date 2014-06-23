/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonG4SD/CSCSensitiveDetectorCosmics.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "G4Trd.hh"
#include <string>
#include <sstream>
#include "MuonSimEvent/CscHitIdHelper.h"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

using namespace FADS;

typedef std::istringstream my_isstream;

//comment the following line if you want hit information print out
//#define CSCG4_DEBUG

// Initialize static data
static SensitiveDetectorEntryT<CSCSensitiveDetectorCosmics> cscsd("CSCSens_Cosmics");


// construction/destruction

CSCSensitiveDetectorCosmics::CSCSensitiveDetectorCosmics(std::string name)
  : FadsSensitiveDetector(name),
    momMag(0.),
    m_globalTime(0.),	
    myCSCHitColl(0)
{
	muonHelper = CscHitIdHelper::GetHelper();
	//muonHelper->PrintFields();
}


// Implemenation of memebr functions
void CSCSensitiveDetectorCosmics::Initialize(G4HCofThisEvent*) 
{
    //myCSCHitColl = new CSCSimHitCollection("CSC_Hits");
    myCSCHitColl = m_hitCollHelp.RetrieveNonconstCollection<CSCSimHitCollection>("CSC_Hits");
    mom = Amg::Vector3D(0.,0.,0.);
    globH = Amg::Vector3D(0.,0.,0.);
}

G4bool CSCSensitiveDetectorCosmics::ProcessHits(G4Step* aStep,G4TouchableHistory* /*ROHist*/) {
    
    G4Track* currentTrack = aStep->GetTrack();
    
    /** CSCs sensitive to charged particle only; plus energy threshold */
    if (currentTrack->GetDefinition()->GetPDGCharge() == 0.0) {
	 if (currentTrack->GetDefinition()!=G4Geantino::GeantinoDefinition()) return true;
	 else if (currentTrack->GetDefinition()==G4ChargedGeantino::ChargedGeantinoDefinition()) return true;
    }
   				                  
    G4TouchableHistory* touchHist = (G4TouchableHistory*)aStep->GetPreStepPoint()->GetTouchable();
    G4ThreeVector startPos=aStep->GetPreStepPoint()->GetPosition();
    G4ThreeVector endPos=aStep->GetPostStepPoint()->GetPosition();
    double kinEnergy = aStep->GetPreStepPoint()->GetKineticEnergy();

    /** attributes of the CSC identifier construction */
    
    std::string stationName="";
      int stationEta=0;
      int stationPhi=0;
      int multiLayer=0;
      int wireLayer=0;
      
      /** hit information to be recorded */
    
      Amg::Vector3D HitStart = Amg::Vector3D(-1000,-1000,-1000);
      Amg::Vector3D HitEnd   = Amg::Vector3D(-1000,-1000,-1000);	
      double globalTime   = -1;
      double energyDeposit= -1;
      G4int lundcode      = -1;
      /** the track id */
      int trackid         = aStep->GetTrack()->GetTrackID();

      // global coordinates
      Amg::Vector3D globVrtx = Amg::Hep3VectorToEigen( aStep->GetPreStepPoint()->GetPosition() );

      // distance of the hit from (0,0,0) vertex - calculated from the PreStepPoint (approximation)
      double dist = globVrtx.mag();
      double lightspeed = 299.792458; /* in vacuo speed of light [mm/ns] */
      double tOrigin = dist / lightspeed;

      currVertex = Amg::Hep3VectorToEigen( aStep->GetTrack()->GetVertexPosition() );
    
      // for cosmics: only primary muon tracks - track momentum when first entering the spectrometer (one muon per event)
      if ((currVertex != vertex) && (trackid == 1)) {
          // after calculationg the momentum magnidude, normalize it 
          mom = Amg::Hep3VectorToEigen( currentTrack->GetMomentum() );
          momMag = mom.mag();
          mom.normalize();
          // the direction of the primary mu is used to calculate the t0, the position ot the t0, globH, is ONE for a track
          Amg::Vector3D globVrtxFix = globVrtx;
          double AlphaGlobal = -1*(globVrtxFix[0]*mom[0] + globVrtxFix[1]*mom[1] + globVrtxFix[2]*mom[2])/(mom[0]*mom[0] + mom[1]*mom[1] + mom[2]*mom[2]);   
          globH = globVrtxFix + AlphaGlobal*mom;     
//          std::cout << "COSMICS MAIN TRACK IN THES CSC!"<<std::endl; 
       }  
       double globalDist = sqrt((globH[0] - globVrtx[0])*(globH[0] - globVrtx[0]) +
  			   (globH[1] - globVrtx[1])*(globH[1] - globVrtx[1]) +
  			   (globH[2] - globVrtx[2])*(globH[2] - globVrtx[2]));
       double tof = globalDist / lightspeed;

      /** scan geometry tree to identify the gas layer */
      bool isAssembly = false;
      for (int i = touchHist->GetHistoryDepth(); i>=0; i--) {
      	std::string::size_type npos;
	std::string volName = touchHist->GetVolume(i)->GetName();
		
        if ((npos = volName.find("av_")) != std::string::npos &&
            (npos = volName.find("impr_")) != std::string::npos)  isAssembly = true;
        
	/** station: name, eta and phi (-> chamber!) */
	if ((npos = volName.find("station")) != std::string::npos && (!isAssembly)) {
	
	  /** station name, station eta and station phi */
	  volName       = volName.substr(0,npos-2);
	  int volCopyNo = touchHist->GetVolume(i)->GetCopyNo();
	  stationName   = volName;
	  stationEta    = volCopyNo/100;
	  stationPhi    = abs(volCopyNo%100);
		    
		}
        else if ((npos = volName.find("CSC")) != std::string::npos && isAssembly ) {
            // vol name for Assembly components are
            // av_WWW_impr_XXX_Muon::BMSxMDTxx_pv_ZZZ_NAME
            //         where WWW is ass. istance nr.
            //               XXX is comp. imprint nr.
            //               BMSxMDTxx is the name of the comp. log.Vol.
            //                  x station sub-type; xx technology subtype
            //               ZZZ is No of order of this daugther
            //               NAME is the comp. tag (geoIdentifierTag)
            //               for CSCs it is cl[1] or cl[2]
            // copy numbers for Ass.components are =
            //                CopyNoBase(= geoIdentifierTag of the assembly) + geoIdentifierTag of the component
            //                geoIdentifierTag of the component = Job
            //                geoIdentifierTag of the assembly = (sideC*10000 +
            //                             mirsign*1000 + abs(zi)*100 + fi+1)*100000;
            //                             mirsign*1000 + abs(zi)*100 + fi+1)*100000;
            
            /** station name */
            std::string::size_type loc1,loc2;
            if ((loc1 = volName.find("Muon::")) != std::string::npos) 
            {
                stationName = volName.substr(loc1+6,3); //type only
            }
            
            
            /** station eta and station phi */
            int volCopyNo = touchHist->GetVolume(i)->GetCopyNo();
            int copyNrBase = int(volCopyNo/100000);
            int sideC  = int(copyNrBase/10000);
            int zi     = int((copyNrBase%1000)/100);
            // int mirfl  = int((copyNrBase%10000)/1000);
            int fi     = int(copyNrBase%100);
            if (sideC == 1) zi = -zi;
            stationEta    = zi;
            stationPhi    = fi;
            
            // now get the geoIdentifierTag of the rpc components 
            int gmID = 0;
            if ((loc1 = volName.find("[")) != std::string::npos) 
            {
                if ((loc2 = volName.find("]", loc1+1)) != std::string::npos) 
                {
                    my_isstream istrvar(volName.substr(loc1+1,loc2-loc1-1));
                    istrvar>>gmID;
                }
            }	    
            /** chamber layer */
            multiLayer = gmID;
	}
	else if ((npos = volName.find("component")) != std::string::npos && (!isAssembly)) {
		    
	  /** chamber layer */
	  multiLayer = touchHist->GetVolume(i)->GetCopyNo();
	  if(multiLayer==3) multiLayer=2; //multilayer index
	  //std::cout<<"CSC:::::: multiLayer  "<<multiLayer<<std::endl;
	}
	else if ((npos = volName.find("CscArCO2")) != std::string::npos) {
	
	  /** the wire layer number */
	  wireLayer=touchHist->GetVolume(i)->GetCopyNo();
	  wireLayer+=1;
	  if(wireLayer==4) wireLayer=1;
	  else if(wireLayer==3) wireLayer=2;
	  else if(wireLayer==2) wireLayer=3;
	  else if(wireLayer==1) wireLayer=4;
	  //std::cout<<"CSC:::::: wireLayer  "<<wireLayer<<std::endl;

	  /** get the particle ID */	 
	  G4String particle=aStep->GetTrack()->GetDefinition()->GetParticleName();
	  if (particle=="photon") lundcode=1;
	  else if (particle=="mu-") lundcode=6;
	  else if (particle=="mu+") lundcode=5;
	  else if (particle=="geantino") lundcode=999;
	  else lundcode=0;
  
 	  /** record global time */
	  globalTime = aStep->GetPreStepPoint()->GetGlobalTime();

	  /** energy deposit */
	  energyDeposit = aStep->GetTotalEnergyDeposit();


          /** get the transformation */
          const G4AffineTransform transform = touchHist->GetHistory()->GetTopTransform();

	  /** local position of HitStart and HitEnd */
	  HitStart = Amg::Hep3VectorToEigen( transform.TransformPoint(startPos) );
          
          /** get the end of the step as return by G4 - could be at the beginning of 
	      the next volume */
          HitEnd = Amg::Hep3VectorToEigen( transform.TransformPoint(endPos) );

	}
      }
      
      vertex = Amg::Hep3VectorToEigen( aStep->GetTrack()->GetVertexPosition() );
      // if the track vertex is far from (0,0,0), takes the tof, otherwise take the "usual" g4 globalTime
      ((((vertex.mag()) < 100) || ((fabs(globalTime - tOrigin)) < 0.1) ) ? (m_globalTime  = globalTime) : (m_globalTime = tof));
      // if m_globalTime  != globalTime and m_globalTime != tof in the output, this is due to multiple hits
      // before founding the good one (small approximation)

      /** construct the hit identifier */
      HitID CSCid = muonHelper->BuildCscHitId(stationName, stationPhi, 
					      stationEta, multiLayer, wireLayer);
	
      /** construct new csc hit */
      CSCSimHit* newHit = new CSCSimHit(CSCid, m_globalTime, energyDeposit,
					HitStart, HitEnd, lundcode, trackid, kinEnergy);

      /** insert hit in collection */
      myCSCHitColl->Insert(*newHit);
	  delete newHit;

// #ifndef CSCG4_DEBUG
// 
// // printouts for cosmics
//         std::cout<<"------------------CSC------------------"<<std::endl;
// 	std::cout << "Track "<<trackid<<std::endl;
// 	std::cout << "Track vertex "<<vertex[0]<<" " <<vertex[1]<<" " <<vertex[2]<<std::endl;
// 	std::cout << "Global position of the hit " << globVrtx[0] <<" " << globVrtx[1] <<" " << globVrtx[2] <<std::endl;
//         std::cout << "Distance from (0,0,0) and time " << dist << " " <<tOrigin <<std::endl;
//         std::cout << "Momentum "<<momMag<<std::endl;
// 	std::cout << "Momentum director cosines " <<mom[0]<<" " <<mom[1]<<" " <<mom[2]<<std::endl; 
// 	std::cout << "Eta and phi "<<mom.eta()<<" " <<mom.phi()<<std::endl;
//         std::cout << "Closest approach position and distance from (0,0,0) " 
// 	          << globH[0] <<" "<<globH[1]<<" "<<globH[2]<<" "
// 	          << sqrt(globH[0]*globH[0] + globH[1]*globH[1] + globH[2]*globH[2]) << std::endl; 
//         std::cout << "Distance from t0 and tof " << globalDist <<" " <<tof <<std::endl; 
//         std::cout << "g4 globalTime " << globalTime <<std::endl;
//         std::cout << "Time " << m_globalTime <<std::endl;
// 
//    	std::cout<<"TUB "<<muonHelper->GetStationName(CSCid);	
//  	std::cout<<" "<<muonHelper->GetFieldValue("PhiSector");
//  	std::cout<<" "<<muonHelper->GetFieldValue("ZSector");
//  	std::cout<<" "<<muonHelper->GetFieldValue("ChamberLayer");
//  	std::cout<<" "<<muonHelper->GetFieldValue("WireLayer")<<std::endl;
//   	    
//    	std::cout<<muonHelper->GetStationName(CSCid)<<" "<<newHit->print()<<std::endl;
//  	
//  #endif

    return true;
}


void CSCSensitiveDetectorCosmics::EndOfEvent(G4HCofThisEvent* /*HCE*/) {
	if (!m_allowMods)
          m_hitCollHelp.SetConstCollection(myCSCHitColl);
}
