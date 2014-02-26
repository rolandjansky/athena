/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSerializeTP/TrigSerTPTool.h"

//TP
#include "AthenaPoolCnvSvc/ITPConverter.h"
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPCnvBase.h"

#include "AthenaKernel/getMessageSvc.h"
#include "AthenaKernel/IDictLoaderSvc.h"
#include "AthenaKernel/ITPCnvSvc.h"
#include "AthenaKernel/ITPCnvBase.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/System.h"

#include "DataModel/Arena.h"

//ROOT
#include "TROOT.h"
#include "TClass.h"
#include "TBaseClass.h"

#include <iostream>
#include <algorithm>

#include "Reflex/Reflex.h"
using ROOT::Reflex::Type;
using std::map;
using std::string;

TrigSerTPTool::TrigSerTPTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent) :
  AlgTool(type,name,parent),
  m_log(0),
  m_logTP(0),
  m_tpCnvBaseClass(0),
  m_msgsvcTP("",name),
  m_dictSvc("AthDictLoaderSvc",name),
  m_useAthDictLoader(true),
  m_online(false),
  m_tpcnvsvc("AthTPCnvSvc", name)
{

  declareInterface<TrigSerTPTool>(this);
  declareProperty("TPMap", m_TPmap, "map of T->P classes" );
  declareProperty("ActiveClasses", m_activeClasses, "allows to skip classes not relevant in the current trigger level");
  declareProperty("debuggingMsgSvc", m_msgsvcTP, "an instance of the messageSvc for debugging purposes");
  declareProperty("onlineMode", m_online, "simpler setup for running online");
  declareProperty("useAthDictLoader", m_useAthDictLoader, "use AthDictLoaderSvc instead of plain ROOT");
  m_log = new MsgStream(msgSvc(), name);

}

TrigSerTPTool::~TrigSerTPTool(){
  if (m_logTP!=m_log)
    delete m_logTP;
  delete m_log;
}


TClass *TrigSerTPTool::getClass(const std::string &cname) const {

  TClass *cl(0);
  if (m_useAthDictLoader && m_dictSvc){
    m_dictSvc->load_type(cname);
  }
  cl = gROOT->GetClass(cname.c_str());
  return cl;
}

StatusCode TrigSerTPTool::initialize(){

  //  std::cout << "TrigSerTPTool::initialize: " << name() << ":" << msgSvc()->outputLevel(name()) << std::endl;
  m_log->setLevel(msgSvc()->outputLevel(name()));
  *m_log << MSG::INFO << "in initialize " << name() << endreq;

  //a special instance of the MessageSvc for the T/P
  m_logTP = m_log;

  if(!m_msgsvcTP.empty()){
    if (m_msgsvcTP.retrieve().isFailure()){
      *m_log << MSG::INFO << "Could not retrieve a separate MsgSvc for the T/P debugging" << endreq;
    } else {
      *m_log << MSG::DEBUG << "Using " << m_msgsvcTP << " for debugging " << endreq;
      IMessageSvc* msvc = m_msgsvcTP.operator->();
      m_logTP = new MsgStream(msvc, "TrigSerTPTool-T/P");
    }
  }

  if (!m_dictSvc.retrieve().isSuccess()) {
    *m_log << MSG::ERROR << "could not retrieve athena dict. loader service !" << endreq;
    return StatusCode::FAILURE;
  }

  if (!m_tpcnvsvc.retrieve().isSuccess()) {
    *m_log << MSG::ERROR << "could not retrieve T/P cnv svc" << endmsg;
    return StatusCode::FAILURE;
  }

  m_tpCnvBaseClass = getClass ("ITPCnvBase");

  if (!m_tpCnvBaseClass ) {
    *m_log << MSG::DEBUG << "Cannot find class ITPCnvBase" << endreq;
  } else {
    *m_log << MSG::DEBUG << "Found class ITPCnvBase" << endreq;
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigSerTPTool::finalize(){
  
  map <void*,TClass*>::const_iterator mit(m_convObj.begin());
  map <void*,TClass*>::const_iterator mend(m_convObj.end());

  while (mit!=mend){
    void *p = (*mit).first;
    TClass *cl = (*mit).second;
    if (cl && p){
      //      *m_log << MSG::DEBUG << "cl=" << cl << " p=" << p << endreq;
      cl->Destructor(p);
    }
    mit++;
  }
  m_convObj.clear();

  return StatusCode::SUCCESS;
}

StatusCode TrigSerTPTool::forceDictLoading(const std::string &clname) const {
  TClass *cl = getClass(clname);
  if (!cl){
    *m_log << MSG::DEBUG << "Dict loading of " << clname << " failed" << endreq;
    return StatusCode::FAILURE;
  } else {
    void *p = cl->New();
    cl->Destructor(p);
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigSerTPTool::testTPCnv(const std::string &clname) {
  TClass *cl = getClass(clname);
  if (!cl){
    *m_log << MSG::DEBUG << "Dict loading of " << clname << " failed" << endreq;
    return StatusCode::FAILURE;
  } else {
    void *p = cl->New();
    if (p){
      std::string persname;
      void *pers = convertTP(clname, p, persname);
      //cleanup
      TClass *pcl = getClass(persname);
      if (pcl)
	pcl->Destructor(pers);
    }
    cl->Destructor(p);
  }
  return StatusCode::SUCCESS;
}

void*  TrigSerTPTool::convertTP(const std::string &clname, void *ptr, std::string &persName){
  const int loglvl = m_log->level();

  if (loglvl<=MSG::DEBUG)
    *m_log << MSG::DEBUG << "TrigSerTPTool::convertTP " << endreq;

  //pers and persName set only after successful conversion
  persName = "";
  void *pers(0);

  // Start by trying to find the correct persistent class from the TP map
  // Otherwise, use the internal logic of the TPCnvSvc
  ITPCnvBase* cnvtr;
  if(m_TPmap.find(clname) == m_TPmap.end())
    {
      *m_log << MSG::ERROR << "Transient class " << clname 
	     << " is not in the T/P Converter map" << endreq;
      return 0;
    }
  
  cnvtr = m_tpcnvsvc->p2t_cnv(m_TPmap[clname]);
  
  if (!cnvtr) {
    *m_log << MSG::ERROR << "T/P Converter for transient class "
           << clname << " could not be retrieved" << endreq;
    return 0;
  }

  const std::string _persname = System::typeinfoName(cnvtr->persistentTInfo());

  TClass *persObjCl = getClass(_persname);
  void *persptr(0);
  if (persObjCl){
    persptr = persObjCl->New();
    if (loglvl<=MSG::DEBUG){
      *m_log << MSG::DEBUG << "created object of " << persptr << " at " << persptr << endreq;
    }
  } else {
    return 0;
  }

  if (loglvl<=MSG::DEBUG){
    *m_log << MSG::DEBUG << "invoking TP for " << clname  << " at " << ptr << " - ";
  }

  try {
    cnvtr->transToPersUntyped(ptr, persptr, *m_logTP);
    persName = _persname;
    pers = persptr;
    if (loglvl<=MSG::DEBUG){
      *m_log << MSG::DEBUG <<  "succeeded at " << persptr << endreq;
    }
  }
  catch (const std::runtime_error& e){

    //delete persObjCl->New();
    const std::string issue = e.what();
    if (issue.find("is deprecated")!=std::string::npos){
      *m_log << MSG::INFO << "An exception " << e.what() << endreq;
    } else {
      pers = 0;
      *m_log << MSG::ERROR << "An exception " << e.what() << endreq;
    }
  }

  return pers;
}

void* TrigSerTPTool::convertPT(const std::string &persName, void *pers, std::string &transName){
  const int loglvl = m_log->level();

  // The p->t converter logic ensures the correct transient class is always loaded
  ITPCnvBase* cnvtr = m_tpcnvsvc->p2t_cnv(persName);
  if (!cnvtr) {
    *m_log << MSG::ERROR << "T/P Converter for persistent class "
           << persName << " could not be retrieved" << endreq;
    return 0;
  }
  transName = System::typeinfoName(cnvtr->transientTInfo());
  
  TClass *transCl = getClass(transName);
  void *trans(0);
  if (transCl){
    trans = transCl->New();
    if (loglvl<=MSG::DEBUG){
      *m_log << MSG::DEBUG << "trans  " << trans << endreq;
    }
  }
    
  if (loglvl<=MSG::DEBUG){
    *m_log << MSG::DEBUG << "invoking PT for " << transName << " - ";
  }
  try {
    cnvtr->persToTransUntyped(pers, trans, *m_logTP);
    if (loglvl<=MSG::DEBUG){
      *m_log << MSG::DEBUG << " succeeded at " << trans << endreq;
    }
  }
  catch (const std::runtime_error& e){
    *m_log << MSG::ERROR << "Exception " << e.what() << endreq;
    //it should destruct trans here (is it in a good state?)
    trans = 0;
  }
  
  return trans;
  
}

std::string TrigSerTPTool::persClassName(const std::string &transClassName){
  
  // Try to extract from the TP map
  if(m_TPmap.find(transClassName) == m_TPmap.end())
    {
      *m_log << MSG::ERROR << "Transient class " << transClassName
             << " is not in the T/P Converter map" << endreq;
      return "";
    }

  return m_TPmap[transClassName];  
  
}
