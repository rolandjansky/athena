/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSIMHITTOPRD_H
#define MUONSIMHITTOPRD_H

#include <string.h>
#include "AthenaBaseComps/AthAlgorithm.h"

//#include "CBNT_Utils/CBNT_AthenaAwareBase.h"


#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "EventPrimitives/EventPrimitives.h"

class StatusCode;
class RpcHitIdHelper   ; 
class RpcIdHelper      ;

namespace MuonGM
{
    class MuonDetectorManager;
}


class RPC_SimHitToPrdCBNTAlgo : public AthAlgorithm 
{
 public:
  // Agorithm constructor
  RPC_SimHitToPrdCBNTAlgo (const std::string &name, ISvcLocator *pSvcLocator); 
  
  ~RPC_SimHitToPrdCBNTAlgo();

  // Gaudi hooks
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

  //  virtual StatusCode CBNT_initialize();
  StatusCode clearNTuple();
 
 protected:  
  std::string m_HitCollectionName   ; // name of the input hits
  std::string m_DigitCollectionName ; // name of the digits
  std::string m_RDOCollectionName   ; // name of the RDO
  std::string m_PrdCollectionName   ; // name of the Prd
 
 private: 
  // Private function to add the clusters to the ntuple
  StatusCode doMCtruth  ();
  StatusCode doRPCSimHit();
  StatusCode doRPCDigit ();
  StatusCode doRPCRDO   ();
  StatusCode doRPCPrep  ();

  const MuonGM::MuonDetectorManager*           m_muonMgr     ;
  const RpcIdHelper* 			       m_rpcIdHelper; //offline id helper
  RpcHitIdHelper*                              muonHelper; // simulation id helper
 
  bool m_doMCtruth;
  bool m_doRPCSimHit;
  bool m_doRPCDigit ;
  bool m_doRPCRDO   ;
  bool m_doRPCPrep  ; 
  
 
  
  // Ntuple Variables
  // Define variables in the Ntuple:

  class Clockwork;
  Clockwork *c;

  /*
  int m_npartStored;
  std::vector<float>* m_partVertX   ;
  std::vector<float>* m_partVertY   ;
  std::vector<float>* m_partVertZ   ;
  std::vector<float>* m_partVertT   ;
  std::vector<float>* m_partPdgId   ;
  std::vector<float>* m_partMomX    ;
  std::vector<float>* m_partMomY    ;
  std::vector<float>* m_partMomZ    ;
  std::vector<int>*   m_partStatus  ;
  std::vector<int>*   m_partBarcode ;
  std::vector<float>* m_partMass    ;


  int m_SimHit_nRpc;              // number of SimHits in the collection  
  
  std::vector<long>*  m_SimHit_station     ;    // Return the station
  std::vector<long>*  m_SimHit_eta         ;    // Return the station eta
  std::vector<long>*  m_SimHit_phi         ;    // Return the station phi
  std::vector<long>*  m_SimHit_doubletR    ;    // Return doubletR
  std::vector<long>*  m_SimHit_doubletZ    ;    // Return doubletZ
  std::vector<long>*  m_SimHit_doubletPhi  ;    // Return doubletPhi
  std::vector<long>*  m_SimHit_gasGap      ;    // Return gasGap
  std::vector<long>*  m_SimHit_measuresPhi ;    // Return measuresPhi
  std::vector<float>* m_SimHit_time        ;    // Return the time
  std::vector<float>* m_SimHit_localx      ;    // Return the strip pos x  SD local frame
  std::vector<float>* m_SimHit_localy      ;    // Return the strip pos y 
  std::vector<float>* m_SimHit_localz      ;    // Return the strip pos z 
  std::vector<float>* m_SimHit_globalx     ;    // Return the strip pos x  global frame 
  std::vector<float>* m_SimHit_globaly     ;    // Return the strip pos y 
  std::vector<float>* m_SimHit_globalz     ;    // Return the strip pos z 
  std::vector<float>* m_SimHit_trackNumber ;    // Return the trackNumber of associated track
  std::vector<float>* m_SimHit_stepLen     ;    // length of the G4 step 
  std::vector<float>* m_SimHit_energyDep   ;    // G4 energy loss along step
  std::vector<float>* m_SimHit_kinEnergy   ;    // initial kin energy of particle producing the hit 
  std::vector<int>*   m_SimHit_pdgId       ;    // pdgId of particle producing the hit 

   
  int	 m_nPads;	     // number of pads in the container   
  std::vector<int>*   m_rpc_pad_id;	  // pad id 
  std::vector<int>*   m_rpc_pad_bcid;	  // pad bcid 
  std::vector<int>*   m_rpc_pad_lvl1id;   // pad lvl1 id 
  std::vector<int>*   m_rpc_pad_sectorid; // pad sector id 
  std::vector<int>*   m_rpc_pad_error;    // pad error code 
  std::vector<int>*   m_rpc_pad_status;   // pad status 
  
  int	 m_nCMA;	     // number of coincidence matrices   
  int	 m_nFiredChannels;   // number of fired channel   
  std::vector<int>*   m_hashId; 	  // hash id for pad == offline id 
  std::vector<int>*   m_sector; 	  // pad sector Id 
  std::vector<int>*   m_padId;  	  // pad Id 
  std::vector<int>*   m_status; 	  // status 
  std::vector<int>*   m_errorCode;	  // error code 
  std::vector<int>*   m_cmaId;  	  // CMA Id 
  std::vector<int>*   m_fel1Id; 	  // fel1Id 
  std::vector<int>*   m_febcId; 	  // febcId 
  std::vector<int>*   m_crc;		  // crc 
  std::vector<int>*   m_bcId;		  // bc Id 
  std::vector<int>*   m_time;		  // time 
  std::vector<int>*   m_ijk;		  // ijk code 
  std::vector<int>*   m_channel;	  // fired channel 
  std::vector<int>*   m_overlap;	  // overlap 
  std::vector<int>*   m_threshold;	  // threshold 

  int m_digit_nRpcColl;       // number of collection in the container
  int m_digit_nRpc    ;        // number of digit in the collection 
  
  std::vector<float>* m_digit_time        ;    // Return the time
  std::vector<long>*  m_digit_station     ;    // Return the station
  std::vector<long>*  m_digit_eta         ;    // Return the station eta
  std::vector<long>*  m_digit_phi         ;    // Return the station phi
  std::vector<long>*  m_digit_doubletR    ;    // Return doubletR
  std::vector<long>*  m_digit_doubletZ    ;    // Return doubletZ
  std::vector<long>*  m_digit_doubletPhi  ;    // Return doubletPhi
  std::vector<long>*  m_digit_gasGap      ;    // Return gasGap
  std::vector<long>*  m_digit_measuresPhi ;    // Return measuresPhi
  std::vector<long>*  m_digit_strip       ;    // Return the strip # 
  std::vector<float>* m_digit_stripx      ;    // Return the strip pos x  
  std::vector<float>* m_digit_stripy      ;    // Return the strip pos y 
  std::vector<float>* m_digit_stripz      ;    // Return the strip pos z
  
  int m_nRpcColl;       // number of collection in the container
  int m_nRpcPrd;        // number of digit in the collection 
  
  std::vector<float>* m_prd_time        ;    // Return the time
  std::vector<long>*  m_prd_station     ;    // Return the station
  std::vector<long>*  m_prd_eta         ;    // Return the station eta
  std::vector<long>*  m_prd_phi         ;    // Return the station phi
  std::vector<long>*  m_prd_doubletR    ;    // Return doubletR
  std::vector<long>*  m_prd_doubletZ    ;    // Return doubletZ
  std::vector<long>*  m_prd_doubletPhi  ;    // Return doubletPhi
  std::vector<long>*  m_prd_gasGap      ;    // Return gasGap
  std::vector<long>*  m_prd_measuresPhi ;    // Return measuresPhi
  std::vector<long>*  m_prd_strip       ;    // Return the strip # 
  std::vector<float>* m_prd_stripx      ;    // Return the strip pos x  
  std::vector<float>* m_prd_stripy      ;    // Return the strip pos y 
  std::vector<float>* m_prd_stripz      ;    // Return the strip pos z
  std::vector<float>* m_prd_triggerInfo ;    // Return trigger info
  std::vector<float>* m_prd_ambigFlag   ;    // Return ambiguity flag
  */

};
#endif     // MUONSIMHITTOPRD_H


#include "EventPrimitives/EventPrimitives.h"
