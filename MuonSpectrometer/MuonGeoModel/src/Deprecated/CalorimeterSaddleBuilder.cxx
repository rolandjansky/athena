/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Build the Calorimeter Saddle for MuonGeoModel
 ---------------------------------------------
 Copyright (C) 2007 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: CalorimeterSaddleBuilder.cxx 459783 2011-09-21 15:00:41Z dellacqu $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGeoModel/CalorimeterSaddleBuilder.h"

#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/getMessageSvc.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "MuonGeoModel/SaddleBoxesBuilder.h"
#include "MuonGeoModel/SaddleBrepsBuilder.h"
#include "MuonGeoModel/SaddleTubsBuilder.h"

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

namespace MuonGM {

CalorimeterSaddleBuilder::CalorimeterSaddleBuilder( StoreGateSvc  *pDetStore,
				    IRDBAccessSvc *pRDBAccess, 
                                    std::string    geoTag,
                                    std::string    geoNode)     :
    m_pRDBAccess(pRDBAccess), 
    m_pDetStore (pDetStore)
{
  m_msgSvc = Athena::getMessageSvc();
  MsgStream log(m_msgSvc, "MuGM:CalorimeterSaddleBuilder" );
  log  <<  MSG::INFO  <<  "Fetching data with tag <"  <<  geoTag  <<"> node <"<<geoNode<<">" <<  endreq;

  //here should follows lines like the following, where m_Jdsh must be defined in the include 
  //m_Jdsh = pRDBAccess->getRecordset( "JDSH",geoTag, geoNode);
  //if (m_Jdsh->size() == 0 ) 
  //{
  //    log<<MSG::WARNING<<"Table JDSH not found in tag <"  <<  geoTag <<  ">"  <<" reading table JDSH-00" <<endreq;
  //}  


  m_saddleBoxesBuilder = new SaddleBoxesBuilder(pDetStore,
                                                pRDBAccess, 
                                                geoTag,
                                                geoNode);
  m_saddleBrepsBuilder = new SaddleBrepsBuilder(pDetStore,
                                                pRDBAccess, 
                                                geoTag,
                                                geoNode);
  m_saddleTubsBuilder = new SaddleTubsBuilder(pDetStore,
                                              pRDBAccess, 
                                              geoTag,
                                              geoNode);
  
}

void 
CalorimeterSaddleBuilder::build( GeoPhysVol* container )
{
    // here do something to build whatYouHaveBuilt

    MsgStream log(m_msgSvc, "MuGM:CalorimeterSaddleBuilder" );

    if (m_saddleBoxesBuilder->unavailableData() || m_saddleBrepsBuilder->unavailableData() || m_saddleTubsBuilder->unavailableData()) 
    {
        log<<MSG::WARNING<<"No Data available in the DB for the CalorimeterSaddle "<<endreq;
        return;
    }
    log<<MSG::DEBUG<<"Building the CalorimeterSaddle "<<endreq;
    m_saddleBoxesBuilder->buildSaddleBoxes(container);
    m_saddleBrepsBuilder->buildSaddleBreps(container);
    m_saddleTubsBuilder->buildSaddleTubs(container);
    
    return;
}
}
