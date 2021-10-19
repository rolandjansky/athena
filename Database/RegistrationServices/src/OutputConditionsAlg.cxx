/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// OutputConditionsAlg.cxx
// Algorithm to provide simple writing of conditions data to outputstream
// and optional registration in IOV database
// Richard Hawkings, started 1/9/05, from skeleton by Walter Lampl

#include "GaudiKernel/IClassIDSvc.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"
#include "RegistrationServices/IIOVRegistrationSvc.h"
#include "OutputConditionsAlg.h"


OutputConditionsAlg::OutputConditionsAlg(const std::string& name, 
					 ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{}


OutputConditionsAlg::~OutputConditionsAlg() 
{}


StatusCode OutputConditionsAlg::initialize() {
  ATH_MSG_DEBUG ("in initialize()");

  // get pointer to ClassIDSvc
  if (StatusCode::SUCCESS!= p_clidsvc.retrieve()) {
    ATH_MSG_FATAL ("ClassIDSvc not found");
    return StatusCode::FAILURE;
  }
  if (m_par_writeIOV) {
    // get pointer to IOVRegistrationSvc
    if (StatusCode::SUCCESS!=p_regsvc.retrieve()) {
      ATH_MSG_FATAL ("IOVRegistrationSvc not found");
      return StatusCode::FAILURE;
    }
  }
  m_streamer = IAthenaOutputStreamTool_t("AthenaOutputStreamTool/"+
					 m_streamName);
  StatusCode sc = m_streamer.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR ("Unable to find AthenaOutputStreamTool with name " << 
                   m_streamName);
    return StatusCode::FAILURE;
  }  
  return StatusCode::SUCCESS;
}


StatusCode OutputConditionsAlg::execute() {

  return StatusCode::SUCCESS;
}

StatusCode OutputConditionsAlg::finalize() {
  ATH_MSG_INFO ("Finalize: preparing to write conditions objects ");

  StatusCode sc = m_streamer->connectOutput();
  if (sc.isFailure()) {
    ATH_MSG_ERROR ("Could not connect stream to output");
    return( StatusCode::FAILURE);
  }
  // create list of objects
  std::vector<std::string> types;
  std::vector<std::string> keys;
  std::vector<std::string> folders;
  std::vector<std::string> tags;
  for (unsigned int iobj=0;iobj<m_objectList.size();++iobj) {
    // if object name contains a '#', it represents a specific typename#key
    std::string::size_type ihash=m_objectList[iobj].find_first_of("#");
    if (ihash==std::string::npos) {
      // no hash, get the default object for this class, and set key
      // note this will only get ONE object
      // first need to know CLID from typename
      CLID clid;
      if (StatusCode::SUCCESS==p_clidsvc->getIDOfTypeName(
	       m_objectList[iobj],clid)) {
	SG::DataProxy* proxy=detStore()->proxy(clid);
	if (proxy) {
	  types.push_back(m_objectList[iobj]);
	  keys.push_back(proxy->name());
	  folders.push_back(proxy->name());
	  if (iobj<m_par_iovtags.size()) {
	    tags.push_back(m_par_iovtags[iobj]);
	  } else {
	    tags.push_back("");
	  }
	} else {
          ATH_MSG_ERROR ("Could not get default proxy for CLID " <<
                         clid << " typename " << m_objectList[iobj]);
	}
      } else {
        ATH_MSG_ERROR ("Could not get CLID from typename " <<
                       m_objectList[iobj]);
      }
    } else {
      types.push_back(m_objectList[iobj].substr(0,ihash));
      // a second hash allows the folder to be specified after the key
      // otherwise the folder name is taken to be the same as the key name
      std::string::size_type ihash2=m_objectList[iobj].find_first_of("#",ihash+1);
      if (ihash2==std::string::npos) {
	std::string key=m_objectList[iobj].substr(ihash+1,std::string::npos);
	keys.push_back(key);
	folders.push_back(key);
      } else {
	keys.push_back(m_objectList[iobj].substr(ihash+1,ihash2-ihash-1));
	folders.push_back(m_objectList[iobj].substr(ihash2+1,
						    std::string::npos));
      }
      if (iobj<m_par_iovtags.size()) {
        tags.push_back(m_par_iovtags[iobj]);
      } else {
        tags.push_back("");
      }
    }
  }
  // list out all typename/key pairs to be written and construct vector
  int nObjects=types.size();
  IAthenaOutputStreamTool::TypeKeyPairs typeKeys(nObjects);
  ATH_MSG_INFO ("Identified a total of " << nObjects << 
                " objects to write out:");
  // leave now if nothing to write
  if (nObjects==0) return StatusCode::SUCCESS;
  for (int i=0;i<nObjects;i++) {
    typeKeys[i]=IAthenaOutputStreamTool::TypeKeyPair(types[i],keys[i]);
    ATH_MSG_INFO (i << ": " << types[i] << "#" << keys[i] << 
                  "#" << folders[i]);
    // check object actually exists, else return failure
    CLID clid;
    SG::DataProxy* proxy=0;
    if (StatusCode::SUCCESS==p_clidsvc->getIDOfTypeName(types[i],clid))
      proxy=detStore()->proxy(clid,keys[i]);
    if (proxy==0) {
      ATH_MSG_ERROR ("Could not find proxy for object with key " <<
                     keys[i] << " - abort write");
      return StatusCode::FAILURE;
    }
  }
    
  // stream output (write objects)
  sc=m_streamer->streamObjects(typeKeys);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ("Could not stream out objects");
    return StatusCode::FAILURE;
  }
  // commit output
  sc=m_streamer->commitOutput();
  if (sc.isFailure()) {
    ATH_MSG_ERROR ("Could not commit output stream");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO ("Written " << nObjects << " objects to output stream");

  if (m_par_writeIOV) {
    msg() << MSG::INFO <<         
      "Register objects in IOV database, interval of validity ";
    if (m_par_timestamp) {
      msg() << "[time] from [" << 
	m_par_time1.value() << "] to [" << m_par_time2.value() << "]" << endmsg;
    } else {
      msg() << "[run,LB] from [" <<
	m_par_run1.value() << "," << m_par_lumib1.value() << "] to [" << m_par_run2.value() <<
	"," << m_par_lumib2.value() << "]" << endmsg;
    }
    int nreg=0;
    for (int iobj=0;iobj<nObjects;++iobj) {
      msg() << MSG::INFO << "Register object " << types[iobj] << "#" << 
	keys[iobj] << " in IOV database folder " << folders[iobj] << " ";
      if (tags[iobj]=="") {
	msg() << MSG::INFO << "without tagging" << endmsg;
      } else {
	msg() << MSG::INFO << "with tag " << tags[iobj] << endmsg;
      }
      if (m_par_timestamp) {
	sc=p_regsvc->registerIOV(types[iobj],keys[iobj],
         folders[iobj],tags[iobj],timeToNano(m_par_time1),timeToNano(m_par_time2));
      } else {
	sc=p_regsvc->registerIOV(types[iobj],keys[iobj],
         folders[iobj],tags[iobj],m_par_run1,m_par_run2,m_par_lumib1,m_par_lumib2);
      }
      if (sc==StatusCode::SUCCESS) {
	++nreg;
      } else {
        ATH_MSG_ERROR ("Registration failed!");
      }
    }
    ATH_MSG_INFO ("Registered " << nreg << " objects in IOV database");
  } else {
    ATH_MSG_INFO ("Objects NOT registered in IOV database");
  }
  return StatusCode::SUCCESS;
}

uint64_t OutputConditionsAlg::timeToNano(unsigned long int timesec) const
{
  // convert time specified in seconds to ns used by COOL
  // use the magic value MAXEVENT to signal full range
  if (timesec==IOVTime::MAXEVENT) {
    return IOVTime::MAXTIMESTAMP;
  } else {
    return static_cast<uint64_t>(timesec)*1000000000;
  }
}
