/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**   
 *   @file SCT_Fill_CablingFromText.cxx
 *
 *   @brief Fills an SCT cabling object from a plain text source
 *
 *   @author Shaun Roe
 *   @date 13/11/2008
 */

//package includes
#include "SCT_FillCablingFromText.h"
#include "SCT_CablingUtilities.h"

//Athena includes
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"
#include "PathResolver/PathResolver.h"
#include "SCT_Cabling/SCT_CablingData.h"

//Gaudi includes
#include "GaudiKernel/StatusCode.h"

//STL includes
#include <iostream>
#include <fstream>

//Constants at file scope
static const std::string atlasTableSignature{"Rod Fibre Bec LayerDisk Eta Phi Side RobId Sn"};
static const int disabledFibre{255};

//utility functions in file scope
namespace{
  //report if string s1 contains s2
  bool 
    contains(const std::string& s1, const std::string& s2) {
    return (s1.find(s2) not_eq std::string::npos);
  }
  //fast forward to a line in the file containing the signature string, and return that line
  //or the empty string if the end of file was reached first.
  std::string 
    fastForward(std::ifstream& theFile, const std::string& signature) {
    std::string inString;
    bool found{false}, endOfFile{false};
    do {
      std::getline(theFile, inString);
      found=contains(inString, signature);
      endOfFile = theFile.eof();
    } while ((not found) and (not endOfFile));
    return found ? (inString) : (std::string{""});
  }
}//end of anonymous namespace

// Constructor
SCT_FillCablingFromText::SCT_FillCablingFromText(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent) {
  //nop
}

//
StatusCode
SCT_FillCablingFromText::initialize() {
  return StatusCode::SUCCESS;
}

//
StatusCode 
SCT_FillCablingFromText::finalize() {
  return StatusCode::SUCCESS;
}

//
StatusCode
SCT_FillCablingFromText::setDataSource(const std::string& dataSource) {
  m_source=PathResolver::find_file (dataSource, "DATAPATH");
  if (m_source.empty()) {
    ATH_MSG_FATAL("The SCT data file for cabling, "<<dataSource<<", was not found.");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Reading cabling from "<<m_source);
  return StatusCode::SUCCESS;
}

//
std::string
SCT_FillCablingFromText::getDataSource() const {
  return m_source;
}

//
bool
SCT_FillCablingFromText::filled() const {
  return m_filled;
}

//
SCT_CablingData
SCT_FillCablingFromText::getMaps() const {
  SCT_CablingData data;
  if (readDataFromFile(data).isFailure()) {
    ATH_MSG_FATAL("Could not read cabling from file");
    return data;
  }
  m_filled=true;
  return data;
}

//
StatusCode
SCT_FillCablingFromText::readDataFromFile(SCT_CablingData& data) const {
  const SCT_ID* idHelper{nullptr};
  if (detStore()->retrieve(idHelper, "SCT_ID").isFailure()) {
    ATH_MSG_ERROR("SCT mgr failed to retrieve");
    return StatusCode::FAILURE;
  }
  Identifier offlineId;
  IdentifierHash offlineIdHash;
  unsigned int onlineId;
  int robid{0};
  std::ifstream fromDataFile{m_source.c_str()};
  std::string inString;
  if (not fromDataFile) {
    ATH_MSG_FATAL("The cable mapping file could not be opened: "<<m_source);
    return StatusCode::FAILURE;
  }
  bool endOfFile{false};
  inString=fastForward(fromDataFile, atlasTableSignature);
  if (inString.empty()) {
    ATH_MSG_FATAL("The end of the datafile was reached before an identifier mapping was found");
    return StatusCode::FAILURE;
  }
  //next line should be the identifier table, start reading it
  bool endOfTable{false};
  unsigned int numEntries{0};
  std::vector<IdentifierHash> disabledFibres;
  do {
    std::getline(fromDataFile, inString);
    if (contains(inString, "- ")) continue;
    endOfTable=contains(inString, "</textList>");
    std::istringstream dataLine(inString);
    if (not endOfTable) {
      int rod,link,barrelOrEndcap,layer,phi,eta,side,robidFromfile;
      std::string sn,Link;
      //enable exceptions on the stream in case there are bad conversions
      dataLine.exceptions(std::ios_base::badbit|std::ios_base::failbit);
      try {
        dataLine>>rod>>Link>>barrelOrEndcap>>layer>>eta>>phi>>side>>std::hex>>robidFromfile>>std::dec>>sn;
        offlineId = idHelper->wafer_id(barrelOrEndcap,layer,phi,eta,side);
        offlineIdHash = idHelper->wafer_hash(offlineId);
      } catch (const std::ios_base::failure&) {
        ATH_MSG_ERROR("An error occurred while reading the cabling file "<<m_source
                      <<", it may be badly formatted in the following line: \n"<<inString);
        continue;
      } 
      // Check Link variable looks OK
      // The maximum value of an int is 2147483647 in decimal and 0x7fffffff in hexadecimal.
      if (Link.size()==0 or Link.size()>10) {
        ATH_MSG_ERROR("An error occurred while reading the cabling file "<<m_source
                      <<", Link ("<<Link<<") cannot be converted to an integer");
        continue;
      }
      // Let's Get the Online Id From the link and the ROD
      link = std::stoi(Link, nullptr, 0); // 0 means the base used in deterimed by the format in the sequence
      if (link<0) {
        ATH_MSG_ERROR("link " << link << " seems invalid. This was obtained from Link " << Link << ". Will not be used.");
        continue;
      }
      if (link==disabledFibre) {
        ATH_MSG_DEBUG(sn<<": Disabled fibre encountered in text file. Will attempt to place identifier using the other fibre.");
        offlineId = idHelper->wafer_id(barrelOrEndcap,layer,phi,eta,side);
        offlineIdHash = idHelper->wafer_hash(offlineId);
        disabledFibres.push_back(offlineIdHash);
        continue;
      }
      robid = robidFromfile;
      onlineId = (robid & 0xFFFFFF) | (link<<24);
      //std::cout<<" "<<offlineIdHash<<" "<<std::hex<<onlineId<<" "<<std::dec<<sn<<std::endl;
      bool success{insert(offlineIdHash, onlineId, SCT_SerialNumber(sn), data)};
      if (not success) {
        ATH_MSG_ERROR("Insertion of fibre failed, "<<offlineIdHash<<", "<<std::hex<<onlineId<<std::dec<<" "<<sn);
      } else {
        numEntries++;
      }
    }//end of 'if not end of table'
    endOfFile = fromDataFile.eof();
  } while ((not endOfTable) and (not endOfFile));
  
  //************************************************************************************
  // For the disabled fibres 
  //************************************************************************************
  std::string plural{(disabledFibres.size()==1) ? " was" : "s were"};
  ATH_MSG_INFO(disabledFibres.size()<<" disabled fibre"<<plural<<" found.");
  if (not disabledFibres.empty()) {
    int s,os;
    std::vector<IdentifierHash>::const_iterator it{disabledFibres.begin()};
    for (;it not_eq disabledFibres.end();++it) {
      IdentifierHash offlineIdHash{*it};
      //even hashes are on side 0, hashes start from 0
      s=offlineIdHash % 2; //1 or zero
      os = 1-s; //other side, i.e. the enabled one
      //find the hash for the other side: if its odd, subtract 1; if its even, add 1.
      IdentifierHash otherIdHash{offlineIdHash + (s ? -1 : 1)};
      //and its online id
      onlineId=data.getOnlineIdFromHash(otherIdHash);
      int link{static_cast<int>((onlineId>>24) & 0x7F)};
      bool cableSwapped{(link % 2)!=os}; //if its odd and side is zero, or its even and side is 1.
      //now find the newlink by incrementing or decrementing the link number from the other side, according to whether we are on
      //side 0 or side 1 and which order the cabling is going in general (swapped or not swapped)
      int newlink{(s == 0) ? (link - 1) : (link +1)}; //assumes normal unswapped ordering
      if (cableSwapped) newlink = (s == 0) ? (link+1) : (link-1); //assumes swapped ordering
      int newOnlineId{static_cast<int>((onlineId & 0xFFFFFF)|(newlink << 24))};
      ATH_MSG_DEBUG("new: "<<std::hex<<newOnlineId);
      //start entering for the disabled fibre:
      SCT_SerialNumber sn{data.getSerialNumberFromHash(offlineIdHash)};
      bool success{insert(offlineIdHash, newOnlineId, sn, data)};
      if (not success) {
        ATH_MSG_ERROR("Insertion of disabled fibre failed, "<<offlineIdHash<<", "<<std::hex<<newOnlineId<<std::dec<<" "<<sn.str());
      } else {
        numEntries++;
      }
    }
  }
  ATH_MSG_INFO(numEntries<<" entries were made to the identifier map.");
  m_filled=(numEntries not_eq 0);
  return (numEntries==0) ? (StatusCode::FAILURE) : (StatusCode::SUCCESS);
}

bool
SCT_FillCablingFromText::insert(const IdentifierHash& hash, const SCT_OnlineId& onlineId, const SCT_SerialNumber& sn, SCT_CablingData& data) const {
  if (not sn.isWellFormed()) {
    ATH_MSG_FATAL("Serial number is not in correct format");
    return false;
  }
  if (not hash.is_valid()) {
    ATH_MSG_FATAL("Invalid hash: "<<hash);
    return false;
  }

  if (not data.setHashForOnlineId(hash, onlineId)) return false;
  if (not data.setOnlineIdForHash(onlineId, hash)) return false;

  bool successfulInsert{data.setHashForSerialNumber(hash, sn)};
  successfulInsert &= data.setSerialNumberForHash(sn, hash);
  // in this form, the data->getHashEntries() will be half the number of hashes
  if (successfulInsert) {
    data.setRod(onlineId.rod()); //move this here so insertion only happens for valid onlineId, hash
  }
  return true;
}
