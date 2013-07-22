/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArRampMCCnv.cxx
 * @brief AthenaPool converter LArRampMC
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "LArRampMCCnv.h"
#include "LArCondTPCnv/LArRampSubsetCnv_p1.h"
#include "LArRampCompleteCnv.h"

static LArRampSubsetCnv_p1   TPconverter;

LArRampSubset_p1*
LArRampMCCnv::createPersistent (LArRampTransType* transObj)
{
    MsgStream log(messageService(), "LArRampMCCnv" ); 
    //log << MSG::DEBUG << "LArRampMC write" << endreq;
    LArRampPersType* persObj = TPconverter.createPersistent( transObj, log );
    //log << MSG::DEBUG << "Success" << endreq;
    return persObj; 
}

LArConditionsSubset<LArRampP1>*
LArRampMCCnv::createTransient ()
{
    static pool::Guid   p1_guid("BB629283-BDF5-4FA8-93F4-5D8C2F201364");
    static pool::Guid   p0_guid("4019776D-D528-4401-9CBD-7956C4B00607");
    if( compareClassGuid(p1_guid) ) {
        // using auto_ptr ensures deletion of the persistent object
        std::auto_ptr< LArRampSubset_p1 > col_vect( poolReadObject< LArRampSubset_p1 >() );
        MsgStream log(messageService(), "LArRampMCCnv" ); 
        //log << MSG::INFO << "Reading LArRampSubset_p1" << endreq; 
        return TPconverter.createTransient( col_vect.get(), log );
    }
    else if( compareClassGuid(p0_guid) ) {
        // subset from before TP separation

        MsgStream log(messageService(), "LArRampMCCnv" ); 
        log << MSG::INFO << "Reading LArRampSubset (original)" << endreq; 

        std::auto_ptr< LArConditionsSubset<LArRampP> > subset ( poolReadObject< LArConditionsSubset<LArRampP> >() );
        // Here we must convert from LArRampP to LArRampP1
        
        log << MSG::INFO << "subset ptr " << subset.get() << endreq; 

        return (createTransient(subset.get()));

    } 
    throw std::runtime_error("Unsupported persistent version of LArRampMCCnv");
}

LArConditionsSubset<LArRampP1>* 
LArRampMCCnv::createTransient(LArConditionsSubset<LArRampP>* orig)
{

    MsgStream log(messageService(), "LArRampMCCnv" ); 
    log << MSG::INFO << "LArRampMCCnv::createTransient orig " << orig << endreq; 

    LArConditionsSubset<LArRampP1>* result = new LArConditionsSubset<LArRampP1>();
    
    // Copy LArRampP subset to LArRampP1
    LArRampCopy copier;
    copier.copyOldtoNew(orig, result);

    return (result);
}
    
