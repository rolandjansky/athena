/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAlignmentData/MdtAsBuiltParams.h"
 
MdtAsBuiltParams::MdtAsBuiltParams():
  MuonAlignmentPar(),
  m_XTOMOCHBERNAME     ("XXX"), 
  m_XTOMOSITE          ("YYY"), 	  
  m_XTOMOSITEID        (0),    
  m_XTOMOTIME          (0),	  
  m_XTOMOPASSED        (0),    
  m_XTOMOSIDE          ("X"), 	  
  m_XTOMONBERTUBE1     (0), 
  m_XTOMONBERTUBE2     (0), 
  m_XTOMONBERML        (0),    
  m_XTOMONBERLAYER     (0), 
  m_XTOMOML1NYTUB      (0.),  
  m_XTOMOML1NZTUB      (0.),  
  m_XTOMOML1NDELY      (0.),  
  m_XTOMOML1NDELZ      (0.),  
  m_XTOMOML1NDELA      (0.),  
  m_XTOMOML1NYPIT      (0.),  
  m_XTOMOML1NZPIT      (0.),  
  m_XTOMOML1PYTUB      (0.),  
  m_XTOMOML1PZTUB      (0.),  
  m_XTOMOML1PDELY      (0.),  
  m_XTOMOML1PDELZ      (0.),  
  m_XTOMOML1PDELA      (0.),  
  m_XTOMOML1PYPIT      (0.),  
  m_XTOMOML1PZPIT      (0.),  
  m_XTOMOML2NYTUB      (0.),  
  m_XTOMOML2NZTUB      (0.),  
  m_XTOMOML2NDELY      (0.),  
  m_XTOMOML2NDELZ      (0.),  
  m_XTOMOML2NDELA      (0.),  
  m_XTOMOML2NYPIT      (0.),  
  m_XTOMOML2NZPIT      (0.),  
  m_XTOMOML2PYTUB      (0.),  
  m_XTOMOML2PZTUB      (0.),  
  m_XTOMOML2PDELY      (0.),  
  m_XTOMOML2PDELZ      (0.),  
  m_XTOMOML2PDELA      (0.),  
  m_XTOMOML2PYPIT      (0.),  
  m_XTOMOML2PZPIT      (0.)
{ }

MdtAsBuiltParams::~MdtAsBuiltParams()
{ }


void MdtAsBuiltParams::setParameters(
    std::string XTOMOCHBERNAME,  
    std::string XTOMOSITE, 	    
    int 	XTOMOSITEID,     
    int         XTOMOTIME,	    
    int         XTOMOPASSED,     
    std::string XTOMOSIDE, 	    
    int         XTOMONBERTUBE1,  
    int 	XTOMONBERTUBE2,  
    int         XTOMONBERML,     
    int	        XTOMONBERLAYER,  
    float       XTOMOML1NYTUB,   
    float 	XTOMOML1NZTUB,   
    float 	XTOMOML1NDELY,   
    float 	XTOMOML1NDELZ,   
    float 	XTOMOML1NDELA,   
    float       XTOMOML1NYPIT,   
    float       XTOMOML1NZPIT,   
    float       XTOMOML1PYTUB,   
    float       XTOMOML1PZTUB,   
    float       XTOMOML1PDELY,   
    float       XTOMOML1PDELZ,   
    float       XTOMOML1PDELA,   
    float       XTOMOML1PYPIT,   
    float       XTOMOML1PZPIT,   
    float       XTOMOML2NYTUB,   
    float       XTOMOML2NZTUB,   
    float       XTOMOML2NDELY,   
    float       XTOMOML2NDELZ,   
    float       XTOMOML2NDELA,   
    float       XTOMOML2NYPIT,   
    float       XTOMOML2NZPIT,   
    float       XTOMOML2PYTUB,   
    float       XTOMOML2PZTUB,   
    float       XTOMOML2PDELY,   
    float       XTOMOML2PDELZ,   
    float       XTOMOML2PDELA,   
    float       XTOMOML2PYPIT,   
    float       XTOMOML2PZPIT)
{
  m_XTOMOCHBERNAME     =XTOMOCHBERNAME     ; 
  m_XTOMOSITE          =XTOMOSITE          ; 	  
  m_XTOMOSITEID        =XTOMOSITEID        ;    
  m_XTOMOTIME          =XTOMOTIME          ;	  
  m_XTOMOPASSED        =XTOMOPASSED        ;    
  m_XTOMOSIDE          =XTOMOSIDE          ; 	  
  m_XTOMONBERTUBE1     =XTOMONBERTUBE1     ; 
  m_XTOMONBERTUBE2     =XTOMONBERTUBE2     ; 
  m_XTOMONBERML        =XTOMONBERML        ;    
  m_XTOMONBERLAYER     =XTOMONBERLAYER     ; 
  m_XTOMOML1NYTUB      =XTOMOML1NYTUB      ;  
  m_XTOMOML1NZTUB      =XTOMOML1NZTUB      ;  
  m_XTOMOML1NDELY      =XTOMOML1NDELY      ;  
  m_XTOMOML1NDELZ      =XTOMOML1NDELZ      ;  
  m_XTOMOML1NDELA      =XTOMOML1NDELA      ;  
  m_XTOMOML1NYPIT      =XTOMOML1NYPIT      ;  
  m_XTOMOML1NZPIT      =XTOMOML1NZPIT      ;  
  m_XTOMOML1PYTUB      =XTOMOML1PYTUB      ;  
  m_XTOMOML1PZTUB      =XTOMOML1PZTUB      ;  
  m_XTOMOML1PDELY      =XTOMOML1PDELY      ;  
  m_XTOMOML1PDELZ      =XTOMOML1PDELZ      ;  
  m_XTOMOML1PDELA      =XTOMOML1PDELA      ;  
  m_XTOMOML1PYPIT      =XTOMOML1PYPIT      ;  
  m_XTOMOML1PZPIT      =XTOMOML1PZPIT      ;  
  m_XTOMOML2NYTUB      =XTOMOML2NYTUB      ;  
  m_XTOMOML2NZTUB      =XTOMOML2NZTUB      ;  
  m_XTOMOML2NDELY      =XTOMOML2NDELY      ;  
  m_XTOMOML2NDELZ      =XTOMOML2NDELZ      ;  
  m_XTOMOML2NDELA      =XTOMOML2NDELA      ;  
  m_XTOMOML2NYPIT      =XTOMOML2NYPIT      ;  
  m_XTOMOML2NZPIT      =XTOMOML2NZPIT      ;  
  m_XTOMOML2PYTUB      =XTOMOML2PYTUB      ;  
  m_XTOMOML2PZTUB      =XTOMOML2PZTUB      ;  
  m_XTOMOML2PDELY      =XTOMOML2PDELY      ;  
  m_XTOMOML2PDELZ      =XTOMOML2PDELZ      ;  
  m_XTOMOML2PDELA      =XTOMOML2PDELA      ;  
  m_XTOMOML2PYPIT      =XTOMOML2PYPIT      ;  
  m_XTOMOML2PZPIT      =XTOMOML2PZPIT      ;
}

void MdtAsBuiltParams::getParameters(
    std::string& XTOMOCHBERNAME,  
    std::string& XTOMOSITE, 	    
    int& 	 XTOMOSITEID,     
    int&         XTOMOTIME,	    
    int&         XTOMOPASSED,     
    std::string& XTOMOSIDE, 	    
    int&         XTOMONBERTUBE1,  
    int&  	 XTOMONBERTUBE2,  
    int&         XTOMONBERML,     
    int&         XTOMONBERLAYER,  
    float&       XTOMOML1NYTUB,   
    float& 	XTOMOML1NZTUB,   
    float& 	XTOMOML1NDELY,   
    float& 	XTOMOML1NDELZ,   
    float& 	XTOMOML1NDELA,   
    float&       XTOMOML1NYPIT,   
    float&       XTOMOML1NZPIT,   
    float&       XTOMOML1PYTUB,   
    float&       XTOMOML1PZTUB,   
    float&       XTOMOML1PDELY,   
    float&       XTOMOML1PDELZ,   
    float&       XTOMOML1PDELA,   
    float&       XTOMOML1PYPIT,   
    float&       XTOMOML1PZPIT,   
    float&       XTOMOML2NYTUB,   
    float&       XTOMOML2NZTUB,   
    float&       XTOMOML2NDELY,   
    float&       XTOMOML2NDELZ,   
    float&       XTOMOML2NDELA,   
    float&       XTOMOML2NYPIT,   
    float&       XTOMOML2NZPIT,   
    float&       XTOMOML2PYTUB,   
    float&       XTOMOML2PZTUB,   
    float&       XTOMOML2PDELY,   
    float&       XTOMOML2PDELZ,   
    float&       XTOMOML2PDELA,   
    float&       XTOMOML2PYPIT,   
    float&       XTOMOML2PZPIT)
{
  XTOMOCHBERNAME     =m_XTOMOCHBERNAME     ; 
  XTOMOSITE          =m_XTOMOSITE          ; 	  
  XTOMOSITEID        =m_XTOMOSITEID        ;    
  XTOMOTIME          =m_XTOMOTIME          ;	  
  XTOMOPASSED        =m_XTOMOPASSED        ;    
  XTOMOSIDE          =m_XTOMOSIDE          ; 	  
  XTOMONBERTUBE1     =m_XTOMONBERTUBE1     ; 
  XTOMONBERTUBE2     =m_XTOMONBERTUBE2     ; 
  XTOMONBERML        =m_XTOMONBERML        ;    
  XTOMONBERLAYER     =m_XTOMONBERLAYER     ; 
  XTOMOML1NYTUB      =m_XTOMOML1NYTUB      ;  
  XTOMOML1NZTUB      =m_XTOMOML1NZTUB      ;  
  XTOMOML1NDELY      =m_XTOMOML1NDELY      ;  
  XTOMOML1NDELZ      =m_XTOMOML1NDELZ      ;  
  XTOMOML1NDELA      =m_XTOMOML1NDELA      ;  
  XTOMOML1NYPIT      =m_XTOMOML1NYPIT      ;  
  XTOMOML1NZPIT      =m_XTOMOML1NZPIT      ;  
  XTOMOML1PYTUB      =m_XTOMOML1PYTUB      ;  
  XTOMOML1PZTUB      =m_XTOMOML1PZTUB      ;  
  XTOMOML1PDELY      =m_XTOMOML1PDELY      ;  
  XTOMOML1PDELZ      =m_XTOMOML1PDELZ      ;  
  XTOMOML1PDELA      =m_XTOMOML1PDELA      ;  
  XTOMOML1PYPIT      =m_XTOMOML1PYPIT      ;  
  XTOMOML1PZPIT      =m_XTOMOML1PZPIT      ;  
  XTOMOML2NYTUB      =m_XTOMOML2NYTUB      ;  
  XTOMOML2NZTUB      =m_XTOMOML2NZTUB      ;  
  XTOMOML2NDELY      =m_XTOMOML2NDELY      ;  
  XTOMOML2NDELZ      =m_XTOMOML2NDELZ      ;  
  XTOMOML2NDELA      =m_XTOMOML2NDELA      ;  
  XTOMOML2NYPIT      =m_XTOMOML2NYPIT      ;  
  XTOMOML2NZPIT      =m_XTOMOML2NZPIT      ;  
  XTOMOML2PYTUB      =m_XTOMOML2PYTUB      ;  
  XTOMOML2PZTUB      =m_XTOMOML2PZTUB      ;  
  XTOMOML2PDELY      =m_XTOMOML2PDELY      ;  
  XTOMOML2PDELZ      =m_XTOMOML2PDELZ      ;  
  XTOMOML2PDELA      =m_XTOMOML2PDELA      ;  
  XTOMOML2PYPIT      =m_XTOMOML2PYPIT      ;  
  XTOMOML2PZPIT      =m_XTOMOML2PZPIT      ;
}
				     
bool MdtAsBuiltParams::checkChamberID(std::string stationType, int eta, int phi) {
  std::string stationName = stationType;
  
  // convert eta and phi from int to string
  std::stringstream stationEtaString;
  stationEtaString << eta;
  std::string etaString = stationEtaString.str();
  
  stationName += etaString;
  
  if (eta < 0) {
    stationName += "C";    
  }
  else if (eta > 0) {
    stationName += "A";    
  }
  else {
    stationName += "B";
  }
  
  int phiTmp = phi*2;
  std::stringstream stationPhiString;
  stationPhiString << phiTmp;
  
  std::string stationNameTmp = stationName;
  stationNameTmp += stationPhiString.str();
  
  if (stationName == m_XTOMOCHBERNAME) { return true; }
  
  phiTmp = 2*phi - 1;
  std::stringstream stationPhiString2;
  stationPhiString2 << phiTmp;
  
  stationName += stationPhiString2.str();
  if (stationName == m_XTOMOCHBERNAME) { return true; }
  
  return false;
}

int MdtAsBuiltParams::whichMLtoMove(std::string stationName) {
  /* According to Christoph Amelung the multilayer on which the alignment sensors 
     are mounted shoul remaind fixed in space, so the other multilayer should be moved 
     relative to this one. For the endcap chamber check the following reference for 
     the positions of the alignment sensors (especially fig. 20):
     http://iopscience.iop.org/1748-0221/3/11/P11005/
   */
  
  // endcap
  if (stationName.substr(0,1) == "E") {
     if (stationName.substr(1,2) == "ML") { return 2; }
     if (stationName.substr(1,2) == "OS") { return 2; }
     if (stationName.substr(1,2) == "EL") { return 2; }
     if (stationName.substr(1,2) == "ES") { return 2; }
     if (stationName.substr(1,3) == "IL1") { return 2; }
     if (stationName.substr(1,3) == "IL2") { return 2; }
     if (stationName.substr(1,3) == "IL3") { return 2; }
     if (stationName.substr(1,2) == "IS") { return 1; }
     if (stationName.substr(1,2) == "MS") { return 1; }
     if (stationName.substr(1,2) == "OL") { return 1; }
     if (stationName.substr(1,3) == "IL4") { return 1; }
     if (stationName.substr(1,3) == "IL5") { return 1; }
  }
  
  // barrel
  else if (stationName.substr(0,1) == "B") {
     return 2;
  }
  
  std::cerr << "Unknown station type for MDT chamber " << stationName << std::endl;
  return -1;
}

std::string MdtAsBuiltParams::getChamberName() {
  return m_XTOMOCHBERNAME;
}

bool MdtAsBuiltParams::wasTomographed() {
  return m_XTOMOPASSED;
}

// ----- get functions for RO side -----

float MdtAsBuiltParams::getDeltaZ_RO(int multilayer, std::string stationName) {
   if (multilayer == 1) { return m_XTOMOML1PDELZ; }
   if (multilayer == 2) { return m_XTOMOML2PDELZ; }
   
   std::cerr << "Unvalid number for multilayer " << multilayer << " in MDT chamber " << stationName << std::endl; 
   return 0;
}

float MdtAsBuiltParams::getDeltaY_RO(int multilayer, std::string stationName) {
   if (multilayer != 1 && multilayer != 2) {
      std::cerr << "Unvalid number for multilayer " << multilayer << " in MDT chamber " << stationName << std::endl; 
      return 0;
   }
   
   float deltaY = m_XTOMOML1PDELY + m_XTOMOML2PDELY;
   if (multilayer == whichMLtoMove(stationName)) {
       if (multilayer == 1) { return deltaY * (-1); }
       if (multilayer == 2) { return deltaY; }
   }
   
   return 0;
}

float MdtAsBuiltParams::getZPitch_RO(int multilayer, std::string stationName) {
   if (multilayer == 1) { return fabs(m_XTOMOML1PZPIT); }
   if (multilayer == 2) { return fabs(m_XTOMOML2PZPIT); }
   
   std::cerr << "Unvalid number for multilayer " << multilayer << " in MDT chamber " << stationName << std::endl; 
   return 0;
}

float MdtAsBuiltParams::getYPitch_RO(int multilayer, std::string stationName) {
   if (multilayer == 1) { return fabs(m_XTOMOML1PYPIT); }
   if (multilayer == 2) { return fabs(m_XTOMOML2PYPIT); }
   
   std::cerr << "Unvalid number for multilayer " << multilayer << " in MDT chamber " << stationName << std::endl; 
   return 0;
}

float MdtAsBuiltParams::getDeltaAlpha_RO(int multilayer, std::string stationName) {
   if (multilayer != 1 && multilayer != 2) {
      std::cerr << "Unvalid number for multilayer " << multilayer << " in MDT chamber " << stationName << std::endl; 
      return 0;
   }
   
   float deltaA = m_XTOMOML1PDELA + m_XTOMOML2PDELA;
   if (multilayer == whichMLtoMove(stationName)) {
       if (multilayer == 1) { return deltaA * (-1); }
       if (multilayer == 2) { return deltaA; }
   }
   
   return 0;
}

// ----- get functions for HV side -----

float MdtAsBuiltParams::getDeltaZ_HV(int multilayer, std::string stationName) {
   if (multilayer == 1) { return m_XTOMOML1NDELZ; }
   if (multilayer == 2) { return m_XTOMOML2NDELZ; }

   std::cerr << "Unvalid number for multilayer " << multilayer << " in MDT chamber " << stationName << std::endl; 
   return 0;
}

float MdtAsBuiltParams::getDeltaY_HV(int multilayer, std::string stationName) {
   if (multilayer != 1 && multilayer != 2) {
      std::cerr << "Unvalid number for multilayer " << multilayer << " in MDT chamber " << stationName << std::endl; 
      return 0;
   }
   
   float deltaY = m_XTOMOML1NDELY + m_XTOMOML2NDELY;
   if (multilayer == whichMLtoMove(stationName)) {
       if (multilayer == 1) { return deltaY * (-1); }
       if (multilayer == 2) { return deltaY; }
   }
   
   return 0;
}

float MdtAsBuiltParams::getZPitch_HV(int multilayer, std::string stationName) {
   if (multilayer == 1) { return fabs(m_XTOMOML1NZPIT); }
   if (multilayer == 2) { return fabs(m_XTOMOML2NZPIT); }

   std::cerr << "Unvalid number for multilayer " << multilayer << " in MDT chamber " << stationName << std::endl; 
   return 0;
}

float MdtAsBuiltParams::getYPitch_HV(int multilayer, std::string stationName) {
   if (multilayer == 1) { return fabs(m_XTOMOML1NYPIT); }
   if (multilayer == 2) { return fabs(m_XTOMOML2NYPIT); }

   std::cerr << "Unvalid number for multilayer " << multilayer << " in MDT chamber " << stationName << std::endl; 
   return 0;
}

float MdtAsBuiltParams::getDeltaAlpha_HV(int multilayer, std::string stationName) {
   if (multilayer != 1 && multilayer != 2) {
      std::cerr << "Unvalid number for multilayer " << multilayer << " in MDT chamber " << stationName << std::endl; 
      return 0;
   }
   
   float deltaA = m_XTOMOML1NDELA + m_XTOMOML2NDELA;
   if (multilayer == whichMLtoMove(stationName)) {
       if (multilayer == 1) { return deltaA * (-1); }
       if (multilayer == 2) { return deltaA; }
   }

   return 0;
}
