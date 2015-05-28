/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauClusterContainerCnv.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainer_tlp1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainerCnv_tlp1.h"

#include "TrigCaloEventTPCnv/TrigTauClusterContainer_p3.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainerCnv_p3.h"

#include "TrigCaloEventTPCnv/TrigTauClusterContainer_p4.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainerCnv_p4.h"

#include "TrigCaloEventTPCnv/TrigTauClusterContainer_p5.h"
#include "TrigCaloEventTPCnv/TrigTauClusterContainerCnv_p5.h"

static TrigTauClusterContainerCnv_tlp1   TPconverter1;
static TrigTauClusterContainerCnv_p3     TPconverter3;
static TrigTauClusterContainerCnv_p4     TPconverter4;
static TrigTauClusterContainerCnv_p5     TPconverter5;

//createPersistent 
TrigTauClusterContainer_PERS* TrigTauClusterContainerCnv::createPersistent(TrigTauClusterContainer *transObj)
{
    MsgStream mlog(messageService(), "TrigTauClusterContainerConverter");

    mlog << MSG::DEBUG << "TrigTauClusterContainerCnv::createPersistent called" << endreq;

    TrigTauClusterContainer_PERS* p_TauClusterCont = TPconverter5.createPersistent(transObj, mlog);

    return p_TauClusterCont;

}//end of create persistent method


//createTransient
TrigTauClusterContainer* TrigTauClusterContainerCnv::createTransient()
{
    MsgStream mlog(messageService(), "TrigTauClusterContainerConverter");

    mlog << MSG::DEBUG << "TrigTauClusterContainerCnv::createTransient called" << endreq;

    static pool::Guid p5_guid("8384F60B-952E-4DE4-B307-1AC1C405E156");
    static pool::Guid p4_guid("D41E29BA-0FE8-4319-85F4-93EA68AD9195");
    static pool::Guid p3_guid("3BB1A500-2188-47D4-9352-6638DEA3FC7D");
    static pool::Guid tlp1_guid("E1FC5307-A747-42D7-9542-998A9AACB445");
    static pool::Guid tlp2_guid("07437EBD-7637-4C3F-B177-6D640AE36FAD");

    if(compareClassGuid(p5_guid))
    {
        std::auto_ptr< TrigTauClusterContainer_p5 > col_vect(poolReadObject< TrigTauClusterContainer_p5 >());
        //std::cout << "Reading TTCC p5" << std::endl;
        return TPconverter5.createTransient(col_vect.get(), mlog);
    }
    else if(compareClassGuid(p4_guid))
    {
        std::auto_ptr< TrigTauClusterContainer_p4 > col_vect(poolReadObject< TrigTauClusterContainer_p4 >());
        //std::cout << "Reading TTCC p4" << std::endl;
        return TPconverter4.createTransient(col_vect.get(), mlog);
    }
    else if(compareClassGuid(p3_guid))
    {
        std::auto_ptr< TrigTauClusterContainer_p3 > col_vect(poolReadObject< TrigTauClusterContainer_p3 >());
        //std::cout << "Reading TTCC p3" << std::endl;
        return TPconverter3.createTransient(col_vect.get(), mlog);
    }
    else if(compareClassGuid(tlp1_guid))
    {
        std::auto_ptr< TrigTauClusterContainer_tlp1 > col_vect(poolReadObject< TrigTauClusterContainer_tlp1 >());
        //std::cout << "Reading TTC tlp1" << std::endl;
        return TPconverter1.createTransient( col_vect.get(), mlog );
    }
    else
    {
        throw std::runtime_error("Unsupported persistent version of TrigTauClusterContainer");
    }

}//end of create transient method

