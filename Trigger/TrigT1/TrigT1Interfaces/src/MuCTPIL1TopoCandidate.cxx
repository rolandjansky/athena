/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1Interfaces/MuCTPIL1TopoCandidate.h"
#include <iostream> 
#include <iomanip>

namespace LVL1 {

  MuCTPIL1TopoCandidate::MuCTPIL1TopoCandidate(): 
    m_sectorName(""), m_roiID(0),     
    m_bcid(0), m_ptThresholdID(0), m_ptL1TopoCode(0), m_ptValue(0), m_eta(0), m_phi(0), m_etacode(0), m_phicode(0), 
    m_etamin(0), m_etamax(0), m_phimin(0), m_phimax(0), m_roiWord(0), m_mioctID(0), m_ieta(0), m_iphi(0) {}

  void MuCTPIL1TopoCandidate::setCandidateData(std::string  sectorName, 
					       unsigned int roiID,     
					       unsigned int bcid,      
					       unsigned int ptThresholdID,  
					       unsigned int ptL1TopoCode,   
					       unsigned int ptValue,     
					       float eta,       
					       float phi,       
					       unsigned int etacode,   
					       unsigned int phicode,   
					       float etamin,    
					       float etamax,    
					       float phimin,    
					       float phimax,
					       unsigned int mioctID,
					       int ieta,
					       int iphi){
    m_sectorName =    sectorName ;      
    m_roiID =         roiID ;           
    m_bcid =          bcid ;            
    m_ptThresholdID = ptThresholdID ;   
    m_ptL1TopoCode =  ptL1TopoCode ;    
    m_ptValue =       ptValue ;         
    m_eta =           eta ;             
    m_phi =           phi ;             
    m_etacode =       etacode ;         
    m_phicode =       phicode ;         
    m_etamin =        etamin ;          
    m_etamax =        etamax ;          
    m_phimin =        phimin ;          
    m_phimax =        phimax ;             
    m_mioctID =       mioctID;
    m_ieta =          ieta;
    m_iphi =          iphi;
  }

  void MuCTPIL1TopoCandidate::setRoiWord(unsigned int roi){
    m_roiWord = roi;
  } 


  void MuCTPIL1TopoCandidate::print() const {
    
    std::cout << "MioctID: " << m_mioctID << ", Sec: " <<  m_sectorName << ", RoI: " << m_roiID  << ", BCID: " << m_bcid << ", ThrIDX: " 
	      <<  m_ptThresholdID << ", ptCode: " << m_ptL1TopoCode << ", ptVal: " << m_ptValue << ", eta: " 
	      << m_eta << ", phi: " << m_phi << ", etaCode: " << m_etacode << ", phiCode: " << m_phicode 
	      << ", etaMin: " << m_etamin << ", etaMax: " << m_etamax
	      << ", phiMin: " << m_phimin << ", phiMax: " << m_phimax
	      << ", RoIWord: " << std::setw( 8 ) << std::setfill('0') << std::hex << m_roiWord << std::dec
	      << ", ieta: " << m_ieta << ", iphi: " << m_iphi << std::endl << std::setfill(' ') << std::setw(0);

  }


} // namespace LVL1
