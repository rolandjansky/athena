/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonG4SD/RPCSensitiveDetectorCosmics.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "G4ThreeVector.hh"
#include "G4Trd.hh"
#include "MuonSimEvent/RpcHitIdHelper.h"
#include <string>
// to permit access to StoreGate
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"

#include "SimHelpers/DetectorGeometryHelper.h"
#include "MCTruth/TrackHelper.h"

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

using namespace FADS;

typedef std::istringstream my_isstream;

//comment the following line if you want hit information print out
//#define RPCG4_DEBUG

// Initialization of static data
static SensitiveDetectorEntryT<RPCSensitiveDetectorCosmics> rpcsd("RPCSens_Cosmics");


// construction/destruction
RPCSensitiveDetectorCosmics::RPCSensitiveDetectorCosmics(std::string s)
  : FadsSensitiveDetector(s),
    myRPCHitColl(0),
    m_globalTime(0.),
    m_isGeoModel(true),
    momMag(0.)
{
  muonHelper = RpcHitIdHelper::GetHelper();
  //muonHelper->PrintFields();
  
  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  StatusCode status = svcLocator->service("MessageSvc", m_msgSvc);
  MsgStream log(m_msgSvc, "RpcSD");
  if (status.isFailure()) {
    log << MSG::FATAL << "MessageSvc not found !" << endreq;
  } else {
    log << MSG::DEBUG << "MessageSvc initialized." << endreq;
  }
  status = svcLocator->service("StoreGateSvc", m_sgSvc);
  if (status.isFailure()) {
    log << MSG::FATAL << "StoreGateSvc  not found !" << endreq;
  } else {
    log << MSG::DEBUG << "StoreGateSvc initialized." << endreq;
  }
  
  
}


void RPCSensitiveDetectorCosmics::Initialize(G4HCofThisEvent*) 
{
  //myRPCHitColl = new RPCSimHitCollection("RPC_Hits");
  myRPCHitColl = m_hitCollHelp.RetrieveNonconstCollection<RPCSimHitCollection>("RPC_Hits");

  MsgStream log(m_msgSvc, "RpcSD");
  log << MSG::DEBUG << "Initializing SD" << endreq;
  DetectorGeometryHelper DGHelp;
  if(  DGHelp.GeometryType("Muon") == GeoModel )
    {
      m_isGeoModel = true;
      if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "Muon Geometry is from GeoModel" << endreq;
    } else {
      m_isGeoModel = false;
      if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "Muon Geometry is from pure G4" << endreq;
    }
  mom = Amg::Vector3D(0.,0.,0.);
  globH = Amg::Vector3D(0.,0.,0.);
}

G4bool RPCSensitiveDetectorCosmics::ProcessHits(G4Step* aStep,G4TouchableHistory*) {


    
    MsgStream log(m_msgSvc, "RpcSD");
    G4Track* currentTrack = aStep->GetTrack();
    

    if (currentTrack->GetDefinition()->GetPDGCharge() == 0.0) {
      
	 if (currentTrack->GetDefinition()!=G4Geantino::GeantinoDefinition()) return true;
	 //	 else if (currentTrack->GetDefinition()==G4ChargedGeantino::ChargedGeantinoDefinition()) return true;
    } 

    G4TouchableHistory*     touchHist = (G4TouchableHistory*)aStep->GetPreStepPoint()->GetTouchable();	
    G4ThreeVector           position  = aStep->GetPreStepPoint()->GetPosition();
    G4ThreeVector       postPosition  = aStep->GetPostStepPoint()->GetPosition();
    const G4AffineTransform trans     = currentTrack->GetTouchable()->GetHistory()->GetTopTransform(); // from global to local

    // necessary to assign correct identifiers
    int rpcIsRotated      = 0;
    // int layer             = 0;

    std::string tech;
    // fields for the RPC identifier construction
    std::string stationName;
    int         stationEta= 0;
    int         stationPhi= 0;
    int         doubletR= 0;
    int         doubletZ= 0;
    int         doubletPhi= 0;
    int         gasGap= 0;
    // int         eta_inversed=0;
    // int         phi_inversed=0;
    int         station_rotated=0; // tells us if the station was rotated before being positioned.
                                   // if =1 we have to correct some IDs
    int         zNeg_original=0; // tells if the station at z<0 was obtained duplicating a station at z>0
    

    // RPC hit information
    double     globalTime    = aStep->GetPreStepPoint()->GetGlobalTime();
    Amg::Vector3D localPosition = Amg::Hep3VectorToEigen( trans.TransformPoint(position) );
    Amg::Vector3D localPostPosition = Amg::Hep3VectorToEigen( trans.TransformPoint(postPosition) );

    // global coordinates
    G4ThreeVector globVrtx = aStep->GetPreStepPoint()->GetPosition();

    // distance of the hit from (0,0,0) vertex - calculated from the PreStepPoint (approximation)
    // double dist = globVrtx.mag();
    double lightspeed = 299.792458; /* in vacuo speed of light [mm/ns] */
    // double tOrigin = dist / lightspeed;

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
//       std::cout << "COSMICS MAIN TRACK IN THE RPC!"<<std::endl; 
    }  
    double globalDist = sqrt((globH[0] - globVrtx[0])*(globH[0] - globVrtx[0]) +
  			   (globH[1] - globVrtx[1])*(globH[1] - globVrtx[1]) +
  			   (globH[2] - globVrtx[2])*(globH[2] - globVrtx[2]));
    double tof = globalDist / lightspeed;

    int mydbZ=0;
    int mydbPMod=0;
    int mydbP=0;
    bool isAssembly = false;
    // scan geometry tree to identify the hit channel
    for (int i=touchHist->GetHistoryDepth();i>=0;i--) {
	
	std::string::size_type npos;
	std::string volName = touchHist->GetVolume(i)->GetName();
	std::string num=volName.substr(3,2);
	if(num[0]==' ') num[0]=0;

        // check if this station is an assembly 
        if ((npos = volName.find("av_")) != std::string::npos &&
            (npos = volName.find("impr_")) != std::string::npos)  isAssembly = true;

        
	// stationName, stationEta, stationPhi
	if ((npos = volName.find("station")) != std::string::npos  && (!isAssembly)) {
  
	
	    stationName = volName.substr(0,npos-1);
	 
  
	    
	    int volCopyNo = touchHist->GetVolume(i)->GetCopyNo();
	  
	    if(abs(volCopyNo/1000)==1){
	      zNeg_original=1;
	      volCopyNo=volCopyNo%1000;
	    }

	    stationEta = volCopyNo/100;			
	    stationPhi = abs(volCopyNo%100);
	    
	    if(stationEta<0&&!zNeg_original) station_rotated=1;

	} 
	// stationName, stationEta, stationPhi
        else if ((npos = volName.find("RPC")) != std::string::npos && isAssembly) {
            // vol name for Assembly components are
            // av_WWW_impr_XXX_Muon::BMSxMDTxx_pv_ZZZ_NAME
            //         where WWW is ass. istance nr.
            //               XXX is comp. imprint nr.
            //               BMSxMDTxx is the name of the comp. log.Vol.
            //                  x station sub-type; xx technology subtype
            //               ZZZ is the comp. number of order
            //               NAME is the comp. tag (geoIdentifierTag)
            //                    for RPCs it is SwapdbPdbRdbZ[aaa]
            //                    with aaa = doubletZ + doubletR*100 + dbphi*1000;
            //                         aaa = -aaa if iswap == -1
            //                         dbphi = 1 always but for S1 or S2 at doubletPhi = 2
            // copy numbers for Ass.components are =
            //                CopyNoBase(= geoIdentifierTag of the assembly) + geoIdentifierTag of the component
            //                geoIdentifierTag of the component = Job
            //                geoIdentifierTag of the assembly = (sideC*10000 +
            //                             mirsign*1000 + abs(zi)*100 + fi+1)*100000;
            //                             mirsign = 1 if zi<0 and !mirrored; otherwise 0
            std::string::size_type loc1,loc2;
            if ((loc1 = volName.find("Muon::")) != std::string::npos) 
            {
                stationName = volName.substr(loc1+6,4); //type and subtype
            }
	    
	    int volCopyNo = touchHist->GetVolume(i)->GetCopyNo();
            int copyNrBase = int(volCopyNo/100000);
//             std::cout<<"  CopyNumber = "<<volCopyNo<<" base= "<<copyNrBase
//                      <<" comp.Id= "<<int(volCopyNo%100000)<<std::endl;
            int sideC  = int(copyNrBase/10000);
            int zi     = int((copyNrBase%1000)/100);
            int mirfl  = int((copyNrBase%10000)/1000);
            int fi     = int(copyNrBase%100);
            if (sideC == 1) zi = -zi;
            stationEta    = zi;
            stationPhi    = fi;
            zNeg_original = mirfl;
//             std::cout<<"stName = "<<stationName<<" -- stEta,Phi,sideC, zNeg_original "<<stationEta<<" "
//                      <<stationPhi<<" "<<sideC<<" "<<zNeg_original<<std::endl;
	    
	    if(stationEta<0&&!zNeg_original) station_rotated=1;
            tech=volName.substr(npos,5);
            
            // now get the geoIdentifierTag of the rpc components 
            int gmID = 0;
            if ((loc1 = volName.find("[")) != std::string::npos) 
            {
                if ((loc2 = volName.find("]", loc1+1)) != std::string::npos) 
                {
                    //std::cout<<"first [ is at "<<loc1<<" first ] at "<<loc2<<std::endl;
                    my_isstream istrvar(volName.substr(loc1+1,loc2-loc1-1));
                    istrvar>>gmID;
                    //std::cout<<"FOUND ---- gmID = "<<gmID<<std::endl;
                }
            }
            int kk=gmID;
	  
            if (kk < 0) rpcIsRotated=1;
	  
            doubletR =(abs(kk)%1000)/100;	  
            mydbZ    = abs(int(kk%10));
            mydbPMod = abs(int(kk/1000));
	}
	// doubleR, doubletZ
	else if ((npos = volName.find("rpccomponent")) != std::string::npos && (!isAssembly)) {
	  
            //sstech=volName.substr(0,3)+num;
            tech=volName.substr(npos-5,5);
//           std::cout<<"RPC - rpcModule VolName    "<<volName<<std::endl;
//           std::cout<<"RPC - rpcModule copyNumber "<<touchHist->GetVolume(i)->GetCopyNo()<<" tech = "<<tech<<std::endl;

          std::string::size_type loc1,loc2;
          int gmID = 0;
          if ((loc1 = volName.find("[")) != std::string::npos) 
          {
              if ((loc2 = volName.find("]", loc1+1)) != std::string::npos) 
              {
                  //std::cout<<"first [ is at "<<loc1<<" first ] at "<<loc2<<std::endl;
                  my_isstream istrvar(volName.substr(loc1+1,loc2-loc1-1));
                  istrvar>>gmID;
//                   std::cout<<"FOUND ---- gmID = "<<gmID<<std::endl;
              }
          }
          mydbZ    = abs(int(gmID%10));
          mydbPMod = abs(int(gmID/1000));
//          std::cout<<"gmID = ["<<gmID<<"] mydbZ, mydbPMod = "<<mydbZ<<" "<<mydbPMod<<std::endl;
          
	  int kk=touchHist->GetVolume(i)->GetCopyNo();
	  
	  if (kk < 0) rpcIsRotated=1;
	  
	  doubletR=(abs(kk)%1000)/100;
	  
	}
	// doubleR
	else if ((npos = volName.find("rpccomponent")) != std::string::npos && (!isAssembly)) {


          std::string::size_type loc1,loc2;
          int gmID = 0;
          if ((loc1 = volName.find("[")) != std::string::npos) 
          {
              if ((loc2 = volName.find("]", loc1+1)) != std::string::npos) 
              {
                  my_isstream istrvar(volName.substr(loc1+1,loc2-loc1-1));
                  istrvar>>gmID;
              }
          }
          mydbZ    = abs(int(gmID%10));
          mydbPMod = abs(int(gmID/1000));
          
	  int kk=touchHist->GetVolume(i)->GetCopyNo();
	  
	  if (kk < 0) rpcIsRotated=1;
	  
	  doubletR=(abs(kk)%1000)/100;
	  
            
// 	  tech=volName.substr(0,3)+num;
	  
// 	  if(tech=="RPC07"||tech=="RPC08") littleChamber=1;
// 	  else if(m_isGeoModel&&(tech=="RPC7 "||tech=="RPC8 ")) littleChamber=1;
// 	  if(tech=="RPC21"||tech=="RPC22"){ noDoubletZ=1; onlyOneGap=0; } // needed for BMF1 in layout Q. this overrides any previous assignment to noDoubletZ and onlyOneGap. It doesn't affect layout P


// 	  //log<<MSG::DEBUG<<"hit in volume "<<volName<<" station "<<stationName<< " on technology "<<tech<<endreq;	
// 	  int kk=touchHist->GetVolume(i)->GetCopyNo();
	  
// 	  if (kk < 0) rpcIsRotated=1;
	  
// 	  doubletR=(abs(kk)%1000)/100;
	  
// 	  if(abs(kk)/1000==2) isARibChamber=1; // we use this to correct DoubletPhi
	  
	}
	
	// gasGap
	else if ((npos = volName.find("layer")) != std::string::npos) {
	  
	  int copyNo = touchHist->GetVolume(i)->GetCopyNo();
	  
	  if (copyNo == 1) { 
	    rpcIsRotated ? gasGap = 2 : gasGap = 1;
	  } else if (copyNo ==2) {
	    rpcIsRotated ? gasGap = 1 : gasGap = 2;
	  } 
	}
	
	// doubletPhi
	else if((npos = volName.find("gas volume")) != std::string::npos) { 
	  
	  int copyNo = touchHist->GetVolume(i)->GetCopyNo();
	  if (copyNo == 0) {
	    doubletPhi = 1; 
	  } else if (copyNo == 10) {
	    doubletPhi = 2;
	  }
          mydbP = doubletPhi;
//           std::cout<<"RPC - gasgapWithinLayer VolName    "<<volName<<std::endl;
//           std::cout<<"RPC - gasgapWithinLayer copyNumber "<<touchHist->GetVolume(i)->GetCopyNo()<<" dbPhi = "<<doubletPhi<<std::endl;
          int ngap_in_s=0;
          int nstrippanel_in_s=0;
          std::string::size_type loc1;
          if ((loc1 = volName.find("gg_in_s")) != std::string::npos) 
          {
              my_isstream istrvar(volName.substr(loc1-1,1));
              istrvar>>ngap_in_s;
          }
          if ((loc1 = volName.find("sp_in_s")) != std::string::npos) 
          {
              my_isstream istrvar(volName.substr(loc1-1,1));
              istrvar>>nstrippanel_in_s;
          }
          if (ngap_in_s == 1 && nstrippanel_in_s == 2)
          {
              if(localPosition.y()>0) mydbP=2;
              else mydbP=1;
          }
          else if (ngap_in_s == 1 && nstrippanel_in_s == 1)
          {
              mydbP = mydbPMod;
          }          
	}
	
	
    }	
    
    ////////////////////////////////////////////////
    // correct wrong IDs due to station rotation. only doubletZ and doubletPhi are affected at this point
    ////////////////////////////////////////////////

    if(station_rotated){
      // doubletPhi correction
      // note that this is correct also for ribs chambers
      mydbP++;
      if (mydbP>2) mydbP=1;
      // doubletZ correction: specialStations have already been correted
      // the others do not need any correction, because doubletZ increases with |Z|, thus not
      // changing in the rotation

      // strip numbering: if the station is rotated both eta and phi directions get inversed.
      // commented out for geomodel! 

      if(!m_isGeoModel){
	localPosition.z() = -localPosition.z();
	localPosition.y() = -localPosition.y();     
	localPostPosition.z() = -localPostPosition.z();
	localPostPosition.y() = -localPostPosition.y();     
      }
    }

    // further correction on the eta direction due to rpc component rotation
    // commented for geomodel!
    if(!rpcIsRotated&&!m_isGeoModel) 
      {
	localPosition.z() = -localPosition.z(); 
	localPostPosition.z() = -localPostPosition.z(); 
      }

    /////////////////////////////////////


    // now we have the position in the gas gap, with correct axis orientation, and the offlineIDs *of the strip panel*
    // nstrip is supposed to be calculated by RPC_Digitizer.
    
    // construct two (one in eta and one in phi) new RPC hits and store them in hit collection
    
    //construct the hit identifiers

    log<<MSG::DEBUG<<"hit in station "<<stationName<< " on technology "<<tech<<endreq;	
    log<<MSG::DEBUG<<"constructing ids (stName, stEta, stPhi, dr, dZ, dPhi)= "<<stationName<< " "<< stationEta<<" " << stationPhi<< " "<<doubletR<< " "<< doubletZ<< " "<<doubletPhi<<endreq;
    
    HitID RPCid_eta = muonHelper->BuildRpcHitId(stationName, stationPhi, stationEta, 
                                                mydbZ, doubletR, gasGap, mydbP,0);
    
    HitID RPCid_phi = muonHelper->BuildRpcHitId(stationName, stationPhi, stationEta, 
						mydbZ, doubletR, gasGap, mydbP,1);

    // retrieve track barcode
    
    TrackHelper trHelp(aStep->GetTrack());
    
   int barcode = trHelp.GetBarcode();
     
    //construct new rpc hit
    vertex = Amg::Hep3VectorToEigen( aStep->GetTrack()->GetVertexPosition() );
    
    // if the track vertex is far from (0,0,0), takes the tof, otherwise take the "usual" g4 globalTime
    (((vertex.mag()) < 100) ? (m_globalTime  = globalTime) : (m_globalTime = tof));
   
    	RPCSimHit* NewHit_eta = new RPCSimHit(RPCid_eta, m_globalTime,
					      localPosition, barcode, localPostPosition, 
					      aStep->GetTotalEnergyDeposit(), 
					      aStep->GetStepLength(), 
					      currentTrack->GetDefinition()->GetPDGEncoding(), 
					      aStep->GetPreStepPoint()->GetKineticEnergy());
    	RPCSimHit* NewHit_phi = new RPCSimHit(RPCid_phi, m_globalTime,
					      localPosition, barcode, localPostPosition, 
					      aStep->GetTotalEnergyDeposit(), 
					      aStep->GetStepLength(), 
					      currentTrack->GetDefinition()->GetPDGEncoding(), 
					      aStep->GetPreStepPoint()->GetKineticEnergy());
	
    	myRPCHitColl->Insert(*NewHit_eta);
    	myRPCHitColl->Insert(*NewHit_phi);
		delete NewHit_eta;
		delete NewHit_phi;
	
// #ifndef RPCG4_DEBUG
//   
// // printouts for cosmics
//         std::cout<<"------------------RPC------------------"<<std::endl;
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
// 	std::cout<<"TRI ";//muonHelper->GetStationName();
// 	std::cout<<" "<<muonHelper->GetFieldValue("PhiSector");
// 	std::cout<<" "<<muonHelper->GetFieldValue("ZSector");
// 	std::cout<<" "<<muonHelper->GetFieldValue("DoubletZ");
// 	std::cout<<" "<<muonHelper->GetFieldValue("DoubletR");
// 	std::cout<<" "<<muonHelper->GetFieldValue("GasGapLayer");
// 	std::cout<<" "<<muonHelper->GetFieldValue("DoubletPhi");
// 	std::cout<<" "<<muonHelper->GetFieldValue("MeasuresPhi")<<std::endl;
// 
//         std::cout<<stationName<<" "<<NewHit_eta->print()<<std::endl;
// #endif
    return true;
}


void RPCSensitiveDetectorCosmics::EndOfEvent(G4HCofThisEvent* /*HCE*/) {
  if (!m_allowMods)
    m_hitCollHelp.SetConstCollection(myRPCHitColl);
}
