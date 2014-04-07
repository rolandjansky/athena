/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 MuonGeoModel description
 -----------------------------------------
 Copyright (C) 2004 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: MuonGMTestOnPrd.cxx,v 1.8 2009-03-28 10:59:01 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PropertyMgr.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"

#include "MuonGeoModelTest/MuonGMTestOnPrd.h"

#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/MdtDetectorElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"

// #include "MuonPrepRawData/MdtPrepDataContainer.h"
// #include "MuonPrepRawData/MdtPrepDataCollection.h"
// #include "MuonPrepRawData/MdtPrepData.h"

// #include "Identifier/Identifier.h"
// #include "Identifier/IdentifierHash.h"
#include "TrkDistortedSurfaces/SaggedLineSurface.h"

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>


MuonGMTestOnPrd::MuonGMTestOnPrd(const std::string& name, ISvcLocator* pSvcLocator)
  : Algorithm               ( name, pSvcLocator ),
    m_debug(false),
    m_verbose(false),
    p_EventStore            ( 0 ),
    p_MuonMgr		    ( 0 ),
    p_RpcIdHelper           ( 0 ),
    p_TgcIdHelper           ( 0 ),
    p_CscIdHelper           ( 0 ),
    p_MdtIdHelper           ( 0 ),
    _mdt(true),
    _rpc(true),
    _tgc(true),
    _csc(true),
    _check_misal(false)
{
    declareProperty("doMDT", _mdt );        
    declareProperty("doRPC", _rpc );        
    declareProperty("doTGC", _tgc );        
    declareProperty("doCSC", _csc );        
    declareProperty("doCheckMisal", _check_misal );        
}

MuonGMTestOnPrd::~MuonGMTestOnPrd()
{ }

StatusCode
MuonGMTestOnPrd::initialize()
{
    StatusCode status = StatusCode::SUCCESS;

    MsgStream ini_log(messageService(), name());
    ini_log <<MSG::DEBUG<<"in initialize"<<endreq;
    m_debug = (ini_log.level()<=MSG::DEBUG);
    m_verbose = (ini_log.level()<=MSG::VERBOSE);
    

    // Locate the StoreGateSvc and initialize our local ptr
    status = serviceLocator()->service("StoreGateSvc", p_EventStore);
    if (!status.isSuccess() || 0 == p_EventStore) {
        ini_log << MSG::ERROR << " Could not find StoreGateSvc" << endreq;
        return status;
    }
    else ini_log << MSG::DEBUG << " StoreGateSvc found" << endreq;
    
    // Locate the DetectorStore
    StoreGateSvc* detStore=0;
    status = serviceLocator()->service("DetectorStore", detStore);
    if ( status.isSuccess() ) {
        status = detStore->retrieve( p_MuonMgr );
        if ( status.isFailure() ) {
            ini_log << MSG::ERROR << " Cannot retrieve MuonDetectorManager " << endreq;
        }
        else
        {
            ini_log << MSG::DEBUG << " MuonDetectorManager  is retriven " << endreq;
            p_CscIdHelper = p_MuonMgr->cscIdHelper();
            p_RpcIdHelper = p_MuonMgr->rpcIdHelper();
            p_TgcIdHelper = p_MuonMgr->tgcIdHelper();
            p_MdtIdHelper = p_MuonMgr->mdtIdHelper();
            ini_log << MSG::DEBUG << " Id Helpers are obtained from MuonDetectorManager " << endreq;

            // test when is the geometry really loaded
            ini_log <<MSG::INFO<<"# of Mdt/Rpc/Tgc/Csc ReadoutElements "<<p_MuonMgr->nMdtRE()<<"/"<<p_MuonMgr->nRpcRE()<<"/"<<p_MuonMgr->nTgcRE()<<"/"<<p_MuonMgr->nCscRE()<<endreq;
        }
    }
    else {
        ini_log << MSG::ERROR << " DetectorStore not accessible" << endreq;
    }

    return  status;
}


StatusCode
MuonGMTestOnPrd::execute()
{
    StatusCode status = StatusCode::SUCCESS;

    MsgStream log(messageService(), name());
    log << MSG::DEBUG << "Executing" << endreq;

    std::string key = "XXX_Measurements";

    if (_mdt) 
    {
        // Mdt 
        key = "MDT_DriftCircles";
        const MdtPrepDataContainer * mdtContainer;
        if ( StatusCode::SUCCESS != p_EventStore->retrieve(mdtContainer, key) )
        {
            log<<MSG::ERROR<<"Cannot retrieve MDT PRD Container "<<endreq;
            return StatusCode::FAILURE;
        }
    
        log<<MSG::INFO<<"Retrieved MDT PRD Container with size = " << mdtContainer->size()<< endreq;
    
        //     const DataHandle<MdtPrepDataCollection> mdtColl;
        //     const DataHandle<MdtPrepDataCollection> lastMdtColl;
        //     if (StatusCode::SUCCESS != p_EventStore->retrieve(mdtColl,lastMdtColl)) {
        //         log << MSG::DEBUG << "No MdtPrepDataCollections found" << endreq;
        //     }
        //     if (mdtColl==lastMdtColl) log<<MSG::DEBUG<<"NO MdtPrepDataCollections in the event"<<endreq;

        for (MdtPrepDataContainer::const_iterator mdtColli = mdtContainer->begin(); mdtColli!=mdtContainer->end(); ++mdtColli)
        {
            const Muon::MdtPrepDataCollection* mdtColl = *mdtColli;
            if (!mdtColl->empty())
            {
                Identifier collid = mdtColl->identify();
                IdentifierHash collidh = mdtColl->identifyHash();
                log<<MSG::INFO<<"Mdt Collection "<<p_MdtIdHelper->show_to_string(collid)<<" hashId = "<<(int)collidh<<" found with size "<<mdtColl->size()<<endreq;
                processMdtCollection(mdtColl, collid, collidh);
                //processMdtCollectionOld(mdtColl, collid, collidh);
            }
            else 
            {
                Identifier collid = mdtColl->identify();
                log<<MSG::INFO<<"Mdt Collection "<<p_MdtIdHelper->show_to_string(collid)<<" is empty"<<endreq;
            }        
        }    
    }

    if (_rpc)
    {
        // Rpc 
        key = "RPC_Measurements";
        const RpcPrepDataContainer * rpcContainer;
        if ( StatusCode::SUCCESS != p_EventStore->retrieve(rpcContainer, key) )
        {
            log<<MSG::ERROR<<"Cannot retrieve RPC PRD Container "<<endreq;
            return StatusCode::FAILURE;
        }
    
        log<<MSG::INFO<<"Retrieved RPC PRD Container with size = " << rpcContainer->size()<< endreq;

        int ict = 0;
        int ictphi = 0;
        int icteta = 0;
        int icttrg = 0;
        int ictamb = 0;
        for (RpcPrepDataContainer::const_iterator rpcColli = rpcContainer->begin(); rpcColli!=rpcContainer->end(); ++rpcColli)
        {
            int ncoll = 0;
            const Muon::RpcPrepDataCollection* rpcColl = *rpcColli;
            if (!rpcColl->empty())
            {
                Identifier collid = rpcColl->identify();
                IdentifierHash collidh = rpcColl->identifyHash();
                log<<MSG::INFO<<"Rpc Collection "<<p_RpcIdHelper->show_to_string(collid)<<" hashId = "<<(int)collidh<<" found with size "<<rpcColl->size()<<endreq;
                RpcPrepDataCollection::const_iterator it_rpcPrepData;
                int icc = 0;
                int iccphi = 0;
                int icceta = 0;
                int icctrg = 0;
                for (it_rpcPrepData=rpcColl->begin(); it_rpcPrepData != rpcColl->end(); it_rpcPrepData++) {
                    icc++;
                    ict++;
                    if ((*it_rpcPrepData)->triggerInfo() > 0) 
                    {
                        icctrg++;
                        icttrg++;
                    }
                    else
                    {                    
                        if (p_RpcIdHelper->measuresPhi((*it_rpcPrepData)->identify())) 
                        {
                            iccphi++;
                            ictphi++;
                            if ((*it_rpcPrepData)->ambiguityFlag()>1) ictamb++;
                        }
                        else 
                        {
                            icceta++;
                            icteta++;
                        }                    
                    }
                    log << MSG::INFO<<ict<<" in this coll. "<<icc<<" prepData id = "
                          <<p_RpcIdHelper->show_to_string((*it_rpcPrepData)->identify())
                          <<" time "<<(*it_rpcPrepData)->time()<<" triggerInfo "<<(*it_rpcPrepData)->triggerInfo()<<" ambiguityFlag "<<(*it_rpcPrepData)->ambiguityFlag()<<endreq;
                    if (_check_misal)
                    {
                        const RpcReadoutElement* rpcRE = (*it_rpcPrepData)->detectorElement();
                        Amg::Vector3D cgg = rpcRE->center((*it_rpcPrepData)->identify());
                        log << MSG::INFO<<"     position of the center of the gas gap is "<<cgg.x()<<" "<<cgg.y()<<" "<<cgg.z()<<endreq;
                    }                    
                }
                ncoll++;
                log << MSG::INFO<<"*** Collection "<<ncoll<<" Summary: "
                      <<icctrg<<" trigger hits / "
                      <<iccphi<<" phi hits / "
                      <<icceta<<" eta hits "<<endreq;
                log << MSG::INFO<<"--------------------------------------------------------------------------------------------"<<endreq;
            }
        }
    }
    
    if (_tgc)
    {
        // Tgc 
        key = "TGC_Measurements";
        const TgcPrepDataContainer * tgcContainer;
        if ( StatusCode::SUCCESS != p_EventStore->retrieve(tgcContainer, key) )
        {
            log<<MSG::ERROR<<"Cannot retrieve TGC PRD Container "<<endreq;
            return StatusCode::FAILURE;
        }
    
        log<<MSG::INFO<<"Retrieved TGC PRD Container with size = " << tgcContainer->size()<< endreq;
    
        for (TgcPrepDataContainer::const_iterator tgcColli = tgcContainer->begin(); tgcColli!=tgcContainer->end(); ++tgcColli)
        {
            int ncoll = 0;
            const Muon::TgcPrepDataCollection* tgcColl = *tgcColli;
            if (!tgcColl->empty())
            {
                ncoll++;
                Identifier collid = tgcColl->identify();
                IdentifierHash collidh = tgcColl->identifyHash();
                log<<MSG::INFO<<"Tgc Collection "<<p_TgcIdHelper->show_to_string(collid)<<" hashId = "<<(int)collidh<<" found with size "<<tgcColl->size()<<endreq;
                TgcPrepDataCollection::const_iterator it_tgcPrepData;
                
                for (it_tgcPrepData=tgcColl->begin(); it_tgcPrepData != tgcColl->end(); it_tgcPrepData++) {
                    log << MSG::INFO<<"in Coll n. "<<ncoll<<"      PrepData Offline ID "
                          <<p_TgcIdHelper->show_to_string((*it_tgcPrepData)->identify())
                          <<endreq;
                }
            }
        }
    }

    if (_csc)
    {
        // Csc 
        key = "CSC_Clusters";
        const CscPrepDataContainer * cscContainer;
        if ( StatusCode::SUCCESS != p_EventStore->retrieve(cscContainer, key) )
        {
            log<<MSG::ERROR<<"Cannot retrieve CSC PRD Container "<<endreq;
            return StatusCode::FAILURE;
        }
    
        log<<MSG::INFO<<"Retrieved CSC PRD Container with size = " << cscContainer->size()<< endreq;

        int ict = 0;
        for (CscPrepDataContainer::const_iterator cscColli = cscContainer->begin(); cscColli!=cscContainer->end(); ++cscColli)
        {
            int ncoll = 0;
            const Muon::CscPrepDataCollection* cscColl = *cscColli;
            if (!cscColl->empty())
            {
                Identifier collid = cscColl->identify();
                IdentifierHash collidh = cscColl->identifyHash();
                log<<MSG::INFO<<"Csc Collection "<<p_CscIdHelper->show_to_string(collid)<<" hashId = "<<(int)collidh<<" found with size "<<cscColl->size()<<endreq;
                CscPrepDataCollection::const_iterator it_cscPrepData;
                int icc = 0;
                int iccphi = 0;
                int icceta = 0;
                for (it_cscPrepData=cscColl->begin(); it_cscPrepData != cscColl->end(); ++it_cscPrepData) {
                    icc++;
                    ict++;
                    if (p_CscIdHelper->measuresPhi((*it_cscPrepData)->identify()))
                        iccphi++;
                    else
                        icceta++;
                    
                    log << MSG::INFO << ict <<" in this coll. " << icc << " prepData id = "
                           << p_CscIdHelper->show_to_string((*it_cscPrepData)->identify()) << endreq;
                    if (_check_misal)
                    {
                        const CscReadoutElement* cscRE = (const CscReadoutElement*)(*it_cscPrepData)->detectorElement();
                        Amg::Vector3D cgg = cscRE->center((*it_cscPrepData)->identify());
                        log << MSG::INFO<<"     position of the center of the gas gap is "<<cgg.x()<<" "<<cgg.y()<<" "<<cgg.z()<<endreq;
                    }          
                }
                ncoll++;
                log << MSG::INFO << "*** Collection " << ncoll << " Summary: "
                       << iccphi <<" phi hits / " << icceta << " eta hits " << endreq;
                log << MSG::INFO << "-------------------------------------------------------------" << endreq;           
            }
        }
    }

//     // Rpc
//     RpcPrepDataContainer * rpcContainer;

//     // Tgc 
//     TgcPrepDataContainer * tgcContainer;
//     if ( StatusCode::SUCCESS != p_EventStore->retrieve(tgcContainer, key) )
//     {
//         log<<MSG::ERROR<<"Cannot retrieve TGC PRD Container "<<endreq;
//         return StatusCode::FAILURE;
//     }
//     else
//         log<<MSG::INFO<<"Retrieved TGC PRD Container with size = " << tgcContainer->size()<< endreq;

//     // Csc
//     CscPrepDataContainer * cscContainer;
    
    return status;
}

StatusCode
MuonGMTestOnPrd::finalize()
{
    StatusCode status = StatusCode::SUCCESS;
        
    MsgStream fin_log(messageService(), name());
    fin_log << MSG::DEBUG << "Finalizing" << endreq;
     
    return status;
}

void MuonGMTestOnPrd::processMdtCollection(const MdtPrepDataCollection* mdtColl,
                                           Identifier& collid,
                                           IdentifierHash& collidh) const 
{

    MsgStream log(messageService(), name());
    typedef MdtPrepDataCollection::const_iterator prd_iterator; 
    prd_iterator it_prep = mdtColl->begin();
    prd_iterator it_prep_end = mdtColl->end();

    const MdtDetectorElement* mdtDE = p_MuonMgr->getMdtDetectorElement(collidh);
    if (!mdtDE)
    {
        MsgStream log(messageService(), name()+"::processMdtCollection");
        log<<MSG::ERROR<<"Impossible to retrive MdtDetectorElement for collection with hashId = "<<(int)collidh<<endreq;
        return;
    }
    for( ; it_prep != it_prep_end; ++it_prep)
    {
        MdtPrepData *mdtPrepDatum=(*it_prep);
        Identifier idchannel = mdtPrepDatum->identify();

        Amg::Vector3D xc = mdtDE->center(idchannel);
        log<<MSG::INFO<<"Tube = "<<p_MdtIdHelper->show_to_string(collid)<<"    tdc = "<<mdtPrepDatum->tdc()<<"   adc = "<<mdtPrepDatum->adc()<<endreq;
        if (_check_misal)
        {
            log << MSG::INFO<<"     position of the center of the tube is ("<<xc.x()<<", "<<xc.y()<<", "<<xc.z()<<")"<<endreq;
        }          
    }
    return;
}
void MuonGMTestOnPrd::processMdtCollectionOld(const MdtPrepDataCollection* mdtColl,
                                           Identifier& collid,
                                              IdentifierHash& /*collidh*/) const 
{

    typedef MdtPrepDataCollection::const_iterator prd_iterator; 
    prd_iterator it_prep = mdtColl->begin();
    prd_iterator it_prep_end = mdtColl->end();

    for( ; it_prep != it_prep_end; ++it_prep)
    {
        MdtPrepData *mdtPrepDatum=(*it_prep);
        Identifier idchannel = mdtPrepDatum->identify();

        const MdtReadoutElement* mdtRE = p_MuonMgr->getMdtReadoutElement(collid);
        if (!mdtRE)
        {
            MsgStream log(messageService(), name()+"::processMdtCollectionOld");
            log<<MSG::ERROR<<"Impossible to retrive MdtReadoutElement for prd with Id = "<<p_MdtIdHelper->show_to_string(idchannel)
               <<endreq;
            return;
        }

        
        Amg::Vector3D xc = mdtRE->center(idchannel);
        //Amg::Vector3D xct = (mdtRE->transform(idchannel))*Amg::Vector3D(0.,0.,0.);
        //Amg::Vector3D xvt = (mdtRE->transform(idchannel))*Amg::Vector3D(10.,10.,10.);
        Trk::SaggedLineSurface surf = mdtRE->surface(idchannel);
        
        //log<<MSG::INFO<<"Tube center is ("<<xc.x()<<", "<<xc.y()<<", "<<xc.z()<<")"<<endreq;

        if (m_debug)
        {
          MsgStream log(messageService(), name()+"::processMdtCollectionOld");
          log<<MSG::DEBUG<<"center "<<xc<<endreq;
          surf.dump(log);
          log<<"\n surface transform center "<<(surf.transform())*Amg::Vector3D(0.,0.,0.)<<endreq;
        }
        
    }
    return;
}
