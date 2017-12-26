/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "SCT_Cabling/SCT_CablingSvc.h"
#include "SCT_CablingUtilities.h"

//indet includes
#include "InDetIdentifier/SCT_ID.h"

//Athena includes
#include "PathResolver/PathResolver.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "StoreGate/StoreGateSvc.h"

//Gaudi includes
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ServiceHandle.h"

//STL includes
#include <iostream>
#include <fstream>

//using local utilities
using SCT_Cabling::inRange;
using SCT_Cabling::stringToInt;

//Constants at file scope
static const std::string atlasTableSignature("Rod Fibre Bec LayerDisk Eta Phi Side RobId Sn");
static const int disabledFibre(255);

//utility functions in file scope
namespace{
  //report if string s1 contains s2
  bool 
  contains(const std::string & s1, const std::string & s2){
    return (s1.find(s2) not_eq std::string::npos);
  }
  //fast forward to a line in the file containing the signature string, and return that line
  //or the empty string if the end of file was reached first.
  std::string 
  fastForward(std::ifstream & theFile, const std::string & signature){
    std::string inString;
    bool found(false),endOfFile(false);
    do{
      std::getline(theFile, inString);
      found=contains(inString, signature);
      endOfFile = theFile.eof();
    }while((not found) and (not endOfFile));
    return found?(inString): (std::string(""));
  }
}//end of anonymous namespace

// Constructor
SCT_FillCablingFromText::SCT_FillCablingFromText( const std::string & name, ISvcLocator * pSvcLocator):
AthService(name,pSvcLocator), m_filled(false){
//nop
}

//
StatusCode
SCT_FillCablingFromText::initialize(){
   return StatusCode::SUCCESS;
}

//
StatusCode 
SCT_FillCablingFromText::finalize(){
  StatusCode sc(StatusCode::SUCCESS);
  return sc;
}

//
StatusCode 
SCT_FillCablingFromText::queryInterface(const InterfaceID & riid, void** ppvInterface ){
  if ( ISCT_FillCabling::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ISCT_FillCabling*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

//
StatusCode 
SCT_FillCablingFromText::setDataSource(const std::string & dataSource){
  m_source=PathResolver::find_file (dataSource, "DATAPATH") ;
  if (m_source.empty()){
    msg(MSG::FATAL)<<"The SCT data file for cabling, "<<dataSource<<", was not found."<<endmsg;
    return StatusCode::FAILURE;
  }
  msg(MSG::INFO)<<"Reading cabling from "<<m_source<<endmsg;
  return StatusCode::SUCCESS;
}

//
std::string 
SCT_FillCablingFromText::getDataSource() const {
  return m_source;
}

//
bool 
SCT_FillCablingFromText::filled() const{
  return m_filled;
}

//
StatusCode 
SCT_FillCablingFromText::fillMaps(SCT_CablingSvc * cabling){
  if (readDataFromFile(cabling).isFailure()) return msg(MSG::FATAL)<<"Could not read cabling from file"<<endmsg, StatusCode::FAILURE;
  m_filled=true;
  return StatusCode::SUCCESS;
}

//
StatusCode
SCT_FillCablingFromText::readDataFromFile(SCT_CablingSvc * cabling){
  ServiceHandle<StoreGateSvc> detStore("DetectorStore",name());
  if (detStore.retrieve().isFailure()) return msg(MSG::FATAL)<< "Detector service  not found !" << endmsg, StatusCode::FAILURE;
  const SCT_ID *  idHelper(0);
  if (detStore->retrieve(idHelper,"SCT_ID").isFailure()) return msg(MSG::ERROR) << "SCT mgr failed to retrieve" << endmsg, StatusCode::FAILURE;
  Identifier offlineId;
  IdentifierHash offlineIdHash;
  unsigned int onlineId;
  int  robid(0);
  std::ifstream fromDataFile(m_source.c_str());
  std::string inString;
  if (not fromDataFile) return (msg(MSG::FATAL) << "The cable mapping file could not be opened: "<<m_source<<endmsg), StatusCode::FAILURE;
  bool endOfFile(false);
  inString=fastForward(fromDataFile, atlasTableSignature );
  if (inString.empty()){
    msg(MSG::FATAL) <<"The end of the datafile was reached before an identifier mapping was found"<<endmsg;
    return StatusCode::FAILURE;
  }
  //next line should be the identifier table, start reading it
  bool endOfTable(false);
  unsigned int numEntries(0);
  std::vector<IdentifierHash> disabledFibres;
  do{
    std::getline(fromDataFile, inString);
    if (contains(inString,"- ")) continue;
    endOfTable=contains(inString,"</textList>");
    std::istringstream dataLine(inString);
    if (not endOfTable){
      int rod,link,barrelOrEndcap,layer,phi,eta,side,robidFromfile ;
      std::string sn,Link ;
      //enable exceptions on the stream in case there are bad conversions
      dataLine.exceptions(std::ios_base::badbit|std::ios_base::failbit);
      try{
        dataLine>>rod>>Link>>barrelOrEndcap>>layer>>eta>>phi>>side>>std::hex>>robidFromfile>>std::dec>>sn ;
        offlineId = idHelper->wafer_id(barrelOrEndcap,layer,phi,eta,side) ;
        offlineIdHash = idHelper->wafer_hash(offlineId);       
      }catch(const std::ios_base::failure&){
        msg(MSG::FATAL)<<"An error occurred while reading the cabling file "<<m_source
        <<", it may be badly formatted in the following line: \n"<<inString<<endmsg;
        //shall we continue or fail? continue... to see whether there are more errors
        continue;
      } 
      // Let's Get the Online Id From the link and the ROD
      link = stringToInt(Link) ;
      if (link==disabledFibre){
        msg(MSG::DEBUG)<<sn<<": Disabled fibre encountered in text file. Will attempt to place identifier using the other fibre."<<endmsg;
        offlineId = idHelper->wafer_id(barrelOrEndcap,layer,phi,eta,side);
        offlineIdHash = idHelper->wafer_hash(offlineId);       
        disabledFibres.push_back(offlineIdHash);
        continue; 
      }
      robid = robidFromfile ;
      onlineId = (robid & 0xFFFFFF)|(link<<24) ;
      //std::cout<<" "<<offlineIdHash<<" "<<std::hex<<onlineId<<" "<<std::dec<<sn<<std::endl;
      bool success(cabling->insert(offlineIdHash, onlineId, SCT_SerialNumber(sn)) );
      if (not success){
        msg(MSG::ERROR)<<"Insertion of fibre failed, "<<offlineIdHash<<", "<<std::hex<<onlineId<<std::dec<<" "<<sn<<endmsg;
      } else numEntries++ ;
    }//end of 'if not end of table'
    endOfFile = fromDataFile.eof();
  }while((not endOfTable) and (not endOfFile));
  
  //************************************************************************************
  // For the disabled fibres 
  //************************************************************************************
  std::string plural=(disabledFibres.size()==1)?" was":"s were";
  msg(MSG::INFO)<<disabledFibres.size()<<" disabled fibre"<<plural<<" found."<<endmsg;
  if (not disabledFibres.empty()){
    int s,os;
    std::vector<IdentifierHash>::const_iterator it(disabledFibres.begin());
    for(;it not_eq disabledFibres.end();++it){
      IdentifierHash offlineIdHash=*it;
      //even hashes are on side 0, hashes start from 0
      s=offlineIdHash % 2; //1 or zero
      os = 1-s; //other side, i.e. the enabled one
      //find the hash for the other side: if its odd, subtract 1; if its even, add 1.
      IdentifierHash otherIdHash = offlineIdHash + (s ? -1 : 1);
      //and its online id
      onlineId=cabling->getOnlineIdFromHash(otherIdHash);
      int link = ((onlineId>>24) & 0x7F) ;
      bool cableSwapped=(link % 2)!=os; //if its odd and side is zero, or its even and side is 1.
      //now find the newlink by incrementing or decrementing the link number from the other side, according to whether we are on
      //side 0 or side 1 and which order the cabling is going in general (swapped or not swapped)
      int newlink = (s == 0)?(link - 1):(link +1);//assumes normal unswapped ordering
      if (cableSwapped) newlink=(s == 0)?(link + 1):(link -1);//assumes swapped ordering
      int newOnlineId = (onlineId & 0xFFFFFF)|(newlink << 24) ;
      msg(MSG::DEBUG)<<"new: "<<std::hex<<newOnlineId<<endmsg;
      //start entering for the disabled fibre:
      SCT_SerialNumber sn(cabling->getSerialNumberFromHash(offlineIdHash));
      bool success(cabling->insert(offlineIdHash, newOnlineId, sn ));
      if (not success){
        msg(MSG::ERROR)<<"Insertion of disabled fibre failed, "<<offlineIdHash<<", "<<std::hex<<newOnlineId<<std::dec<<" "<<sn.str()<<endmsg;
      } else numEntries++;
    }
  }
  msg(MSG::INFO)<<numEntries<<" entries were made to the identifier map."<<endmsg;
  m_filled=(numEntries not_eq 0);
  return (numEntries==0)?(StatusCode::FAILURE) : (StatusCode::SUCCESS);
}

