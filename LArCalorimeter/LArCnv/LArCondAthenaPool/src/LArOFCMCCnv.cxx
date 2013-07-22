/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArOFCMCCnv.cxx
 * @brief AthenaPool converter LArOFCMC
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "LArOFCMCCnv.h"
#include "LArCondTPCnv/LArOFCSubsetCnv_p1.h"
#include "LArOFCCompleteCnv.h"

static LArOFCSubsetCnv_p1   TPconverter;

LArOFCSubset_p1*
LArOFCMCCnv::createPersistent (LArOFCTransType* transObj)
{
    MsgStream log(messageService(), "LArOFCMCCnv" ); 
    //log << MSG::DEBUG << "LArOFCMC write" << endreq;
    LArOFCPersType* persObj = TPconverter.createPersistent( transObj, log );
    //log << MSG::DEBUG << "Success" << endreq;
    return persObj; 
}

LArConditionsSubset<LArOFCP1>*
LArOFCMCCnv::createTransient () {
    static pool::Guid   p1_guid("0A1DE2E2-90E4-4A24-BC6E-2092EDC9FDF6");
    static pool::Guid   p0_guid("3E5389EF-D163-4099-91D9-D3F0EE06C1CD");

    if( compareClassGuid(p1_guid) ) {
        // using auto_ptr ensures deletion of the persistent object
        std::auto_ptr< LArOFCSubset_p1 > col_vect( poolReadObject< LArOFCSubset_p1 >() );
        MsgStream log(messageService(), "LArOFCMCCnv" ); 
        //log << MSG::INFO << "Reading LArOFCSubset_p1" << endreq; 
        return TPconverter.createTransient( col_vect.get(), log );
    }
    else if( compareClassGuid(p0_guid) ) {
        // subset from before TP separation

        MsgStream log(messageService(), "LArOFCMCCnv" ); 
        log << MSG::INFO << "Reading LArOFCSubset (original)" << endreq; 

        std::auto_ptr< LArConditionsSubset<LArOFCP> > subset ( poolReadObject< LArConditionsSubset<LArOFCP> >() );
        // Here we must convert from LArOFCP to LArOFCP1
        
        log << MSG::INFO << "subset ptr " << subset.get() << endreq; 

        return (createTransient(subset.get()));

    } 
    throw std::runtime_error("Unsupported persistent version of LArOFCMCCnv");
}

LArConditionsSubset<LArOFCP1>* 
LArOFCMCCnv::createTransient(LArConditionsSubset<LArOFCP>* orig)
{

    MsgStream log(messageService(), "LArOFCMCCnv" ); 
    log << MSG::INFO << "LArOFCMCCnv::createTransient orig " << orig << endreq; 

    LArConditionsSubset<LArOFCP1>* result = new LArConditionsSubset<LArOFCP1>();
    
    // Copy LArOFCP subset to LArOFCP1
    LArOFCCopy copier;
    copier.copyOldtoNew(orig, result);
    
    return (result);
}



    

