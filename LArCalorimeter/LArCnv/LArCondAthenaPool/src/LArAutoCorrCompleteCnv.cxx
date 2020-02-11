/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArAutoCorrCompleteCnv.cxx
 * @brief AthenaPool converter LArAutoCorrComplete
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "LArAutoCorrCompleteCnv.h"
#include "LArCondTPCnv/LArAutoCorrSubsetCnv_p1.h"

static LArAutoCorrSubsetCnv_p1   TPconverter;

LArAutoCorrSubset_p1*
LArAutoCorrCompleteCnv::createPersistent (LArAutoCorrTransType* transObj)
{
    MsgStream log(msgSvc(), "LArAutoCorrCompleteCnv" ); 
    //log << MSG::DEBUG << "LArAutoCorrComplete write" << endmsg;
    LArAutoCorrPersType* persObj = TPconverter.createPersistent( transObj, log );
    //log << MSG::DEBUG << "Success" << endmsg;
    return persObj; 
}

LArConditionsSubset<LArAutoCorrP1>*
LArAutoCorrCompleteCnv::createTransient ()
{
    static pool::Guid   p1_guid("FA16A69D-241E-40F3-B710-77A95937E394");
    static pool::Guid   p0_guid("4E7E36E9-2121-4327-88C5-8A516D6D6D2A");
    if( compareClassGuid(p1_guid) ) {
        // using auto_ptr ensures deletion of the persistent object
        std::auto_ptr< LArAutoCorrSubset_p1 > col_vect( poolReadObject< LArAutoCorrSubset_p1 >() );
        MsgStream log(msgSvc(), "LArAutoCorrCompleteCnv" ); 
        //log << MSG::INFO << "Reading LArAutoCorrSubset_p1" << endmsg; 
        return TPconverter.createTransient( col_vect.get(), log );
    }
    else if( compareClassGuid(p0_guid) ) {
        // subset from before TP separation

        MsgStream log(msgSvc(), "LArAutoCorrCompleteCnv" ); 
        log << MSG::DEBUG << "Reading LArAutoCorrSubset (original)" << endmsg; 

        std::auto_ptr< LArConditionsSubset<LArAutoCorrP> > subset ( poolReadObject< LArConditionsSubset<LArAutoCorrP> >() );
        // Here we must convert from LArAutoCorrP to LArAutoCorrP1
        
        log << MSG::DEBUG << "subset ptr " << subset.get() << endmsg; 

        return (createTransient(subset.get()));

    } 
    throw std::runtime_error("Unsupported persistent version of LArAutoCorrCompleteCnv");
}

LArConditionsSubset<LArAutoCorrP1>* 
LArAutoCorrCompleteCnv::createTransient(LArConditionsSubset<LArAutoCorrP>* orig)
{

    MsgStream log(msgSvc(), "LArAutoCorrCompleteCnv" ); 
    log << MSG::DEBUG << "LArAutoCorrCompleteCnv::createTransient orig " << orig << endmsg; 

    LArConditionsSubset<LArAutoCorrP1>* result = new LArConditionsSubset<LArAutoCorrP1>();
    
    // Copy LArAutoCorrP subset to LArAutoCorrP1
    LArAutoCorrCopy copier;
    copier.copyOldtoNew(orig, result);
    
    return (result);
}
    

// Copy LArAutoCorrP subset to LArAutoCorrP1
void 
LArAutoCorrCopy::copyOldtoNew(const LArConditionsSubset<LArAutoCorrP>* oldAutoCorr,
			   LArConditionsSubset<LArAutoCorrP1>* newAutoCorr)
{
  newAutoCorr->assign (*oldAutoCorr,
                       [] (const LArAutoCorrP& from,
                           LArAutoCorrP1& to)
                       {
                         to.m_vAutoCorr.assign (from.m_vAutoCorr.begin(),
                                                from.m_vAutoCorr.end());
                       });
}
