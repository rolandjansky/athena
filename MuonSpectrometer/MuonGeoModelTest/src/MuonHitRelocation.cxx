/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModelTest/MuonHitRelocation.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "GeneratorObjects/McEventCollection.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimEvent/GenericMuonSimHitCollection.h"
#include "MuonSimEvent/MDTSimHit.h"
#include "MuonSimEvent/RPCSimHit.h"
#include "MuonSimEvent/TGCSimHit.h"
#include "MuonSimEvent/CSCSimHit.h"

#include "GeoAdaptors/GeoMuonHits.h"


#include <string>

class MuonHitRelocation::Clockwork {

public:

  // Constructor:
  Clockwork();

  // Destructor:
  ~Clockwork();



  // Point to NTuple:
  NTuple::Tuple  *nt;

  // And the items in this Ntuple:
  NTuple::Item<long>   run;
  NTuple::Item<long>   event;
  NTuple::Item<double> phi;
  NTuple::Item<double> theta;
  NTuple::Item<double> x;
  NTuple::Item<double> y;
  NTuple::Item<double> z;
  NTuple::Item<double> lx;
  NTuple::Item<double> ly;
  NTuple::Item<double> lz;
  NTuple::Item<double> zResid;
  NTuple::Item<double> phiResid;
  NTuple::Item<double> dist;
  NTuple::Item<long>   tech;
  NTuple::Item<long>   stname;
  NTuple::Item<long>   steta;
  NTuple::Item<long>   stphi;
  NTuple::Item<long>   mlayer;
  NTuple::Item<long>   dbr;
  NTuple::Item<long>   dbz;
  NTuple::Item<long>   dbphi;
  NTuple::Item<long>   layer;
  NTuple::Item<long>   tube;


};


MuonHitRelocation::Clockwork::Clockwork()
{
}

MuonHitRelocation::Clockwork::~Clockwork() {
}


/////////////////////////////////////////////////////////////////////////////

MuonHitRelocation::MuonHitRelocation(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),c(new Clockwork())
{
    declareProperty("checkMdt", _checkMdt = true );
    declareProperty("checkRpc", _checkRpc = true );
    declareProperty("checkTgc", _checkTgc = true );
    declareProperty("checkCsc", _checkCsc = true );
    declareProperty("checksTgc", _checksTgc = true );
    declareProperty("checkMM", _checkMM = true );
}

MuonHitRelocation::~MuonHitRelocation() {
  delete c;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MuonHitRelocation::initialize(){

  StatusCode status = StatusCode::SUCCESS;

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "HELLO from MuonHitRelocation" << endreq;
  

  NTupleFilePtr     file(ntupleSvc(),"/NTUPLES/FILE");

  if (!file) return StatusCode::FAILURE;

  NTuple::Directory *col=ntupleSvc()->createDirectory("/NTUPLES/FILE/COL");

  NTuplePtr nt(ntupleSvc(),"/NTUPLES/FILE/COL/MuonHitRelocation");
  if (!nt) nt=ntupleSvc()->book(col, 1, CLID_ColumnWiseTuple, "MuonHitRelocation");

  status = nt->addItem("Run",          c->run      );
  status = nt->addItem("Event",        c->event    );
  status = nt->addItem("Phi",          c->phi      );
  status = nt->addItem("Theta",        c->theta    );
  status = nt->addItem("X",            c->x        );
  status = nt->addItem("Y",            c->y        );
  status = nt->addItem("Z",            c->z        );
  status = nt->addItem("LX",           c->lx       );
  status = nt->addItem("LY",           c->ly       );
  status = nt->addItem("LZ",           c->lz       );
  status = nt->addItem("ZRESID",       c->zResid   );
  status = nt->addItem("PHIRESID",     c->phiResid );
  status = nt->addItem("DIST",         c->dist     );
  status = nt->addItem("TECH",         c->tech     );
  status = nt->addItem("STNAME",       c->stname   );
  status = nt->addItem("STETA",        c->steta    );
  status = nt->addItem("STPHI",        c->stphi    );
  status = nt->addItem("MLAYER",       c->mlayer   ); //multi layer or chamber layer
  status = nt->addItem("DBR",          c->dbr      );
  status = nt->addItem("DBZ",          c->dbz      );
  status = nt->addItem("DBPHI",        c->dbphi    );
  status = nt->addItem("LAYER",        c->layer    ); //tube layer or gas gap
  status = nt->addItem("TUBE",         c->tube     ); //tube layer or gas gap
  //status =  nt->addItem("CHANNEL",      c->channel   ); //tube number or strip number or wire gang number
  //status =  nt->addItem("MEASPHI",      c->measphi   ); 
  if(status.isFailure()) {
    log << MSG::FATAL
	<< "Unable to add some item to the ntuple"
	<< endreq;
    return status;
  }

  c->nt = nt;
  
  // Get the detector store:
  StoreGateSvc *detStore;
  status = service("DetectorStore",detStore);
  if(status.isFailure()) {
    log << MSG::FATAL
	<< "Unable to retrieve DetectorStore"
	<< endreq;
    return status;
  }


  cmuonHelper = CscHitIdHelper::GetHelper();
  rmuonHelper = RpcHitIdHelper::GetHelper();
  tmuonHelper = TgcHitIdHelper::GetHelper();
  mmuonHelper = MdtHitIdHelper::GetHelper();

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MuonHitRelocation::execute() {
  


  StoreGateSvc *stg;
  StatusCode status=service("StoreGateSvc", stg);
  MsgStream log(msgSvc(), name());
  
  const EventInfo* pevt;
  if (StatusCode::SUCCESS == stg->retrieve(pevt)) {
    
    c->event = pevt->event_ID()->event_number();
    c->run   = pevt->event_ID()->run_number();
  }


  const DataHandle<McEventCollection> mcEvent;
  StatusCode sc=stg->retrieve(mcEvent,"TruthEvent");
  if (sc.isFailure()) return StatusCode::SUCCESS; 



  DataVector<HepMC::GenEvent>::const_iterator e;
  if (mcEvent->size()!=1) return StatusCode::SUCCESS;

  Amg::Vector3D direction;
  for (e=mcEvent->begin();e!=mcEvent->end(); e++) {
    for (HepMC::GenEvent::particle_const_iterator p= (**e).particles_begin();
	 p!= (**e).particles_end(); p++) {

        float xv = (**p).production_vertex()->point3d().x();
        float yv = (**p).production_vertex()->point3d().y();
        float zv = (**p).production_vertex()->point3d().z();
 	float xd = (**p).momentum().px();
 	float yd = (**p).momentum().py();
 	float zd = (**p).momentum().pz();
 	float mag = sqrt(xd*xd + yd*yd + zd*zd);
        direction[0] = xd/mag;
        direction[1] = yd/mag;
        direction[2] = zd/mag;
        //direction = (**p).momentum().vect().normalize();
        log<<MSG::INFO<<"Event # "<<c->event<<" vertex at "<<xv<<" "<<yv<<" "<<zv
           <<" direction theta/phi = "<<direction.theta()<<" "<<direction.phi()<<endreq;
    }
  }




  if (_checkMdt)
  {
      //-------------------------------------------------------------------------------------------------------
      //-------------------------------------------------------------------------------------------------------
      // MDT:
      // 
      const DataHandle<MDTSimHitCollection> mdt_collection;
      if (stg->retrieve(mdt_collection)==StatusCode::SUCCESS) {
          log<<MSG::VERBOSE<<"MDT hit Collection found with size = "<<mdt_collection->size()<<endreq;
          for(MDTSimHitConstIterator i_hit=mdt_collection->begin() ; i_hit!=mdt_collection->end() ; ++i_hit) {
              GeoMDTHit ghit(*i_hit);

              c->event = pevt->event_ID()->event_number();
              c->run   = pevt->event_ID()->run_number();
              c->theta = direction.theta();
              c->phi   = direction.phi();
              //std::cout<<"Event # "<<c->event<<"  phi/theta  "<<c->phi<<"/"<<c->theta<<std::endl;

      
              c->lx = (*i_hit).localPosition().x();
              c->ly = (*i_hit).localPosition().y();
              c->lz = (*i_hit).localPosition().z();
              log<<MSG::DEBUG<<"        MDT hit - local coords "<<c->lx<<" "<<c->ly<<" "<<c->lz<<endreq;

              const int idHit    = (*i_hit).MDTid();
              Amg::Vector3D u = ghit.getGlobalPosition();
              c->x=u.x();
              c->y=u.y();
              c->z=u.z();
              log<<MSG::DEBUG<<"        MDT hit - global coords "<<c->x<<" "<<c->y<<" "<<c->z<<endreq;
      
	      Amg::Vector3D tmp1 = direction.cross(Amg::Vector3D(0,0,1));
	      tmp1.normalize();
	      Amg::Vector3D tmp2 = u.cross(direction);
              c->zResid   = tmp2.dot(tmp1);
              c->phiResid = tmp2.z();
              c->dist     = tmp2.mag(); // length of the vector
      
              c->tech    = 0;
              std::string stName = mmuonHelper->GetStationName(idHit);
              c->stname          = getIntStName(stName);
              c->steta           = mmuonHelper->GetZSector(idHit);
              c->stphi           = mmuonHelper->GetPhiSector(idHit);
              c->mlayer          = mmuonHelper->GetMultiLayer(idHit);
              c->dbr             = -999;
              c->dbz             = -999;
              c->dbphi           = -999;
              c->layer           = mmuonHelper->GetLayer(idHit);
              c->tube            = mmuonHelper->GetTube(idHit);
              if ( fabs(c->zResid)>0.001 || fabs(c->phiResid)>0.001 ) {
                  log<<MSG::WARNING<<"        MDT hit ---   zResid "
                     <<c->zResid<<" >1micron or phiResid "<<c->phiResid
                     <<" for stName/stEta/stphi "<<stName<<"/"<<c->steta<<"/"<<c->stphi
                     <<" ml/l/t "<<c->mlayer<<"/"<<c->layer<<"/"<<c->tube<<endreq;
              }

              ntupleSvc()->writeRecord(c->nt);
          }
      }
  }
  

  if (_checkTgc)
  {      
      //-------------------------------------------------------------------------------------------------------
      //-------------------------------------------------------------------------------------------------------
      // TGC:
      // 
      const DataHandle<TGCSimHitCollection> tgc_collection;
      if (stg->retrieve(tgc_collection)==StatusCode::SUCCESS) {
          log<<MSG::VERBOSE<<"TGC hit Collection found with size = "<<tgc_collection->size()<<endreq;
          for(TGCSimHitConstIterator i_hit=tgc_collection->begin() ; i_hit!=tgc_collection->end() ; ++i_hit) {
              GeoTGCHit ghit(*i_hit);
              c->event = pevt->event_ID()->event_number();
              c->run   = pevt->event_ID()->run_number();
              c->theta = direction.theta();
              c->phi   = direction.phi();
      
              c->lx = (*i_hit).localPosition().x();
              c->ly = (*i_hit).localPosition().y();
              c->lz = (*i_hit).localPosition().z();
              log<<MSG::DEBUG<<"        TGC hit - local coords "<<c->lx<<" "<<c->ly<<" "<<c->lz<<endreq;

              const int idHit    = (*i_hit).TGCid();
              //std::cout<<"TGC idHit = "<<idHit<<std::endl;
      
              Amg::Vector3D u = ghit.getGlobalPosition();
              c->x=u.x();
              c->y=u.y();
              c->z=u.z();
              log<<MSG::DEBUG<<"        TGC hit - global coords "<<c->x<<" "<<c->y<<" "<<c->z<<endreq;      

	      Amg::Vector3D tmp1 = direction.cross(Amg::Vector3D(0,0,1));
	      tmp1.normalize();
	      Amg::Vector3D tmp2 = u.cross(direction);
              c->zResid   = tmp2.dot(tmp1);
              c->phiResid = tmp2.z();
              c->dist     = tmp2.mag(); // length of the vector
      
              c->tech    = 3;

              std::string stName = tmuonHelper->GetStationName(idHit);
              c->stname          = getIntStName(stName);
              c->steta           = tmuonHelper->GetStationEta(idHit);
              c->stphi           = tmuonHelper->GetStationPhi(idHit);
              c->mlayer          = -999;
              c->dbr             = -999;
              c->dbz             = -999;
              c->dbphi           = -999;
              c->layer           = tmuonHelper->GetGasGap(idHit);
              c->tube            = -999;
              if ( fabs(c->zResid)>0.001 || fabs(c->phiResid)>0.001 ) {
                  log<<MSG::WARNING<<"        TGC hit ---   zResid "
                     <<c->zResid<<" >1micron or phiResid "<<c->phiResid
                     <<" for stName/stEta/stphi "<<stName<<"/"<<c->steta<<"/"<<c->stphi
                     <<" layer "<<c->layer<<endreq;
              }

              ntupleSvc()->writeRecord(c->nt);
          }
      }
  }

  if (_checkRpc)
  {
      //-------------------------------------------------------------------------------------------------------
      //-------------------------------------------------------------------------------------------------------
      // RPC:
      // 
      const DataHandle<RPCSimHitCollection> rpc_collection;
      if (stg->retrieve(rpc_collection)==StatusCode::SUCCESS) {
          log<<MSG::VERBOSE<<"RPC hit Collection found with size = "<<rpc_collection->size()<<endreq;
          for(RPCSimHitConstIterator i_hit=rpc_collection->begin() ; i_hit!=rpc_collection->end() ; ++i_hit) {

              c->event = pevt->event_ID()->event_number();
              c->run   = pevt->event_ID()->run_number();

              GeoRPCHit ghit(*i_hit);
              c->theta = direction.theta();
              c->phi   = direction.phi();
      
              c->lx = (*i_hit).localPosition().x();
              c->ly = (*i_hit).localPosition().y();
              c->lz = (*i_hit).localPosition().z();
              log<<MSG::DEBUG<<"        RPC hit - local coords "<<c->lx<<" "<<c->ly<<" "<<c->lz<<endreq;

              const int idHit    = (*i_hit).RPCid();
              Amg::Vector3D u = ghit.getGlobalPosition();
              c->x=u.x();
              c->y=u.y();
              c->z=u.z();
              log<<MSG::DEBUG<<"        RPC hit - global coords "<<c->x<<" "<<c->y<<" "<<c->z<<endreq;      
      
	      Amg::Vector3D tmp1 = direction.cross(Amg::Vector3D(0,0,1));
	      tmp1.normalize();
	      Amg::Vector3D tmp2 = u.cross(direction);
              c->zResid   = tmp2.dot(tmp1);
              c->phiResid = tmp2.z();
              c->dist     = tmp2.mag(); // length of the vector
      
              c->tech    = 2;
              std::string stName = rmuonHelper->GetStationName(idHit);
              c->stname          = getIntStName(stName);
              c->steta           = rmuonHelper->GetZSector(idHit);
              c->stphi           = rmuonHelper->GetPhiSector(idHit);
              c->mlayer          = -999;
              c->dbr             = rmuonHelper->GetDoubletR(idHit);
              c->dbz             = rmuonHelper->GetDoubletZ(idHit);
              c->dbphi           = rmuonHelper->GetDoubletPhi(idHit);
              c->layer           = rmuonHelper->GetGasGapLayer(idHit);
              c->tube            = -999;
              if ( fabs(c->zResid)>0.001 || fabs(c->phiResid)>0.001 ) {
                  log<<MSG::WARNING<<"        RPC hit ---   zResid "
                     <<c->zResid<<" >1micron or phiResid "<<c->phiResid
                     <<" for stName/stEta/stphi "<<stName<<"/"<<c->steta<<"/"<<c->stphi
                     <<" dbR/dbZ/dbP "<<c->dbr<<"/"<<c->dbz<<"/"<<c->dbphi<<" gg "<<c->layer<<endreq;
              }

              ntupleSvc()->writeRecord(c->nt);
          }
      }
  }

  if (_checkCsc)
  {
      //-------------------------------------------------------------------------------------------------------
      //-------------------------------------------------------------------------------------------------------
      // CSC:
      // 
      const DataHandle<CSCSimHitCollection> csc_collection;
      if (stg->retrieve(csc_collection)==StatusCode::SUCCESS) {
          log<<MSG::VERBOSE<<"CSC hit Collection found with size = "<<csc_collection->size()<<endreq;
          for(CSCSimHitConstIterator i_hit=csc_collection->begin() ; i_hit!=csc_collection->end() ; ++i_hit) {
              GeoCSCHit ghit(*i_hit);
              c->event = pevt->event_ID()->event_number();
              c->run   = pevt->event_ID()->run_number();
              c->theta = direction.theta();
              c->phi   = direction.phi();
      
              c->lx = 0;
              c->ly = 0;
              c->lz = 0;
              log<<MSG::DEBUG<<"        CSC hit - local coords "<<c->lx<<" "<<c->ly<<" "<<c->lz<<endreq;

              const int idHit    = (*i_hit).CSCid();
              Amg::Vector3D u = ghit.getGlobalPosition();
              c->x=u.x();
              c->y=u.y();
              c->z=u.z();
              log<<MSG::DEBUG<<"        CSC hit - global coords "<<c->x<<" "<<c->y<<" "<<c->z<<endreq;      

      
	      Amg::Vector3D tmp1 = direction.cross(Amg::Vector3D(0,0,1));
	      tmp1.normalize();
	      Amg::Vector3D tmp2 = u.cross(direction);
              c->zResid   = tmp2.dot(tmp1);
              c->phiResid = tmp2.z();
              c->dist     = tmp2.mag(); // length of the vector
      
              c->tech    = 1;
              std::string stName = cmuonHelper->GetStationName(idHit);
              c->stname          = getIntStName(stName);
              c->steta           = cmuonHelper->GetZSector(idHit);
              c->stphi           = cmuonHelper->GetPhiSector(idHit);
              c->mlayer          = cmuonHelper->GetChamberLayer(idHit);
              c->dbr             = -999;
              c->dbz             = -999;
              c->dbphi           = -999;
              c->layer           = cmuonHelper->GetWireLayer(idHit);
              c->tube            = -999;
              if ( fabs(c->zResid)>0.001 || fabs(c->phiResid)>0.001 ) {
                  log<<MSG::WARNING<<"        CSC hit ---   zResid "
                     <<c->zResid<<" >1micron or phiResid "<<c->phiResid
                     <<" for stName/stEta/stphi "<<stName<<"/"<<c->steta<<"/"<<c->stphi
                     <<" clay/lay "<<c->mlayer<<"/"<<c->layer<<endreq;
              }
      
              ntupleSvc()->writeRecord(c->nt);
          }
      }
  }

  if (_checksTgc)
  {      
      //-------------------------------------------------------------------------------------------------------
      //-------------------------------------------------------------------------------------------------------
      // sTGC:
      // 
      stmuonHelper=sTgcHitIdHelper::GetHelper();
	
      const DataHandle<GenericMuonSimHitCollection> stgc_collection;
      if (stg->retrieve(stgc_collection,"sTGCSensitiveDetector")==StatusCode::SUCCESS) {
          log<<MSG::VERBOSE<<"Generic Muon hit Collection sTGC found with size = "<<stgc_collection->size()<<endreq;
          for(GenericMuonSimHitConstIterator i_hit=stgc_collection->begin() ; i_hit!=stgc_collection->end() ; ++i_hit) {
	    
              GeosTGCHit ghit(*i_hit);

              c->event = pevt->event_ID()->event_number();
              c->run   = pevt->event_ID()->run_number();
              c->theta = direction.theta();
              c->phi   = direction.phi();
      
              c->lx = (*i_hit).localPosition().x();
              c->ly = (*i_hit).localPosition().y();
              c->lz = (*i_hit).localPosition().z();
              log<<MSG::DEBUG<<"        sTGC hit - local coords "<<c->lx<<" "<<c->ly<<" "<<c->lz<<endreq;

              const int idHit    = (*i_hit).GenericId();
              //std::cout<<"TGC idHit = "<<idHit<<std::endl;
      
              Amg::Vector3D u = ghit.getGlobalPosition();
              c->x=u.x();
              c->y=u.y();
              c->z=u.z();
              log<<MSG::DEBUG<<"       sTGC hit - OrigGlobCoord "
		 <<(*i_hit).globalPosition().x()<<" "
		 <<(*i_hit).globalPosition().y()<<" "
		 <<(*i_hit).globalPosition().z()<<endreq;      
              log<<MSG::DEBUG<<"       sTGC hit - global coords "<<c->x<<" "<<c->y<<" "<<c->z<<endreq;      

	      Amg::Vector3D tmp1 = direction.cross(Amg::Vector3D(0,0,1));
	      tmp1.normalize();
	      Amg::Vector3D tmp2 = u.cross(direction);
              c->zResid   = tmp2.dot(tmp1);
              c->phiResid = tmp2.z();
              c->dist     = tmp2.mag(); // length of the vector
      
              c->tech    = 4;

              std::string stName = stmuonHelper->GetStationName(idHit);
              c->stname          = getIntStName(stName);
	      int side = stmuonHelper->GetSide(idHit);
	      int eta  = stmuonHelper->GetZSector(idHit);
              c->steta           = (side == 1 ? eta+1 : -eta-1);
              c->stphi           = (stmuonHelper->GetPhiSector(idHit)-1)/2+1;
              c->mlayer          = stmuonHelper->GetMultiLayer(idHit);
              c->dbr             = -999;
              c->dbz             = -999;
              c->dbphi           = -999;
              c->layer           = stmuonHelper->GetLayer(idHit);
              c->tube            = -999;
              if ( fabs(c->zResid)>0.001 || fabs(c->phiResid)>0.001 ) {
                  log<<MSG::WARNING<<"        sTGC hit ---   zResid "
                     <<c->zResid<<" >1micron or phiResid "<<c->phiResid
                     <<" for stName/stEta/stphi "<<stName<<"/"<<c->steta<<"/"<<c->stphi
                     <<" layer "<<c->layer<<endreq;
              }

              ntupleSvc()->writeRecord(c->nt);
          }
      }
  }
  if (_checkMM)
  {      
      //-------------------------------------------------------------------------------------------------------
      //-------------------------------------------------------------------------------------------------------
      // MM:
      // 
      mmmuonHelper=MicromegasHitIdHelper::GetHelper();
	
      const DataHandle<GenericMuonSimHitCollection> mm_collection;
      if (stg->retrieve(mm_collection,"MicromegasSensitiveDetector")==StatusCode::SUCCESS) {
          log<<MSG::VERBOSE<<"Generic Muon hit Collection (Micromegas) found with size = "<<mm_collection->size()<<endreq;
          for(GenericMuonSimHitConstIterator i_hit=mm_collection->begin() ; i_hit!=mm_collection->end() ; ++i_hit) {
	    
              GeoMMHit ghit(*i_hit);

              c->event = pevt->event_ID()->event_number();
              c->run   = pevt->event_ID()->run_number();
              c->theta = direction.theta();
              c->phi   = direction.phi();
      
              c->lx = (*i_hit).localPosition().x();
              c->ly = (*i_hit).localPosition().y();
              c->lz = (*i_hit).localPosition().z();
              log<<MSG::DEBUG<<"        MM hit - local coords "<<c->lx<<" "<<c->ly<<" "<<c->lz<<endreq;

              const int idHit    = (*i_hit).GenericId();
      
              Amg::Vector3D u = ghit.getGlobalPosition();
              c->x=u.x();
              c->y=u.y();
              c->z=u.z();
              log<<MSG::DEBUG<<"        MM hit - OrigGlobCoord "
		 <<(*i_hit).globalPosition().x()<<" "
		 <<(*i_hit).globalPosition().y()<<" "
		 <<(*i_hit).globalPosition().z()<<endreq;      
              log<<MSG::DEBUG<<"        MM hit - global coords "<<c->x<<" "<<c->y<<" "<<c->z<<endreq;   
              Amg::Vector3D gtrk = ghit.getTrkGlobalPosition();
              Amg::Vector3D ltrk = ghit.getTrkLocalPosition();
              log<<MSG::DEBUG<<"        MM h trk  local coords "<<ltrk.x()<<" "<<ltrk.y()<<" "<<ltrk.z()<<endreq;   
              log<<MSG::DEBUG<<"        MM h trk global coords "<<gtrk.x()<<" "<<gtrk.y()<<" "<<gtrk.z()<<endreq;   


	      Amg::Vector3D tmp1 = direction.cross(Amg::Vector3D(0,0,1));
	      tmp1.normalize();
	      Amg::Vector3D tmp2 = u.cross(direction);
              c->zResid   = tmp2.dot(tmp1);
              c->phiResid = tmp2.z();
              c->dist     = tmp2.mag(); // length of the vector
      
              c->tech    = 5;

              std::string stName = mmmuonHelper->GetStationName(idHit);
              c->stname          = getIntStName(stName);
	      int side = mmmuonHelper->GetSide(idHit);
	      int eta  = mmmuonHelper->GetZSector(idHit);
              c->steta           = (side == 1 ? eta+1 : -eta-1);
              c->stphi           = (mmmuonHelper->GetPhiSector(idHit)-1)/2+1;
              c->mlayer          =  mmmuonHelper->GetMultiLayer(idHit);
              c->dbr             = -999;
              c->dbz             = -999;
              c->dbphi           = -999;
              c->layer           = mmmuonHelper->GetLayer(idHit);
              c->tube            = -999;
              if ( fabs(c->zResid)>0.001 || fabs(c->phiResid)>0.001 ) {
                  log<<MSG::WARNING<<"        MM hit ---   zResid "
                     <<c->zResid<<" >1micron or phiResid "<<c->phiResid
                     <<" for stName/stEta/stphi "<<stName<<"/"<<c->steta<<"/"<<c->stphi
                     <<" layer "<<c->layer<<endreq;
              }

              ntupleSvc()->writeRecord(c->nt);
          }
      }
  }
  
  //-------------------------------------------------------------------------------------------------------
  return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MuonHitRelocation::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "GOODBYE from MuonHitRelocation" << endreq;

  return StatusCode::SUCCESS;
}

long MuonHitRelocation::getIntStName(std::string stName) const
{
    if (stName == "BIL")       return 0;
    else if (stName == "BIS" ) return 1;
    else if (stName == "BML" ) return 2;
    else if (stName == "BMS" ) return 3;
    else if (stName == "BOL" ) return 4;
    else if (stName == "BOS" ) return 5;
    else if (stName == "BEE" ) return 6;
    else if (stName == "BIR" ) return 7;
    else if (stName == "BMF" ) return 8;
    else if (stName == "BOF" ) return 9;
    else if (stName == "BOG" ) return 10;
    else if (stName == "BOH" ) return 11;
    else if (stName == "EIC" ) return 12;
    else if (stName == "EIL" ) return 13;
    else if (stName == "EEL" ) return 14;
    else if (stName == "EES" ) return 15;
    else if (stName == "EMC" ) return 16;
    else if (stName == "EML" ) return 17;
    else if (stName == "EMS" ) return 18;
    else if (stName == "EOC" ) return 19;
    else if (stName == "EOL" ) return 20;
    else if (stName == "EOS" ) return 21;
    else if (stName == "T1C" ) return 22;
    else if (stName == "T1L" ) return 23;
    else if (stName == "T1S" ) return 24;
    else if (stName == "T2C" ) return 25;
    else if (stName == "T2L" ) return 26;
    else if (stName == "T2S" ) return 27;
    else if (stName == "T3C" ) return 28;
    else if (stName == "T3L" ) return 29;
    else if (stName == "T3S" ) return 30;
    else if (stName == "CI1" ) return 31;
    else if (stName == "CI2" ) return 32;
    else if (stName == "CI3" ) return 33;
    else if (stName == "CI4" ) return 34;
    else if (stName == "FIL" ) return 35;
    else if (stName == "FIS" ) return 36;
    else if (stName == "FML" ) return 37;
    else if (stName == "FMS" ) return 38;
    else if (stName == "FOL" ) return 39;
    else if (stName == "FOS" ) return 40;
    else if (stName == "T1F" ) return 41;
    else if (stName == "T1E" ) return 42;
    else if (stName == "T2F" ) return 43;
    else if (stName == "T2E" ) return 44;
    else if (stName == "T3F" ) return 45;
    else if (stName == "T3E" ) return 46;
    else if (stName == "T4F" ) return 47;
    else if (stName == "T4E" ) return 48;
    else if (stName == "EIS" ) return 49;
    else if (stName == "CSS" ) return 50;
    else if (stName == "CSL" ) return 51;
    else if (stName == "BIM" ) return 52;
    else return -999;
}






