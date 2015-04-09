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

#include "AthAllocators/Arena.h"
#include "CxxUtils/make_unique.h"

//ROOT
#include "TROOT.h"
#include "TClass.h"
#include "TBaseClass.h"

#include <iostream>
#include <algorithm>

using std::map;
using std::string;

TrigSerTPTool::TrigSerTPTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent) :
  AthAlgTool(type,name,parent),
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
}

TrigSerTPTool::~TrigSerTPTool(){
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
  ATH_MSG_INFO( "in initialize " << name()  );

  if(!m_msgsvcTP.empty()){
    if (m_msgsvcTP.retrieve().isFailure()){
      ATH_MSG_INFO( "Could not retrieve a separate MsgSvc for the T/P debugging"  );
    } else {
      ATH_MSG_DEBUG( "Using " << m_msgsvcTP << " for debugging "  );
      IMessageSvc* msvc = m_msgsvcTP.operator->();
      m_logTP = CxxUtils::make_unique<MsgStream>(msvc, "TrigSerTPTool-T/P");
    }
  }

  ATH_CHECK( m_dictSvc.retrieve().isSuccess() );
  ATH_CHECK( m_tpcnvsvc.retrieve() );

  m_tpCnvBaseClass = getClass ("ITPCnvBase");

  if (!m_tpCnvBaseClass ) {
    ATH_MSG_DEBUG( "Cannot find class ITPCnvBase"  );
  } else {
    ATH_MSG_DEBUG( "Found class ITPCnvBase"  );
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
      //   ATH_MSG_DEBUG( "cl=" << cl << " p=" << p  );
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
    ATH_MSG_DEBUG( "Dict loading of " << clname << " failed"  );
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
    ATH_MSG_DEBUG( "Dict loading of " << clname << " failed"  );
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

  ATH_MSG_DEBUG( "TrigSerTPTool::convertTP "  );

  //pers and persName set only after successful conversion
  persName = "";
  void *pers(0);

  // Start by trying to find the correct persistent class from the TP map
  // Otherwise, use the internal logic of the TPCnvSvc
  ITPCnvBase* cnvtr;
  if(m_TPmap.find(clname) == m_TPmap.end())
    {
      ATH_MSG_ERROR( "Transient class " << clname 
                     << " is not in the T/P Converter map"  );
      return 0;
    }
  
  cnvtr = m_tpcnvsvc->p2t_cnv(m_TPmap[clname]);
  
  if (!cnvtr) {
    ATH_MSG_ERROR( "T/P Converter for transient class "
                   << clname << " could not be retrieved"  );
    return 0;
  }

  const std::string _persname = System::typeinfoName(cnvtr->persistentTInfo());

  TClass *persObjCl = getClass(_persname);
  void *persptr(0);
  if (persObjCl){
    persptr = persObjCl->New();
    ATH_MSG_DEBUG( "created object of " << persptr << " at " << persptr  );
  } else {
    return 0;
  }

  ATH_MSG_DEBUG( "invoking TP for " << clname  << " at " << ptr << " - ");

  try {
    cnvtr->transToPersUntyped(ptr, persptr, m_logTP ? *m_logTP : msg());
    persName = _persname;
    pers = persptr;
    ATH_MSG_DEBUG(  "succeeded at " << persptr  );
  }
  catch (const std::runtime_error& e){

    //delete persObjCl->New();
    const std::string issue = e.what();
    if (issue.find("is deprecated")!=std::string::npos){
      ATH_MSG_INFO( "An exception " << e.what()  );
    } else {
      pers = 0;
      ATH_MSG_ERROR( "An exception " << e.what()  );
    }
  }

  return pers;
}

void* TrigSerTPTool::convertPT(const std::string &persName, void *pers, std::string &transName){

  // The p->t converter logic ensures the correct transient class is always loaded
  ITPCnvBase* cnvtr = m_tpcnvsvc->p2t_cnv(persName);
  if (!cnvtr) {
    ATH_MSG_ERROR( "T/P Converter for persistent class "
                   << persName << " could not be retrieved"  );
    return 0;
  }
  transName = System::typeinfoName(cnvtr->transientTInfo());
  
  TClass *transCl = getClass(transName);
  void *trans(0);
  if (transCl){
    trans = transCl->New();
    ATH_MSG_DEBUG( "trans  " << trans  );
  }
    
  ATH_MSG_DEBUG( "invoking PT for " << transName << " - "  );
  try {
    cnvtr->persToTransUntyped(pers, trans, m_logTP ? *m_logTP : msg());
    ATH_MSG_DEBUG( " succeeded at " << trans  );
  }
  catch (const std::runtime_error& e){
    ATH_MSG_ERROR( "Exception " << e.what()  );
    //it should destruct trans here (is it in a good state?)
    trans = 0;
  }
  
  return trans;
  
}

std::string TrigSerTPTool::persClassName(const std::string &transClassName){
  
  // Try to extract from the TP map
  if(m_TPmap.find(transClassName) == m_TPmap.end())
    {
      ATH_MSG_ERROR( "Transient class " << transClassName
                     << " is not in the T/P Converter map"  );
      return "";
    }

  return m_TPmap[transClassName];  
  
}
