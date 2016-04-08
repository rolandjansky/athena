/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Algorithm.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "LUCID_DigiTools/LUCID_DigiSettings.h"
#include <iostream>

LUCID_DigiSettings::LUCID_DigiSettings(): 
  m_msgSvc(0),
  m_parValueNotSetByUserDouble(10.0e39),
  m_parValueNotSetByUserInt   (32767) {
  
  SetDefaultValues();
}

void LUCID_DigiSettings::Initialize(IMessageSvc* msgSvc) {
  
  m_msgSvc = msgSvc;
  
  MsgStream log(m_msgSvc, "LUCID_DigiSettings::initialize");
  
  log << MSG::INFO << "Initializing" << endreq;
  
  OverwriteDigiParValues();
}

void LUCID_DigiSettings::SetDefaultValues() {
  
  MsgStream log(m_msgSvc, "LUCID_DigiSettings::SetDefaultValues");

  DefNewParameterInt   ("Number of LUCID tubes"          , "numTubes"         , &m_numTubes         , 1,  100);
  DefNewParameterInt   ("QDC channels per photo-electron", "qdcChannelsPerPE" , &m_qdcChannelsPerPE , 1 , 100);
  DefNewParameterDouble("QDC FED NoiseFactor"            , "qdcFedNoiseFactor", &m_qdcFedNoiseFactor, 1., 50.);
  DefNewParameterDouble("TDC PMT NoiseFactor"            , "tdcPmtNoiseFactor", &m_tdcPmtNoiseFactor, 1., 50.);
  DefNewParameterDouble("TDC FED NoiseFactor"            , "tdcFedNoiseFactor", &m_tdcFedNoiseFactor, 1., 50.);
  
  m_numTubes          = 40;
  m_qdcChannelsPerPE  = 10;
  m_qdcFedNoiseFactor = 3.0;
  m_tdcPmtNoiseFactor = 2.8;
  m_tdcFedNoiseFactor = 0.8;
}

void LUCID_DigiSettings::OverwriteDigiParProperties(Algorithm* alg) {
  
  std::map <std::string, parDouble>::iterator doubleMapIt  = m_doubleMap.begin();
  std::map <std::string, parInt>::iterator    intMapIt     = m_intMap.begin();
  
  for (; doubleMapIt != m_doubleMap.end(); ++doubleMapIt) alg->declareProperty(std::string("Overwrite_") + doubleMapIt->first, doubleMapIt->second.overwriteVal); 
  for (; intMapIt    != m_intMap.end()   ; ++intMapIt   ) alg->declareProperty(std::string("Overwrite_") + intMapIt->first   , intMapIt->second.overwriteVal);   
}

void LUCID_DigiSettings::OverwriteDigiParValues() {
  
  MsgStream log(m_msgSvc, "LUCID_DigiSettings");

  log << MSG::INFO << " OverwriteDigiParValues " << endreq;
  
  std::map <std::string, parDouble>::iterator doubleMapIt = m_doubleMap.begin();
  std::map <std::string, parInt>::iterator    intMapIt    = m_intMap.begin();

  if (doubleMapIt->second.overwriteVal != m_parValueNotSetByUserDouble) {
    
    for (; doubleMapIt != m_doubleMap.end(); ++doubleMapIt) {
      
      if (doubleMapIt->second.overwriteVal > doubleMapIt->second.low && 
	  doubleMapIt->second.overwriteVal < doubleMapIt->second.high) {
	
	*(doubleMapIt->second.par) = doubleMapIt->second.overwriteVal;
	
	log << MSG::INFO << " Overwriting: "
	    << doubleMapIt->second.parDescription << " to : " 
	    << doubleMapIt->second.overwriteVal   << endreq;
      } 
    }
  }
  
  if (intMapIt->second.overwriteVal != m_parValueNotSetByUserInt) {
    
    for (; intMapIt != m_intMap.end(); ++intMapIt) {
      
      if (intMapIt->second.overwriteVal > intMapIt->second.low &&
	  intMapIt->second.overwriteVal < intMapIt->second.high) {
	
	*(intMapIt->second.par) = intMapIt->second.overwriteVal;
	
	log << MSG::INFO << " Overwriting: " 
	    << intMapIt->second.parDescription << " to : " 
	    << intMapIt->second.overwriteVal   << endreq;
      } 
    }
  }
}

void LUCID_DigiSettings::Print() const {
  
  MsgStream log(m_msgSvc, "LUCID_DigiSettings::Print");
  
  std::map <std::string, parDouble>::const_iterator doubleMapIt = m_doubleMap.begin();
  std::map <std::string, parInt>::const_iterator    intMapIt    = m_intMap.begin();
  
  log << MSG::INFO << "===================== Doubles ======================" << endreq;

  for (; doubleMapIt != m_doubleMap.end(); ++doubleMapIt) 
    log << MSG::INFO << doubleMapIt->second.parDescription << ": " << *(doubleMapIt->second.par) <<endreq; 
  
  log << MSG::INFO << "===================== Integers ======================" << endreq;
    
  for (; intMapIt != m_intMap.end(); ++intMapIt)
    log << MSG::INFO  << intMapIt->second.parDescription << ": " << *(intMapIt->second.par) << endreq;

  log << MSG::INFO << "=====================================================" << endreq;
}

void LUCID_DigiSettings::SetDigiParDouble(std::string parname, double parval) {
  
  if (!m_doubleMap.count(parname))
    if (!m_intMap.count(parname)) {
      
      std::cout << " LUCID_DigiSettings: parameter does not exist: "  << parname << std::endl; 
      exit(1);
    }
  
  m_doubleMap[parname].par = &parval;
}

void LUCID_DigiSettings::SetDigiParInt(std::string parname, int parval) {
  
  if (!m_doubleMap.count(parname))
    if (!m_intMap.count(parname)) {
      
      std::cout << " LUCID_DigiSettings: parameter does not exist: "  << parname << std::endl; 
      exit(1);
    }
  
  m_intMap[parname].par = &parval;
}

double LUCID_DigiSettings::GetDigiParDouble(std::string parname) {
  
  if (!m_doubleMap.count(parname))
    if (!m_intMap.count(parname)) {
      
      std::cout << " LUCID_DigiSettings: parameter does not exist: "  << parname << std::endl; 
      exit(1);
    }
  
  return *m_doubleMap[parname].par;
}

int LUCID_DigiSettings::GetDigiParInt(std::string parname) {
  
  if (!m_doubleMap.count(parname))
    if (!m_intMap.count(parname)) {
      
      std::cout << " LUCID_DigiSettings: parameter does not exist: "  << parname << std::endl; 
      exit(1);
    }
  
  return *m_intMap[parname].par;
}

void LUCID_DigiSettings::DefNewParameterDouble(std::string parDescription,
					       std::string parname,
					       double* par,
					       double  low,
					       double  high) { 
  
  if (low > high)                 { std::cout << " Swap the boundaries on the range "       << parname << std::endl; exit(1); }
  if (m_doubleMap.count(parname)) { std::cout << " More than one parameter with that name " << parname << std::endl; exit(1); }
  if (m_intMap.count   (parname)) { std::cout << " More than one parameter with that name " << parname << std::endl; exit(1); }
  
  parDouble parD;
  
  parD.parDescription = parDescription;
  parD.par            = par;
  parD.low            = low;
  parD.high           = high;
  parD.overwriteVal   = m_parValueNotSetByUserDouble;
  
  m_doubleMap[parname] = parD;
}

void LUCID_DigiSettings::DefNewParameterInt(std::string parDescription,
					    std::string parname,
					    int* par,
					    int  low,
					    int  high) {
  
  if (low > high)                 { std::cout << " Swap the boundaries on the range "       << parname << std::endl; exit(1); }
  if (m_doubleMap.count(parname)) { std::cout << " More than one parameter with that name " << parname << std::endl; exit(1); }
  if (m_intMap.count   (parname)) { std::cout << " More than one parameter with that name " << parname << std::endl; exit(1); }
  
  parInt parI;
  
  parI.parDescription = parDescription;
  parI.par            = par;
  parI.low            = low;
  parI.high           = high;
  parI.overwriteVal   = m_parValueNotSetByUserInt;
  
  m_intMap[parname] = parI;
}
