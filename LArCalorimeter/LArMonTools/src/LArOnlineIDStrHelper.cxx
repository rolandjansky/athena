/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     LArOnlineIDStrHelper.cxx
// PACKAGE:  LArMonTools
// AUTHOR:   Michel Lefebvre
//
// Simple helper that returns string to describe FT/FEB in different styles
// and provides uniform ways to label histograms with FT/FEB names
//
// ********************************************************************

#include "Identifier/IdentifierHash.h"
#include "LArIdentifier/LArOnlineID.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#include "LArMonTools/LArOnlineIDStrHelper.h"
#include "TAxis.h"
#include "TH2.h"

#include "boost/io/ios_state.hpp"


/*---------------------------------------------------------*/
LArOnlineIDStrHelper::LArOnlineIDStrHelper(const LArOnlineID* pLArOnlineIDHelper, 
                                           NameType defaultNameType)
{
  // keep the LArOnlineIDHelper pointer
  m_LArOnlineIDHelper = pLArOnlineIDHelper;

  // set default name type
  this->setDefaultNameType(defaultNameType);

  // fill feedthrough experts names vector
  this->fillFeedthroughExpertsNames();

  // fill feb experts names vectors
  this->fillFebExpertsNames();

  // fill feedthrough HWId map, keyed by feedthrough names (all upper case)
  this->fillFeedthroughHWIds();

  // fill feb HWId map, keyed by feb names (all upper case)
  this->fillFebHWIds();
}

/*---------------------------------------------------------*/
LArOnlineIDStrHelper::~LArOnlineIDStrHelper()
{
}

/*---------------------------------------------------------*/
void 
LArOnlineIDStrHelper::setDefaultNameType(NameType defaultNameType)
{
  // set default name type
  // if not valid, use EXPERT as default of defaults
  if (defaultNameType <= DEFAULT || defaultNameType >= BEYONDLASTNAMETYPE) {
    m_defaultNameType = EXPERTS;  // default of defaults!
  } else {
    m_defaultNameType = defaultNameType;
  }
}

/*---------------------------------------------------------*/
std::string 
LArOnlineIDStrHelper::nameType_str(NameType nameType)
{
  // get name type string
  NameType type = (nameType <= DEFAULT || nameType >= BEYONDLASTNAMETYPE) ? m_defaultNameType : nameType;
  std::string outStr = "";
  if        (type == EXPERTS) {
    outStr =        "EXPERTS";
  } else if (type == EXPERTSCOMPACT) {
    outStr =        "EXPERTSCOMPACT";
  } else if (type == LARONLINEID) {
    outStr =        "LARONLINEID";
  } else if (type == LARONLINEIDCOMPACT) {
    outStr        = "LARONLINEIDCOMPACT";
  }
  return outStr;
}

/*---------------------------------------------------------*/
std::string
LArOnlineIDStrHelper::cryostatEnd_str(HWIdentifier id,
                                      NameType nameType)
{
  // return a string for the cryostatEnd

  // get the name type
  NameType type = (nameType <= DEFAULT || nameType >= BEYONDLASTNAMETYPE) ? m_defaultNameType : nameType;

  std::string outStr = "";
  if (type == EXPERTS || type == LARONLINEID) {
    bool sideA = (bool)m_LArOnlineIDHelper->pos_neg(id);
    if (m_LArOnlineIDHelper->isEmBarrelOnline(id)) {
      outStr = sideA ? "BarrelA" : "BarrelC";
    } else if (m_LArOnlineIDHelper->isEndcapOnline(id)) {
      outStr = sideA ? "EndcapA" : "EndcapC";
    }
  } else if (type == EXPERTSCOMPACT) {
    bool sideA = (bool)m_LArOnlineIDHelper->pos_neg(id);
    if (m_LArOnlineIDHelper->isEmBarrelOnline(id)) {
      outStr = sideA ? "EMA" : "EMC";
    } else if (m_LArOnlineIDHelper->isEndcapOnline(id)) {
      outStr = sideA ? "ECA" : "ECC";
    }
  } else if (type == LARONLINEIDCOMPACT) {
    IdContext cryostatEndC = m_LArOnlineIDHelper->cryostatEndContext();
    outStr = m_LArOnlineIDHelper->show_to_string(id, &cryostatEndC);
  }

  return outStr;
}

/*---------------------------------------------------------*/
std::string
LArOnlineIDStrHelper::feedthrough_str(HWIdentifier id, 
                                      NameType nameType,
                                      bool noSpaces) 
{
  // return a string for a feedthrough

  // get the name type
  NameType type = (nameType <= DEFAULT || nameType >= BEYONDLASTNAMETYPE) ? m_defaultNameType : nameType;

  if (type == LARONLINEIDCOMPACT) {
    IdContext feedthroughC = m_LArOnlineIDHelper->feedthroughContext();
    return m_LArOnlineIDHelper->show_to_string(id, &feedthroughC);
  }

  // get the cryostatEnd part
  std::string outStr = this->cryostatEnd_str(id, nameType);

  if (type == EXPERTS) {
    HWIdentifier feedthroughID = m_LArOnlineIDHelper->feedthrough_Id(id);
    IdentifierHash feedthroughHash = m_LArOnlineIDHelper->feedthrough_Hash(feedthroughID);
    std::string s = (noSpaces) ? "" : " ";
    outStr += s + "FT" + m_feedthroughExpertsNames[feedthroughHash];
  } else if (type == EXPERTSCOMPACT) {
    HWIdentifier feedthroughID = m_LArOnlineIDHelper->feedthrough_Id(id);
    IdentifierHash feedthroughHash = m_LArOnlineIDHelper->feedthrough_Hash(feedthroughID);
    outStr += m_feedthroughExpertsNames[feedthroughHash];
  } else if (type == LARONLINEID) {
    std::string s = (noSpaces) ? "" : " ";
    outStr += s + "FT" + this->int_to_str(m_LArOnlineIDHelper->feedthrough(id), 2, true);
  }

  return outStr;
}

/*---------------------------------------------------------*/
std::string
LArOnlineIDStrHelper::feedthroughOnly_str(HWIdentifier id, 
                                          NameType nameType)
{
  // return a string for a feedthrough only

  // get the name type
  NameType type = (nameType <= DEFAULT || nameType >= BEYONDLASTNAMETYPE) ? m_defaultNameType : nameType;

  std::string outStr = "";

  if (type == EXPERTS || type == EXPERTSCOMPACT) {
    HWIdentifier feedthroughID = m_LArOnlineIDHelper->feedthrough_Id(id);
    IdentifierHash feedthroughHash = m_LArOnlineIDHelper->feedthrough_Hash(feedthroughID);
    outStr = m_feedthroughExpertsNames[feedthroughHash];
  } else if (type == LARONLINEID || type == LARONLINEIDCOMPACT) {
    outStr = this->int_to_str(m_LArOnlineIDHelper->feedthrough(id), 2, true);
  }

  return outStr;
}

/*---------------------------------------------------------*/
std::string
LArOnlineIDStrHelper::feb_str(HWIdentifier id,
                              NameType nameType,
                              bool noSpaces) 
{
  // return a string for a feb

  // get the name type
  NameType type = (nameType <= DEFAULT || nameType >= BEYONDLASTNAMETYPE) ? m_defaultNameType : nameType;

  if (type == LARONLINEIDCOMPACT) {
    IdContext febC = m_LArOnlineIDHelper->febContext();
    return m_LArOnlineIDHelper->show_to_string(id, &febC);
  }

  // get the cryostatEnd and feedthrough part
  std::string outStr = this->feedthrough_str(id, nameType, noSpaces);

  if (type == EXPERTS) {
    HWIdentifier febID = m_LArOnlineIDHelper->feb_Id(id);
    IdentifierHash febHash = m_LArOnlineIDHelper->feb_Hash(febID);
    std::string s = (noSpaces) ? "" : " ";
    outStr += s + m_febExpertsNames[febHash];
  } else if (type == EXPERTSCOMPACT) {
    HWIdentifier febID = m_LArOnlineIDHelper->feb_Id(id);
    IdentifierHash febHash = m_LArOnlineIDHelper->feb_Hash(febID);
    outStr += m_febExpertsCompactNames[febHash];
  } else if (type == LARONLINEID) {
    std::string s = (noSpaces) ? "" : " ";
    outStr += s + "Slot" + this->int_to_str(m_LArOnlineIDHelper->slot(id), 2, true);
  }
 
  return outStr;
}

/*---------------------------------------------------------*/
std::string
LArOnlineIDStrHelper::febOnly_str(HWIdentifier id,
                                  NameType nameType)
{
  // return a string for a feb only

  // get the name type
  NameType type = (nameType <= DEFAULT || nameType >= BEYONDLASTNAMETYPE) ? m_defaultNameType : nameType;

  std::string outStr = "";

  if (type == EXPERTS) {
    HWIdentifier febID = m_LArOnlineIDHelper->feb_Id(id);
    IdentifierHash febHash = m_LArOnlineIDHelper->feb_Hash(febID);
    outStr = m_febExpertsNames[febHash];
  } else if (type == EXPERTSCOMPACT) {
    HWIdentifier febID = m_LArOnlineIDHelper->feb_Id(id);
    IdentifierHash febHash = m_LArOnlineIDHelper->feb_Hash(febID);
    outStr = m_febExpertsCompactNames[febHash];
  } else if (type == LARONLINEID || type == LARONLINEIDCOMPACT) {
    outStr = this->int_to_str(m_LArOnlineIDHelper->slot(id), 2, true);
  }
 
  return outStr;
}

/*---------------------------------------------------------*/
std::string
LArOnlineIDStrHelper::channel_str(HWIdentifier id,
                                  NameType nameType,
                                  bool noSpaces) 
{
  // return a string for a channel

  // get the name type
  NameType type = (nameType <= DEFAULT || nameType >= BEYONDLASTNAMETYPE) ? m_defaultNameType : nameType;

  if (type == LARONLINEIDCOMPACT) {
    IdContext channelC = m_LArOnlineIDHelper->channelContext();
    return m_LArOnlineIDHelper->show_to_string(id, &channelC);
  }

  // get the cryostatEnd, feedthrough and feb part
  std::string outStr = this->feb_str(id, nameType, noSpaces);

  if (type == EXPERTS || type == LARONLINEID) {
    std::string s = (noSpaces) ? "" : " ";
    outStr += s + "Channel" + this->int_to_str(m_LArOnlineIDHelper->channel(id), 3, true);
  } else if (type == EXPERTSCOMPACT) {
    outStr += "Ch" + this->int_to_str(m_LArOnlineIDHelper->channel(id), 3, true);
  }

  return outStr;
}

/*---------------------------------------------------------*/
HWIdentifier
LArOnlineIDStrHelper::feedthrough_id(const std::string& feedthroughName)
{
  // get feedthrough HWId from feedthrough name (case insensitive)
  std::map<std::string, HWIdentifier>::iterator iFeed = m_feedthroughHWIds.find(this->str_to_upper(feedthroughName));
  if (iFeed != m_feedthroughHWIds.end()) return iFeed->second;
  // invalid name, returns an invalid id
  HWIdentifier idInvalid;
  idInvalid.clear();
  return idInvalid;
}

/*---------------------------------------------------------*/
HWIdentifier
LArOnlineIDStrHelper::feb_id(const std::string& febName)
{
  // get feb HWId from feb name (case insensitive)
  std::map<std::string, HWIdentifier>::iterator iFeb = m_febHWIds.find(this->str_to_upper(febName));
  if (iFeb != m_febHWIds.end()) return iFeb->second;
  // invalid name, returns an invalid id
  HWIdentifier idInvalid;
  idInvalid.clear();
  return idInvalid;
}

/*---------------------------------------------------------*/
void
LArOnlineIDStrHelper::fillFeedthroughExpertsNames()
{
  // feedthrough experts names (based on baseplane numbers) vector
  // From ATL-AP-IN-0003, Numberings for the Barrel Feedthroughs

  // Barrel A and C feedthrough experts names
  // indexed by feedthrough number (which starts at 0)
  std::vector<std::string> ftBarrelA, ftBarrelC;
  ftBarrelA.resize(32);     ftBarrelC.resize(32); 
  ftBarrelA[0]  = "01L";    ftBarrelC[0]  = "09L";
  ftBarrelA[1]  = "02R";    ftBarrelC[1]  = "08R";
  ftBarrelA[2]  = "02L";    ftBarrelC[2]  = "08L";
  ftBarrelA[3]  = "03R";    ftBarrelC[3]  = "07R";
  ftBarrelA[4]  = "03L";    ftBarrelC[4]  = "07L";
  ftBarrelA[5]  = "04R";    ftBarrelC[5]  = "06R";
  ftBarrelA[6]  = "04L";    ftBarrelC[6]  = "06L";
  ftBarrelA[7]  = "05R";    ftBarrelC[7]  = "05R";
  ftBarrelA[8]  = "05L";    ftBarrelC[8]  = "05L";
  ftBarrelA[9]  = "06R";    ftBarrelC[9]  = "04R";
  ftBarrelA[10] = "06L";    ftBarrelC[10] = "04L";
  ftBarrelA[11] = "07R";    ftBarrelC[11] = "03R";
  ftBarrelA[12] = "07L";    ftBarrelC[12] = "03L";
  ftBarrelA[13] = "08R";    ftBarrelC[13] = "02R";
  ftBarrelA[14] = "08L";    ftBarrelC[14] = "02L";
  ftBarrelA[15] = "09R";    ftBarrelC[15] = "01R";
  ftBarrelA[16] = "09L";    ftBarrelC[16] = "01L";
  ftBarrelA[17] = "10R";    ftBarrelC[17] = "16R";
  ftBarrelA[18] = "10L";    ftBarrelC[18] = "16L";
  ftBarrelA[19] = "11R";    ftBarrelC[19] = "15R";
  ftBarrelA[20] = "11L";    ftBarrelC[20] = "15L";
  ftBarrelA[21] = "12R";    ftBarrelC[21] = "14R";
  ftBarrelA[22] = "12L";    ftBarrelC[22] = "14L";
  ftBarrelA[23] = "13R";    ftBarrelC[23] = "13R";
  ftBarrelA[24] = "13L";    ftBarrelC[24] = "13L";
  ftBarrelA[25] = "14R";    ftBarrelC[25] = "12R";
  ftBarrelA[26] = "14L";    ftBarrelC[26] = "12L";
  ftBarrelA[27] = "15R";    ftBarrelC[27] = "11R";
  ftBarrelA[28] = "15L";    ftBarrelC[28] = "11L";
  ftBarrelA[29] = "16R";    ftBarrelC[29] = "10R";
  ftBarrelA[30] = "16L";    ftBarrelC[30] = "10L";
  ftBarrelA[31] = "01R";    ftBarrelC[31] = "09R";
  
  // Endcap A and C feedthrough experts names, indexed by feedthrough number
  std::vector<std::string> ftEndcapA, ftEndcapC;
  ftEndcapA.resize(25);     ftEndcapC.resize(25); 
  ftEndcapA[0]  = "01R";    ftEndcapC[0]  = "07R";
  ftEndcapA[1]  = "01L";    ftEndcapC[1]  = "07L";
  ftEndcapA[2]  = "02R";    ftEndcapC[2]  = "06R";
  ftEndcapA[3]  = "02L";    ftEndcapC[3]  = "06L";
  ftEndcapA[4]  = "03R";    ftEndcapC[4]  = "05R";
  ftEndcapA[5]  = "03L";    ftEndcapC[5]  = "05L";
  ftEndcapA[6]  = "04R";    ftEndcapC[6]  = "04L";  // the FCAL baseplane
  ftEndcapA[7]  = "05R";    ftEndcapC[7]  = "03R";
  ftEndcapA[8]  = "05L";    ftEndcapC[8]  = "03L";
  ftEndcapA[9]  = "06R";    ftEndcapC[9]  = "02R";
  ftEndcapA[10] = "06L";    ftEndcapC[10] = "02L";
  ftEndcapA[11] = "07R";    ftEndcapC[11] = "01R";
  ftEndcapA[12] = "07L";    ftEndcapC[12] = "01L";
  ftEndcapA[13] = "08R";    ftEndcapC[13] = "13R";
  ftEndcapA[14] = "08L";    ftEndcapC[14] = "13L";
  ftEndcapA[15] = "09R";    ftEndcapC[15] = "12R";
  ftEndcapA[16] = "09L";    ftEndcapC[16] = "12L";
  ftEndcapA[17] = "10R";    ftEndcapC[17] = "11R";
  ftEndcapA[18] = "10L";    ftEndcapC[18] = "11L";
  ftEndcapA[19] = "11R";    ftEndcapC[19] = "10R";
  ftEndcapA[20] = "11L";    ftEndcapC[20] = "10L";
  ftEndcapA[21] = "12R";    ftEndcapC[21] = "09R";
  ftEndcapA[22] = "12L";    ftEndcapC[22] = "09L";
  ftEndcapA[23] = "13R";    ftEndcapC[23] = "08R";
  ftEndcapA[24] = "13L";    ftEndcapC[24] = "08L";

  // fill the feedthrough_hash indexed vector of feedthrough experts names
  m_feedthroughExpertsNames.resize(m_LArOnlineIDHelper->feedthroughHashMax());
  for (std::vector<HWIdentifier>::const_iterator p = m_LArOnlineIDHelper->feedthrough_begin();
       p != m_LArOnlineIDHelper->feedthrough_end(); ++p) {
    HWIdentifier feedthroughID = HWIdentifier(*p);
    IdentifierHash feedthroughHash = m_LArOnlineIDHelper->feedthrough_Hash(feedthroughID);
    int feedthrough = m_LArOnlineIDHelper->feedthrough(feedthroughID);
    bool sideA = (bool)m_LArOnlineIDHelper->pos_neg(feedthroughID);
    if (m_LArOnlineIDHelper->isEmBarrelOnline(feedthroughID)) {
      m_feedthroughExpertsNames[feedthroughHash] = 
        (sideA) ? ftBarrelA[feedthrough] : ftBarrelC[feedthrough];
    } else if (m_LArOnlineIDHelper->isEndcapOnline(feedthroughID)) {
      m_feedthroughExpertsNames[feedthroughHash] = 
        (sideA) ? ftEndcapA[feedthrough] : ftEndcapC[feedthrough];
    }
  }
}

/*---------------------------------------------------------*/
void
LArOnlineIDStrHelper::fillFebExpertsNames()
{
  // feb experts names vectors
  // inspired from ATL-A-EN-0001, Cabling of the ATLAS liquid argon calorimeters

  // feb experts names for Barrel feedthroughs
  // indexed using slotIndex = slot number - 1 (which starts at 0)
  std::vector<std::string> febBarrel, febBarrelCompact;
  febBarrel.resize(14);            febBarrelCompact.resize(14);            
  febBarrel[0]  = "Presampler";    febBarrelCompact[0]  = "Ps"; // slot 1
  febBarrel[1]  = "Front0";        febBarrelCompact[1]  = "F0";      
  febBarrel[2]  = "Front1";        febBarrelCompact[2]  = "F1";      
  febBarrel[3]  = "Front2";        febBarrelCompact[3]  = "F2";      
  febBarrel[4]  = "Front3";        febBarrelCompact[4]  = "F3";      
  febBarrel[5]  = "Front4";        febBarrelCompact[5]  = "F4";      
  febBarrel[6]  = "Front5";        febBarrelCompact[6]  = "F5";      
  febBarrel[7]  = "Front6";        febBarrelCompact[7]  = "F6";      
  febBarrel[8]  = "Back0";         febBarrelCompact[8]  = "B0";       
  febBarrel[9]  = "Back1";         febBarrelCompact[9]  = "B1";       
  febBarrel[10] = "Middle0";       febBarrelCompact[10] = "M0";     
  febBarrel[11] = "Middle1";       febBarrelCompact[11] = "M1";     
  febBarrel[12] = "Middle2";       febBarrelCompact[12] = "M2";     
  febBarrel[13] = "Middle3";       febBarrelCompact[13] = "M3";     

  // feb experts names for Endcap EM Standard feedthroughs
  // indexed using slotIndex = slot number - 1 (which starts at 0)
  std::vector<std::string> febStd, febStdCompact;
  febStd.resize(13);               febStdCompact.resize(13);            
  febStd[0]  = "StdPresampler";    febStdCompact[0]  = "StdPs"; // slot 1
  febStd[1]  = "StdFront0";        febStdCompact[1]  = "StdF0";           
  febStd[2]  = "StdFront1";        febStdCompact[2]  = "StdF1";           
  febStd[3]  = "StdFront2";        febStdCompact[3]  = "StdF2";           
  febStd[4]  = "StdFront3";        febStdCompact[4]  = "StdF3";           
  febStd[5]  = "StdFront4";        febStdCompact[5]  = "StdF4";           
  febStd[6]  = "StdFront5";        febStdCompact[6]  = "StdF5";           
  febStd[7]  = "StdBack0";         febStdCompact[7]  = "StdB0";            
  febStd[8]  = "StdBack1";         febStdCompact[8]  = "StdB1";             
  febStd[9]  = "StdMiddle0";       febStdCompact[9]  = "StdM0";           
  febStd[10] = "StdMiddle1";       febStdCompact[10] = "StdM1";         
  febStd[11] = "StdMiddle2";       febStdCompact[11] = "StdM2";         
  febStd[12] = "StdMiddle3";       febStdCompact[12] = "StdM3";         

  // feb experts names for Endcap EM Special feedthroughs
  // indexed using slotIndex = slot number - 1 (which starts at 0)
  std::vector<std::string> febSpe, febSpeCompact;
  febSpe.resize(15);               febSpeCompact.resize(15);            
  febSpe[0]  = "SpePresampler";    febSpeCompact[0]  = "SpePs"; // slot 1
  febSpe[1]  = "SpeFront0";        febSpeCompact[1]  = "SpeF0";               
  febSpe[2]  = "SpeMiddle0";       febSpeCompact[2]  = "SpeM0";              
  febSpe[3]  = "SpeMiddle1";       febSpeCompact[3]  = "SpeM1";              
  febSpe[4]  = "SpeFront1";        febSpeCompact[4]  = "SpeF1";               
  febSpe[5]  = "SpeFront2";        febSpeCompact[5]  = "SpeF2";               
  febSpe[6]  = "SpeFront3";        febSpeCompact[6]  = "SpeF3";               
  febSpe[7]  = "SpeFront4";        febSpeCompact[7]  = "SpeF4";                
  febSpe[8]  = "SpeBack0";         febSpeCompact[8]  = "SpeB0";                  
  febSpe[9]  = "SpeMiddle2";       febSpeCompact[9]  = "SpeM2";              
  febSpe[10] = "SpeMiddle3";       febSpeCompact[10] = "SpeM3";            
  febSpe[11] = "SpeFront5";        febSpeCompact[11] = "SpeF5";             
  febSpe[12] = "SpeBack1";         febSpeCompact[12] = "SpeB1";              
  febSpe[13] = "SpeMiddle4";       febSpeCompact[13] = "SpeM4";   
  febSpe[14] = "SpeMiddle5";       febSpeCompact[14] = "SpeM5"; 

  // feb experts names for Endcap HEC feedthroughs
  // indexed using slotIndex = slot number - 1 (which starts at 0)
  std::vector<std::string> febHEC, febHECCompact;
  febHEC.resize(10);         febHECCompact.resize(10);      
  febHEC[0]  = "EMInner1";   febHECCompact[0]  = "EMI1";    // slot 1
  febHEC[1]  = "EMInner2";   febHECCompact[1]  = "EMI2";    // note: slot 3 and 4 not used
  febHEC[4]  = "HECL1";      febHECCompact[4]  = "HECL1";            
  febHEC[5]  = "HECL2";      febHECCompact[5]  = "HECL2";             
  febHEC[6]  = "HECM1";      febHECCompact[6]  = "HECM1";             
  febHEC[7]  = "HECM2";      febHECCompact[7]  = "HECM2";              
  febHEC[8]  = "HECH1";      febHECCompact[8]  = "HECH1";               
  febHEC[9]  = "HECH2";      febHECCompact[9]  = "HECH2"; 
             
  // feb experts names for Endcap FCAL feedthroughs
  // indexed using slotIndex = slot number - 1 (which starts at 0)
  std::vector<std::string> febFCal;
  febFCal.resize(15);
  febFCal[0]  = "FCal100"; // slot 1              
  febFCal[1]  = "FCal101";               
  febFCal[2]  = "FCal102";             
  febFCal[3]  = "FCal103";             
  febFCal[4]  = "FCal104";               
  febFCal[5]  = "FCal105";               
  febFCal[6]  = "FCal106"; 
  febFCal[7]  = "Slot08";  // note: slot 8 not used, but valid feb number!
  febFCal[8]  = "FCal107"; 
  febFCal[9]  = "FCal200";             
  febFCal[10] = "FCal201";           
  febFCal[11] = "FCal202";             
  febFCal[12] = "FCal203";               
  febFCal[13] = "FCal300";  
  febFCal[14] = "FCal301";

  // fill the feb_hash indexed vector of feb experts names
  m_febExpertsNames.resize(m_LArOnlineIDHelper->febHashMax());
  m_febExpertsCompactNames.resize(m_LArOnlineIDHelper->febHashMax());
  for (std::vector<HWIdentifier>::const_iterator p = m_LArOnlineIDHelper->feb_begin(); 
       p != m_LArOnlineIDHelper->feb_end(); ++p) {
    HWIdentifier febID = HWIdentifier(*p);
    IdentifierHash febHash = m_LArOnlineIDHelper->feb_Hash(febID);
    int slot = m_LArOnlineIDHelper->slot(febID);  // starts at 1
    int slotIndex = slot - 1;                     // starts at 0
    if (m_LArOnlineIDHelper->isEmBarrelOnline(febID)) {
      m_febExpertsNames[febHash]        = febBarrel[slotIndex];
      m_febExpertsCompactNames[febHash] = febBarrelCompact[slotIndex];
    } else if (m_LArOnlineIDHelper->isEmEndcapStandardOnline(febID)) {
      m_febExpertsNames[febHash]        = febStd[slotIndex];
      m_febExpertsCompactNames[febHash] = febStdCompact[slotIndex];
    } else if (m_LArOnlineIDHelper->isEmEndcapSpecialOnline(febID)) {
      m_febExpertsNames[febHash]        = febSpe[slotIndex];
      m_febExpertsCompactNames[febHash] = febSpeCompact[slotIndex];
    } else if (m_LArOnlineIDHelper->isHecOnlineFebId(febID)) {
      m_febExpertsNames[febHash]        = febHEC[slotIndex];
      m_febExpertsCompactNames[febHash] = febHECCompact[slotIndex];
    } else if (m_LArOnlineIDHelper->isFcalOnline(febID)) {
      m_febExpertsNames[febHash]        = febFCal[slotIndex];
      m_febExpertsCompactNames[febHash] = febFCal[slotIndex];
    }
  }
}

/*---------------------------------------------------------*/
void
LArOnlineIDStrHelper::fillFeedthroughHWIds()
{
  // fill feedthrough HWId map, keyed by feedthrough names (all upper case)

  for (std::vector<HWIdentifier>::const_iterator p = m_LArOnlineIDHelper->feedthrough_begin();
       p != m_LArOnlineIDHelper->feedthrough_end(); ++p) {
    HWIdentifier feedthroughID = HWIdentifier(*p);
    std::string feedthroughNameUpper;
    feedthroughNameUpper = str_to_upper(this->feedthrough_str(feedthroughID, EXPERTS));
    m_feedthroughHWIds[feedthroughNameUpper] = feedthroughID;
    // std::cout << this->feedthrough_str(feedthroughID, EXPERTS) << "  " << feedthroughNameUpper << std::endl;
    feedthroughNameUpper = str_to_upper(this->feedthrough_str(feedthroughID, EXPERTSCOMPACT));
    m_feedthroughHWIds[feedthroughNameUpper] = feedthroughID;
    feedthroughNameUpper = str_to_upper(this->feedthrough_str(feedthroughID, LARONLINEID));
    m_feedthroughHWIds[feedthroughNameUpper] = feedthroughID;
    feedthroughNameUpper = str_to_upper(this->feedthrough_str(feedthroughID, LARONLINEIDCOMPACT));
    m_feedthroughHWIds[feedthroughNameUpper] = feedthroughID;
  }
}

/*---------------------------------------------------------*/
void
LArOnlineIDStrHelper::fillFebHWIds()
{
  // fill feb HWId map, keyed by feb names (all upper case)
  for (std::vector<HWIdentifier>::const_iterator p = m_LArOnlineIDHelper->feb_begin(); 
       p != m_LArOnlineIDHelper->feb_end(); ++p) {
    HWIdentifier febID = HWIdentifier(*p);
    std::string febNameUpper;
    febNameUpper = str_to_upper(this->feb_str(febID, EXPERTS));
    m_febHWIds[febNameUpper] = febID;
    // std::cout << this->feb_str(febID, EXPERTS) << "  " << febNameUpper << std::endl;
    febNameUpper = str_to_upper(this->feb_str(febID, EXPERTSCOMPACT));
    m_febHWIds[febNameUpper] = febID;
    febNameUpper = str_to_upper(this->feb_str(febID, LARONLINEID));
    m_febHWIds[febNameUpper] = febID;
    febNameUpper = str_to_upper(this->feb_str(febID, LARONLINEIDCOMPACT));
    m_febHWIds[febNameUpper] = febID;
  }
}

/*---------------------------------------------------------*/
std::string
LArOnlineIDStrHelper::int_to_str(int n, int width, bool leadingZeros)
{
  // integer to string converter
  // width <= 0 means no setw used and no leading zeros

  std::ostringstream o;
  if (width <= 0) {
    o << n;
  } else {
    if (leadingZeros) {      
      o << std::setfill('0') << std::setw(width) << n;
    } else {
      o << std::setw(width) << n;
    }
  }

  return o.str();
}

/*---------------------------------------------------------*/
std::string
LArOnlineIDStrHelper::str_to_upper(std::string stringToConvert)
{
  // convert a string to an all upper case string
  for (unsigned int i = 0; i < stringToConvert.size(); i++) {
    stringToConvert[i] = toupper(stringToConvert[i]);
  }
  return stringToConvert;
}

/*---------------------------------------------------------*/
void
LArOnlineIDStrHelper::setAxisBinLabelsFeedthroughs(TAxis* axis, const std::string& detector)
{
  // set the axis bin labels for a feedthrough axis
  // see setAxisBinLabelsFeedthroughNames

  this->setAxisBinLabelsFeedthroughNames(axis, detector, false);
}

/*---------------------------------------------------------*/
void
LArOnlineIDStrHelper::test()
{
  std::ios_base::fmtflags coutflags = std::cout.flags();
  std::cout << "LArOnlineIDStrHelper: Default name type = "
            << this->nameType_str(m_defaultNameType) << std::endl;

  std::cout << "Feedthroughs" << std::endl;
  std::cout << "HWId       EXPERTS  EXPERTSCOMPACT LARONLINEID   LARONLINEIDCOMPACT" << std::endl;
  for (std::vector<HWIdentifier>::const_iterator p = m_LArOnlineIDHelper->feedthrough_begin();
       p != m_LArOnlineIDHelper->feedthrough_end(); ++p) {
    HWIdentifier feedthroughID = HWIdentifier(*p);
    std::string nameEXPERTS            = this->feedthrough_str(feedthroughID, EXPERTS);
    std::string nameEXPERTSCOMPACT     = this->feedthrough_str(feedthroughID, EXPERTSCOMPACT);
    std::string nameLARONLINEID        = this->feedthrough_str(feedthroughID, LARONLINEID);
    std::string nameLARONLINEIDCOMPACT = this->feedthrough_str(feedthroughID, LARONLINEIDCOMPACT);
    std::string nameOnlyEXPERTS        = this->feedthroughOnly_str(feedthroughID, EXPERTS);
    std::string nameOnlyLARONLINEID    = this->feedthroughOnly_str(feedthroughID, LARONLINEID);
    HWIdentifier idEXPERTS            = this->feedthrough_id(nameEXPERTS);
    HWIdentifier idEXPERTSCOMPACT     = this->feedthrough_id(nameEXPERTSCOMPACT);
    HWIdentifier idLARONLINEID        = this->feedthrough_id(nameLARONLINEID);
    HWIdentifier idLARONLINEIDCOMPACT = this->feedthrough_id(nameLARONLINEIDCOMPACT);
    boost::io::ios_base_all_saver foutsave (std::cout);
    if (idEXPERTS == feedthroughID
        && idEXPERTSCOMPACT == feedthroughID
        && idLARONLINEID == feedthroughID
        && idLARONLINEIDCOMPACT == feedthroughID)
      {
      std::cout << std::setw(11) << std::left << std::hex << feedthroughID << std::dec
                << std::setw(15) << nameEXPERTS
                << std::setw(9)  << nameEXPERTSCOMPACT
                << std::setw(14) << nameLARONLINEID 
                << std::setw(13) << nameLARONLINEIDCOMPACT
                << std::setw(4)  << nameOnlyEXPERTS 
                << std::setw(3)  << nameOnlyLARONLINEID 
                << std::endl;
    } else {
      std::cout << "Bad HWId extraction for " << nameEXPERTS << ", " << idEXPERTS << std::endl;
    }
  }

  std::cout << "Febs" << std::endl;
  std::cout << "HWId       EXPERTS                     EXPERTSCOMPACT   LARONLINEID           LARONLINEIDCOMPACT" << std::endl;
  for (std::vector<HWIdentifier>::const_iterator p = m_LArOnlineIDHelper->feb_begin(); 
       p != m_LArOnlineIDHelper->feb_end(); ++p) {
    HWIdentifier febID = HWIdentifier(*p);
    std::string nameEXPERTS            = this->feb_str(febID, EXPERTS);
    std::string nameEXPERTSCOMPACT     = this->feb_str(febID, EXPERTSCOMPACT);
    std::string nameLARONLINEID        = this->feb_str(febID, LARONLINEID);
    std::string nameLARONLINEIDCOMPACT = this->feb_str(febID, LARONLINEIDCOMPACT);
    std::string nameOnlyEXPERTS        = this->febOnly_str(febID, EXPERTS);
    std::string nameOnlyEXPERTSCOMPACT = this->febOnly_str(febID, EXPERTSCOMPACT);
    std::string nameOnlyLARONLINEID    = this->febOnly_str(febID, LARONLINEID);
    HWIdentifier idEXPERTS            = this->feb_id(nameEXPERTS);
    HWIdentifier idEXPERTSCOMPACT     = this->feb_id(nameEXPERTSCOMPACT);
    HWIdentifier idLARONLINEID        = this->feb_id(nameLARONLINEID);
    HWIdentifier idLARONLINEIDCOMPACT = this->feb_id(nameLARONLINEIDCOMPACT);
    if (idEXPERTS == febID
        && idEXPERTSCOMPACT == febID
        && idLARONLINEID == febID
        && idLARONLINEIDCOMPACT == febID)
      { 
        std::cout << std::setw(11) << std::left << std::hex << febID << std::dec
                  << std::setw(28) << nameEXPERTS           
                  << std::setw(17) << nameEXPERTSCOMPACT    
                  << std::setw(22) << nameLARONLINEID       
                  << std::setw(16) << nameLARONLINEIDCOMPACT
                  << std::setw(15) << nameOnlyEXPERTS    
                  << std::setw(8)  << nameOnlyEXPERTSCOMPACT
                  << std::setw(3)  << nameOnlyLARONLINEID
                  << std::endl;
    } else {
      std::cout << "Bad HWId extraction for " << nameEXPERTS << ", " << idEXPERTS << std::endl;
    }
  }

  std::cout << "A few channels" << std::endl;
//   int barrel_ec = 1;    // endcap
//   int pos_neg = 1;      // A side
//   int feedthrough = 6;  // start at 0; 6 is FCal
//   int slot = 14;        // start at 1; 15 is where the FCal ID bug is
  int barrel_ec = 0;    // barrel
  int pos_neg = 0;      // C side
  int feedthrough = 4;  // start at 0
  int slot = 1;         // start at 1
  std::vector<int> channels;
  channels.push_back(5);
  channels.push_back(10);
  channels.push_back(105);
  std::cout << "HWId       EXPERTS" << std::endl;
  for (int j = 0; j < (int)channels.size(); ++j) {
    HWIdentifier id = m_LArOnlineIDHelper->channel_Id(barrel_ec, pos_neg, feedthrough, slot, channels[j]);
    std::cout << std::setw(11) << std::left << std::hex << id << std::dec
              << this->channel_str(id, EXPERTS)         << "   "
              << this->feb_str(id, EXPERTS)             << "   "
              << this->feedthrough_str(id, EXPERTS)     << "   "
              << this->cryostatEnd_str(id, EXPERTS)     << "   "
              << "   febHash = " << m_LArOnlineIDHelper->feb_Hash(id)
              << std::endl;
  }
  std::cout << "HWId       EXPERTSCOMPACT" << std::endl;
  for (int j = 0; j < (int)channels.size(); ++j) {
    HWIdentifier id = m_LArOnlineIDHelper->channel_Id(barrel_ec, pos_neg, feedthrough, slot, channels[j]);
    std::cout << std::setw(11) << std::left << std::hex << id << std::dec
              << this->channel_str(id, EXPERTSCOMPACT)         << "   "
              << this->feb_str(id, EXPERTSCOMPACT)             << "   "
              << this->feedthrough_str(id, EXPERTSCOMPACT)     << "   "
              << this->cryostatEnd_str(id, EXPERTSCOMPACT)
              << std::endl;
  }
  std::cout << "HWId       LARONLINEID" << std::endl;
  for (int j = 0; j < (int)channels.size(); ++j) {
    HWIdentifier id = m_LArOnlineIDHelper->channel_Id(barrel_ec, pos_neg, feedthrough, slot, channels[j]);
    std::cout << std::setw(11) << std::left << std::hex << id << std::dec
              << this->channel_str(id, LARONLINEID)         << "   "
              << this->feb_str(id, LARONLINEID)             << "   "
              << this->feedthrough_str(id, LARONLINEID)     << "   "
              << this->cryostatEnd_str(id, LARONLINEID)
              << std::endl;
  }
  std::cout << "HWId       LARONLINEIDCOMPACT" << std::endl;
  for (int j = 0; j < (int)channels.size(); ++j) {
    HWIdentifier id = m_LArOnlineIDHelper->channel_Id(barrel_ec, pos_neg, feedthrough, slot, channels[j]);
    std::cout << std::setw(11) << std::left << std::hex << id << std::dec
              << this->channel_str(id, LARONLINEIDCOMPACT)         << "   "
              << this->feb_str(id, LARONLINEIDCOMPACT)             << "   "
              << this->feedthrough_str(id, LARONLINEIDCOMPACT)     << "   "
              << this->cryostatEnd_str(id, LARONLINEIDCOMPACT)
              << std::endl;
  }

  std::cout << "Id searches" << std::endl;
  std::vector<std::string> febNames;
  febNames.push_back("banana");
  febNames.push_back("BarrelCFT09LMiddle3");
  febNames.push_back("BarrelCfT09Lmiddle3");
  for (std::vector<std::string>::iterator it = febNames.begin(); it != febNames.end(); ++it) {
    std::string febName = *it;
    HWIdentifier febID = this->feb_id(febName);
    if (febID.is_valid()) {
      std::cout << febName << " has valid feb id = "   << std::hex << febID << std::dec << std::endl;
    } else {
      std::cout << febName << " has invalid feb id = " << std::hex << febID << std::dec << std::endl;
    }
  }
  std::cout.flags( coutflags ); 
}

