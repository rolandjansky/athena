/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonG4SD/RPCSensitiveDetector.h"
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
#include <sstream>

#include "GeoPrimitives/CLHEPtoEigenConverter.h"

typedef std::istringstream my_isstream;

//comment the following line if you want hit information print out
//#define RPCG4_DEBUG

// Initialization of static data
static FADS::SensitiveDetectorEntryT<RPCSensitiveDetector> rpcsd("RPCSens");


// construction/destruction
RPCSensitiveDetector::RPCSensitiveDetector(std::string s)
  : FadsSensitiveDetector(s),
    myRPCHitColl(0),
    m_isGeoModel(true)
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


void RPCSensitiveDetector::Initialize(G4HCofThisEvent*) 
{
  //myRPCHitColl = new RPCSimHitCollection("RPC_Hits");
  myRPCHitColl = m_hitCollHelp.RetrieveNonconstCollection<RPCSimHitCollection>("RPC_Hits");

  MsgStream log(m_msgSvc, "RpcSD");
  if (log.level()<=MSG::VERBOSE)log << MSG::VERBOSE << "Initializing SD" << endreq;
  DetectorGeometryHelper DGHelp;
  if(  DGHelp.GeometryType("Muon") == GeoModel )
    {
      m_isGeoModel = true;
      if (log.level()<=MSG::VERBOSE) log << MSG::VERBOSE << "Muon Geometry is from GeoModel" << endreq;
    } else {
      m_isGeoModel = false;
      if (log.level()<=MSG::VERBOSE) log << MSG::VERBOSE << "Muon Geometry is from pure G4" << endreq;
    }
  
}

G4bool RPCSensitiveDetector::ProcessHits(G4Step* aStep,G4TouchableHistory*) {


    
    MsgStream log(m_msgSvc, "RpcSD");
    G4Track* track = aStep->GetTrack();
    

    /** RPCs sensitive to charged particle only */
    if (track->GetDefinition()->GetPDGCharge() == 0.0) 
      {
	//	std::cout<<"are we in the case charge 0 - pdGid =  "<<track->GetDefinition()->GetPDGEncoding()<<std::endl;
	if (track->GetDefinition()!=G4Geantino::GeantinoDefinition()) 
	  {
	    //std::cout<<"are we in the case charge 0 but no Geantino definition"<<std::endl;
	    return true;
	  }

      }
    // else 
    //   {
    // 	std::cout<<"we are not in the case charge 0 - pdGid =  "<<track->GetDefinition()->GetPDGEncoding()<<std::endl;
    //   }
    

    G4TouchableHistory*     touchHist = (G4TouchableHistory*)aStep->GetPreStepPoint()->GetTouchable();	
    G4ThreeVector           position  = aStep->GetPreStepPoint()->GetPosition();
    G4ThreeVector           postPosition  = aStep->GetPostStepPoint()->GetPosition();
    const G4AffineTransform trans     = track->GetTouchable()->GetHistory()->GetTopTransform(); // from global to local

    // necessary to assign correct identifiers
    int rpcIsRotated      = 0;
    // int layer             = 0;

    std::string tech;
    // fields for the RPC identifier construction
    std::string stationName;
    int         stationEta= 0;
    int         stationPhi= 0;
    int         doubletR= 0;
    // int         doubletZ= 0;
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

//         std::cout<<"Rpc SD: swimming through the tree: level "<<i-touchHist->GetHistoryDepth()<<std::endl;
//         std::cout<<"Rpc SD: name "<<volName<<std::endl;

        // check if this station is an assembly 
        if ((npos = volName.find("av_")) != std::string::npos &&
            (npos = volName.find("impr_")) != std::string::npos)  isAssembly = true;
  
	// stationName, stationEta, stationPhi
	if ((npos = volName.find("station")) != std::string::npos  && (!isAssembly)) {

//             std::cout<<"RPC - station VolName    "<<volName<<std::endl;
//             std::cout<<"RPC - station copyNumber "<<touchHist->GetVolume(i)->GetCopyNo()<<std::endl;
	
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

	    // doubletZ   = 1; 
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
	  
          std::string::size_type loc1,loc2;
          tech=volName.substr(npos-5,5);
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
	  
	}
	
	// gasGap
	else if ((npos = volName.find("layer")) != std::string::npos) {
	  
	  int copyNo = touchHist->GetVolume(i)->GetCopyNo();
//           std::cout<<"RPC - gasgapLayer VolName    "<<volName<<std::endl;
//           std::cout<<"RPC - gasgapLayer copyNumber "<<touchHist->GetVolume(i)->GetCopyNo()<<std::endl;
          
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
      //      std::cout<<"Rcd PC - swap dbPhi because ch is MIRRORED "<<doubletPhi<<std::endl;

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
    if(!rpcIsRotated&&!m_isGeoModel) {
      localPosition.z() = -localPosition.z(); 
      localPostPosition.z() = -localPostPosition.z(); 
    }
    /////////////////////////////////////


    // now we have the position in the gas gap, with correct axis orientation, and the offlineIDs *of the strip panel*
    // nstrip is supposed to be calculated by RPC_Digitizer.
    
    // construct two (one in eta and one in phi) new RPC hits and store them in hit collection
    
    //construct the hit identifiers

    if (log.level()<=MSG::VERBOSE)
      {
	log<<MSG::VERBOSE<<"hit in station "<<stationName<< " on technology "<<tech<<endreq;	
	log<<MSG::VERBOSE<<"constructing ids (stName, stEta, stPhi, dr, dZ, dPhi)= "<<stationName<< " "<< stationEta<<" " << stationPhi<< " "<<doubletR<< " "<< mydbZ<< " "<<mydbP<<endreq;
      }
    
//     HitID RPCid_eta = muonHelper->BuildRpcHitId(stationName, stationPhi, stationEta, 
// 						doubletZ, doubletR, gasGap, doubletPhi,0);
     HitID RPCid_eta = muonHelper->BuildRpcHitId(stationName, stationPhi, stationEta, 
                                                 mydbZ, doubletR, gasGap, mydbP,0);
    
//     HitID RPCid_phi = muonHelper->BuildRpcHitId(stationName, stationPhi, stationEta, 
// 						doubletZ, doubletR, gasGap, doubletPhi,1);
     HitID RPCid_phi = muonHelper->BuildRpcHitId(stationName, stationPhi, stationEta, 
						 mydbZ, doubletR, gasGap, mydbP,1);

    // retrieve track barcode
    
    TrackHelper trHelp(aStep->GetTrack());
    
   int barcode = trHelp.GetBarcode();
     
    //construct new rpc hit
    
   RPCSimHit* NewHit_eta = new RPCSimHit(RPCid_eta, globalTime,
					 localPosition, barcode, localPostPosition, 
					 aStep->GetTotalEnergyDeposit(), 
					 aStep->GetStepLength(), 
					 track->GetDefinition()->GetPDGEncoding(), 
					 aStep->GetPreStepPoint()->GetKineticEnergy());
 RPCSimHit* NewHit_phi = new RPCSimHit(RPCid_phi, globalTime,
					 localPosition, barcode, localPostPosition, 
					 aStep->GetTotalEnergyDeposit(), 
					 aStep->GetStepLength(), 
					 track->GetDefinition()->GetPDGEncoding(), 
					 aStep->GetPreStepPoint()->GetKineticEnergy());
   
   if (log.level()<=MSG::DEBUG)
     { 
       log<<MSG::DEBUG<<"---A new RPC hit------dumping the eta version ---------"<<endreq;
       log<<MSG::DEBUG<<" StationName/eta/phi/dbR/dbZ/dbPhi/gg "<<stationName<<"/"<<stationEta<<"/"
	  <<stationPhi<<"/"<<doubletR<<"/"<<mydbZ<<"/"<<mydbP<<"/"<<gasGap<<endreq;
       log<<MSG::DEBUG<<NewHit_eta->print()<<endreq;
       log<<MSG::DEBUG<<"----------------------dumping the phi version ---------"<<endreq;
       log<<MSG::DEBUG<<NewHit_phi->print()<<endreq;
       log<<MSG::DEBUG<<"---------done------------------------------------------"<<endreq;
     }
	
  myRPCHitColl->Insert(*NewHit_eta);
  myRPCHitColl->Insert(*NewHit_phi);
  delete NewHit_eta;
  delete NewHit_phi;
	
// #ifndef RPCG4_DEBUG
//  
// 	std::cout<<"TRI ";//muonHelper->GetStationName();
//	std::cout<<" "<<muonHelper->GetFieldValue("PhiSector");
//	std::cout<<" "<<muonHelper->GetFieldValue("ZSector");
//	std::cout<<" "<<muonHelper->GetFieldValue("DoubletZ");
//	std::cout<<" "<<muonHelper->GetFieldValue("DoubletR");
//	std::cout<<" "<<muonHelper->GetFieldValue("GasGapLayer");
//	std::cout<<" "<<muonHelper->GetFieldValue("DoubletPhi");
//	std::cout<<" "<<muonHelper->GetFieldValue("MeasuredPhi")<<std::endl;
// 
	//std::cout<<stationName<<" "<<NewHit_eta->print()<<std::endl;
// #endif
    return true;
}


void RPCSensitiveDetector::EndOfEvent(G4HCofThisEvent* /*HCE*/) {
        if (!m_allowMods)
	  m_hitCollHelp.SetConstCollection(myRPCHitColl);
}
