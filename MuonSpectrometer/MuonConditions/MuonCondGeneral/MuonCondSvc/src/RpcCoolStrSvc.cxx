/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//Service designed to read in calibration files to the cool database. Can also read them
//back out again to check.
// author lampen@physics.arizona.edu

#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <inttypes.h>
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonCondData/RpcCalibData.h"
#include "MuonCondData/RpcCalibDBEntry.h"
#include "MuonCondData/RpcCalibDataContainer.h"
#include "MuonCondSvc/RpcCoolStrSvc.h"

namespace MuonCalib {

  //actually the number strip hash id numbers.
  //Half are for potential upgrade.
  
  RpcCoolStrSvc::RpcCoolStrSvc(const std::string& name, ISvcLocator* svc) :
    AthService(name,svc),
    p_detstore(nullptr),
    m_folder("")
  {
    //declare properties
    declareProperty("Folder",m_folder);
  }
  
  const InterfaceID& RpcCoolStrSvc::type() const
  {
    return RpcICoolStrSvc::interfaceID();
  }
  
  StatusCode RpcCoolStrSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
  {
    if (RpcICoolStrSvc::interfaceID().versionMatch(riid)) {
      *ppvInterface=(RpcICoolStrSvc*)this;
    } else {
      return AthService::queryInterface(riid,ppvInterface);
    }
    return StatusCode::SUCCESS;
  }
  
  StatusCode RpcCoolStrSvc::initialize()
  {
    ATH_MSG_DEBUG("Initializing RpcCoolStrSvc");
    ATH_CHECK(service("DetectorStore",p_detstore));
    ATH_MSG_DEBUG("THIS CODE IS EXPERIMENTAL, NOT TO BE USED IN PRODUCTION");
    ATH_MSG_DEBUG("THIS CODE IS EXPERIMENTAL, NOT TO BE USED IN PRODUCTION");
    ATH_MSG_DEBUG("************** LOTS OF MEMORY LEAKS YET ***************");
    ATH_MSG_DEBUG("                 USE AT YOUR OWN RISK");
    ATH_MSG_DEBUG("                 USE AT YOUR OWN RISK");
    ATH_MSG_DEBUG("using folder " << m_folder);
    return StatusCode::SUCCESS;
  }

  StatusCode RpcCoolStrSvc::putOnlineFile(const std::string filename) const {

    // for the time being let's keep this 

    m_theOnlineEntries.clear();
    ATH_MSG_DEBUG("Opening the online mask file " << filename << " for entry into COOL database.");
    
    //open file
    std::ifstream in(filename.c_str());       
    if(!in.is_open())
      {
	ATH_MSG_ERROR("Can't open online mask file " << filename << "!");
	return StatusCode::FAILURE;
      }	

    std::string theLine;

    while (getline(in, theLine)) { // Reads all lines

      std::istringstream line_str;

      line_str.str(theLine);

      uint64_t mask1,mask2,mask3,id;

      line_str>>std::oct>>id>>std::hex>>mask1>>std::hex>>mask2>>std::hex>>mask3;

      const RpcOnlineDBEntry* newEntry=new RpcOnlineDBEntry(id, mask1,mask2,mask3);

      m_theOnlineEntries.push_back(newEntry);

    }
    
    ATH_MSG_DEBUG("Finished reading file, now writing to database "); 

    return writeToOnlineDB();
 
  }


  //RpcCoolStrSvc::putFile - Copies data from a calibration file to the database. Most of the substance
  //of it is just collecting data from the calibration file and sorting it to a more convenient format
  //for placing in the database. It uses putDbParameter() to actually put a parameter's values into a database.


  StatusCode RpcCoolStrSvc::putFile(const std::string filename) const 
  {
    

    m_theEntries.clear();
    ATH_MSG_DEBUG("Opening the calibration file " << filename << " for entry into COOL database.");
    
    //open file
    std::ifstream in(filename.c_str());       
    if(!in.is_open())
      {
	ATH_MSG_ERROR("Can't open calibration file " << filename << "!");
	return StatusCode::FAILURE;
      }	

    std::string theLine;

    while (getline(in, theLine)) { // Reads all lines

      int delimiter=theLine.find(";");
      Identifier gapID(atoi(theLine.substr(0,delimiter).c_str()));
      std::string payLoad=theLine.substr(delimiter+2,theLine.size()-delimiter-2);


      const RpcCalibDBEntry* newEntry=new RpcCalibDBEntry(gapID, payLoad);


      m_theEntries.push_back(newEntry);

    }

    ATH_MSG_DEBUG("Finished reading file, now writing to database "); 

    return writeToDB();
    
  }	


  StatusCode RpcCoolStrSvc::writeToOnlineDB() const{

    CondAttrListCollection* atrc=nullptr;
    if (!p_detstore->contains<CondAttrListCollection>(m_folder)) {
      ATH_MSG_DEBUG("Creating new CondAttrListCollection for folder "
	  << m_folder);
      CondAttrListCollection* atrc=new CondAttrListCollection(true);
      if (StatusCode::SUCCESS!=p_detstore->record(atrc,m_folder)) {
	ATH_MSG_ERROR("Could not create CondAttrListCollection " <<
	  m_folder);
	return StatusCode::FAILURE;
      }
    }
    

    // do const cast here so we can add to already exisiting collections
    const CondAttrListCollection* catrc=nullptr;
    ATH_MSG_DEBUG("Attempting to retrieve collection (const)");
    ATH_CHECK(p_detstore->retrieve(catrc,m_folder));

    atrc=const_cast<CondAttrListCollection*>(catrc);
    if (!atrc) {
      ATH_MSG_ERROR("Could not retrieve non-const pointer to atrc");
      return StatusCode::FAILURE;
    }
    
    ATH_MSG_DEBUG("About to create AttributeListSpecification");
    
    coral::AttributeListSpecification* aspec=nullptr;
    aspec=new coral::AttributeListSpecification();
    aspec->extend("Mask1","string");
    aspec->extend("Mask2","string");
    aspec->extend("Mask3","string");

    for(unsigned int k=0;k<m_theOnlineEntries.size();k++){

      std::string mask1,mask2,mask3;

      m_theOnlineEntries[k]->getColumns(mask1,mask2,mask3);
      
      AthenaAttributeList alist(*aspec);
      
      alist["Mask1"].setValue(mask1);
      alist["Mask2"].setValue(mask2);
      alist["Mask3"].setValue(mask3);
      

    CondAttrListCollection::ChanNum channum=m_theOnlineEntries[k]->getID();

    ATH_MSG_DEBUG("About to add channel to: " << atrc);
    atrc->add(channum,alist);

    }

    // now clear DB entries

    for(unsigned int k=0;k<m_theOnlineEntries.size();k++){
    
      delete m_theOnlineEntries[k];

    }

    return StatusCode::SUCCESS;


  }

  StatusCode RpcCoolStrSvc::writeToDB() const{
    
    // this writes the contents of theEntries in the db
    
    
    CondAttrListCollection* atrc=nullptr;
    if (!p_detstore->contains<CondAttrListCollection>(m_folder)) {
      ATH_MSG_DEBUG("Creating new CondAttrListCollection for folder "
	    << m_folder);
      CondAttrListCollection* atrc=new CondAttrListCollection(true);
      if (StatusCode::SUCCESS!=p_detstore->record(atrc,m_folder)) {
	ATH_MSG_ERROR("Could not create CondAttrListCollection " <<
	  m_folder);
	return StatusCode::FAILURE;
      }
    }
    
    // do const cast here so we can add to already exisiting collections
    const CondAttrListCollection* catrc=nullptr;
    ATH_MSG_DEBUG("Attempting to retrieve collection (const)");
    if (StatusCode::SUCCESS!=p_detstore->retrieve(catrc,m_folder)) {
      ATH_MSG_ERROR("Could not retrieve CondAttrListCollection " <<
	m_folder);
      return StatusCode::FAILURE;
    }
    atrc=const_cast<CondAttrListCollection*>(catrc);
    if (!atrc) {
      ATH_MSG_ERROR("Could not retrieve non-const pointer to atrc");
      return StatusCode::FAILURE;
    }
    
    ATH_MSG_DEBUG("About to create AttributeListSpecification");
    
    coral::AttributeListSpecification* aspec=0;
    aspec=new coral::AttributeListSpecification();
    aspec->extend("recEta","string");
    aspec->extend("detEta","string");
    aspec->extend("recPhi1","string");
    aspec->extend("recPhi2","string");
    aspec->extend("detPhi1","string");
    aspec->extend("detPhi2","string");
    

    for(unsigned int k=0;k<m_theEntries.size();k++){


      std::string recEta, detEta,recPhi1,recPhi2,detPhi1,detPhi2;


      m_theEntries[k]->getColumns(recEta,detEta,recPhi1,recPhi2,detPhi1,detPhi2);

      AthenaAttributeList alist(*aspec);
   
       alist["recEta"].setValue(recEta);
       alist["detEta"].setValue(detEta);
       alist["recPhi1"].setValue(recPhi1);
       alist["recPhi2"].setValue(recPhi2);
       alist["detPhi1"].setValue(detPhi1);
       alist["detPhi2"].setValue(detPhi2);
   
      //Changed by Caleb Lampen <lampen@physics.arizona.edu> on Aug 4, 2009. 
      CondAttrListCollection::ChanNum channum = (m_theEntries[k]->getGapID()).get_identifier32().get_compact();

      ATH_MSG_DEBUG("About to add channel to: " << atrc);
      atrc->add(channum,alist);
    }
    
    // now clear DB entries
    
    for(unsigned int k=0;k<m_theOnlineEntries.size();k++){
      
      delete m_theEntries[k];
      
    }
    
    return StatusCode::SUCCESS;
    
  }
  
  StatusCode RpcCoolStrSvc::makeOnlineFile(const std::string fileName) const{
    
        ATH_MSG_DEBUG("Opening online mask output file "<< fileName << " for writing.");
        std::ofstream out(fileName.c_str());
        if(!out.is_open())
        {
            ATH_MSG_ERROR("Failed opening " << fileName << "!");
            return StatusCode::FAILURE;
        }
        ATH_MSG_DEBUG("File is open");
	
	const CondAttrListCollection* atrc;
	if (StatusCode::SUCCESS!=p_detstore->retrieve(atrc,m_folder)) {
	  ATH_MSG_ERROR("can't find data for folder " << m_folder);
	  return StatusCode::FAILURE;
	}

	CondAttrListCollection::const_iterator itr=atrc->begin();

	for(;itr!=atrc->end();++itr){

	  const coral::AttributeList& atr=itr->second;
	  CondAttrListCollection::ChanNum ID= itr->first;

	  out<< std::oct<<ID<<std::dec<<" \t" <<atr["Mask1"].data<std::string>()<< "\t"
	     <<atr["Mask2"].data<std::string>()<< "\t"
	     <<atr["Mask3"].data<std::string>()<<std::endl;

	}

        out.close();

        return StatusCode::SUCCESS;	
  }





  /**Generate a calibration file*/
  StatusCode RpcCoolStrSvc::makeFile(const std::string fileName) const
  {

        ATH_MSG_DEBUG("Opening calibration output file "<< fileName << " for writing.");
        std::ofstream out(fileName.c_str());
        if(!out.is_open())
        {
            ATH_MSG_ERROR("Failed opening " << fileName << "!");
            return StatusCode::FAILURE;
        }
        ATH_MSG_DEBUG("File is open");
	
	const CondAttrListCollection* atrc;
	if (StatusCode::SUCCESS!=p_detstore->retrieve(atrc,m_folder)) {
	  ATH_MSG_ERROR("can't find data for folder " << m_folder);
	  return StatusCode::FAILURE;
	}

	CondAttrListCollection::const_iterator itr=atrc->begin();

	for(;itr!=atrc->end();++itr){

	  const coral::AttributeList& atr=itr->second;
	  CondAttrListCollection::ChanNum gapID= itr->first;

	  out<< gapID<<"; " <<atr["recEta"].data<std::string>()<< " END "<<atr["detEta"].data<std::string>()<< " END "
	     << atr["recPhi1"].data<std::string>()<< " END "<<atr["recPhi2"].data<std::string>()<< " END "
	     << atr["detPhi1"].data<std::string>()<< " END "<<atr["detPhi2"].data<std::string>()<< " END "<<std::endl;


	}
        out.close();
        return StatusCode::SUCCESS;	
    }		

} //end namespace
