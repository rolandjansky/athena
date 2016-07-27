/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file IOVMetaDataContainerCnv.cxx
 * @brief Implementation file for AthenaPool converter for IOVMetaDataContainer 
 * @author R.D.Schaffer@cern.ch
 */

#include "IOVMetaDataContainerCnv.h"
#include "IOVDbTPCnv/IOVMetaDataContainer_p1.h"
#include "IOVDbTPCnv/IOVMetaDataContainerCnv_p1.h"
#include "IOVDbDataModel/IOVPayloadContainer.h"

static IOVMetaDataContainerCnv_p1   TPconverter;

IOVMetaDataContainer_PERS* IOVMetaDataContainerCnv::createPersistent(IOVMetaDataContainer* transObj) {

//     // set the placement for first write
//     static bool first = true;
//     if (first) {
// 	first = false;
// 	const std::string containerName = m_athenaPoolCnvSvc->getOutputContainer(tname, key);
// 	const std::pair<std::string, std::string> entry(key, containerName);
// 	m_placementHints.insert(entry);
	


    MsgStream log(msgSvc(), "IOVMetaDataContainerConverter" ); 
    IOVMetaDataContainer_PERS *persObj = TPconverter.createPersistent( transObj, log );
    log << MSG::DEBUG << "Success" << endmsg;
    return persObj; 
}
    
IOVMetaDataContainer* IOVMetaDataContainerCnv::createTransient() {

    static pool::Guid   p1_guid("6C2DE6DF-6D52-43F6-B435-9F29812F40C0");
    if( compareClassGuid(p1_guid) ) {
        // using auto_ptr ensures deletion of the persistent object
        std::auto_ptr< IOVMetaDataContainer_p1 > col_vect( poolReadObject< IOVMetaDataContainer_p1 >() );
        MsgStream log(msgSvc(), "IOVMetaDataContainerCnv" );
        log << MSG::DEBUG << "Reading IOVMetaDataContainer_p1" << endmsg; 
        return TPconverter.createTransient( col_vect.get(), log );
    }
    throw std::runtime_error("Unsupported persistent version of IOVMetaDataContainer");
}



