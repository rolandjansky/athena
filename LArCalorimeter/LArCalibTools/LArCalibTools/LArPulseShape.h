/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARPULSESHAPE_H
#define LARPULSESHAPE_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/NTuple.h"

#include "LArRecConditions/ILArBadChanTool.h"
#include "LArCabling/LArCablingService.h"
#include "LArCabling/LArSuperCellCablingTool.h"
#include "LArElecCalib/ILArFEBTempTool.h"
#include "LArCalibTools/LArCond2NtupleBase.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include <iostream>
#include "TProfile.h"
#include "TH1.h"

#include "LArRawEvent/LArDigitContainer.h"
#include "LumiBlockComps/ILuminosityTool.h"
#include "LArElecCalib/ILArPedestal.h"

class HWIdentifier;
class LArOnlineID_Base;
class StoreGateSvc;
class LArEM_Base_ID;
class LArHEC_Base_ID;
class LArFCAL_Base_ID;
class CaloCell_ID;

class LArPulseShape : public AthAlgorithm {

 public:
  LArPulseShape(const std::string & name, ISvcLocator * pSvcLocator);
  ~LArPulseShape();

  //Standard algo methods
  StatusCode initialize();
  StatusCode execute(); //   {return StatusCode::SUCCESS;}
  //Finalize needs to be implemented by the deriving class
  StatusCode stop();

 std::map<std::string, TProfile*>::const_iterator TEffbegin() const { return m_effhistos.begin(); }
 std::map<std::string, TProfile*>::const_iterator TEffend()   const { return m_effhistos.end(); }
 std::map<int, TProfile*>::const_iterator TCellbegin() const { return m_cellHistos.begin(); }
 std::map<int, TProfile*>::const_iterator TCellend()   const { return m_cellHistos.end(); } 
 
 std::map<std::string, TH1*>::const_iterator THbegin() const { return m_histos.begin(); }
 std::map<std::string, TH1*>::const_iterator THend()   const { return m_histos.end(); }

 private:
  bool m_initialized;
   std::string m_ntname; 
   std::string m_folder;
   const CaloCell_ID*       m_calo_id;
   NTuple::Item<short> m_peakSample;
   NTuple::Item<short> m_useHgIntercept;
   NTuple::Item<short> m_useMgIntercept;
   NTuple::Item<short> m_useLgIntercept;
   long m_event;
   const DataHandle<CaloIdManager> m_caloIdMgr;
   int m_OffId;  
   int m_OffId_conv;     
   const char * id_char;  
   int isample;
   int i;
   int m_mindist;
   int m_closestBC;

   ToolHandle<Trig::TrigDecisionTool> m_trigDec;  
   
   ToolHandle<ILuminosityTool>  m_lumiTool; 
   const ILArPedestal* m_larPedestal;

   std::map<std::string, TProfile*> m_effhistos;  
   std::map<int, TProfile*> m_cellHistos;     
   TProfile* TProfpulse; 
   TProfile* TProfpulse_diff;    
   std::map<std::string, TH1*> m_histos;
   TProfile** n_cellsP;
   TH1D* h_BCID;
   TH1D* h_diffBCID;   
   TH1D* h_actualInt;
   TH1D* Th;   
  void addHistogram( TH1* h ) {
    std::string name = h->GetName();
    m_histos.insert( std::map<std::string, TH1*>::value_type( name, h) );
  } 
  void addHistogram( TProfile* h ) {
    std::string name = h->GetName();
    m_effhistos.insert( std::map<std::string, TProfile*>::value_type( name, h) );
  }
  
 protected:   
  bool  m_addBC, m_addFEBTemp;
  enum {NOT_VALID = -999};

  std::string m_ntpath, m_ntTitle;

  //Ntuple pointer
  NTuple::Tuple* m_nt;

  //Ntuple variables:
  NTuple::Item<long> m_detector, m_region, m_layer, m_eta, m_phi, m_onlChanId, m_oflChanId;
  NTuple::Item<long> m_pos_neg, m_barrel_ec, m_FT, m_slot, m_channel;
  NTuple::Item<long> m_calibLine,m_badChanWord;
  NTuple::Item<long> m_isConnected;
  NTuple::Item<long> m_chanHash, m_febHash, m_oflHash;
  
  NTuple::Item<float> m_FEBTemp1, m_FEBTemp2;


  StoreGateSvc* m_detStore; 
  const LArEM_Base_ID* m_emId;
  const LArHEC_Base_ID* m_hecId;
  const LArFCAL_Base_ID* m_fcalId;
  const LArOnlineID_Base* m_onlineId;
  const CaloCell_ID* m_caloId;

  ToolHandle<LArCablingService> m_larCablingSvc;
  ToolHandle<ILArBadChanTool> m_badChanTool;
  ToolHandle<ILArFEBTempTool> m_FEBTempTool;
  bool m_isSC;
  bool m_isFlat;

  bool m_addHash;

};
#endif
