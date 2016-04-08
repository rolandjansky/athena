/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArPedestalMCCnv.cxx
 * @brief AthenaPool converter LArPedestalMC
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "LArRawConditions/LArPedestalMC.h"
#include "LArPedestalMCCnv.h"
#include "LArCondTPCnv/LArPedestalMCCnv_p1.h"

static LArPedestalMCCnv_p1   TPconverter;

LArPedestalMC_p1*
LArPedestalMCCnv::createPersistent (LArPedestalMCTransType* transObj)
{
    MsgStream log(messageService(), "LArPedestalMCCnv" ); 
    //log << MSG::DEBUG << "LArPedestalMC write" << endreq;
    LArPedestalMCPersType* persObj = TPconverter.createPersistent( transObj, log );
    //log << MSG::DEBUG << "Success" << endreq;
    return persObj; 
}

LArPedestalMC*
LArPedestalMCCnv::createTransient ()
{
    static pool::Guid   p1_guid("3891D5E0-82D1-45AB-97B1-CE4CF25D6E16");
    static pool::Guid   p0_guid("C147EFC8-5283-4DAE-AD20-0E2CB79E54B6");
    if( compareClassGuid(p1_guid) ) {
        // using auto_ptr ensures deletion of the persistent object
        std::auto_ptr< LArPedestalMC_p1 > col_vect( poolReadObject< LArPedestalMC_p1 >() );
        MsgStream log(messageService(), "LArPedestalMCCnv" ); 
        //log << MSG::INFO << "Reading LArPedestalMC_p1" << endreq; 
        return TPconverter.createTransient( col_vect.get(), log );
    }
    else if( compareClassGuid(p0_guid) ) {
        // subset from before TP separation

        MsgStream log(messageService(), "LArPedestalMCCnv" ); 
        log << MSG::INFO << "Reading LArPedestalMC (original)" << endreq; 

        LArPedestalMC* peds =  poolReadObject< LArPedestalMC >();

        return (peds);

    } 
    throw std::runtime_error("Unsupported persistent version of LArPedestalMCCnv");
}

    

