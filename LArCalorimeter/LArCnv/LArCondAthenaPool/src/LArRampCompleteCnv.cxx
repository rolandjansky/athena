/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArRampCompleteCnv.cxx
 * @brief AthenaPool converter LArRampComplete
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "LArRampCompleteCnv.h"
#include "LArCondTPCnv/LArRampSubsetCnv_p1.h"

static const LArRampSubsetCnv_p1   TPconverter;

LArRampSubset_p1*
LArRampCompleteCnv::createPersistent (LArRampTransType* transObj)
{
    MsgStream log(msgSvc(), "LArRampCompleteCnv" ); 
    //log << MSG::DEBUG << "LArRampComplete write" << endmsg;
    LArRampPersType* persObj = TPconverter.createPersistentConst( transObj, log );
    //log << MSG::DEBUG << "Success" << endmsg;
    return persObj; 
}

LArConditionsSubset<LArRampP1>*
LArRampCompleteCnv::createTransient ()
{
    static const pool::Guid   p1_guid("BB629283-BDF5-4FA8-93F4-5D8C2F201364");
    static const pool::Guid   p0_guid("4019776D-D528-4401-9CBD-7956C4B00607");
    if( compareClassGuid(p1_guid) ) {
        // using unique_ptr ensures deletion of the persistent object
        std::unique_ptr< LArRampSubset_p1 > col_vect( poolReadObject< LArRampSubset_p1 >() );
        MsgStream log(msgSvc(), "LArRampCompleteCnv" ); 
        //log << MSG::INFO << "Reading LArRampSubset_p1" << endmsg; 
        return TPconverter.createTransientConst( col_vect.get(), log );
    }
    else if( compareClassGuid(p0_guid) ) {
        // subset from before TP separation

        MsgStream log(msgSvc(), "LArRampCompleteCnv" ); 
        log << MSG::DEBUG << "Reading LArRampSubset (original)" << endmsg; 

        std::unique_ptr< LArConditionsSubset<LArRampP> > subset ( poolReadObject< LArConditionsSubset<LArRampP> >() );
        // Here we must convert from LArRampP to LArRampP1
        
        log << MSG::DEBUG << "subset ptr " << subset.get() << endmsg; 

        return (createTransient(subset.get()));

    } 
    throw std::runtime_error("Unsupported persistent version of LArRampCompleteCnv");
}

LArConditionsSubset<LArRampP1>* 
LArRampCompleteCnv::createTransient(LArConditionsSubset<LArRampP>* orig)
{

    MsgStream log(msgSvc(), "LArRampCompleteCnv" ); 
    log << MSG::DEBUG << "LArRampCompleteCnv::createTransient orig " << orig << endmsg; 

    LArConditionsSubset<LArRampP1>* result = new LArConditionsSubset<LArRampP1>();
    
    // Copy LArRampP subset to LArRampP1
    LArRampCopy copier;
    copier.copyOldtoNew(orig, result);

    return (result);
}
    
// Copy LArRampP subset to LArRampP1
void 
LArRampCopy::copyOldtoNew(const LArConditionsSubset<LArRampP>* oldRamp,
			   LArConditionsSubset<LArRampP1>* newRamp)
{
  newRamp->assign (*oldRamp,
                   [] (const LArRampP& from,
                       LArRampP1& to)
                   {
                     to.m_vRamp = from.m_vRamp;
                   });
}

