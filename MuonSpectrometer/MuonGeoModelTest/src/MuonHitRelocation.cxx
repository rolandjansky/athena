/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModelTest/MuonHitRelocation.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "StoreGate/StoreGateSvc.h"
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


MuonHitRelocation::Clockwork::Clockwork() : nt(0)
{
}

MuonHitRelocation::Clockwork::~Clockwork() {
}


/////////////////////////////////////////////////////////////////////////////

MuonHitRelocation::MuonHitRelocation(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),m_c(new Clockwork()), m_rmuonHelper(0), m_mmuonHelper(0),
  m_tmuonHelper(0), m_cmuonHelper(0), m_stmuonHelper(0), m_mmmuonHelper(0)
{
    declareProperty("checkMdt", m_checkMdt = true );
    declareProperty("checkRpc", m_checkRpc = true );
    declareProperty("checkTgc", m_checkTgc = true );
    declareProperty("checkCsc", m_checkCsc = true );
    declareProperty("checksTgc", m_checksTgc = true );
    declareProperty("checkMM", m_checkMM = true );
}

MuonHitRelocation::~MuonHitRelocation() {
  delete m_c;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MuonHitRelocation::initialize(){

  ATH_MSG_INFO( "HELLO from MuonHitRelocation"  );
  
  NTupleFilePtr     file(ntupleSvc(),"/NTUPLES/FILE");

  if (!file) return StatusCode::FAILURE;

  NTuple::Directory *col=ntupleSvc()->createDirectory("/NTUPLES/FILE/COL");

  NTuplePtr nt(ntupleSvc(),"/NTUPLES/FILE/COL/MuonHitRelocation");
  if (!nt) nt=ntupleSvc()->book(col, 1, CLID_ColumnWiseTuple, "MuonHitRelocation");

  ATH_CHECK( nt->addItem("Run",          m_c->run      ) );
  ATH_CHECK( nt->addItem("Event",        m_c->event    ) );
  ATH_CHECK( nt->addItem("Phi",          m_c->phi      ) );
  ATH_CHECK( nt->addItem("Theta",        m_c->theta    ) );
  ATH_CHECK( nt->addItem("X",            m_c->x        ) );
  ATH_CHECK( nt->addItem("Y",            m_c->y        ) );
  ATH_CHECK( nt->addItem("Z",            m_c->z        ) );
  ATH_CHECK( nt->addItem("LX",           m_c->lx       ) );
  ATH_CHECK( nt->addItem("LY",           m_c->ly       ) );
  ATH_CHECK( nt->addItem("LZ",           m_c->lz       ) );
  ATH_CHECK( nt->addItem("ZRESID",       m_c->zResid   ) );
  ATH_CHECK( nt->addItem("PHIRESID",     m_c->phiResid ) );
  ATH_CHECK( nt->addItem("DIST",         m_c->dist     ) );
  ATH_CHECK( nt->addItem("TECH",         m_c->tech     ) );
  ATH_CHECK( nt->addItem("STNAME",       m_c->stname   ) );
  ATH_CHECK( nt->addItem("STETA",        m_c->steta    ) );
  ATH_CHECK( nt->addItem("STPHI",        m_c->stphi    ) );
  ATH_CHECK( nt->addItem("MLAYER",       m_c->mlayer   ) ); //multi layer or chamber layer
  ATH_CHECK( nt->addItem("DBR",          m_c->dbr      ) );
  ATH_CHECK( nt->addItem("DBZ",          m_c->dbz      ) );
  ATH_CHECK( nt->addItem("DBPHI",        m_c->dbphi    ) );
  ATH_CHECK( nt->addItem("LAYER",        m_c->layer    ) ); //tube layer or gas gap
  ATH_CHECK( nt->addItem("TUBE",         m_c->tube     ) ); //tube layer or gas gap
  //status =  nt->addItem("CHANNEL",      m_c->channel   ); //tube number or strip number or wire gang number
  //status =  nt->addItem("MEASPHI",      m_c->measphi   ); 

  m_c->nt = nt;
  
  m_cmuonHelper = CscHitIdHelper::GetHelper();
  m_rmuonHelper = RpcHitIdHelper::GetHelper();
  m_tmuonHelper = TgcHitIdHelper::GetHelper();
  m_mmuonHelper = MdtHitIdHelper::GetHelper();

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MuonHitRelocation::execute() {
  
  const EventContext& context = getContext();
  m_c->event = context.eventID().event_number();
  m_c->run   = context.eventID().run_number();

  const DataHandle<McEventCollection> mcEvent;
  StatusCode sc=evtStore()->retrieve(mcEvent,"TruthEvent");
  if (sc.isFailure()) return StatusCode::SUCCESS; 



  DataVector<HepMC::GenEvent>::const_iterator e;
  if (mcEvent->size()!=1) return StatusCode::SUCCESS;

  Amg::Vector3D direction(0., 0., 0.);
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
        ATH_MSG_INFO("Event # "<<m_c->event<<" vertex at "<<xv<<" "<<yv<<" "<<zv
                     <<" direction theta/phi = "<<direction.theta()<<" "<<direction.phi() );
    }
  }




  if (m_checkMdt)
  {
      //-------------------------------------------------------------------------------------------------------
      //-------------------------------------------------------------------------------------------------------
      // MDT:
      // 
      const DataHandle<MDTSimHitCollection> mdt_collection;
      if (evtStore()->retrieve(mdt_collection)==StatusCode::SUCCESS) {
          ATH_MSG_VERBOSE("MDT hit Collection found with size = "<<mdt_collection->size() );
          for(MDTSimHitConstIterator i_hit=mdt_collection->begin() ; i_hit!=mdt_collection->end() ; ++i_hit) {
              GeoMDTHit ghit(*i_hit);

              m_c->event = context.eventID().event_number();
              m_c->run   = context.eventID().run_number();
              m_c->theta = direction.theta();
              m_c->phi   = direction.phi();
              //std::cout<<"Event # "<<m_c->event<<"  phi/theta  "<<m_c->phi<<"/"<<m_c->theta<<std::endl;

      
              m_c->lx = (*i_hit).localPosition().x();
              m_c->ly = (*i_hit).localPosition().y();
              m_c->lz = (*i_hit).localPosition().z();
              ATH_MSG_DEBUG("        MDT hit - local coords "<<m_c->lx<<" "<<m_c->ly<<" "<<m_c->lz );

              const int idHit    = (*i_hit).MDTid();
              Amg::Vector3D u = ghit.getGlobalPosition();
              m_c->x=u.x();
              m_c->y=u.y();
              m_c->z=u.z();
              ATH_MSG_DEBUG("        MDT hit - global coords "<<m_c->x<<" "<<m_c->y<<" "<<m_c->z );
      
	      Amg::Vector3D tmp1 = direction.cross(Amg::Vector3D(0,0,1));
	      tmp1.normalize();
	      Amg::Vector3D tmp2 = u.cross(direction);
              m_c->zResid   = tmp2.dot(tmp1);
              m_c->phiResid = tmp2.z();
              m_c->dist     = tmp2.mag(); // length of the vector
      
              m_c->tech    = 0;
              std::string stName = m_mmuonHelper->GetStationName(idHit);
              m_c->stname          = getIntStName(stName);
              m_c->steta           = m_mmuonHelper->GetZSector(idHit);
              m_c->stphi           = m_mmuonHelper->GetPhiSector(idHit);
              m_c->mlayer          = m_mmuonHelper->GetMultiLayer(idHit);
              m_c->dbr             = -999;
              m_c->dbz             = -999;
              m_c->dbphi           = -999;
              m_c->layer           = m_mmuonHelper->GetLayer(idHit);
              m_c->tube            = m_mmuonHelper->GetTube(idHit);
              if ( fabs(m_c->zResid)>0.001 || fabs(m_c->phiResid)>0.001 ) {
                ATH_MSG_WARNING("        MDT hit ---   zResid "
                                <<m_c->zResid<<" >1micron or phiResid "<<m_c->phiResid
                                <<" for stName/stEta/stphi "<<stName<<"/"<<m_c->steta<<"/"<<m_c->stphi
                                <<" ml/l/t "<<m_c->mlayer<<"/"<<m_c->layer<<"/"<<m_c->tube );
              }

              ntupleSvc()->writeRecord(m_c->nt);
          }
      }
  }
  

  if (m_checkTgc)
  {      
      //-------------------------------------------------------------------------------------------------------
      //-------------------------------------------------------------------------------------------------------
      // TGC:
      // 
      const DataHandle<TGCSimHitCollection> tgc_collection;
      if (evtStore()->retrieve(tgc_collection)==StatusCode::SUCCESS) {
          ATH_MSG_VERBOSE("TGC hit Collection found with size = "<<tgc_collection->size() );
          for(TGCSimHitConstIterator i_hit=tgc_collection->begin() ; i_hit!=tgc_collection->end() ; ++i_hit) {
              GeoTGCHit ghit(*i_hit);
              m_c->event = context.eventID().event_number();
              m_c->run   = context.eventID().run_number();
              m_c->theta = direction.theta();
              m_c->phi   = direction.phi();
      
              m_c->lx = (*i_hit).localPosition().x();
              m_c->ly = (*i_hit).localPosition().y();
              m_c->lz = (*i_hit).localPosition().z();
              ATH_MSG_DEBUG("        TGC hit - local coords "<<m_c->lx<<" "<<m_c->ly<<" "<<m_c->lz );

              const int idHit    = (*i_hit).TGCid();
              //std::cout<<"TGC idHit = "<<idHit<<std::endl;
      
              Amg::Vector3D u = ghit.getGlobalPosition();
              m_c->x=u.x();
              m_c->y=u.y();
              m_c->z=u.z();
              ATH_MSG_DEBUG("        TGC hit - global coords "<<m_c->x<<" "<<m_c->y<<" "<<m_c->z );

	      Amg::Vector3D tmp1 = direction.cross(Amg::Vector3D(0,0,1));
	      tmp1.normalize();
	      Amg::Vector3D tmp2 = u.cross(direction);
              m_c->zResid   = tmp2.dot(tmp1);
              m_c->phiResid = tmp2.z();
              m_c->dist     = tmp2.mag(); // length of the vector
      
              m_c->tech    = 3;

              std::string stName = m_tmuonHelper->GetStationName(idHit);
              m_c->stname          = getIntStName(stName);
              m_c->steta           = m_tmuonHelper->GetStationEta(idHit);
              m_c->stphi           = m_tmuonHelper->GetStationPhi(idHit);
              m_c->mlayer          = -999;
              m_c->dbr             = -999;
              m_c->dbz             = -999;
              m_c->dbphi           = -999;
              m_c->layer           = m_tmuonHelper->GetGasGap(idHit);
              m_c->tube            = -999;
              if ( fabs(m_c->zResid)>0.001 || fabs(m_c->phiResid)>0.001 ) {
                ATH_MSG_WARNING("        TGC hit ---   zResid "
                                <<m_c->zResid<<" >1micron or phiResid "<<m_c->phiResid
                                <<" for stName/stEta/stphi "<<stName<<"/"<<m_c->steta<<"/"<<m_c->stphi
                                <<" layer "<<m_c->layer );
              }

              ntupleSvc()->writeRecord(m_c->nt);
          }
      }
  }

  if (m_checkRpc)
  {
      //-------------------------------------------------------------------------------------------------------
      //-------------------------------------------------------------------------------------------------------
      // RPC:
      // 
      const DataHandle<RPCSimHitCollection> rpc_collection;
      if (evtStore()->retrieve(rpc_collection)==StatusCode::SUCCESS) {
          ATH_MSG_VERBOSE("RPC hit Collection found with size = "<<rpc_collection->size() );
          for(RPCSimHitConstIterator i_hit=rpc_collection->begin() ; i_hit!=rpc_collection->end() ; ++i_hit) {

              m_c->event = context.eventID().event_number();
              m_c->run   = context.eventID().run_number();

              GeoRPCHit ghit(*i_hit);
              m_c->theta = direction.theta();
              m_c->phi   = direction.phi();
      
              m_c->lx = (*i_hit).localPosition().x();
              m_c->ly = (*i_hit).localPosition().y();
              m_c->lz = (*i_hit).localPosition().z();
              ATH_MSG_DEBUG("        RPC hit - local coords "<<m_c->lx<<" "<<m_c->ly<<" "<<m_c->lz );

              const int idHit    = (*i_hit).RPCid();
              Amg::Vector3D u = ghit.getGlobalPosition();
              m_c->x=u.x();
              m_c->y=u.y();
              m_c->z=u.z();
              ATH_MSG_DEBUG("        RPC hit - global coords "<<m_c->x<<" "<<m_c->y<<" "<<m_c->z );
      
	      Amg::Vector3D tmp1 = direction.cross(Amg::Vector3D(0,0,1));
	      tmp1.normalize();
	      Amg::Vector3D tmp2 = u.cross(direction);
              m_c->zResid   = tmp2.dot(tmp1);
              m_c->phiResid = tmp2.z();
              m_c->dist     = tmp2.mag(); // length of the vector
      
              m_c->tech    = 2;
              std::string stName = m_rmuonHelper->GetStationName(idHit);
              m_c->stname          = getIntStName(stName);
              m_c->steta           = m_rmuonHelper->GetZSector(idHit);
              m_c->stphi           = m_rmuonHelper->GetPhiSector(idHit);
              m_c->mlayer          = -999;
              m_c->dbr             = m_rmuonHelper->GetDoubletR(idHit);
              m_c->dbz             = m_rmuonHelper->GetDoubletZ(idHit);
              m_c->dbphi           = m_rmuonHelper->GetDoubletPhi(idHit);
              m_c->layer           = m_rmuonHelper->GetGasGapLayer(idHit);
              m_c->tube            = -999;
              if ( fabs(m_c->zResid)>0.001 || fabs(m_c->phiResid)>0.001 ) {
                ATH_MSG_WARNING("        RPC hit ---   zResid "
                                <<m_c->zResid<<" >1micron or phiResid "<<m_c->phiResid
                                <<" for stName/stEta/stphi "<<stName<<"/"<<m_c->steta<<"/"<<m_c->stphi
                                <<" dbR/dbZ/dbP "<<m_c->dbr<<"/"<<m_c->dbz<<"/"<<m_c->dbphi<<" gg "<<m_c->layer );
              }

              ntupleSvc()->writeRecord(m_c->nt);
          }
      }
  }

  if (m_checkCsc)
  {
      //-------------------------------------------------------------------------------------------------------
      //-------------------------------------------------------------------------------------------------------
      // CSC:
      // 
      const DataHandle<CSCSimHitCollection> csc_collection;
      if (evtStore()->retrieve(csc_collection)==StatusCode::SUCCESS) {
          ATH_MSG_VERBOSE("CSC hit Collection found with size = "<<csc_collection->size() );
          for(CSCSimHitConstIterator i_hit=csc_collection->begin() ; i_hit!=csc_collection->end() ; ++i_hit) {
              GeoCSCHit ghit(*i_hit);
              m_c->event = context.eventID().event_number();
              m_c->run   = context.eventID().run_number();
              m_c->theta = direction.theta();
              m_c->phi   = direction.phi();
      
              m_c->lx = 0;
              m_c->ly = 0;
              m_c->lz = 0;
              ATH_MSG_DEBUG("        CSC hit - local coords "<<m_c->lx<<" "<<m_c->ly<<" "<<m_c->lz );

              const int idHit    = (*i_hit).CSCid();
              Amg::Vector3D u = ghit.getGlobalPosition();
              m_c->x=u.x();
              m_c->y=u.y();
              m_c->z=u.z();
              ATH_MSG_DEBUG("        CSC hit - global coords "<<m_c->x<<" "<<m_c->y<<" "<<m_c->z );

      
	      Amg::Vector3D tmp1 = direction.cross(Amg::Vector3D(0,0,1));
	      tmp1.normalize();
	      Amg::Vector3D tmp2 = u.cross(direction);
              m_c->zResid   = tmp2.dot(tmp1);
              m_c->phiResid = tmp2.z();
              m_c->dist     = tmp2.mag(); // length of the vector
      
              m_c->tech    = 1;
              std::string stName = m_cmuonHelper->GetStationName(idHit);
              m_c->stname          = getIntStName(stName);
              m_c->steta           = m_cmuonHelper->GetZSector(idHit);
              m_c->stphi           = m_cmuonHelper->GetPhiSector(idHit);
              m_c->mlayer          = m_cmuonHelper->GetChamberLayer(idHit);
              m_c->dbr             = -999;
              m_c->dbz             = -999;
              m_c->dbphi           = -999;
              m_c->layer           = m_cmuonHelper->GetWireLayer(idHit);
              m_c->tube            = -999;
              if ( fabs(m_c->zResid)>0.001 || fabs(m_c->phiResid)>0.001 ) {
                ATH_MSG_WARNING("        CSC hit ---   zResid "
                                <<m_c->zResid<<" >1micron or phiResid "<<m_c->phiResid
                                <<" for stName/stEta/stphi "<<stName<<"/"<<m_c->steta<<"/"<<m_c->stphi
                                <<" clay/lay "<<m_c->mlayer<<"/"<<m_c->layer );
              }
      
              ntupleSvc()->writeRecord(m_c->nt);
          }
      }
  }

  if (m_checksTgc)
  {      
      //-------------------------------------------------------------------------------------------------------
      //-------------------------------------------------------------------------------------------------------
      // sTGC:
      // 
      m_stmuonHelper=sTgcHitIdHelper::GetHelper();
	
      const DataHandle<GenericMuonSimHitCollection> stgc_collection;
      if (evtStore()->retrieve(stgc_collection,"sTGCSensitiveDetector")==StatusCode::SUCCESS) {
          ATH_MSG_VERBOSE("Generic Muon hit Collection sTGC found with size = "<<stgc_collection->size() );
          for(GenericMuonSimHitConstIterator i_hit=stgc_collection->begin() ; i_hit!=stgc_collection->end() ; ++i_hit) {
	    
              GeosTGCHit ghit(*i_hit);

              m_c->event = context.eventID().event_number();
              m_c->run   = context.eventID().run_number();
              m_c->theta = direction.theta();
              m_c->phi   = direction.phi();
      
              m_c->lx = (*i_hit).localPosition().x();
              m_c->ly = (*i_hit).localPosition().y();
              m_c->lz = (*i_hit).localPosition().z();
              ATH_MSG_DEBUG("        sTGC hit - local coords "<<m_c->lx<<" "<<m_c->ly<<" "<<m_c->lz );

              const int idHit    = (*i_hit).GenericId();
              //std::cout<<"TGC idHit = "<<idHit<<std::endl;
      
              Amg::Vector3D u = ghit.getGlobalPosition();
              m_c->x=u.x();
              m_c->y=u.y();
              m_c->z=u.z();
              ATH_MSG_DEBUG("       sTGC hit - OrigGlobCoord "
                            <<(*i_hit).globalPosition().x()<<" "
                            <<(*i_hit).globalPosition().y()<<" "
                            <<(*i_hit).globalPosition().z() );
              ATH_MSG_DEBUG("       sTGC hit - global coords "<<m_c->x<<" "<<m_c->y<<" "<<m_c->z );

	      Amg::Vector3D tmp1 = direction.cross(Amg::Vector3D(0,0,1));
	      tmp1.normalize();
	      Amg::Vector3D tmp2 = u.cross(direction);
              m_c->zResid   = tmp2.dot(tmp1);
              m_c->phiResid = tmp2.z();
              m_c->dist     = tmp2.mag(); // length of the vector
      
              m_c->tech    = 4;

              std::string stName = m_stmuonHelper->GetStationName(idHit);
              m_c->stname          = getIntStName(stName);
	      int side = m_stmuonHelper->GetSide(idHit);
	      int eta  = m_stmuonHelper->GetZSector(idHit);
              m_c->steta           = (side == 1 ? eta+1 : -eta-1);
              m_c->stphi           = (m_stmuonHelper->GetPhiSector(idHit)-1)/2+1;
              m_c->mlayer          = m_stmuonHelper->GetMultiLayer(idHit);
              m_c->dbr             = -999;
              m_c->dbz             = -999;
              m_c->dbphi           = -999;
              m_c->layer           = m_stmuonHelper->GetLayer(idHit);
              m_c->tube            = -999;
              if ( fabs(m_c->zResid)>0.001 || fabs(m_c->phiResid)>0.001 ) {
                ATH_MSG_WARNING("        sTGC hit ---   zResid "
                                <<m_c->zResid<<" >1micron or phiResid "<<m_c->phiResid
                                <<" for stName/stEta/stphi "<<stName<<"/"<<m_c->steta<<"/"<<m_c->stphi
                                <<" layer "<<m_c->layer );
              }

              ntupleSvc()->writeRecord(m_c->nt);
          }
      }
  }
  if (m_checkMM)
  {      
      //-------------------------------------------------------------------------------------------------------
      //-------------------------------------------------------------------------------------------------------
      // MM:
      // 
      m_mmmuonHelper=MicromegasHitIdHelper::GetHelper();
	
      const DataHandle<GenericMuonSimHitCollection> mm_collection;
      if (evtStore()->retrieve(mm_collection,"MicromegasSensitiveDetector")==StatusCode::SUCCESS) {
          ATH_MSG_VERBOSE("Generic Muon hit Collection (Micromegas) found with size = "<<mm_collection->size() );
          for(GenericMuonSimHitConstIterator i_hit=mm_collection->begin() ; i_hit!=mm_collection->end() ; ++i_hit) {
	    
              GeoMMHit ghit(*i_hit);

              m_c->event = context.eventID().event_number();
              m_c->run   = context.eventID().run_number();
              m_c->theta = direction.theta();
              m_c->phi   = direction.phi();
      
              m_c->lx = (*i_hit).localPosition().x();
              m_c->ly = (*i_hit).localPosition().y();
              m_c->lz = (*i_hit).localPosition().z();
              ATH_MSG_DEBUG("        MM hit - local coords "<<m_c->lx<<" "<<m_c->ly<<" "<<m_c->lz );

              const int idHit    = (*i_hit).GenericId();
      
              Amg::Vector3D u = ghit.getGlobalPosition();
              m_c->x=u.x();
              m_c->y=u.y();
              m_c->z=u.z();
              ATH_MSG_DEBUG("        MM hit - OrigGlobCoord "
                            <<(*i_hit).globalPosition().x()<<" "
                            <<(*i_hit).globalPosition().y()<<" "
                            <<(*i_hit).globalPosition().z() );
              ATH_MSG_DEBUG("        MM hit - global coords "<<m_c->x<<" "<<m_c->y<<" "<<m_c->z );
              Amg::Vector3D gtrk = ghit.getTrkGlobalPosition();
              Amg::Vector3D ltrk = ghit.getTrkLocalPosition();
              ATH_MSG_DEBUG("        MM h trk  local coords "<<ltrk.x()<<" "<<ltrk.y()<<" "<<ltrk.z() );
              ATH_MSG_DEBUG("        MM h trk global coords "<<gtrk.x()<<" "<<gtrk.y()<<" "<<gtrk.z() );


	      Amg::Vector3D tmp1 = direction.cross(Amg::Vector3D(0,0,1));
	      tmp1.normalize();
	      Amg::Vector3D tmp2 = u.cross(direction);
              m_c->zResid   = tmp2.dot(tmp1);
              m_c->phiResid = tmp2.z();
              m_c->dist     = tmp2.mag(); // length of the vector
      
              m_c->tech    = 5;

              std::string stName = m_mmmuonHelper->GetStationName(idHit);
              m_c->stname          = getIntStName(stName);
	      int side = m_mmmuonHelper->GetSide(idHit);
	      int eta  = m_mmmuonHelper->GetZSector(idHit);
              m_c->steta           = (side == 1 ? eta+1 : -eta-1);
              m_c->stphi           = (m_mmmuonHelper->GetPhiSector(idHit)-1)/2+1;
              m_c->mlayer          =  m_mmmuonHelper->GetMultiLayer(idHit);
              m_c->dbr             = -999;
              m_c->dbz             = -999;
              m_c->dbphi           = -999;
              m_c->layer           = m_mmmuonHelper->GetLayer(idHit);
              m_c->tube            = -999;
              if ( fabs(m_c->zResid)>0.001 || fabs(m_c->phiResid)>0.001 ) {
                ATH_MSG_WARNING("        MM hit ---   zResid "
                                <<m_c->zResid<<" >1micron or phiResid "<<m_c->phiResid
                                <<" for stName/stEta/stphi "<<stName<<"/"<<m_c->steta<<"/"<<m_c->stphi
                                <<" layer "<<m_c->layer );
              }

              ntupleSvc()->writeRecord(m_c->nt);
          }
      }
  }
  
  //-------------------------------------------------------------------------------------------------------
  return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MuonHitRelocation::finalize() {

  ATH_MSG_INFO( "GOODBYE from MuonHitRelocation"  );
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






