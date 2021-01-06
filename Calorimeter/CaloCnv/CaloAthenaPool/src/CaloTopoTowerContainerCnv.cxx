/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTopoTowerContainerCnv.h"

// LArDetDescr includes
#include "CaloDetDescr/CaloDetDescrManager.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ThreadLocalContext.h"

// Athena
#include "CaloUtils/CaloTopoTowerBuilderToolBase.h"
#include "CaloUtils/CaloTopoTowerBuilderTool.h"


CaloTopoTowerContainerCnv::CaloTopoTowerContainerCnv(ISvcLocator* svcloc)
    :
    // Base class constructor
    CaloTopoTowerContainerCnvBase(svcloc),
    m_TopoTowerBldr(0),
    p0_guid("00B7F56C-1E49-4469-BEBA-C74620575A00"),
    p1_guid("4ED29686-28E9-426F-B076-C4E4600F66A7")
{}



CaloTopoTowerContainerCnv::~CaloTopoTowerContainerCnv()
{}


// StatusCode CaloTopoTowerContainerCnv::initialize()
// {
//     // Call base clase initialize
//     AthenaPoolConverter::initialize();

//     // Get the messaging service, print where you are
//     MsgStream log(msgSvc(), "CaloTopoTowerContainerCnv");
//     log << MSG::INFO << "initialize()" << endmsg;

//     return StatusCode::SUCCESS;
// }


//StatusCode CaloTopoTowerContainerCnv::PoolToDataObject(DataObject*& pObj,const std::string &token)
CaloTopoTowerContainer* CaloTopoTowerContainerCnv::createTransient() {
    MsgStream log(msgSvc(), "CaloTopoTowerContainerCnv::createTransient" );
    CaloTopoTowerContainer* Cont = 0;
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Starting CaloTopoTowerContainerCnv::PoolToDataObject" << endmsg;
    if (compareClassGuid(p0_guid)) {
     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Read version p0 of CaloTopoTowerContainer. GUID=" 
	 << m_classID.toString() << endmsg;
     Cont=poolReadObject<CaloTopoTowerContainer>();
    }
    else if(compareClassGuid(p1_guid)) {
      if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Read version p1 of CaloTopoTowerContainer. GUID=" 
	  << m_classID.toString() << endmsg;
      CaloTopoTowerContainerPERS* pers=poolReadObject<CaloTopoTowerContainer_p1>();
      Cont=new CaloTopoTowerContainer();
      m_converter.persToTrans(pers,Cont,log);
      delete pers;
    }
    if (!Cont) {
      log << MSG::FATAL << "Unable to get object from pool" << endmsg;
      return Cont;
    }
    
    Cont->init();

    // rebuild the CaloTopoTowers in the container.
    const EventContext& ctx = Gaudi::Hive::currentContext();

    m_TopoTowerBldr= getTool("CaloTopoTowerBuilderTool","TopoTowerTwrBldr");
    if(!m_TopoTowerBldr){
      log<<MSG::ERROR<< " Failed to create CaloTopoTowerContainer " <<endmsg;
      return 0;
    }
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "creating CaloTopoTowerContainerCnv::PoolToDataObject" << endmsg; 
    StatusCode scfcal = m_TopoTowerBldr->execute(ctx, Cont);
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<" TopoTowers rebuild m_TopoTowerBldr->execute(Cont); Successful "<<endmsg; 
    if (scfcal.isFailure()) {
      log<<MSG::ERROR<<" TopoTowers rebuild failed "<<endmsg; 
    }
    if (log.level() <= MSG::DEBUG) log<<MSG::DEBUG<<" TopoTowers rebuild worked "<<endmsg;

    return Cont; 
}

CaloTopoTowerContainerPERS* CaloTopoTowerContainerCnv::createPersistent(CaloTopoTowerContainer* trans) {
    MsgStream log(msgSvc(), "CaloTopoTowerContainerCnv::createPersistent");
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Writing CaloTopoTowerContainer_p1" << endmsg;
    CaloTopoTowerContainerPERS* pers=new CaloTopoTowerContainerPERS();
    m_converter.transToPers(trans,pers,log); 
    return pers;
}


CaloTopoTowerBuilderToolBase* CaloTopoTowerContainerCnv::getTool(
const std::string& type, const std::string& nm)
{
  IToolSvc* myToolSvc;
  StatusCode sc = service("ToolSvc",myToolSvc);

  MsgStream log(msgSvc(), "CaloTopoTowerContainerCnv");
  if ( sc.isFailure() )
    {
      log << MSG::ERROR
          << "Tool Service not found"
          << endmsg;
      return 0 ; 
    }

  ////////////////////
  // Allocate Tools //
  ////////////////////

  IAlgTool* algToolPtr;
  sc = myToolSvc->retrieveTool(type,nm,algToolPtr);
  // tool not found
  if ( sc.isFailure() )
   {
          log << MSG::INFO
              << "Cannot find tool named <"
              << type << "/" << nm 
              << ">"
              << endmsg;
          return 0; 
    }
  return   dynamic_cast<CaloTopoTowerBuilderToolBase*>(algToolPtr);

}
