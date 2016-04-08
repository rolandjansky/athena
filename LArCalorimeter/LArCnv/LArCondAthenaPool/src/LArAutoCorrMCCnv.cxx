/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArAutoCorrMCCnv.cxx
 * @brief AthenaPool converter LArAutoCorrMC
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "LArAutoCorrMCCnv.h"
#include "LArCondTPCnv/LArAutoCorrSubsetCnv_p1.h"
#include "LArAutoCorrCompleteCnv.h"

static LArAutoCorrSubsetCnv_p1   TPconverter;

LArAutoCorrSubset_p1*
LArAutoCorrMCCnv::createPersistent (LArAutoCorrTransType* transObj)
{
    MsgStream log(messageService(), "LArAutoCorrMCCnv" ); 
    //log << MSG::DEBUG << "LArAutoCorrMC write" << endreq;
    LArAutoCorrPersType* persObj = TPconverter.createPersistent( transObj, log );
    //log << MSG::DEBUG << "Success" << endreq;
    return persObj; 
}

LArConditionsSubset<LArAutoCorrP1>*
LArAutoCorrMCCnv::createTransient ()
{
    static pool::Guid   p1_guid("FA16A69D-241E-40F3-B710-77A95937E394");
    static pool::Guid   p0_guid("4E7E36E9-2121-4327-88C5-8A516D6D6D2A");
    if( compareClassGuid(p1_guid) ) {
        // using auto_ptr ensures deletion of the persistent object
        std::auto_ptr< LArAutoCorrSubset_p1 > col_vect( poolReadObject< LArAutoCorrSubset_p1 >() );
        MsgStream log(messageService(), "LArAutoCorrMCCnv" ); 
        //log << MSG::INFO << "Reading LArAutoCorrSubset_p1" << endreq; 
        return TPconverter.createTransient( col_vect.get(), log );
    }
    else if( compareClassGuid(p0_guid) ) {
        // subset from before TP separation

        MsgStream log(messageService(), "LArAutoCorrMCCnv" ); 
        log << MSG::INFO << "Reading LArAutoCorrSubset (original)" << endreq; 

        std::auto_ptr< LArConditionsSubset<LArAutoCorrP> > subset ( poolReadObject< LArConditionsSubset<LArAutoCorrP> >() );
        // Here we must convert from LArAutoCorrP to LArAutoCorrP1
        
        log << MSG::INFO << "subset ptr " << subset.get() << endreq; 

        return (createTransient(subset.get()));

    } 
    throw std::runtime_error("Unsupported persistent version of LArAutoCorrMCCnv");
}

LArConditionsSubset<LArAutoCorrP1>* 
LArAutoCorrMCCnv::createTransient(LArConditionsSubset<LArAutoCorrP>* orig)
{

    MsgStream log(messageService(), "LArAutoCorrMCCnv" ); 
    log << MSG::INFO << "LArAutoCorrMCCnv::createTransient orig " << orig << endreq; 

    LArConditionsSubset<LArAutoCorrP1>* result = new LArConditionsSubset<LArAutoCorrP1>();
    
    // Copy LArAutoCorrP subset to LArAutoCorrP1
    LArAutoCorrCopy copier;
    copier.copyOldtoNew(orig, result);
    
    return (result);
}
    

