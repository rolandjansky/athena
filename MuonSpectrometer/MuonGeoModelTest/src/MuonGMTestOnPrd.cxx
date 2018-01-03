/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 MuonGeoModel description
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: MuonGMTestOnPrd.cxx,v 1.8 2009-03-28 10:59:01 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

#include "GaudiKernel/MsgStream.h"
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

MuonGMTestOnPrd::MuonGMTestOnPrd(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm               ( name, pSvcLocator ),
    m_MuonMgr		    ( 0 ),
    m_RpcIdHelper           ( 0 ),
    m_TgcIdHelper           ( 0 ),
    m_CscIdHelper           ( 0 ),
    m_MdtIdHelper           ( 0 ),
    m_mdt(true),
    m_rpc(true),
    m_tgc(true),
    m_csc(true),
    m_check_misal(false)
{
    declareProperty("doMDT", m_mdt );        
    declareProperty("doRPC", m_rpc );        
    declareProperty("doTGC", m_tgc );        
    declareProperty("doCSC", m_csc );        
    declareProperty("doCheckMisal", m_check_misal );        
}

MuonGMTestOnPrd::~MuonGMTestOnPrd()
{ }

StatusCode
MuonGMTestOnPrd::initialize()
{
  ATH_MSG_DEBUG("in initialize" );

  ATH_CHECK( detStore()->retrieve( m_MuonMgr ) );
  ATH_MSG_DEBUG( " MuonDetectorManager  is retrieved "  );
  m_CscIdHelper = m_MuonMgr->cscIdHelper();
  m_RpcIdHelper = m_MuonMgr->rpcIdHelper();
  m_TgcIdHelper = m_MuonMgr->tgcIdHelper();
  m_MdtIdHelper = m_MuonMgr->mdtIdHelper();
  ATH_MSG_DEBUG( " Id Helpers are obtained from MuonDetectorManager "  );

  // test when is the geometry really loaded
  ATH_MSG_INFO("# of Mdt/Rpc/Tgc/Csc ReadoutElements "<<m_MuonMgr->nMdtRE()<<"/"<<m_MuonMgr->nRpcRE()<<"/"<<m_MuonMgr->nTgcRE()<<"/"<<m_MuonMgr->nCscRE() );

  return  StatusCode::SUCCESS;
}


StatusCode
MuonGMTestOnPrd::execute()
{
    ATH_MSG_DEBUG( "Executing"  );

    std::string key = "XXX_Measurements";

    if (m_mdt) 
    {
        // Mdt 
        key = "MDT_DriftCircles";
        const Muon::MdtPrepDataContainer * mdtContainer = nullptr;
        ATH_CHECK( evtStore()->retrieve(mdtContainer, key) );
    
        ATH_MSG_INFO("Retrieved MDT PRD Container with size = " << mdtContainer->size() );
    
        //     const DataHandle<MdtPrepDataCollection> mdtColl;
        //     const DataHandle<MdtPrepDataCollection> lastMdtColl;
        //     if (StatusCode::SUCCESS != p_EventStore->retrieve(mdtColl,lastMdtColl)) {
        //         log << MSG::DEBUG << "No MdtPrepDataCollections found" << endmsg;
        //     }
        //     if (mdtColl==lastMdtColl) log<<MSG::DEBUG<<"NO MdtPrepDataCollections in the event"<<endmsg;

        for (const Muon::MdtPrepDataCollection* mdtColl : *mdtContainer)
        {
            if (!mdtColl->empty())
            {
                Identifier collid = mdtColl->identify();
                IdentifierHash collidh = mdtColl->identifyHash();
                ATH_MSG_INFO("Mdt Collection "<<m_MdtIdHelper->show_to_string(collid)<<" hashId = "<<(int)collidh<<" found with size "<<mdtColl->size() );
                processMdtCollection(mdtColl, collid, collidh);
                //processMdtCollectionOld(mdtColl, collid, collidh);
            }
            else 
            {
                Identifier collid = mdtColl->identify();
                ATH_MSG_INFO("Mdt Collection "<<m_MdtIdHelper->show_to_string(collid)<<" is empty" );
            }        
        }    
    }

    if (m_rpc)
    {
        // Rpc 
        key = "RPC_Measurements";
        const Muon::RpcPrepDataContainer * rpcContainer = nullptr;
        ATH_CHECK( evtStore()->retrieve(rpcContainer, key) );
    
        ATH_MSG_INFO("Retrieved RPC PRD Container with size = " << rpcContainer->size() );

        int ict = 0;
        int ictphi = 0;
        int icteta = 0;
        int icttrg = 0;
        int ictamb = 0;
        for (const Muon::RpcPrepDataCollection* rpcColl : *rpcContainer)
        {
            int ncoll = 0;
            if (!rpcColl->empty())
            {
                Identifier collid = rpcColl->identify();
                IdentifierHash collidh = rpcColl->identifyHash();
                ATH_MSG_INFO("Rpc Collection "<<m_RpcIdHelper->show_to_string(collid)<<" hashId = "<<(int)collidh<<" found with size "<<rpcColl->size() );
                Muon::RpcPrepDataCollection::const_iterator it_rpcPrepData;
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
                        if (m_RpcIdHelper->measuresPhi((*it_rpcPrepData)->identify())) 
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
                    ATH_MSG_INFO(ict<<" in this coll. "<<icc<<" prepData id = "
                                 <<m_RpcIdHelper->show_to_string((*it_rpcPrepData)->identify())
                                 <<" time "<<(*it_rpcPrepData)->time()<<" triggerInfo "<<(*it_rpcPrepData)->triggerInfo()<<" ambiguityFlag "<<(*it_rpcPrepData)->ambiguityFlag() );
                    if (m_check_misal)
                    {
                        const MuonGM::RpcReadoutElement* rpcRE = (*it_rpcPrepData)->detectorElement();
                        Amg::Vector3D cgg = rpcRE->center((*it_rpcPrepData)->identify());
                        ATH_MSG_INFO("     position of the center of the gas gap is "<<cgg.x()<<" "<<cgg.y()<<" "<<cgg.z() );
                    }                    
                }
                ncoll++;
                ATH_MSG_INFO("*** Collection "<<ncoll<<" Summary: "
                             <<icctrg<<" trigger hits / "
                             <<iccphi<<" phi hits / "
                             <<icceta<<" eta hits " );
                ATH_MSG_INFO("--------------------------------------------------------------------------------------------" );
            }
        }
    }
    
    if (m_tgc)
    {
        // Tgc 
        key = "TGC_Measurements";
        const Muon::TgcPrepDataContainer * tgcContainer = nullptr;
        ATH_CHECK( evtStore()->retrieve(tgcContainer, key) );
    
        ATH_MSG_INFO("Retrieved TGC PRD Container with size = " << tgcContainer->size() );
    
        for (const Muon::TgcPrepDataCollection* tgcColl : *tgcContainer)
        {
            int ncoll = 0;
            if (!tgcColl->empty())
            {
                ncoll++;
                Identifier collid = tgcColl->identify();
                IdentifierHash collidh = tgcColl->identifyHash();
                ATH_MSG_INFO("Tgc Collection "<<m_TgcIdHelper->show_to_string(collid)<<" hashId = "<<(int)collidh<<" found with size "<<tgcColl->size() );
                Muon::TgcPrepDataCollection::const_iterator it_tgcPrepData;
                
                for (it_tgcPrepData=tgcColl->begin(); it_tgcPrepData != tgcColl->end(); it_tgcPrepData++) {
                  ATH_MSG_INFO("in Coll n. "<<ncoll<<"      PrepData Offline ID "
                               <<m_TgcIdHelper->show_to_string((*it_tgcPrepData)->identify()) );
                }
            }
        }
    }

    if (m_csc)
    {
        // Csc 
        key = "CSC_Clusters";
        const Muon::CscPrepDataContainer * cscContainer = nullptr;
        ATH_CHECK( evtStore()->retrieve(cscContainer, key) );
    
        ATH_MSG_INFO("Retrieved CSC PRD Container with size = " << cscContainer->size() );

        int ict = 0;
        for (const Muon::CscPrepDataCollection* cscColl : *cscContainer)
        {
            int ncoll = 0;
            if (!cscColl->empty())
            {
                Identifier collid = cscColl->identify();
                IdentifierHash collidh = cscColl->identifyHash();
                ATH_MSG_INFO("Csc Collection "<<m_CscIdHelper->show_to_string(collid)<<" hashId = "<<(int)collidh<<" found with size "<<cscColl->size() );
                Muon::CscPrepDataCollection::const_iterator it_cscPrepData;
                int icc = 0;
                int iccphi = 0;
                int icceta = 0;
                for (it_cscPrepData=cscColl->begin(); it_cscPrepData != cscColl->end(); ++it_cscPrepData) {
                    icc++;
                    ict++;
                    if (m_CscIdHelper->measuresPhi((*it_cscPrepData)->identify()))
                        iccphi++;
                    else
                        icceta++;
                    
                    ATH_MSG_INFO( ict <<" in this coll. " << icc << " prepData id = "
                                  << m_CscIdHelper->show_to_string((*it_cscPrepData)->identify())  );
                    if (m_check_misal)
                    {
                        const MuonGM::CscReadoutElement* cscRE = (const MuonGM::CscReadoutElement*)(*it_cscPrepData)->detectorElement();
                        Amg::Vector3D cgg = cscRE->center((*it_cscPrepData)->identify());
                        ATH_MSG_INFO("     position of the center of the gas gap is "<<cgg.x()<<" "<<cgg.y()<<" "<<cgg.z() );
                    }          
                }
                ncoll++;
                ATH_MSG_INFO( "*** Collection " << ncoll << " Summary: "
                              << iccphi <<" phi hits / " << icceta << " eta hits "  );
                ATH_MSG_INFO( "-------------------------------------------------------------"  );
            }
        }
    }

//     // Rpc
//     Muon::RpcPrepDataContainer * rpcContainer;

//     // Tgc 
//     TgcPrepDataContainer * tgcContainer;
//     if ( StatusCode::SUCCESS != p_EventStore->retrieve(tgcContainer, key) )
//     {
//         log<<MSG::ERROR<<"Cannot retrieve TGC PRD Container "<<endmsg;
//         return StatusCode::FAILURE;
//     }
//     else
//         log<<MSG::INFO<<"Retrieved TGC PRD Container with size = " << tgcContainer->size()<< endmsg;

//     // Csc
//     CscPrepDataContainer * cscContainer;
    
    return StatusCode::SUCCESS;
}

StatusCode
MuonGMTestOnPrd::finalize()
{
  ATH_MSG_DEBUG( "Finalizing"  );
  return StatusCode::SUCCESS;
}

void MuonGMTestOnPrd::processMdtCollection(const Muon::MdtPrepDataCollection* mdtColl,
                                           Identifier& collid,
                                           IdentifierHash& collidh) const 
{
    const MuonGM::MdtDetectorElement* mdtDE = m_MuonMgr->getMdtDetectorElement(collidh);
    if (!mdtDE)
    {
        ATH_MSG_ERROR("Impossible to retrieve MdtDetectorElement for collection with hashId = "<<(int)collidh );
        return;
    }
    for (const Muon::MdtPrepData *mdtPrepDatum : *mdtColl)
    {
        Identifier idchannel = mdtPrepDatum->identify();

        Amg::Vector3D xc = mdtDE->center(idchannel);
        ATH_MSG_INFO("Tube = "<<m_MdtIdHelper->show_to_string(collid)<<"    tdc = "<<mdtPrepDatum->tdc()<<"   adc = "<<mdtPrepDatum->adc() );
        if (m_check_misal)
        {
          ATH_MSG_INFO("     position of the center of the tube is ("<<xc.x()<<", "<<xc.y()<<", "<<xc.z()<<")" );
        }          
    }
    return;
}
void MuonGMTestOnPrd::processMdtCollectionOld(const Muon::MdtPrepDataCollection* mdtColl,
                                              Identifier& collid,
                                              IdentifierHash& /*collidh*/) const 
{
    for (const Muon::MdtPrepData *mdtPrepDatum : *mdtColl)
    {
        Identifier idchannel = mdtPrepDatum->identify();

        const MuonGM::MdtReadoutElement* mdtRE = m_MuonMgr->getMdtReadoutElement(collid);
        if (!mdtRE)
        {
            ATH_MSG_ERROR("Impossible to retrive MdtReadoutElement for prd with Id = "<<m_MdtIdHelper->show_to_string(idchannel) );
            return;
        }

        
        Amg::Vector3D xc = mdtRE->center(idchannel);
        //Amg::Vector3D xct = (mdtRE->transform(idchannel))*Amg::Vector3D(0.,0.,0.);
        //Amg::Vector3D xvt = (mdtRE->transform(idchannel))*Amg::Vector3D(10.,10.,10.);
        Trk::SaggedLineSurface surf = mdtRE->surface(idchannel);
        
        //log<<MSG::INFO<<"Tube center is ("<<xc.x()<<", "<<xc.y()<<", "<<xc.z()<<")"<<endmsg;

        if (msgLvl(MSG::DEBUG))
        {
          ATH_MSG_DEBUG("center "<<xc );
          surf.dump(msg());
          ATH_MSG_DEBUG ("\n surface transform center "<<(surf.transform())*Amg::Vector3D(0.,0.,0) );
        }
        
    }
    return;
}
