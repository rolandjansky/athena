/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArPedestalCompleteCnv.cxx
 * @brief AthenaPool converter LArPedestalComplete
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "LArPedestalCompleteCnv.h"
#include "LArCondTPCnv/LArPedestalSubsetCnv_p1.h"

static const LArPedestalSubsetCnv_p1   TPconverter1;
static const LArPedestalSubsetCnv_p2   TPconverter2;


LArPedestalSubset_p2*
LArPedestalCompleteCnv::createPersistent (LArPedTransType* transObj)
{
    MsgStream log(msgSvc(), "LArPedestalCompleteCnv" ); 
    //log << MSG::DEBUG << "LArPedestalComplete write" << endmsg;
    return TPconverter2.createPersistentConst( transObj, log );
}

LArConditionsSubset<LArPedestalP1>*
LArPedestalCompleteCnv::createTransient ()
{
    static const pool::Guid p2_guid("8BD3FE69-A3C4-418C-ACB9-E362CE524353");
    static const pool::Guid   p1_guid("E365F747-264B-4A0C-B80A-570DBE099881");
    static const pool::Guid   p0_guid("E17191DD-4C0A-4B1A-AE49-7D587C6BE3EE");

    if (compareClassGuid(p2_guid)) {
      // using unique_ptr ensures deletion of the persistent object
      std::unique_ptr< LArPedestalSubset_p2 > col_vect( poolReadObject< LArPedestalSubset_p2 >() );
      MsgStream log(msgSvc(), "LArPedestalCompleteCnv" ); 
      //log << MSG::INFO << "Reading LArPedestalSubset_p1" << endmsg; 
      return TPconverter2.createTransientConst( col_vect.get(), log );
    }
    else if( compareClassGuid(p1_guid) ) {
        // using unique_ptr ensures deletion of the persistent object
        std::unique_ptr< LArPedestalSubset_p1 > col_vect( poolReadObject< LArPedestalSubset_p1 >() );
        MsgStream log(msgSvc(), "LArPedestalCompleteCnv" ); 
        //log << MSG::INFO << "Reading LArPedestalSubset_p1" << endmsg; 
        return TPconverter1.createTransientConst( col_vect.get(), log );
    }
    else if( compareClassGuid(p0_guid) ) {
        // subset from before TP separation

        MsgStream log(msgSvc(), "LArPedestalCompleteCnv" ); 
        log << MSG::DEBUG << "Reading LArPedestalSubset (original)" << endmsg; 

        std::unique_ptr< LArConditionsSubset<LArPedestalP> > subset ( poolReadObject< LArConditionsSubset<LArPedestalP> >() );
        // Here we must convert from LArPedestalP to LArPedestalP1
        
        log << MSG::DEBUG << "subset ptr " << subset.get() << endmsg; 

        return (createTransient(subset.get()));

    } 
    throw std::runtime_error("Unsupported persistent version of LArPedestalCompleteCnv");
}

LArConditionsSubset<LArPedestalP1>* 
LArPedestalCompleteCnv::createTransient(LArConditionsSubset<LArPedestalP>* orig)
{

    MsgStream log(msgSvc(), "LArPedestalCompleteCnv" ); 
    log << MSG::DEBUG << "LArPedestalCompleteCnv::createTransient orig " << orig << endmsg; 

    LArConditionsSubset<LArPedestalP1>* result = new LArConditionsSubset<LArPedestalP1>();
    
    // Copy LArPedestalP subset to LArPedestalP1
    LArPedestalCopy copier;
    copier.copyOldtoNew(orig, result);

    return (result);
}
    

// Copy LArPedestalP subset to LArPedestalP1
void 
LArPedestalCopy::copyOldtoNew(const LArConditionsSubset<LArPedestalP>* oldPedestal,
                              LArConditionsSubset<LArPedestalP1>* newPedestal)
{
  newPedestal->assign (*oldPedestal,
                       [] (const LArPedestalP& from,
                           LArPedestalP1& to)
                       {
                         to.m_Pedestal = from.m_vPedestal.empty() ? 0 : from.m_vPedestal[0];
                         to.m_PedestalRMS = from.m_vPedestalRMS.empty() ? 0 : from.m_vPedestalRMS[0];
                       });
}


