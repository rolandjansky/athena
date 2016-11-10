/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************************8
//******************************************************************************************
//RpcCoolStrSvc.cxx
//******************************************************************************************
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
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "MuonIdHelpers/RpcIdHelper.h"

// temporary includes to access CLOBs
//#include "CoolKernel/ExtendedAttributeListSpecification.h"
//#include "CoolKernel/PredefinedStorageHints.h"

//Calibration data containers
#include "MuonCondData/RpcCalibData.h"
#include "MuonCondData/RpcCalibDBEntry.h"
#include "MuonCondData/RpcCalibDataContainer.h"


// root class for string manipulation 
//#include "TString.h"

#include "MuonCondSvc/RpcCoolStrSvc.h"

using namespace std;
namespace MuonCalib {

  //actually the number strip hash id numbers.
  //Half are for potential upgrade.
  
  RpcCoolStrSvc::RpcCoolStrSvc(const string& name, ISvcLocator* svc) :
    AthService(name,svc),
    p_detstore(0),
    m_rpcId(0),
    m_log(msgSvc(),name),  
    m_folder(""),   
    m_debugLevel(false)
  {
    //declare properties
    declareProperty("Folder",m_folder);
  }
  
  RpcCoolStrSvc::~RpcCoolStrSvc() {}
  
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
    // service initialisation 
    m_log.setLevel(outputLevel());    //individual outputlevel not known before inialize
    m_debugLevel = (m_log.level() <= MSG::DEBUG);
    
    m_log << MSG::INFO << "Initializing RpcCoolStrSvc" <<endmsg;
    
    // get detector store, linked to cool database by other algorithms in your
    // jobOptions file.
    if (StatusCode::SUCCESS!=service("DetectorStore",p_detstore)) {
      m_log << MSG::FATAL << "Detector store not found" << endmsg; 
      return StatusCode::FAILURE;
    }
    
    
    m_log << MSG::ERROR << "THIS CODE IS EXPERIMENTAL, NOT TO BE USED IN PRODUCTION" << endmsg;
    m_log << MSG::ERROR << "THIS CODE IS EXPERIMENTAL, NOT TO BE USED IN PRODUCTION" << endmsg;
    m_log << MSG::ERROR << "************** LOTS OF MEMORY LEAKS YET ***************" << endmsg;
    m_log << MSG::ERROR << "                 USE AT YOUR OWN RISK" << endmsg;
    m_log << MSG::ERROR << "                 USE AT YOUR OWN RISK" << endmsg;

    m_log << MSG::INFO << "using folder " << m_folder<<endmsg;

    //Setup RpcIdHelper
    StoreGateSvc* detStore= 0;
    StatusCode sc = serviceLocator()->service("DetectorStore",detStore);
    
    if(sc.isSuccess())
      {
	sc = detStore->retrieve(m_rpcId,"RPCIDHELPER");
	if(sc.isFailure())
	  {
	    m_log << MSG::ERROR << "Cannot retrieve RpcIdHelper from detector store" << endmsg;
	    return sc;
	  }
      }
    else
      {
	m_log << MSG::ERROR << "MuonDetDescrMgr not found in DetectorStore " << endmsg;
	return sc;
      } 

    return StatusCode::SUCCESS;
  }
  
  StatusCode RpcCoolStrSvc::finalize()
  {
    m_log << MSG::DEBUG << "in finalize()" << endmsg;
    return StatusCode::SUCCESS;
  }
  

  StatusCode RpcCoolStrSvc::putOnlineFile(const std::string filename) const {

    // for the time being let's keep this 

    m_theOnlineEntries.clear();
    m_log << MSG::INFO << "Opening the online mask file " << filename << " for entry into COOL database." << endmsg;
    
    //open file
    ifstream in(filename.c_str());       
    if(!in.is_open())
      {
	m_log << MSG::ERROR << "Can't open online mask file " << filename << "!" << endmsg;
	return StatusCode::FAILURE;
      }	

    string theLine;

    while (getline(in, theLine)) { // Reads all lines

      istringstream line_str;

      line_str.str(theLine);

      uint64_t mask1,mask2,mask3,id;

      line_str>>std::oct>>id>>std::hex>>mask1>>std::hex>>mask2>>std::hex>>mask3;

      const RpcOnlineDBEntry* newEntry=new RpcOnlineDBEntry(id, mask1,mask2,mask3);

      std::cout<<" created entry with id "<<std::oct<<id<<std::dec<<" " <<id<<std::endl;

      m_theOnlineEntries.push_back(newEntry);

    }
    
    m_log << MSG::DEBUG << "Finished reading file, now writing to database " << endmsg; 

    return writeToOnlineDB();
 
  }


  //RpcCoolStrSvc::putFile - Copies data from a calibration file to the database. Most of the substance
  //of it is just collecting data from the calibration file and sorting it to a more convenient format
  //for placing in the database. It uses putDbParameter() to actually put a parameter's values into a database.


  StatusCode RpcCoolStrSvc::putFile(const std::string filename) const 
  {
    

    m_theEntries.clear();
    m_log << MSG::INFO << "Opening the calibration file " << filename << " for entry into COOL database." << endmsg;
    
    //open file
    ifstream in(filename.c_str());       
    if(!in.is_open())
      {
	m_log << MSG::ERROR << "Can't open calibration file " << filename << "!" << endmsg;
	return StatusCode::FAILURE;
      }	

    string theLine;

    while (getline(in, theLine)) { // Reads all lines

      //      std::cout<<" new line "<<theLine<< "*** "<<std::endl;

      int delimiter=theLine.find(";");
      Identifier gapID(atoi(theLine.substr(0,delimiter).c_str()));
      string payLoad=theLine.substr(delimiter+2,theLine.size()-delimiter-2);


      const RpcCalibDBEntry* newEntry=new RpcCalibDBEntry(gapID, payLoad);


      m_theEntries.push_back(newEntry);

    }

    m_log << MSG::INFO << "Finished reading file, now writing to database " << endmsg; 

    return writeToDB();
    
  }	


  StatusCode RpcCoolStrSvc::writeToOnlineDB() const{

    //    std::cout<<"folder is "<<m_folder<<std::endl;

    CondAttrListCollection* atrc=0;
    if (!p_detstore->contains<CondAttrListCollection>(m_folder)) {
      m_log << MSG::DEBUG << "Creating new CondAttrListCollection for folder "
	  << m_folder << endmsg;
      CondAttrListCollection* atrc=new CondAttrListCollection(true);
      if (StatusCode::SUCCESS!=p_detstore->record(atrc,m_folder)) {
	m_log << MSG::ERROR << "Could not create CondAttrListCollection " <<
	  m_folder << endmsg;
	return StatusCode::FAILURE;
      }
    }
    

    // do const cast here so we can add to already exisiting collections
    const CondAttrListCollection* catrc=0;
    m_log << MSG::DEBUG << "Attempting to retrieve collection (const)" << endmsg;
    if (StatusCode::SUCCESS!=p_detstore->retrieve(catrc,m_folder)) {
      m_log << MSG::ERROR << "Could not retrieve CondAttrListCollection " <<
	m_folder << endmsg;
      return StatusCode::FAILURE;
    }


    atrc=const_cast<CondAttrListCollection*>(catrc);
    if (atrc==0) {
      m_log << MSG::ERROR << "Could not retrieve non-const pointer to atrc" <<
	endmsg;
      return StatusCode::FAILURE;
    }
    
    m_log << MSG::DEBUG << "About to create AttributeListSpecification" << endmsg;
    
    coral::AttributeListSpecification* aspec=0;
    aspec=new coral::AttributeListSpecification();
    aspec->extend("Mask1","string");
    aspec->extend("Mask2","string");
    aspec->extend("Mask3","string");

    //    std::cout<<" *********** about to loop on "<<m_theOnlineEntries.size()<<" entries"<<std::endl;


    for(unsigned int k=0;k<m_theOnlineEntries.size();k++){

      std::cout<<"                k is "<<k<<std::endl;

      string mask1,mask2,mask3;

      m_theOnlineEntries[k]->getColumns(mask1,mask2,mask3);
      
      //      std::cout<<" going to write these values "<<mask1<< " " <<mask2<< " "<<mask3<<std::endl;

      AthenaAttributeList alist(*aspec);
      
      alist["Mask1"].setValue(mask1);
      alist["Mask2"].setValue(mask2);
      alist["Mask3"].setValue(mask3);
      

    CondAttrListCollection::ChanNum channum=m_theOnlineEntries[k]->getID();

    //    std::cout<<"****** "<<std::oct<<channum<< " " <<m_theOnlineEntries[k]->getID()<<std::dec<<std::endl;

    m_log << MSG::DEBUG << "About to add channel to: " << atrc << endmsg;
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
    
    
    CondAttrListCollection* atrc=0;
    if (!p_detstore->contains<CondAttrListCollection>(m_folder)) {
      m_log << MSG::DEBUG << "Creating new CondAttrListCollection for folder "
	    << m_folder << endmsg;
      CondAttrListCollection* atrc=new CondAttrListCollection(true);
      if (StatusCode::SUCCESS!=p_detstore->record(atrc,m_folder)) {
	m_log << MSG::ERROR << "Could not create CondAttrListCollection " <<
	  m_folder << endmsg;
	return StatusCode::FAILURE;
      }
    }
    
    // do const cast here so we can add to already exisiting collections
    const CondAttrListCollection* catrc=0;
    m_log << MSG::DEBUG << "Attempting to retrieve collection (const)" << endmsg;
    if (StatusCode::SUCCESS!=p_detstore->retrieve(catrc,m_folder)) {
      m_log << MSG::ERROR << "Could not retrieve CondAttrListCollection " <<
	m_folder << endmsg;
      return StatusCode::FAILURE;
    }
    atrc=const_cast<CondAttrListCollection*>(catrc);
    if (atrc==0) {
      m_log << MSG::ERROR << "Could not retrieve non-const pointer to atrc" <<
	endmsg;
      return StatusCode::FAILURE;
    }
    
    m_log << MSG::DEBUG << "About to create AttributeListSpecification" << endmsg;
    
    coral::AttributeListSpecification* aspec=0;
    aspec=new coral::AttributeListSpecification();
    aspec->extend("recEta","string");
    aspec->extend("detEta","string");
    aspec->extend("recPhi1","string");
    aspec->extend("recPhi2","string");
    aspec->extend("detPhi1","string");
    aspec->extend("detPhi2","string");
    

    for(unsigned int k=0;k<m_theEntries.size();k++){


      string recEta, detEta,recPhi1,recPhi2,detPhi1,detPhi2;


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
      //   std::cout << "About to add channel " << channum << " "<< m_theEntries[k]->getGapID()<< " "<<std::endl;

      m_log << MSG::DEBUG << "About to add channel to: " << atrc << endmsg;
      atrc->add(channum,alist);
    }
    
    // now clear DB entries
    
    for(unsigned int k=0;k<m_theOnlineEntries.size();k++){
      
      delete m_theEntries[k];
      
    }
    
    return StatusCode::SUCCESS;
    
  }
  
  StatusCode RpcCoolStrSvc::makeOnlineFile(const string fileName) const{
    
        m_log << MSG::INFO << "Opening online mask output file "<< fileName << " for writing." << endmsg;
        ofstream out(fileName.c_str());
        if(!out.is_open())
        {
            m_log << MSG::ERROR << "Failed opening " << fileName << "!" << endmsg;
            return StatusCode::FAILURE;
        }
        m_log << MSG::DEBUG <<"File is open" << endmsg;
	
	const CondAttrListCollection* atrc;
	if (StatusCode::SUCCESS!=p_detstore->retrieve(atrc,m_folder)) {
	  m_log << MSG::ERROR << "can't find data for folder " << m_folder << endmsg;
	  return StatusCode::FAILURE;
	}


	//	std::cout<<"found collection with size "<<atrc->size()<<std::endl;

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
  StatusCode RpcCoolStrSvc::makeFile(const string fileName) const
  {

        m_log << MSG::INFO << "Opening calibration output file "<< fileName << " for writing." << endmsg;
        ofstream out(fileName.c_str());
        if(!out.is_open())
        {
            m_log << MSG::ERROR << "Failed opening " << fileName << "!" << endmsg;
            return StatusCode::FAILURE;
        }
        m_log << MSG::DEBUG <<"File is open" << endmsg;
	
	const CondAttrListCollection* atrc;
	if (StatusCode::SUCCESS!=p_detstore->retrieve(atrc,m_folder)) {
	  m_log << MSG::ERROR << "can't find data for folder " << m_folder << endmsg;
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
