/*
 Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//              jFEXForwardElecInfo - Class to store information about trigger towers
//                                    in the fcal and associated jet information.
//                              -------------------
//     begin                : 12 05 2022
//     email                : ulla.blumenschein
//***************************************************************************

#ifndef jFEXForwardElecInfo_H
#define jFEXForwardElecInfo_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "L1CaloFEXSim/jFEXForwardElecTOB.h"
#include "StoreGate/StoreGateSvc.h"

namespace LVL1 {

class jFEXForwardElecInfo {

 public:
  
  
  // Setup with key parameters and menu info
  void setup(int jfex, uint ttid, int neta, int nphi); //used 
  void setup(uint *cval, uint reso = 200);

  // Basic IDs and energies (TT level)
  uint  getCoreTTID();
  uint  getCoreIphi();
  uint  getCoreIeta();             //used
  void  setCoreTTEtEM(uint ET_EM );//used
  uint  getCoreTTEtEM();           //used 
  void  setNextTTID(uint TTID );   //used  
  uint  getNextTTID();
  void  setNextTTEtEM(uint ET_EM); //used 
  uint  getNextTTEtEM();           //used 
  void  setTTEtEMiso(uint iso_ET); //used     
  void  addTTEtEMiso(int iso_ET);  //used 
  uint  getTTEtEMiso();
  void  setTTEtHad1(uint ET_HAD);  //used, EMfr1 
  void  addTTEtHad1(uint ET_HAD);  //used, special SC 
  uint  getTTEtHad1();
  void  setTTEtHad2(uint ET_HAD);  //used, EMfr2 
  uint  getTTEtHad2();
  void  calcTTClusEtEM();          //used
  uint  getTTClusEtEM();           //used
  void  includeTTinSearchWindow(uint TT_ID);
  std::vector<uint> getTTinSearchWindow();
  // floating point values
  void  setCoreTTfEta(float feta);//used
  float getCoreTTfEta();          //used
  void  setCoreTTfPhi(float fphi);//used
  float getCoreTTfPhi();          //used
  uint  getCoreTTiEta();          //used  
  // EDM quantities, corrected for resolution  
  uint  getEtEMiso();
  uint  getEtHad1();
  uint  getEtHad2();
  uint  getEtEM();
  uint  getEt();
  int   getGlobalEta();
  uint  getGlobalPhi();
  uint32_t getTobWord();          //used

  /** Calculate variables for top word **/
  void calcFwdElEDM();            //used

  /** Calculate top word **/
  std::unique_ptr<jFEXForwardElecTOB> getFwdElTOBs();


 private:

  uint  m_coreTTEtEM = 0;
  uint  m_nextTTID = 0;
  uint  m_nextTTEtEM = 0;
  uint  m_TTEtEMiso = 0;
  uint  m_TTEtHad1 = 0;
  uint  m_TTEtHad2 = 0;
  uint  m_TTClusEtEM = 0;
  float m_coreTTfEta = 0;
  float m_coreTTfPhi = 0;
  std::vector<uint> m_TTsInSearchWindow = {};
  //---- configured (constructor, setup) ----- 
  uint  m_coreTTID = 0; 
  int   m_ieta = 0; 
  int   m_iphi = 0; 
  uint  m_jfex = 0; 
  uint  m_reso = 200; 
  uint  m_cval[9] = {1,2,3,20,30,40,20,30,40}; 
  //---- modified in calcTobValues  -----  
  uint  m_EtEMiso = 0;
  uint  m_EtHad1  = 0;
  uint  m_EtHad2  = 0;
  uint  m_EtEM    = 0;
  uint  m_et      = 0; 
  uint  m_emiso   = 0; 
  uint  m_emfr1   = 0; 
  uint  m_emfr2   = 0; 
  uint  m_eta     = 0; 
  uint  m_phi     = 0; 
  uint  m_sat     = 0; 
  uint  m_res     = 0;
  uint32_t m_tob  = 0; 
  //----
  /*
  uint m_coreL2TTID = 0;
  uint m_coreL3TTID = 0; 
  uint m_nextL2TTID = 0;
  uint m_thirdL2TTID = 0;   
   */



};


}//end of namespace

CLASS_DEF( LVL1::jFEXForwardElecInfo , 219384518 , 1 )
#endif
