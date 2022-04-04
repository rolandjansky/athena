/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArOFCCompleteCnv.cxx
 * @brief AthenaPool converter LArOFCComplete
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "LArOFCCompleteCnv.h"
#include "LArCondTPCnv/LArOFCSubsetCnv_p1.h"

static const LArOFCSubsetCnv_p1   TPconverter;

LArOFCSubset_p1*
LArOFCCompleteCnv::createPersistent (LArOFCTransType* transObj)
{
    MsgStream log(msgSvc(), "LArOFCCompleteCnv" ); 
    //log << MSG::DEBUG << "LArOFCComplete write" << endmsg;
    LArOFCPersType* persObj = TPconverter.createPersistentConst( transObj, log );
    //log << MSG::DEBUG << "Success" << endmsg;
    return persObj; 
}

LArConditionsSubset<LArOFCP1>*
LArOFCCompleteCnv::createTransient () {
    static const pool::Guid   p1_guid("0A1DE2E2-90E4-4A24-BC6E-2092EDC9FDF6");
    static const pool::Guid   p0_guid("3E5389EF-D163-4099-91D9-D3F0EE06C1CD");

    if( compareClassGuid(p1_guid) ) {
        // using unique_ptr ensures deletion of the persistent object
        std::unique_ptr< LArOFCSubset_p1 > col_vect( poolReadObject< LArOFCSubset_p1 >() );
        MsgStream log(msgSvc(), "LArOFCCompleteCnv" ); 
        //log << MSG::INFO << "Reading LArOFCSubset_p1" << endmsg; 
        return TPconverter.createTransientConst( col_vect.get(), log );
    }
    else if( compareClassGuid(p0_guid) ) {
        // subset from before TP separation

        MsgStream log(msgSvc(), "LArOFCCompleteCnv" ); 
        log << MSG::DEBUG << "Reading LArOFCSubset (original)" << endmsg; 

        std::unique_ptr< LArConditionsSubset<LArOFCP> > subset ( poolReadObject< LArConditionsSubset<LArOFCP> >() );
        // Here we must convert from LArOFCP to LArOFCP1
        
        log << MSG::DEBUG << "subset ptr " << subset.get() << endmsg; 

        return (createTransient(subset.get()));

    } 
    throw std::runtime_error("Unsupported persistent version of LArOFCCompleteCnv");
}

LArConditionsSubset<LArOFCP1>* 
LArOFCCompleteCnv::createTransient(LArConditionsSubset<LArOFCP>* orig)
{

    MsgStream log(msgSvc(), "LArOFCCompleteCnv" ); 
    log << MSG::DEBUG << "LArOFCCompleteCnv::createTransient orig " << orig << endmsg; 

    LArConditionsSubset<LArOFCP1>* result = new LArConditionsSubset<LArOFCP1>();
    
    // Copy LArOFCP subset to LArOFCP1
    LArOFCCopy copier;
    copier.copyOldtoNew(orig, result);
    
    return (result);
}
    
// Copy LArOFCP subset to LArOFCP1
void 
LArOFCCopy::copyOldtoNew(const LArConditionsSubset<LArOFCP>* oldOFC,
			   LArConditionsSubset<LArOFCP1>* newOFC)
{
  newOFC->assign (*oldOFC,
                  [] (const LArOFCP& from,
                      LArOFCP1& to)
                  {
                    to = LArOFCP1 (from.m_timeOffset,
                                   25./24,
                                   from.m_vOFC_a,
                                   from.m_vOFC_b);
                  });
}
