/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Detector description conversion service package
 -----------------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: DetDescrConverter.cxx,v 1.6 2008-12-14 02:24:44 ssnyder Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "DetDescrCnvSvc/DetDescrCnvSvc.h"
#include "GaudiKernel/MsgStream.h"

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

StatusCode 
DetDescrConverter::initialize()
{
    // Just call parent initialize
    return Converter::initialize();
}

StatusCode 
DetDescrConverter::finalize()
{
    // Just call parent finalize
    return Converter::finalize();
}


StatusCode
DetDescrConverter::fillObjRefs(IOpaqueAddress* /*pAddr*/, DataObject* /*pObj*/)
{
    return StatusCode::SUCCESS;
}

StatusCode
DetDescrConverter::createRep(DataObject* /*pObj*/, IOpaqueAddress*& /*pAddr*/)
{
    return StatusCode::SUCCESS;
}

StatusCode
DetDescrConverter::fillRepRefs(IOpaqueAddress* /*pAddr*/, DataObject* /*pObj*/)
{
    return StatusCode::SUCCESS;
}

long
DetDescrConverter::storageType()
{
    return DetDescr_StorageType;
}

//  StatusCode 
//  DetDescrConverter::addToDetStore(const CLID& clid, const std::string& name) const
//  {

//      StatusCode status = StatusCode::SUCCESS;
//      MsgStream log(messageService(), "DetDescrConverter");
//      log << MSG::DEBUG << "in addToDetStore" << endmsg;

//      if(!m_cnvSvc) {
//  	log << MSG::INFO << " Unable to add address for " << name 
//  	    << " with CLID " << clid 
//  	    << " to detector store " <<endmsg;
//  	return StatusCode::FAILURE;
//      }
//      else {
//  	m_cnvSvc->addToDetStore(clid, name);
//      }
//      return status;
//  }


//  d_Ref_Any
//  DetDescrConverter::getRef(IOpaqueAddress* pAddr) const {
//      d_Ref_Any retVal;
//      DetDescrAddress *nAddr = dynamic_cast<DetDescrAddress*>(pAddr);
//      if (0 != nAddr) {
//          nAddr->getRef(retVal);
//  // check for Gaudi NULL (different than ODMG NULL) here...
//          if (65535==*((unsigned short*)(&retVal))) {
//              retVal.clear();
//          }
//      }
//      return retVal;
//  }

//  DetDescrAddress*
//  DetDescrConverter::newDetDescrAddress(const d_Ref_Any& outRef) const {
//      return new DetDescrAddress(m_CLID, "", "", outRef);
//  }

//  d_Ref_Any
//  DetDescrConverter::where() const {
//      return m_cnvSvc->getClustering(m_CLID);
//  }

DetDescrConverter::DetDescrConverter(const CLID& myCLID, ISvcLocator* svcloc) :
    Converter(DetDescr_StorageType, myCLID, svcloc)
    //m_CLID(myCLID)
{

    IInterface* toCnvSvc = 0;
    m_cnvSvc = 0;
    StatusCode status = serviceLocator()
      ->getService("DetDescrCnvSvc",
                   IConversionSvc::interfaceID(),
                   toCnvSvc);
    if (status.isSuccess()) {
	m_cnvSvc = dynamic_cast<DetDescrCnvSvc*>(toCnvSvc);
    }
    if (0 == m_cnvSvc) {
	// cout<<"DMM:  ERROR--converter could not find DetDescrCnvSvc"<<endl;
    }
}

