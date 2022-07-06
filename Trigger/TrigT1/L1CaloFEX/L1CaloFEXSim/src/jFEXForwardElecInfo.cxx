/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration  
*/
//***************************************************************************  
//              jFEXForwardElecInfo - 
//                              -------------------
//     begin                : 12 05 2022
//     email                : ulla.blumenschein@cern.ch
//***************************************************************************  

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "L1CaloFEXSim/jFEXForwardElecInfo.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"


void LVL1::jFEXForwardElecInfo::setup(int jfex, uint ttid, int neta, int nphi){
  m_jfex = jfex;
  m_coreTTID = ttid;
  m_ieta = neta;
  m_iphi = nphi;
}


void LVL1::jFEXForwardElecInfo::setup(uint *cval, uint reso){
  m_reso = reso;
  for(int i=0;i<9;i++) m_cval[i] = cval[i];
}


uint LVL1::jFEXForwardElecInfo::getCoreTTID(){
  return m_coreTTID;
}

uint LVL1::jFEXForwardElecInfo::getCoreIeta(){
  return m_ieta;
}

uint LVL1::jFEXForwardElecInfo::getCoreIphi(){
  return m_iphi;
}

void   LVL1::jFEXForwardElecInfo::setCoreTTfEta(float fEta){
  m_coreTTfEta = fEta;
}


float  LVL1::jFEXForwardElecInfo::getCoreTTfEta(){
  return m_coreTTfEta;
}

uint  LVL1::jFEXForwardElecInfo::getCoreTTiEta(){
  return m_ieta;
}

void   LVL1::jFEXForwardElecInfo::setCoreTTfPhi(float fPhi){
  m_coreTTfPhi = fPhi;
}

float  LVL1::jFEXForwardElecInfo::getCoreTTfPhi(){
  return m_coreTTfPhi;
}

void   LVL1::jFEXForwardElecInfo::setCoreTTEtEM(uint ET_EM ){
   m_coreTTEtEM  = ET_EM;
}

uint   LVL1::jFEXForwardElecInfo::getCoreTTEtEM(){
  return m_coreTTEtEM;
}

void   LVL1::jFEXForwardElecInfo::setNextTTID(uint TTID ){
  m_nextTTID = TTID;
}

uint   LVL1::jFEXForwardElecInfo::getNextTTID(){
  return m_nextTTID;
}

void   LVL1::jFEXForwardElecInfo::setNextTTEtEM(uint nextEt){
  m_nextTTEtEM = nextEt;
}

uint   LVL1::jFEXForwardElecInfo::getNextTTEtEM(){
  return m_nextTTEtEM;
}

void   LVL1::jFEXForwardElecInfo::setTTEtEMiso(uint iso_ET){
  m_TTEtEMiso  = iso_ET;
}

void   LVL1::jFEXForwardElecInfo::addTTEtEMiso(int iso_ET){
  m_TTEtEMiso  += iso_ET;
}

uint   LVL1::jFEXForwardElecInfo::getTTEtEMiso(){
  return m_TTEtEMiso;
}

void   LVL1::jFEXForwardElecInfo::calcTTClusEtEM(){
  m_TTClusEtEM   = m_coreTTEtEM + m_nextTTEtEM;
}

uint   LVL1::jFEXForwardElecInfo::getTTClusEtEM(){
  return m_TTClusEtEM ;
}

void   LVL1::jFEXForwardElecInfo::setTTEtHad1(uint ET_HAD){
  m_TTEtHad1 = ET_HAD;
}

void   LVL1::jFEXForwardElecInfo::addTTEtHad1(uint ET_HAD){
  m_TTEtHad1 += ET_HAD;
}

uint   LVL1::jFEXForwardElecInfo::getTTEtHad1(){
  return m_TTEtHad1;
}

void   LVL1::jFEXForwardElecInfo::setTTEtHad2(uint ET_HAD){
  m_TTEtHad2 = ET_HAD;
}

uint   LVL1::jFEXForwardElecInfo::getTTEtHad2(){
  return m_TTEtHad2;
}

uint   LVL1::jFEXForwardElecInfo::getEtEMiso(){
  return m_EtEMiso;
}

uint   LVL1::jFEXForwardElecInfo::getEtHad2(){
  return m_EtHad2; 
}   

uint   LVL1::jFEXForwardElecInfo::getEtHad1(){
  return m_EtHad1;
} 

uint   LVL1::jFEXForwardElecInfo::getEt(){
  return m_et ;
}

int    LVL1::jFEXForwardElecInfo::getGlobalEta(){
  return m_eta;
}

uint   LVL1::jFEXForwardElecInfo::getGlobalPhi(){
  return m_phi;
}

uint32_t LVL1::jFEXForwardElecInfo::getTobWord(){
  return m_tob;
}

void LVL1::jFEXForwardElecInfo::includeTTinSearchWindow(uint TT_ID){
  m_TTsInSearchWindow.push_back(TT_ID);
}

std::vector<uint> LVL1::jFEXForwardElecInfo::getTTinSearchWindow(){
  return m_TTsInSearchWindow;
}



// Calculating Top quantities

void LVL1::jFEXForwardElecInfo::calcFwdElEDM() {
  m_res     = 0;
  m_sat     = 0;
  m_emiso   = 0;
  m_emfr1   = 0;
  m_emfr2   = 0;
  m_eta     = 0;
  m_phi     = 0;
  m_et      = 0;
  m_tob     = 0;
  m_EtEMiso = 0;
  m_EtHad1  = 0;
  m_EtHad2  = 0;
  m_EtEM    = 0;

  if(m_jfex == 5) {
    m_eta = m_ieta -8;
    if(m_ieta < FEXAlgoSpaceDefs::jFEX_algoSpace_A_EMIE_eta) { // ieta lower than EMIE stats -> belong to EMB
      m_phi = m_iphi-8;
    }
    else if(m_ieta < FEXAlgoSpaceDefs::jFEX_algoSpace_A_FCAL_start_eta) { // ieta lower than FCAL stats -> belong to EMIE
      m_phi = m_iphi -4;
    }
    else { // rest ieta belongs to FCAL
      m_phi = m_iphi -2;
    }
  }
  else if(m_jfex == 0) {
    m_eta = 36 - m_ieta;

    if(m_ieta < FEXAlgoSpaceDefs::jFEX_algoSpace_C_FCAL_end_eta) { // ieta lower than FCal ends -> FCAL 
      m_phi = m_iphi -2 ;
    }
    else if(m_ieta < FEXAlgoSpaceDefs::jFEX_algoSpace_C_EMIE_end_eta) {// ieta lower than EMIE ends -> EMIE 
      m_phi = m_iphi -4 ;
    }
    else {// rest of ieta -> EMB 
      m_phi = m_iphi -8 ;
    }
  }

  calcTTClusEtEM(); 

  if(m_reso > 0){
    m_et = m_TTClusEtEM/m_reso;
    m_EtEMiso =  m_TTEtEMiso/m_reso;
    m_EtHad1  =  m_TTEtHad1/m_reso;
    m_EtHad2  =  m_TTEtHad2/m_reso;
    m_EtEM    =  m_coreTTEtEM/m_reso;   
 }
  if (m_et > 0x7ff) { //0x7ff is 11 bits
    m_et = 0x7ff;
    m_sat=1;
  }



  // values divided by m_reso
  if (m_et > m_cval[2] * m_EtEMiso)  {
    m_emiso  = 3;
  } else if (m_et > m_cval[1] * m_EtEMiso){
    m_emiso  = 2;
  } else if (m_et > m_cval[0] * m_EtEMiso){
    m_emiso  = 1;
  }

  // values not yet divided by m_reso
  if (m_EtEM > m_cval[5] * m_EtHad1)  {
    m_emfr1  = 3;
  } else if (m_EtEM > m_cval[4] * m_EtHad1){
    m_emfr1  = 2;
  } else if (m_EtEM > m_cval[3] * m_EtHad1){
    m_emfr1  = 1;
  }
  if (m_EtEM > m_cval[8] * m_EtHad2)  {
    m_emfr2  = 3;
  } else if (m_EtEM > m_cval[7] * m_EtHad2){
    m_emfr2  = 2;
  } else if (m_EtEM > m_cval[6] * m_EtHad2){
    m_emfr2  = 1;
  }

  m_tob = m_tob + (m_res <<  FEXAlgoSpaceDefs::jEM_resBit) +  (m_emfr2 <<  FEXAlgoSpaceDefs::jEM_emf2Bit) +  (m_emfr1 <<  FEXAlgoSpaceDefs::jEM_emf1Bit) + ( m_emiso << FEXAlgoSpaceDefs::jEM_isoBit) + (m_et << FEXAlgoSpaceDefs::jEM_etBit) + (m_eta << FEXAlgoSpaceDefs::jEM_etaBit) + (m_phi << FEXAlgoSpaceDefs::jEM_phiBit) + m_sat;

}



std::unique_ptr<LVL1::jFEXForwardElecTOB> LVL1::jFEXForwardElecInfo::getFwdElTOBs(){

  std::unique_ptr<LVL1::jFEXForwardElecTOB> tob = std::make_unique<LVL1::jFEXForwardElecTOB>();

  calcFwdElEDM();

  tob->setET(m_et);
  tob->setPhi(m_phi);
  tob->setEta(m_eta);
  tob->setIso(m_emiso);
  tob->setEMf1(m_emfr1);
  tob->setEMf2(m_emfr2);
  tob->setSat(m_sat);
  return tob;
}


