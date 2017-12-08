/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSimHitToPrdTest/RPC_SimHitToPrdCBNTAlgo.h"


#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"



#include "GeneratorObjects/McEventCollection.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonRDO/RpcFiredChannel.h" 
#include "MuonRDO/RpcCoinMatrix.h" 
#include "MuonRDO/RpcPad.h" 
#include "MuonRDO/RpcPadContainer.h" 
#include "MuonPrepRawData/RpcPrepDataContainer.h" 

#include "Identifier/Identifier.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonSimEvent/RpcHitIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "GeoAdaptors/GeoMuonHits.h"

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/INTupleSvc.h"


#define pi 3.141592654
static const int maxPad           =   50;
static const int maxCMA           =  400;
static const int maxFiredChannels = 20000;
static const int  MAX_PART     = 100;
static const int  MAX_SIMHRPC  = 10000;
static const int  MAX_DIGITRPC = 10000;
static const int  MAX_PADRPC   = 1000;
static const int  MAX_RDOCHRPC = 10000;
static const int  MAX_PRDRPC   = 10000;


class RPC_SimHitToPrdCBNTAlgo::Clockwork {

public:

  // Constructor:
  Clockwork();

  // Destructor:
  ~Clockwork();




  // Point to NTuple:
  NTuple::Tuple  *nt;
  
  NTuple::Item<long> run;
  NTuple::Item<long> event;

  NTuple::Item<int> m_npartStored;
  NTuple::Array<float> m_partVertX   ;
  NTuple::Array<float> m_partVertY   ;
  NTuple::Array<float> m_partVertZ   ;
  NTuple::Array<float> m_partVertT   ;
  NTuple::Array<float> m_partPdgId   ;
  NTuple::Array<float> m_partMomX    ;
  NTuple::Array<float> m_partMomY    ;
  NTuple::Array<float> m_partMomZ    ;
  NTuple::Array<int>   m_partStatus  ;
  NTuple::Array<int>   m_partBarcode ;
  NTuple::Array<float> m_partMass    ;


  NTuple::Item<int>    m_SimHit_nRpc;              // number of SimHits in the collection  
  
  NTuple::Array<int>  m_SimHit_station     ;    // Return the station
  NTuple::Array<int>  m_SimHit_eta         ;    // Return the station eta
  NTuple::Array<int>  m_SimHit_phi         ;    // Return the station phi
  NTuple::Array<int>  m_SimHit_doubletR    ;    // Return doubletR
  NTuple::Array<int>  m_SimHit_doubletZ    ;    // Return doubletZ
  NTuple::Array<int>  m_SimHit_doubletPhi  ;    // Return doubletPhi
  NTuple::Array<int>  m_SimHit_gasGap      ;    // Return gasGap
  NTuple::Array<int>  m_SimHit_measuresPhi ;    // Return measuresPhi
  NTuple::Array<float> m_SimHit_time        ;    // Return the time
  NTuple::Array<float> m_SimHit_localx      ;    // Return the strip pos x  SD local frame
  NTuple::Array<float> m_SimHit_localy      ;    // Return the strip pos y 
  NTuple::Array<float> m_SimHit_localz      ;    // Return the strip pos z 
  NTuple::Array<float> m_SimHit_globalx     ;    // Return the strip pos x  global frame 
  NTuple::Array<float> m_SimHit_globaly     ;    // Return the strip pos y 
  NTuple::Array<float> m_SimHit_globalz     ;    // Return the strip pos z 
  NTuple::Array<float> m_SimHit_trackNumber ;    // Return the trackNumber of associated track
  NTuple::Array<float> m_SimHit_stepLen     ;    // length of the G4 step 
  NTuple::Array<float> m_SimHit_energyDep   ;    // G4 energy loss along step
  NTuple::Array<float> m_SimHit_kinEnergy   ;    // initial kin energy of particle producing the hit 
  NTuple::Array<int>   m_SimHit_pdgId       ;    // pdgId of particle producing the hit 

   
  NTuple::Item<int>  	m_nPads;	     // number of pads in the container   
  NTuple::Array<int>   m_rpc_pad_id;	  // pad id 
  NTuple::Array<int>   m_rpc_pad_bcid;	  // pad bcid 
  NTuple::Array<int>   m_rpc_pad_lvl1id;   // pad lvl1 id 
  NTuple::Array<int>   m_rpc_pad_sectorid; // pad sector id 
  NTuple::Array<int>   m_rpc_pad_error;    // pad error code 
  NTuple::Array<int>   m_rpc_pad_status;   // pad status 
  
  NTuple::Item<int>    m_nCMA;	            // number of coincidence matrices   
  NTuple::Item<int>  	m_nFiredChannels;   // number of fired channel   
  NTuple::Array<int>   m_hashId; 	  // hash id for pad == offline id 
  NTuple::Array<int>   m_sector; 	  // pad sector Id 
  NTuple::Array<int>   m_padId;  	  // pad Id 
  NTuple::Array<int>   m_status; 	  // status 
  NTuple::Array<int>   m_errorCode;	  // error code 
  NTuple::Array<int>   m_cmaId;  	  // CMA Id 
  NTuple::Array<int>   m_fel1Id; 	  // fel1Id 
  NTuple::Array<int>   m_febcId; 	  // febcId 
  NTuple::Array<int>   m_crc;		  // crc 
  NTuple::Array<int>   m_bcId;		  // bc Id 
  NTuple::Array<int>   m_time;		  // time 
  NTuple::Array<int>   m_ijk;		  // ijk code 
  NTuple::Array<int>   m_channel;	  // fired channel 
  NTuple::Array<int>   m_overlap;	  // overlap 
  NTuple::Array<int>   m_threshold;	  // threshold 

  NTuple::Item<int>    m_digit_nRpcColl;        // number of collection in the container
  NTuple::Item<int>    m_digit_nRpc    ;        // number of digit in the collection 
  
  NTuple::Array<float> m_digit_time        ;    // Return the time
  NTuple::Array<int>  m_digit_station     ;    // Return the station
  NTuple::Array<int>  m_digit_eta         ;    // Return the station eta
  NTuple::Array<int>  m_digit_phi         ;    // Return the station phi
  NTuple::Array<int>  m_digit_doubletR    ;    // Return doubletR
  NTuple::Array<int>  m_digit_doubletZ    ;    // Return doubletZ
  NTuple::Array<int>  m_digit_doubletPhi  ;    // Return doubletPhi
  NTuple::Array<int>  m_digit_gasGap      ;    // Return gasGap
  NTuple::Array<int>  m_digit_measuresPhi ;    // Return measuresPhi
  NTuple::Array<int>  m_digit_strip       ;    // Return the strip # 
  NTuple::Array<float> m_digit_stripx      ;    // Return the strip pos x  
  NTuple::Array<float> m_digit_stripy      ;    // Return the strip pos y 
  NTuple::Array<float> m_digit_stripz      ;    // Return the strip pos z
  
  NTuple::Item<int>    m_nRpcColl;       // number of collection in the container
  NTuple::Item<int>    m_nRpcPrd;        // number of digit in the collection 
  
  NTuple::Array<float> m_prd_time        ;    // Return the time
  NTuple::Array<int>  m_prd_station     ;    // Return the station
  NTuple::Array<int>  m_prd_eta         ;    // Return the station eta
  NTuple::Array<int>  m_prd_phi         ;    // Return the station phi
  NTuple::Array<int>  m_prd_doubletR    ;    // Return doubletR
  NTuple::Array<int>  m_prd_doubletZ    ;    // Return doubletZ
  NTuple::Array<int>  m_prd_doubletPhi  ;    // Return doubletPhi
  NTuple::Array<int>  m_prd_gasGap      ;    // Return gasGap
  NTuple::Array<int>  m_prd_measuresPhi ;    // Return measuresPhi
  NTuple::Array<int>  m_prd_strip       ;    // Return the strip # 
  NTuple::Array<float> m_prd_stripx      ;    // Return the strip pos x  
  NTuple::Array<float> m_prd_stripy      ;    // Return the strip pos y 
  NTuple::Array<float> m_prd_stripz      ;    // Return the strip pos z
  NTuple::Array<float> m_prd_triggerInfo ;    // Return trigger info
  NTuple::Array<float> m_prd_ambigFlag   ;    // Return ambiguity flag
  
};


RPC_SimHitToPrdCBNTAlgo::Clockwork::Clockwork() : nt(NULL)
{
    std::cout<<"HERE WE ARE - Clockwork constructor"<<std::endl;
}


RPC_SimHitToPrdCBNTAlgo::Clockwork::~Clockwork() {
}



// Algorithm constructor
RPC_SimHitToPrdCBNTAlgo::RPC_SimHitToPrdCBNTAlgo(const std::string &name, ISvcLocator *pSvcLocator): 
  AthAlgorithm(name, pSvcLocator) 
  ,m_muonMgr(0)
  ,m_rpcIdHelper(0)
  ,muonHelper(0)
{

    c=new Clockwork();

  declareProperty("doMCtruth",   m_doMCtruth  = true);
  declareProperty("doRPCSimHit", m_doRPCSimHit= true);
  declareProperty("doRPCDigit",  m_doRPCDigit = true);
  declareProperty("doRPCRDO",    m_doRPCRDO   = true);
  declareProperty("doRPCPrep",   m_doRPCPrep  = true);
 
   
  declareProperty("HitCollectionName" 	,  m_HitCollectionName   = "InputRpcHits"    );
  declareProperty("DigitCollectionName" ,  m_DigitCollectionName = "RPC_DIGITS"	     );
  declareProperty("RDOCollectionName"   ,  m_RDOCollectionName   = "RPCPAD"	     );
  declareProperty("PrdCollectionName"   ,  m_PrdCollectionName   = "RPC_Measurements");

}

RPC_SimHitToPrdCBNTAlgo::~RPC_SimHitToPrdCBNTAlgo() 
{ }

// Initialize method

StatusCode RPC_SimHitToPrdCBNTAlgo::initialize()
{
   
  ATH_MSG_DEBUG ( "in initialize()" );

  StatusCode sc;
  
  sc = detStore()->retrieve(  m_muonMgr,"Muon" );
  if (sc.isFailure()) {
      ATH_MSG_FATAL ( "Could not retrieve MuonDetectorManager!" );
      return sc;
  }
  // offline id helpers can be obtained from the MuonDetectorManager
  m_rpcIdHelper =  m_muonMgr->rpcIdHelper();
   
  // book ntuple 
  NTupleFilePtr     file(ntupleSvc(),"/NTUPLES/FILE");

  if (!file) return StatusCode::FAILURE;

  NTuple::Directory *col=ntupleSvc()->createDirectory("/NTUPLES/FILE/RPCvalidNT");

  NTuplePtr nt(ntupleSvc(),"/NTUPLES/FILE/RPCvalidNT/RPC");
  if (!nt) nt=ntupleSvc()->book(col, 1, CLID_ColumnWiseTuple, "RPC");
  else 
    {
       ATH_MSG_ERROR ( "Cannot book the ntuple" );
      return StatusCode::FAILURE;
    }

  // event info 
  sc = nt->addItem("Run",          c->run      );
  sc = nt->addItem("Event",        c->event    );

  if (m_doMCtruth) 
    {
      sc = nt->addItem(       "NpartStored",             c->m_npartStored, 0, MAX_PART);
      sc = nt->addIndexedItem(  "partVertX",             c->m_npartStored,    c->m_partVertX ); 
      sc = nt->addIndexedItem(  "partVertY",             c->m_npartStored,    c->m_partVertY  ); 
      sc = nt->addIndexedItem(  "partVertZ",             c->m_npartStored,    c->m_partVertZ  ); 
      sc = nt->addIndexedItem(  "partVertT",             c->m_npartStored,    c->m_partVertT  ); 
      sc = nt->addIndexedItem(  "partPdgId",             c->m_npartStored,    c->m_partPdgId  ); 
      sc = nt->addIndexedItem(  "partMomX" ,             c->m_npartStored,    c->m_partMomX   ); 
      sc = nt->addIndexedItem(  "partMomY" ,             c->m_npartStored,    c->m_partMomY   ); 
      sc = nt->addIndexedItem(  "partMomZ" ,             c->m_npartStored,    c->m_partMomZ   ); 
      sc = nt->addIndexedItem(  "partStatus" ,           c->m_npartStored,    c->m_partStatus ); 
      sc = nt->addIndexedItem(  "partBarcode" ,          c->m_npartStored,    c->m_partBarcode); 
      sc = nt->addIndexedItem(  "partMass" ,             c->m_npartStored,    c->m_partMass   );       
    }

  if (m_doRPCSimHit) {
  
    sc = nt->addItem(       "rpc_Nsimhit",            c->m_SimHit_nRpc,  0, MAX_SIMHRPC);
    sc = nt->addIndexedItem("rpc_simhit_station",     c->m_SimHit_nRpc,     c->m_SimHit_station);
    sc = nt->addIndexedItem("rpc_simhit_eta",         c->m_SimHit_nRpc,     c->m_SimHit_eta);
    sc = nt->addIndexedItem("rpc_simhit_phi",         c->m_SimHit_nRpc,     c->m_SimHit_phi);
    sc = nt->addIndexedItem("rpc_simhit_doublr",      c->m_SimHit_nRpc,     c->m_SimHit_doubletR);
    sc = nt->addIndexedItem("rpc_simhit_doublz",      c->m_SimHit_nRpc,     c->m_SimHit_doubletZ);
    sc = nt->addIndexedItem("rpc_simhit_doublphi",    c->m_SimHit_nRpc,     c->m_SimHit_doubletPhi);
    sc = nt->addIndexedItem("rpc_simhit_gasgap",      c->m_SimHit_nRpc,     c->m_SimHit_gasGap);
    sc = nt->addIndexedItem("rpc_simhit_measphi",     c->m_SimHit_nRpc,     c->m_SimHit_measuresPhi);
    sc = nt->addIndexedItem("rpc_simhit_time",        c->m_SimHit_nRpc,     c->m_SimHit_time);
    sc = nt->addIndexedItem("rpc_simhit_localx",      c->m_SimHit_nRpc,     c->m_SimHit_localx);
    sc = nt->addIndexedItem("rpc_simhit_localy",      c->m_SimHit_nRpc,     c->m_SimHit_localy);
    sc = nt->addIndexedItem("rpc_simhit_localz",      c->m_SimHit_nRpc,     c->m_SimHit_localz);
    sc = nt->addIndexedItem("rpc_simhit_globalx",     c->m_SimHit_nRpc,     c->m_SimHit_globalx);
    sc = nt->addIndexedItem("rpc_simhit_globaly",     c->m_SimHit_nRpc,     c->m_SimHit_globaly);
    sc = nt->addIndexedItem("rpc_simhit_globalz",     c->m_SimHit_nRpc,     c->m_SimHit_globalz);
    sc = nt->addIndexedItem("rpc_simhit_trackNumber", c->m_SimHit_nRpc,     c->m_SimHit_trackNumber);
    sc = nt->addIndexedItem("rpc_simhit_stepLen"    , c->m_SimHit_nRpc,     c->m_SimHit_stepLen);
    sc = nt->addIndexedItem("rpc_simhit_energyDep"  , c->m_SimHit_nRpc,     c->m_SimHit_energyDep);
    sc = nt->addIndexedItem("rpc_simhit_kinEnergy"  , c->m_SimHit_nRpc,     c->m_SimHit_kinEnergy);
    sc = nt->addIndexedItem("rpc_simhit_pdgId"      , c->m_SimHit_nRpc,     c->m_SimHit_pdgId);
    
  }
  // Digits block
  if (m_doRPCDigit) {
    sc = nt->addItem(       "rpc_NdigitColl",        c->m_digit_nRpcColl);
    sc = nt->addItem(       "rpc_Ndigit",            c->m_digit_nRpc, 0, MAX_DIGITRPC);
    sc = nt->addIndexedItem("rpc_digit_station",     c->m_digit_nRpc,    c->m_digit_station);
    sc = nt->addIndexedItem("rpc_digit_eta",         c->m_digit_nRpc,    c->m_digit_eta);
    sc = nt->addIndexedItem("rpc_digit_phi",         c->m_digit_nRpc,    c->m_digit_phi);
    sc = nt->addIndexedItem("rpc_digit_doublr",      c->m_digit_nRpc,    c->m_digit_doubletR);
    sc = nt->addIndexedItem("rpc_digit_doublz",      c->m_digit_nRpc,    c->m_digit_doubletZ);
    sc = nt->addIndexedItem("rpc_digit_doublphi",    c->m_digit_nRpc,    c->m_digit_doubletPhi);
    sc = nt->addIndexedItem("rpc_digit_gasgap",      c->m_digit_nRpc,    c->m_digit_gasGap);
    sc = nt->addIndexedItem("rpc_digit_measphi",     c->m_digit_nRpc,    c->m_digit_measuresPhi);
    sc = nt->addIndexedItem("rpc_digit_strip",       c->m_digit_nRpc,    c->m_digit_strip);
    sc = nt->addIndexedItem("rpc_digit_time",        c->m_digit_nRpc,    c->m_digit_time);
    sc = nt->addIndexedItem("rpc_digit_stripx",      c->m_digit_nRpc,    c->m_digit_stripx);
    sc = nt->addIndexedItem("rpc_digit_stripy",      c->m_digit_nRpc,    c->m_digit_stripy);
    sc = nt->addIndexedItem("rpc_digit_stripz",      c->m_digit_nRpc,    c->m_digit_stripz);

  }

  //RDO block
  if (m_doRPCRDO){  
    sc = nt->addItem(       "rpc_Npad",          c->m_nPads, 0, MAX_PADRPC);
   sc = nt->addIndexedItem("rpc_pad_id",         c->m_nPads,    c->m_rpc_pad_id);
   sc = nt->addIndexedItem("rpc_pad_bcid",	 c->m_nPads,    c->m_rpc_pad_bcid);
   sc = nt->addIndexedItem("rpc_pad_lvl1id",     c->m_nPads,    c->m_rpc_pad_lvl1id);
   sc = nt->addIndexedItem("rpc_pad_sectorid",   c->m_nPads,    c->m_rpc_pad_sectorid);
   sc = nt->addIndexedItem("rpc_pad_error",	 c->m_nPads,    c->m_rpc_pad_error);
   sc = nt->addIndexedItem("rpc_pad_status",     c->m_nPads,    c->m_rpc_pad_status);
   
   sc = nt->addItem(       "rpc_Ncm"        ,     c->m_nCMA);
   sc = nt->addItem(       "rpc_Nrdo"       ,     c->m_nFiredChannels, 0, MAX_RDOCHRPC);
   sc = nt->addIndexedItem("rpc_rdo_sector" ,     c->m_nFiredChannels,    c->m_sector);
   sc = nt->addIndexedItem("rpc_rdo_padId"  ,     c->m_nFiredChannels,    c->m_padId);
   sc = nt->addIndexedItem("rpc_rdo_status" ,     c->m_nFiredChannels,    c->m_status);
   sc = nt->addIndexedItem("rpc_rdo_errcode",     c->m_nFiredChannels,    c->m_errorCode);
   sc = nt->addIndexedItem("rpc_rdo_cmaId"  ,     c->m_nFiredChannels,    c->m_cmaId);
   sc = nt->addIndexedItem("rpc_rdo_fel1Id" ,     c->m_nFiredChannels,    c->m_fel1Id);
   sc = nt->addIndexedItem("rpc_rdo_febcId" ,     c->m_nFiredChannels,    c->m_febcId);
   sc = nt->addIndexedItem("rpc_rdo_crc"  ,       c->m_nFiredChannels,    c->m_crc); 
   sc = nt->addIndexedItem("rpc_rdo_bcId" ,       c->m_nFiredChannels,    c->m_bcId); 
   sc = nt->addIndexedItem("rpc_rdo_ticks",       c->m_nFiredChannels,    c->m_time); 
   sc = nt->addIndexedItem("rpc_rdo_ijk",         c->m_nFiredChannels,    c->m_ijk); 
   sc = nt->addIndexedItem("rpc_rdo_cmachan",     c->m_nFiredChannels,    c->m_channel); 
   sc = nt->addIndexedItem("rpc_rdo_overlap",     c->m_nFiredChannels,    c->m_overlap); 
   sc = nt->addIndexedItem("rpc_rdo_threshold",   c->m_nFiredChannels,    c->m_threshold); 
  }  


  // Prep block
  if (m_doRPCPrep) {
    sc = nt->addItem(       "rpc_NprdColl",        c->m_nRpcColl);
    sc = nt->addItem(       "rpc_Nprd",            c->m_nRpcPrd, 0, MAX_PRDRPC);
    sc = nt->addIndexedItem("rpc_prd_station",     c->m_nRpcPrd,    c->m_prd_station);
    sc = nt->addIndexedItem("rpc_prd_eta",         c->m_nRpcPrd,    c->m_prd_eta);
    sc = nt->addIndexedItem("rpc_prd_phi",         c->m_nRpcPrd,    c->m_prd_phi);
    sc = nt->addIndexedItem("rpc_prd_doublr",      c->m_nRpcPrd,    c->m_prd_doubletR);
    sc = nt->addIndexedItem("rpc_prd_doublz",      c->m_nRpcPrd,    c->m_prd_doubletZ);
    sc = nt->addIndexedItem("rpc_prd_doublphi",    c->m_nRpcPrd,    c->m_prd_doubletPhi);
    sc = nt->addIndexedItem("rpc_prd_gasgap",      c->m_nRpcPrd,    c->m_prd_gasGap);
    sc = nt->addIndexedItem("rpc_prd_measphi",     c->m_nRpcPrd,    c->m_prd_measuresPhi);
    sc = nt->addIndexedItem("rpc_prd_strip",       c->m_nRpcPrd,    c->m_prd_strip);
    sc = nt->addIndexedItem("rpc_prd_time",        c->m_nRpcPrd,    c->m_prd_time);
    sc = nt->addIndexedItem("rpc_prd_stripx",      c->m_nRpcPrd,    c->m_prd_stripx);
    sc = nt->addIndexedItem("rpc_prd_stripy",      c->m_nRpcPrd,    c->m_prd_stripy);
    sc = nt->addIndexedItem("rpc_prd_stripz",      c->m_nRpcPrd,    c->m_prd_stripz);
    sc = nt->addIndexedItem("rpc_prd_triggerInfo", c->m_nRpcPrd,    c->m_prd_triggerInfo);
    sc = nt->addIndexedItem("rpc_prd_ambigFlag",   c->m_nRpcPrd,    c->m_prd_ambigFlag);

  }
  c->nt = nt;

  return sc;  
}


StatusCode RPC_SimHitToPrdCBNTAlgo::clearNTuple()  
{
  
  StatusCode sc = StatusCode::SUCCESS;
  
  if (m_doMCtruth){
    c->m_npartStored=0;
    c->run=0;
    c->event=0;
  }


  if (m_doRPCSimHit){
    c->m_SimHit_nRpc  = 0;
  }
  
  if (m_doRPCDigit){
    c->m_digit_nRpcColl = 0;
    c->m_digit_nRpc  = 0;
  }
  
  if (m_doRPCRDO){  
    c->m_nPads=0;
  
    c->m_nCMA=0;
    c->m_nFiredChannels=0;
  }  
  
  if (m_doRPCPrep){
    c->m_nRpcColl = 0;
    c->m_nRpcPrd  = 0;
  }
 
  return sc;
}

// Execute method
StatusCode RPC_SimHitToPrdCBNTAlgo::execute()
{
   
  ATH_MSG_DEBUG ( "in execute()" );

  StatusCode sc= clearNTuple(); //this->CBNT_pre_execute();
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "clearNtuple failed!" );
    return  StatusCode::RECOVERABLE;
  }
  
  const EventInfo* pevt;
  StatusCode sc_read = evtStore()->retrieve(pevt);
  if (sc_read.isSuccess())  {
    c->event = pevt->event_ID()->event_number();
    c->run   = pevt->event_ID()->run_number();
  }

  if(m_doMCtruth)
  {
      sc = doMCtruth();
      if (sc.isFailure()) 
      {
	ATH_MSG_WARNING ( "MC truth fill failed" );
	return  StatusCode::RECOVERABLE;
      }
  }
  

  if(m_doRPCSimHit) {
    sc = doRPCSimHit();
    if (sc.isFailure()) 
      {
	ATH_MSG_WARNING ( "RPCSimHit dumping failed" );
	return  StatusCode::RECOVERABLE;
      }
  }
  if(m_doRPCDigit ) 
    {
      sc = doRPCDigit ();
      if (sc.isFailure()) 
	{
	  ATH_MSG_WARNING ( "RPCdigit dumping failed" );
	  return  StatusCode::RECOVERABLE;
	}

    }
  if(m_doRPCRDO	  ) 
    {
      sc = doRPCRDO   ();
      if (sc.isFailure()) 
	{
	  ATH_MSG_WARNING ( "RPCrdo dumping failed" );
	  return  StatusCode::RECOVERABLE;
	}
    }
  if(m_doRPCPrep  ) 
    {
      sc = doRPCPrep  ();
      if (sc.isFailure()) 
	{
	  ATH_MSG_WARNING ( "RPCprd dumping failed" );
	  return  StatusCode::RECOVERABLE;
	}
    }

  ntupleSvc()->writeRecord(c->nt);
  return StatusCode::SUCCESS;  
}


// Execute method
StatusCode RPC_SimHitToPrdCBNTAlgo::doMCtruth()
{
    
    ATH_MSG_DEBUG ("in doMCtruth" ); 
  ATH_MSG_DEBUG ( "RpcSimHitToPrdCBNT::doMCtruth begin." );
  
  // Retrieve test container of all hits in input to the digitization 
  const DataHandle<McEventCollection> mcEvent;
  StatusCode sc_read = evtStore()->retrieve(mcEvent, "TruthEvent"); 
  if (sc_read.isFailure()) { 
     ATH_MSG_WARNING ( " Cannot retrieve McEventCollection" ); 
     return  StatusCode::SUCCESS; 
  } 
  //ATH_MSG_DEBUG ("McEventCollection->size() : " << mcEvent->size() ) ;  

  DataVector<HepMC::GenEvent>::const_iterator e;
  //if (mcEvent->size()!=1) return StatusCode::SUCCESS;

  long eventCounter=0;  
  Amg::Vector3D direction (0.,0.,0.);
  int ipart = 0;
  for (e=mcEvent->begin();e!=mcEvent->end(); e++) {
    ++eventCounter;      


    ATH_MSG_DEBUG ("McEvent n.  " <<eventCounter ) ;        
    int particleCounter=0;    
    for (HepMC::GenEvent::particle_const_iterator p= (**e).particles_begin();	 p!= (**e).particles_end(); ++p) {
	++particleCounter;	
	ATH_MSG_DEBUG ("McEvent n.  " <<eventCounter << " particle # "<<particleCounter) ;        
	float xv = -999999.;
	float yv = -999999.;
	float zv = -999999.;
	float tv = -999999.;
	//std::cout<<"do we come here ??"<<std::endl;
// the following lines give troubles  - protect them 
	if ((**p).production_vertex())
	  {
	    xv = (**p).production_vertex()->point3d().x();
	    yv = (**p).production_vertex()->point3d().y();
	    zv = (**p).production_vertex()->point3d().z();
	    tv = (**p).production_vertex()->position().t();
	    //	    std::cout<<"do we get the vertex time = ??"<<std::endl;
	  }
 	float xd = (**p).momentum().px();
 	float yd = (**p).momentum().py();
 	float zd = (**p).momentum().pz();
	//std::cout<<"do we come here 1??"<<std::endl;
 	float mag = sqrt(xd*xd + yd*yd + zd*zd);
	if (fabs(mag)>0.001) 
	{
	    //direction.setX(xd/mag);
	    //direction.setY(yd/mag);
	    //direction.setZ(zd/mag);
	    
	    direction[0]=(xd/mag);
	    direction[1]=(yd/mag);
	    direction[2]=(zd/mag);
	     
	}
	int status  =  (**p).status();
	int barcode =  (**p).barcode();
	double generatedMass = (**p).generated_mass();

	 float pdgId =  (**p).pdg_id();
	 
	 c->m_partVertX  [ipart] =xv            ;
	 c->m_partVertY  [ipart] =yv            ;
	 c->m_partVertZ  [ipart] =zv            ;
	 c->m_partVertT  [ipart] =tv            ;
	 c->m_partPdgId  [ipart] =pdgId         ;
	 c->m_partMomX   [ipart] =xd            ;
	 c->m_partMomY   [ipart] =yd            ;
	 c->m_partMomZ   [ipart] =zd            ;   
	 c->m_partStatus [ipart] =status        ;
	 c->m_partBarcode[ipart] =barcode       ;
	 c->m_partMass   [ipart] =generatedMass ;
	 ++ipart;
        ATH_MSG_DEBUG("Event # "<<eventCounter 
		      <<" vertex at "<<xv<<" "<<yv<<" "<<zv<<" produced at time = "<<tv
		      //<<" direction theta/eta/phi = "<<direction.theta()<<" "<<direction.pseudoRapidity()<<" "<<direction.phi()
		      <<" direction theta/eta/phi = "<<direction.theta()<<" "<<direction.phi()
		      <<" p = "<<sqrt(xd*xd+yd*yd+zd*zd)<<" pdgId = "<< pdgId <<" Status "<<status);
    }
  }

  //  // temporary   {{{{
  //  c->run   = 99999999;
  //  c->event = eventCounter;
  //  // end of temp }}}}

  c->m_npartStored=ipart;
  ATH_MSG_DEBUG ("out of doMCtruth" ); 
  return StatusCode::SUCCESS;  
}

// Execute method
StatusCode RPC_SimHitToPrdCBNTAlgo::doRPCSimHit()
{   

    ATH_MSG_DEBUG ("in doRPCSimHit" ); 
  ATH_MSG_DEBUG ( "RpcSimHitToPrdCBNT::doRPCSimHit begin." );
  
  // Retrieve test container of all hits in input to the digitization 
  const RPCSimHitCollection* m_inputSimHitColl=NULL;  
  StatusCode sc_read = evtStore()->retrieve(m_inputSimHitColl, m_HitCollectionName); 
  if (sc_read.isFailure()) { 
     ATH_MSG_WARNING ( " Cannot retrieve RPCSimHitCollection" ); 
     return  StatusCode::SUCCESS; 
  } 
  ATH_MSG_DEBUG ("RPCSimHitCollection->size() : " << m_inputSimHitColl->size() ) ;  
  
  int myCounter = 0;
  for (RPCSimHitCollection::const_iterator rpcsimhit = m_inputSimHitColl->begin() ;  rpcsimhit != m_inputSimHitColl->end() ; ++rpcsimhit)  
       {    
           
	const int idHit = rpcsimhit->RPCid(); 
 	
        int StationName = -1 ;

        if (muonHelper==0) muonHelper = RpcHitIdHelper::GetHelper();
        std::string stationName = muonHelper->GetStationName(idHit);
        int         stationEta  = muonHelper->GetZSector    (idHit);
        int         stationPhi  = muonHelper->GetPhiSector  (idHit);
        int         doubletR    = muonHelper->GetDoubletR   (idHit);
        int         doubletZ    = muonHelper->GetDoubletZ   (idHit);
        int         doubletPhi  = muonHelper->GetDoubletPhi (idHit);
        int         gasGap      = muonHelper->GetGasGapLayer(idHit);
        int         measphi     = muonHelper->GetMeasuresPhi(idHit);
 	 

	if(stationName=="BML"){ 
	 StationName=2;
	}
	else if(stationName=="BMS"){ 
	 StationName=3;
	}
	else if(stationName=="BOL"){ 
	 StationName=4;
	}
	else if(stationName=="BOS"){ 
	 StationName=5;
	}
	else if(stationName=="BMF"){ 
	 StationName=8;
	}
	else if(stationName=="BOF"){ 
	 StationName=9;
	}
	else if(stationName=="BOG"){ 
	 StationName=10;
	}
	else if(stationName=="BME"){ 
	 StationName=53;
	
}	else if(stationName=="BOE"){ 
	 StationName=54;
	}

      //construct Atlas identifier from components
      ATH_MSG_DEBUG ( "creating id for hit in element:"
		      << " stationName "  <<    stationName.c_str()
		      << " stationEta "   <<    stationEta
		      << " stationPhi "   <<    stationPhi
		      << " doubletR "     <<    doubletR
		      << " doubletZ "     <<    doubletZ
		      << " doubletPhi "   <<    doubletPhi
		      << " gasGap "	 <<     gasGap
		      << " measphi "	 <<     measphi );//

	// use GeoAdaptors to retrieve global positions of a simhit
	GeoRPCHit ghit(*rpcsimhit);
	Amg::Vector3D gPos = ghit.getGlobalPosition();


        c->m_SimHit_time       [myCounter] =  rpcsimhit->globalTime()  ;    
 	c->m_SimHit_localx     [myCounter] =  rpcsimhit->localPosition().x()  ; 
 	c->m_SimHit_localy     [myCounter] =  rpcsimhit->localPosition().y()  ; 
 	c->m_SimHit_localz     [myCounter] =  rpcsimhit->localPosition().z()  ; 
 	c->m_SimHit_globalx    [myCounter] =  gPos.x()  ; 
 	c->m_SimHit_globaly    [myCounter] =  gPos.y()  ; 
 	c->m_SimHit_globalz    [myCounter] =  gPos.z()  ; 

	
 	c->m_SimHit_station    [myCounter] =   StationName	;
 	c->m_SimHit_eta	       [myCounter] =   stationEta	;
 	c->m_SimHit_phi	       [myCounter] =   stationPhi	;
 	c->m_SimHit_doubletR   [myCounter] =   doubletR	;
 	c->m_SimHit_doubletZ   [myCounter] =   doubletZ	;
 	c->m_SimHit_doubletPhi [myCounter] =   doubletPhi	;
 	c->m_SimHit_gasGap     [myCounter] =   gasGap	;
 	c->m_SimHit_measuresPhi[myCounter] =   measphi	;
      
	c->m_SimHit_trackNumber[myCounter] =   rpcsimhit->trackNumber()      ;
	c->m_SimHit_stepLen    [myCounter] =   rpcsimhit->stepLength()       ;
	c->m_SimHit_energyDep  [myCounter] =   rpcsimhit->energyDeposit()    ;
	c->m_SimHit_kinEnergy  [myCounter] =   rpcsimhit->kineticEnergy()    ;
	c->m_SimHit_pdgId      [myCounter] =   rpcsimhit->particleEncoding() ;

        ++myCounter;
       
       }
 
       c->m_SimHit_nRpc = myCounter;
  ATH_MSG_DEBUG ("out of doRPCSimHit" ); 
  return StatusCode::SUCCESS;  
}

// Execute method
StatusCode RPC_SimHitToPrdCBNTAlgo::doRPCDigit()
{
    ATH_MSG_DEBUG ("in doRPCDigit" ); 
  // Retrieve Digit container in StoreGate
    const RpcDigitContainer*                m_digitContainer=NULL ;
  StatusCode sc_read = evtStore()->retrieve(m_digitContainer, m_DigitCollectionName);
  if (sc_read.isFailure())  {
    ATH_MSG_WARNING ( " Cannot retrieve RPCDigitCollection" ); 
    return StatusCode::RECOVERABLE;
  }
  ATH_MSG_DEBUG ( "RpcDigitContainer size: "  << m_digitContainer->size() ); 
   
  //std::cout << "RpcDigitContainer size: "  << m_digitContainer->size() << std::endl ; 
  
  
  
  int myCounter = 0;
  int myCounterColl = 0;
  for (RpcDigitContainer::const_iterator containerit = m_digitContainer->begin() ; containerit != m_digitContainer->end() ; ++containerit)  
     {       
       for (RpcDigitCollection::const_iterator rpcdigit = (*containerit)->begin() ;	    rpcdigit != (*containerit)->end() ; ++rpcdigit)  
       {     
    
 	 Identifier digit_id = (*rpcdigit)->identify(); 
	 
// 	
 	 c->m_digit_time[myCounter] = (*rpcdigit)->time(); 

         
 	 c->m_digit_station    [myCounter] = m_rpcIdHelper->stationName(digit_id); 
 	 c->m_digit_eta        [myCounter] = m_rpcIdHelper->stationEta(digit_id); 
 	 c->m_digit_phi        [myCounter] = m_rpcIdHelper->stationPhi(digit_id); 
 	 c->m_digit_doubletR   [myCounter] = m_rpcIdHelper->doubletR(digit_id); 
 	 c->m_digit_doubletZ   [myCounter] = m_rpcIdHelper->doubletZ(digit_id); 
 	 c->m_digit_doubletPhi [myCounter] = m_rpcIdHelper->doubletPhi(digit_id); 
 	 c->m_digit_gasGap     [myCounter] = m_rpcIdHelper->gasGap(digit_id);  
 	 c->m_digit_measuresPhi[myCounter] = m_rpcIdHelper->measuresPhi(digit_id); 
 	 c->m_digit_strip      [myCounter] = m_rpcIdHelper->strip(digit_id); 
      
 	 // get the digit position 
 	 const MuonGM::RpcReadoutElement* descriptor =  m_muonMgr->getRpcReadoutElement(digit_id); 
 	     
 	 const Amg::Vector3D stripPos = descriptor->stripPos(digit_id); 
 	     
 	 c->m_digit_stripx  [myCounter] = stripPos.x(); 
 	 c->m_digit_stripy  [myCounter] = stripPos.y(); 
 	 c->m_digit_stripz  [myCounter] = stripPos.z(); 
	       
	 ATH_MSG_DEBUG ("DIG  Station/eta/phi/dbR/dbZ/dbP/measPhi/strip: "<< c->m_digit_station[myCounter] <<" "
			<<	 c->m_digit_eta[myCounter] 	<<" "
			<<	 c->m_digit_phi[myCounter] 	<<" "
			<<	 c->m_digit_doubletR[myCounter]	<<" "
			<<	 c->m_digit_doubletZ[myCounter]	<<" "
			<<	 c->m_digit_doubletPhi[myCounter]	<<" "
			<<	 c->m_digit_gasGap[myCounter] 	<<" "
			<<	 c->m_digit_measuresPhi[myCounter]<<" "
			<<	 c->m_digit_strip[myCounter] );
	 ++myCounter; 
	 //	 ATH_MSG_DEBUG ( " digit number  " << myCounter<< " id = "<<m_rpcIdHelper->show_to_string(digit_id) ); 
       } //end of rpcdigit container loop 
       ++myCounterColl;
     } //end of container iteration loop      

  c->m_digit_nRpcColl=myCounterColl;
  c->m_digit_nRpc    =myCounter    ;
  ATH_MSG_DEBUG ( "Total n. of DIG / DIG collections  " << myCounter<<" "<<    myCounterColl);
  ATH_MSG_DEBUG ("out of doRPCDigit" ); 

  return StatusCode::SUCCESS;  
}


// Execute method
StatusCode RPC_SimHitToPrdCBNTAlgo::doRPCRDO()
{ 
    ATH_MSG_DEBUG ("in doRPCRDO" ); 
  // Retrieve RDO container in StoreGate
    const RpcPadContainer*            m_rpcRDO       = NULL;
    
  StatusCode sc_read = evtStore()->retrieve( m_rpcRDO, m_RDOCollectionName );   
  if ( sc_read != SUCCESS ) { 
     ATH_MSG_WARNING ( " Cannot retrieve RDO Container" ); 
     return sc_read; 
  } 
  ATH_MSG_DEBUG ("m_rpcRDO size: "  << m_rpcRDO->size() ) ;
  //std::cout << "m_rpcRDO size: "  << m_rpcRDO->size() << std::endl ;  

// Access by Collection

   c->m_nPads = 0;
   c->m_nCMA = 0;
   c->m_nFiredChannels = 0;

   RpcPadContainer::const_iterator pad;

   int myCounter = 0;
   int myPadCounter = 0;
   int myCMACounter = 0;
   const RpcPad*                     rdoColl =NULL;
    for (RpcPadContainer::const_iterator rdoColli = m_rpcRDO->begin(); rdoColli!=m_rpcRDO->end(); ++rdoColli)
    {
      rdoColl = *rdoColli;
   
     // Now loop on the RDO
      if ( (rdoColl)->size() != 0 ) { 
       
       ATH_MSG_DEBUG ( " Number of CMA in this pad " << (rdoColl)->size() );
     
       // pad ntuple variables
       c->m_rpc_pad_id         [myPadCounter] = (int)(rdoColl)->onlineId();
       c->m_rpc_pad_sectorid   [myPadCounter] = (int)(rdoColl)->sector();
       c->m_rpc_pad_error      [myPadCounter] = (int)(rdoColl)->errorCode();
       c->m_rpc_pad_status     [myPadCounter] = (int)(rdoColl)->status();
       
       c->m_rpc_pad_lvl1id     [myPadCounter] = (int)(rdoColl)->lvl1Id();
       c->m_rpc_pad_bcid       [myPadCounter] = (int)(rdoColl)->bcId();
       
       // for each pad, loop over cma
       RpcPad::const_iterator it3 = (rdoColl)->begin();
       RpcPad::const_iterator it4 = (rdoColl)->end();
       for (; it3!=it4; ++it3) {
  	 ATH_MSG_DEBUG ( " Number of fired channels in this cma " << (*it3)->size() );
  	 const RpcCoinMatrix * cma = (*it3);
  	 
  	 //for each cma for over fired channels
  	 RpcCoinMatrix::const_iterator it5 = (*it3)->begin();
  	 RpcCoinMatrix::const_iterator it6 = (*it3)->end();
  	 for (; it5!=it6; ++it5) {
  	   
  	   if (myCounter < MAX_RDOCHRPC ) {
  	     const RpcFiredChannel * fChannel = (*it5);
  	     
  	     c->m_sector[myCounter] = (rdoColl)->sector();
  	     c->m_padId[myCounter] = (rdoColl)->onlineId();
  	     c->m_status[myCounter] = (rdoColl)->status();
  	     c->m_errorCode[myCounter] = (rdoColl)->errorCode();
  	     c->m_cmaId[myCounter] = cma->onlineId();
  	     c->m_fel1Id[myCounter] = cma->fel1Id();
  	     c->m_febcId[myCounter] = cma->febcId();
  	     c->m_crc[myCounter] = cma->crc();
  	     
  	     c->m_bcId[myCounter] = fChannel->bcid();
  	     c->m_time[myCounter] = fChannel->time();
  	     c->m_ijk[myCounter] = fChannel->ijk();
  	     c->m_channel[myCounter] = fChannel->channel();
  	     
	     ATH_MSG_DEBUG ( "RDO sector/pad/cma/ijk/channel "<< c->m_sector[myCounter] <<" "
			     <<c->m_padId[myCounter] <<" "<< c->m_cmaId[myCounter]<<" " 
			     << c->m_ijk[myCounter] <<" "<<c->m_channel[myCounter]);
	     if (fChannel->ijk()==7) {
  	       c->m_overlap[myCounter] = fChannel->ovl();
  	       c->m_threshold[myCounter] = fChannel->thr();
  	     }
  	     else {
  	       c->m_overlap[myCounter] = -1;
  	       c->m_threshold[myCounter] = -1;
  	     }
	     ++myCounter;
  	   }
  	   else 
	     {
	       ATH_MSG_WARNING ( "More than " << MAX_RDOCHRPC << " channels " );
	       break;
	     }
  	 } // end of loop over channels in a CM
	 ++myCMACounter;
       }// end of loop pver CMAs in a pad 
       ++myPadCounter;
      } // end of if non empty collection 
    }// end of loop over collections 

    c->m_nFiredChannels=myCounter;
    c->m_nCMA=myCMACounter;
    c->m_nPads = myPadCounter;

    ATH_MSG_DEBUG ("out of doRPCRDO" ); 

    return StatusCode::SUCCESS;  
}

// Execute method
StatusCode RPC_SimHitToPrdCBNTAlgo::doRPCPrep()
{
  
    ATH_MSG_DEBUG ("in doRPCPrep" ); 
  // Retrieve RDO container in StoreGate
  const  Muon::RpcPrepDataContainer* m_rpc_container;
  StatusCode sc_read = evtStore()->retrieve(m_rpc_container, m_PrdCollectionName); 
  if (sc_read.isFailure()) { 
     ATH_MSG_WARNING ( "Cannot retrieve RpcPrepDataContainer " ); 
     return  StatusCode::RECOVERABLE; 
  }  
  ATH_MSG_DEBUG ("m_rpc_container->size() : " << m_rpc_container->size() ) ; 
  //std::cout << "m_rpc_container->size() : " << m_rpc_container->size() << std::endl ;  


  int myCounter = 0;
  int myCollCounter = 0;
  for (Muon::RpcPrepDataContainer::const_iterator containerit = m_rpc_container->begin() ; containerit != m_rpc_container->end() ; ++containerit)  
     {    
       for (Muon::RpcPrepDataCollection::const_iterator rpcPrd = (*containerit)->begin() ;	    rpcPrd != (*containerit)->end() ; ++rpcPrd)  
       {     
    
	 Identifier prd_id = (*rpcPrd)->identify(); 
	
	 c->m_prd_time[myCounter] = (*rpcPrd)->time(); 
	 c->m_prd_station[myCounter] = m_rpcIdHelper->stationName(prd_id); 
	 c->m_prd_eta[myCounter] = m_rpcIdHelper->stationEta(prd_id); 
	 c->m_prd_phi[myCounter] = m_rpcIdHelper->stationPhi(prd_id); 
	 c->m_prd_doubletR[myCounter] = m_rpcIdHelper->doubletR(prd_id); 
	 c->m_prd_doubletZ[myCounter] = m_rpcIdHelper->doubletZ(prd_id); 
	 c->m_prd_doubletPhi[myCounter] = m_rpcIdHelper->doubletPhi(prd_id); 
	 c->m_prd_gasGap[myCounter] = m_rpcIdHelper->gasGap(prd_id);  
	 c->m_prd_measuresPhi[myCounter] = m_rpcIdHelper->measuresPhi(prd_id); 
	 c->m_prd_strip[myCounter] = m_rpcIdHelper->strip(prd_id); 
	 c->m_prd_triggerInfo[myCounter] = (*rpcPrd)->triggerInfo(); 
	 c->m_prd_ambigFlag[myCounter] = (*rpcPrd)->ambiguityFlag(); 
    
	 ATH_MSG_DEBUG ("PRD  Station/eta/phi/dbR/dbZ/dbP/measPhi/strip: "<< c->m_prd_station[myCounter] <<" "
			<<	 c->m_prd_eta[myCounter] 	<<" "
			<<	 c->m_prd_phi[myCounter] 	<<" "
			<<	 c->m_prd_doubletR[myCounter]	<<" "
			<<	 c->m_prd_doubletZ[myCounter]	<<" "
			<<	 c->m_prd_doubletPhi[myCounter]	<<" "
			<<	 c->m_prd_gasGap[myCounter] 	<<" "
			<<	 c->m_prd_measuresPhi[myCounter]<<" "
			<<	 c->m_prd_strip[myCounter] );
	 
	 // get the digit position 
	 const MuonGM::RpcReadoutElement* descriptor =  m_muonMgr->getRpcReadoutElement(prd_id); 
	      
	 const Amg::Vector3D stripPos = descriptor->stripPos(prd_id); 
	      
	 c->m_prd_stripx[myCounter] = stripPos.x(); 
	 c->m_prd_stripy[myCounter] = stripPos.y(); 
	 c->m_prd_stripz[myCounter] = stripPos.z(); 
	      
	 ++myCounter;
	 //ATH_MSG_DEBUG ( " PRD number  " << myCounter ); 
       } //end of rpcPrd container loop 
       ++myCollCounter;
     } //end of container iteration loop     
  c->m_nRpcPrd = myCounter;
  c->m_nRpcColl = myCollCounter;
  ATH_MSG_DEBUG ( "Total n. of PRD / PRD collections  " << myCounter<<" "<<    myCollCounter);
  ATH_MSG_DEBUG ("out of doRPCPrep" ); 
  return StatusCode::SUCCESS;  
}



// Finalize method
StatusCode RPC_SimHitToPrdCBNTAlgo::finalize()
{
   
  ATH_MSG_INFO ( "in finalize()" );

  return StatusCode::SUCCESS;
}



