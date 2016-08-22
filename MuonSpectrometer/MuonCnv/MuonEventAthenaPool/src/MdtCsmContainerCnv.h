/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  custom POOL converter for MDT CSM

  @author Tadashi Maeno 
       based on SCT_RDO_ContainerCnv by RD Schaffer
*/

#ifndef MUONEVENTATHENAPOOL_MdtCsmContainerCnv_H
#define MUONEVENTATHENAPOOL_MdtCsmContainerCnv_H

#include "MuonRdoContainerTPCnv.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "MdtCsmContainerCnv_p1.h"
#include "MuonEventAthenaPool/MdtCsmContainer_p1.h"


typedef MuonRdoContainerTPCnv<MdtCsmContainer, MdtCsmContainer_p1, MdtCsmContainerCnv_p1 > MdtCsmContainerCnv;

template < >
inline
MdtCsmContainer*
MdtCsmContainerCnv::createTransient()
{
    MsgStream log(msgSvc(), "MdtCsmContainerCnv" );
    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << " **** Entered createTransient() "<< endmsg;
    
    // the use of auto pointers ensures that the persistent object is deleted
    // using the correct persistent type pointer
    
    MdtCsmContainer *trans_cont = 0;
    static pool::Guid	p1_guid("66D77835-5290-4C9A-842A-AF10F384FC93");
    static pool::Guid	p0_guid("CFDA6FF6-557F-40CB-9C54-B5A7404A9175");

    if( compareClassGuid(p1_guid) ) {
        std::unique_ptr< MdtCsmContainer_p1 > col_vect( poolReadObject< MdtCsmContainer_p1 >() );
        trans_cont = m_TPconverter.createTransient( col_vect.get(), log );
    } else if(compareClassGuid(p0_guid) ) {
        // old version from before TP separation
        std::unique_ptr< COLL_vector >	col_vect(  this->poolReadObject< COLL_vector >() );
        trans_cont =  createTransientFrom_p0( col_vect.get(), log );
    } else {
        throw std::runtime_error("Unsupported persistent version of CSC RDO container");
    }
    return trans_cont;
}


#endif
