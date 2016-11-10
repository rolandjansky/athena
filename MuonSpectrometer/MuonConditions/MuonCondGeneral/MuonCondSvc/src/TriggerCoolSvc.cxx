/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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

// root class for string manipulation 
#include "TString.h"

#include "MuonCondSvc/TriggerCoolSvc.h"

using namespace std;

namespace MuonCalib{

TriggerCoolSvc::TriggerCoolSvc(const string& name, ISvcLocator* svc) :
  AthService(name,svc),
  p_detstore(0),
  m_log(msgSvc(),name),  
  m_etafolder("/RPC/TRIGGER/CM_THR_ETA"),
  m_phifolder("/RPC/TRIGGER/CM_THR_PHI"),
  m_debugLevel(false)
{
  //declare properties
  declareProperty("EtaFolder",m_etafolder);
  declareProperty("PhiFolder",m_phifolder);
}

TriggerCoolSvc::~TriggerCoolSvc() {}

const InterfaceID& TriggerCoolSvc::type() const
{
  return ITriggerCoolSvc::interfaceID();
}
  
StatusCode TriggerCoolSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if (ITriggerCoolSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface=(ITriggerCoolSvc*)this;
  } else {
    return AthService::queryInterface(riid,ppvInterface);
  }
  return StatusCode::SUCCESS;
}

StatusCode TriggerCoolSvc::initialize()
{
  // service initialisation 
  m_log.setLevel(outputLevel());    //individual outputlevel not known before inialize
  m_debugLevel = (m_log.level() <= MSG::DEBUG);
  
  m_log << MSG::INFO << "Initializing TriggerCoolSvc" <<endmsg;
  
  // get detector store, linked to cool database by other algorithms in your
  // jobOptions file.
  if (StatusCode::SUCCESS!=service("DetectorStore",p_detstore)) {
    m_log << MSG::FATAL << "Detector store not found" << endmsg; 
    return StatusCode::FAILURE;
  }
  
  m_log << MSG::INFO << "using folder phi ******************** " << m_phifolder<<endmsg;
  m_log << MSG::INFO << "using folder eta ********************" << m_etafolder<<endmsg;
  

  // StoreGateSvc* detStore= 0;
  //StatusCode sc = serviceLocator()->service("DetectorStore",detStore);
  
 
  
  return StatusCode::SUCCESS;
}
  
StatusCode TriggerCoolSvc::finalize()
{
  m_log << MSG::DEBUG << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}




StatusCode TriggerCoolSvc::writeToDBEta(const std::string& etafolder, const std::string& filename_CM,const std::string& filename_Th0,
					const int chan, const std::string& sequence,const std::string& info) const{
  
  // this writes the contents of theEntries in the db
  m_log << MSG::INFO << "using folder eta ********************" << m_phifolder<<endmsg;

  // reading files to store in string
 
  std::string sdata_Th0_new;
  //std::string sdata_Th1_new;
  //std::string sdata_Th2_new;

  FILE* f = fopen (filename_Th0.c_str(),"rb");
  if (f != NULL)   {
    fseek (f, 0L, SEEK_END);
    unsigned int size = ftell (f);
    fseek (f, 0L, SEEK_SET);
    m_log << MSG::INFO << "Input Th0 file size is " << size << endmsg;

    std::vector<char> sbuf(size);

    if (sbuf.size()>= size) {
       size_t readbytes = 0;
       readbytes = fread(&sbuf[0],size,1,f);
       if(readbytes == 0)
         m_log << MSG::ERROR << "Empty file read!" << size << endmsg;
    }
    
    fclose (f);
    
    std::string sdata_Th0(sbuf.begin(),sbuf.begin()+size);
    
    sdata_Th0_new=sdata_Th0;
    
  }
  /*
  FILE* f1 = fopen (filename_Th1.c_str(),"rb");
  if (f1 != NULL)   {
    fseek (f1, 0L, SEEK_END);
    int size = ftell (f1);
    fseek (f1, 0L, SEEK_SET);
    m_log << MSG::INFO << "Input Th1 file size is " << size << endmsg;
    std::vector<char> sbuf(size);
    fread(&sbuf[0],size,1,f1);
    fclose (f1);
    
    std::string sdata_Th1(sbuf.begin(),sbuf.begin()+size);
    
    sdata_Th1_new=sdata_Th1;
    
  }

  FILE* f2 = fopen (filename_Th2.c_str(),"rb");
  if (f2 != NULL)   {
    fseek (f2, 0L, SEEK_END);
    int size = ftell (f2);
    fseek (f2, 0L, SEEK_SET);
    m_log << MSG::INFO << "Input Th2 file size is " << size << endmsg;
    std::vector<char> sbuf(size);
    fread(&sbuf[0],size,1,f2);
    fclose (f2);
    
    std::string sdata_Th2(sbuf.begin(),sbuf.begin()+size);
    
    sdata_Th2_new=sdata_Th2;
    
  }
  */

  
  CondAttrListCollection* atrc=0;
  if (!p_detstore->contains<CondAttrListCollection>(etafolder)) {
    m_log << MSG::DEBUG << "Creating new CondAttrListCollection for folder "
	  << etafolder << endmsg;
    CondAttrListCollection* atrc=new CondAttrListCollection(true);
    if (StatusCode::SUCCESS!=p_detstore->record(atrc,etafolder)) {
      m_log << MSG::ERROR << "Could not create CondAttrListCollection " <<
	etafolder << endmsg;
      return StatusCode::FAILURE;
    }
  }
  
  // do const cast here so we can add to already exisiting collections
  const CondAttrListCollection* catrc=0;
  m_log << MSG::DEBUG << "Attempting to retrieve collection (const)" << endmsg;
  if (StatusCode::SUCCESS!=p_detstore->retrieve(catrc,etafolder)) {
    m_log << MSG::ERROR << "Could not retrieve CondAttrListCollection " <<
      etafolder << endmsg;
    return StatusCode::FAILURE;
  }
  atrc=const_cast<CondAttrListCollection*>(catrc);
  if (atrc==0) {
    m_log << MSG::ERROR << "Could not retrieve non-const pointer to atrc" <<
      endmsg;
    return StatusCode::FAILURE;
  }

  // check channel zero is not being requested
  if (chan==0) {
    m_log << MSG::ERROR << "Channel 0 cannot be used" << endmsg;
    return StatusCode::FAILURE;
  }
  
  m_log << MSG::DEBUG << "About to create AttributeListSpecification" << endmsg;
  
  coral::AttributeListSpecification* aspec=0;
  aspec=new coral::AttributeListSpecification();
 
  aspec->extend("CM_File","string");
  aspec->extend("Th0","string");
  // aspec->extend("Th1","string");
  //aspec->extend("Th2","string");
  aspec->extend("Sequence_Th","string");
  aspec->extend("Additional_Info","string");
 
  AthenaAttributeList alist(*aspec);
  alist["CM_File"].setValue(filename_CM);
  alist["Th0"].setValue(sdata_Th0_new);
  //alist["Th1"].setValue(sdata_Th1_new);
  //alist["Th2"].setValue(sdata_Th2_new);
  alist["Sequence_Th"].setValue(sequence);
  alist["Additional_Info"].setValue(info);
  CondAttrListCollection::ChanNum channum=chan;

  std::ostringstream atrstring;
  alist.print(atrstring);
  m_log << MSG::DEBUG << "About to add channel to: " << atrc << endmsg;
  atrc->add(channum,alist);
  m_log << MSG::DEBUG << " craetion folder" << endmsg;
  return StatusCode::SUCCESS;

  
}



StatusCode TriggerCoolSvc::writeToDBPhi(const std::string& phifolder, const std::string& filename_CM,const std::string& filename_Th0,
				  const int chan,const std::string& info) const{
  
  std::string sdata_Th0_new;
    
  FILE* f = fopen (filename_Th0.c_str(),"rb");
  if (f != NULL)   {
    fseek (f, 0L, SEEK_END);
    int size = ftell (f);
    fseek (f, 0L, SEEK_SET);
    m_log << MSG::INFO << "Input file size is " << size << endmsg;
    std::vector<char> sbuf(size);
    size_t readbytes = 0;
    readbytes = fread(&sbuf[0],size,1,f);
    if(readbytes == 0)
      m_log << MSG::ERROR << "Empty file read!" << size << endmsg;
    fclose (f);
    
    std::string sdata_Th0(sbuf.begin(),sbuf.begin()+size);
    
    sdata_Th0_new=sdata_Th0;
    
  }
 
  //m_log << MSG::DEBUG << " Data++++++" << sdata_Th0_new<<endmsg;
  //std::cout<< "dentro writeToDBPhi ************************88 " <<std::endl;
  //m_log << MSG::DEBUG << " folder" << phifolder<<endmsg;
  //m_log << MSG::DEBUG << " filename " << filename_CM<<endmsg;
  CondAttrListCollection* atrc=0;
  if (!p_detstore->contains<CondAttrListCollection>(phifolder)) {
    m_log << MSG::DEBUG << "Creating new CondAttrListCollection for folder "
	  << phifolder << endmsg;
    CondAttrListCollection* atrc=new CondAttrListCollection(true);
    if (StatusCode::SUCCESS!=p_detstore->record(atrc,phifolder)) {
      m_log << MSG::ERROR << "Could not create CondAttrListCollection " <<
	phifolder << endmsg;
      return StatusCode::FAILURE;
    }
  }
  
  // do const cast here so we can add to already exisiting collections
  const CondAttrListCollection* catrc=0;
  m_log << MSG::DEBUG << "Attempting to retrieve collection (const)" << endmsg;
  if (StatusCode::SUCCESS!=p_detstore->retrieve(catrc,phifolder)) {
    m_log << MSG::ERROR << "Could not retrieve CondAttrListCollection " <<
      phifolder << endmsg;
    return StatusCode::FAILURE;
  }
  atrc=const_cast<CondAttrListCollection*>(catrc);
  if (atrc==0) {
    m_log << MSG::ERROR << "Could not retrieve non-const pointer to atrc" <<
      endmsg;
    return StatusCode::FAILURE;
  }

  // check channel zero is not being requested
  if (chan==0) {
    m_log << MSG::ERROR << "Channel 0 cannot be used" << endmsg;
    return StatusCode::FAILURE;
  }
  
  m_log << MSG::DEBUG << "About to create AttributeListSpecification" << endmsg;
  
  coral::AttributeListSpecification* aspec=0;
  aspec=new coral::AttributeListSpecification();
 
  aspec->extend("CM_File","string");
  aspec->extend("Th0","string");
  aspec->extend("Additional_Info","string");
  
  //std::cout<< "file ************************ " <<sdata_Th0_new<<std::endl;
  AthenaAttributeList alist(*aspec);
  alist["CM_File"].setValue(filename_CM);
  alist["Th0"].setValue(sdata_Th0_new);
  alist["Additional_Info"].setValue(info);
  CondAttrListCollection::ChanNum channum=chan;

  std::ostringstream atrstring;
  alist.print(atrstring);
  m_log << MSG::DEBUG << "About to add channel to: " << atrc << endmsg;
  std::cout<< "channel ************************ " <<chan<<std::endl;
  atrc->add(channum,alist);
  m_log << MSG::DEBUG << " craetion folder" << endmsg;
  return StatusCode::SUCCESS;

  
}

StatusCode TriggerCoolSvc::getData(const std::string& phifolder, 
	    const int chan) const {
  MsgStream log(msgSvc(),name());
  //m_log << MSG::INFO << "using folder phi dentro read ******************** " << phifolder<<endmsg;
  const CondAttrListCollection* atrc;
  std::string file="";
  std::string th0="";
  std::string info="";
  if (StatusCode::SUCCESS!=p_detstore->retrieve(atrc,phifolder)) {
    log << MSG::ERROR << "getData failed for folder " << phifolder << " channel "
	<< chan << endmsg;
    return StatusCode::FAILURE;
  }
  CondAttrListCollection::ChanNum channum=chan;
  CondAttrListCollection::const_iterator itr=atrc->chanAttrListPair(channum);
  if (itr!=atrc->end()) {
    const coral::AttributeList& atr=itr->second;
    file=atr["CM_File"].data<std::string>();
    th0=atr["Th0"].data<std::string>();
    info=atr["Additional_Info"].data<std::string>();
    
    if (log.level() < MSG::INFO) {
      std::ostringstream atrstring;
      atr.toOutputStream(atrstring);
      log << MSG::DEBUG << "read Channum " << channum << " atrlist: " << 
	atrstring.str() << endmsg;
    } else {
      log << MSG::ERROR << "Invalid channel number" << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}



}
