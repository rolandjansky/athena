/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArAutoCorrMCCnv.cxx
 * @brief AthenaPool converter LArAutoCorrMC
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "LArAutoCorrMCCnv.h"
#include "LArCondTPCnv/LArAutoCorrSubsetCnv_p1.h"
#include "LArAutoCorrCompleteCnv.h"

static const LArAutoCorrSubsetCnv_p1   TPconverter;

LArAutoCorrSubset_p1*
LArAutoCorrMCCnv::createPersistent (LArAutoCorrTransType* transObj)
{
    MsgStream log(msgSvc(), "LArAutoCorrMCCnv" ); 
    //log << MSG::DEBUG << "LArAutoCorrMC write" << endmsg;
    LArAutoCorrPersType* persObj = TPconverter.createPersistentConst( transObj, log );
    //log << MSG::DEBUG << "Success" << endmsg;
    return persObj; 
}

LArConditionsSubset<LArAutoCorrP1>*
LArAutoCorrMCCnv::createTransient ()
{
    static const pool::Guid   p1_guid("FA16A69D-241E-40F3-B710-77A95937E394");
    static const pool::Guid   p0_guid("4E7E36E9-2121-4327-88C5-8A516D6D6D2A");
    if( compareClassGuid(p1_guid) ) {
        // using unique_ptr ensures deletion of the persistent object
        std::unique_ptr< LArAutoCorrSubset_p1 > col_vect( poolReadObject< LArAutoCorrSubset_p1 >() );
        MsgStream log(msgSvc(), "LArAutoCorrMCCnv" ); 
        //log << MSG::INFO << "Reading LArAutoCorrSubset_p1" << endmsg; 
        return TPconverter.createTransientConst( col_vect.get(), log );
    }
    else if( compareClassGuid(p0_guid) ) {
        // subset from before TP separation

        MsgStream log(msgSvc(), "LArAutoCorrMCCnv" ); 
        log << MSG::INFO << "Reading LArAutoCorrSubset (original)" << endmsg; 

        std::unique_ptr< LArConditionsSubset<LArAutoCorrP> > subset ( poolReadObject< LArConditionsSubset<LArAutoCorrP> >() );
        // Here we must convert from LArAutoCorrP to LArAutoCorrP1
        
        log << MSG::INFO << "subset ptr " << subset.get() << endmsg; 

        return (createTransient(subset.get()));

    } 
    throw std::runtime_error("Unsupported persistent version of LArAutoCorrMCCnv");
}

LArConditionsSubset<LArAutoCorrP1>* 
LArAutoCorrMCCnv::createTransient(LArConditionsSubset<LArAutoCorrP>* orig)
{

    MsgStream log(msgSvc(), "LArAutoCorrMCCnv" ); 
    log << MSG::INFO << "LArAutoCorrMCCnv::createTransient orig " << orig << endmsg; 

    LArConditionsSubset<LArAutoCorrP1>* result = new LArConditionsSubset<LArAutoCorrP1>();
    
    // Copy LArAutoCorrP subset to LArAutoCorrP1
    LArAutoCorrCopy copier;
    copier.copyOldtoNew(orig, result);
    
    return (result);
}
    

