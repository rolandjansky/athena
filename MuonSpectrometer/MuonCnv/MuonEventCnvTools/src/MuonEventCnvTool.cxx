/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonEventCnvTools/MuonEventCnvTool.h"

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
//Muon DD manager
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"

#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonPrepRawData/MdtPrepDataContainer.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"

#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MMClusterOnTrack.h"
#include "MuonRIO_OnTrack/sTgcClusterOnTrack.h"
//Muon helpers
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include <vector>
#include <cassert>
#include <iostream>

#include "EventPrimitives/EventPrimitives.h"


Muon::MuonEventCnvTool::MuonEventCnvTool(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
    :
    AthAlgTool(t,n,p),
    m_muonMgr(0),
    m_rpcClusContName("RPC_Measurements"),
    m_cscClusContName("CSC_Clusters"),
    m_tgcClusContName("TGC_Measurements"),
    m_mdtClusContName("MDT_DriftCircles"),
    m_mmClusContName("MM_Measurements"),
    m_stgcClusContName("STGC_Measurements"),
    m_manuallyFindPRDs(false)
{
    declareInterface<ITrkEventCnvTool>(this);
    declareProperty("RpcClusterContainer", m_rpcClusContName);
    declareProperty("CscClusterContainer", m_cscClusContName);
    declareProperty("TgcClusterContainer", m_tgcClusContName);
    declareProperty("MdtClusterContainer", m_mdtClusContName);    
    declareProperty("MM_ClusterContainer",   m_mdtClusContName);    
    declareProperty("STGC_ClusterContainer", m_mdtClusContName);    
    declareProperty("FindPRDsManually",    m_manuallyFindPRDs);
}

Muon::MuonEventCnvTool::~MuonEventCnvTool()
{
}

StatusCode Muon::MuonEventCnvTool::initialize()
{
//	StatusCode sc = IPatToTrackToolBase::initialize();
//	if (sc.isFailure()) return sc;

    // Get Muon Detector Description Manager
    StatusCode sc = detStore()->retrieve(m_muonMgr);
    if (sc.isFailure()) {
        ATH_MSG_FATAL("Could not get MuonReadoutGeometry DetectorDescription manager");
        return StatusCode::FAILURE;
    }else{
        ATH_MSG_DEBUG( "Found MuonReadoutGeometry DetectorDescription manager at :"<<m_muonMgr);
    }

    return StatusCode::SUCCESS;
}

void 
    Muon::MuonEventCnvTool::checkRoT( const Trk::RIO_OnTrack& rioOnTrack ) const
{
    MuonConcreteType type=TypeUnknown;
    if (0!=dynamic_cast<const Muon::RpcClusterOnTrack*>(&rioOnTrack) )        type = RPC;
    if (0!=dynamic_cast<const Muon::CscClusterOnTrack*>(&rioOnTrack) )        type = CSC;
    if (0!=dynamic_cast<const Muon::TgcClusterOnTrack*>(&rioOnTrack) )        type = TGC;
    if (0!=dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(&rioOnTrack) )    type = MDT;
    if (0!=dynamic_cast<const Muon::MMClusterOnTrack*>(&rioOnTrack) )         type = MM;
    if (0!=dynamic_cast<const Muon::sTgcClusterOnTrack*>(&rioOnTrack) )       type = STGC;
    if (type==TypeUnknown) {
        ATH_MSG_ERROR("Type does not match known concrete type of MuonSpectrometer! Dumping RoT:"<<rioOnTrack);
    }else{
      ATH_MSG_VERBOSE("Type = "<<type<<"(RPC="<<RPC<<", CSC="<<CSC<<", TGC="<<TGC<<"MDT="<<MDT<<", STGC="<<STGC<<"MM="<<MM<<")");
    }

    return;
}



std::pair<const Trk::TrkDetElementBase*, const Trk::PrepRawData*> 
    Muon::MuonEventCnvTool::getLinks( const Trk::RIO_OnTrack& rioOnTrack ) const
{
    using namespace Trk;
    using namespace MuonGM;

    const TrkDetElementBase* detEl = 0;
    const PrepRawData*       prd   = 0;
    const Identifier& id           = rioOnTrack.identify();

    if ( m_muonMgr!=0) {
        //TODO Check that these are in the most likely ordering, for speed. EJWM.
      if (m_muonMgr->rpcIdHelper()->is_rpc(id)){
        detEl =  m_muonMgr->getRpcReadoutElement( id ) ;
        if (m_manuallyFindPRDs) prd = rpcClusterLink(id, rioOnTrack.idDE());
      } else if(m_muonMgr->cscIdHelper()->is_csc(id)){
        detEl =  m_muonMgr->getCscReadoutElement( id ) ;
        if (m_manuallyFindPRDs) prd = cscClusterLink(id, rioOnTrack.idDE());
      } else if(m_muonMgr->tgcIdHelper()->is_tgc(id)){
        detEl = m_muonMgr->getTgcReadoutElement( id ) ;
        if (m_manuallyFindPRDs) prd = tgcClusterLink(id, rioOnTrack.idDE());
      }else if(m_muonMgr->mdtIdHelper()->is_mdt(id)){
        detEl =  m_muonMgr->getMdtReadoutElement( id ) ;
        if (m_manuallyFindPRDs) prd = mdtDriftCircleLink(id, rioOnTrack.idDE());
      } else if(m_muonMgr->mmIdHelper()->is_mm(id)){
        detEl = m_muonMgr->getMMReadoutElement( id ) ;
        if (m_manuallyFindPRDs) prd = mmClusterLink(id, rioOnTrack.idDE());
      } else if(m_muonMgr->stgcIdHelper()->is_stgc(id)){
        detEl = m_muonMgr->getsTgcReadoutElement( id ) ;
        if (m_manuallyFindPRDs) prd = stgcClusterLink(id, rioOnTrack.idDE());
      }else{
        ATH_MSG_WARNING( "Unknown type of Muon detector from identifier :"<< id);
      }
    }
     //                         if (0==detEl) 
//                            m_log << MSG::WARNING << "Got zero detector element from MDT with Identifier="
//                                <<m_muonMgr->mdtIdHelper()->show_to_string(id)<<", that is station="
//                                <<m_muonMgr->mdtIdHelper()->stationName(id)<<", st. eta="
//                                <<m_muonMgr->mdtIdHelper()->stationEta(id)<<", st. phi="
//                                <<m_muonMgr->mdtIdHelper()->stationPhi(id)<<", multilayer="
//                                <<m_muonMgr->mdtIdHelper()->multilayer(id)<<endmsg;        
    if (detEl==0) ATH_MSG_ERROR( "Apparently could not find detector element for "<< id);
    ATH_MSG_VERBOSE("Found PRD at : "<<prd);    
    ////std::cout<<"Debug: pair = ("<<detEl<<","<<prd<<")"<<std::endl;
    
    return std::pair<const Trk::TrkDetElementBase*, const Trk::PrepRawData*>(detEl,prd); 
}

void Muon::MuonEventCnvTool::prepareRIO_OnTrack( Trk::RIO_OnTrack *RoT ) const {
    
    Muon::MdtDriftCircleOnTrack* mdt = dynamic_cast<Muon::MdtDriftCircleOnTrack*>(RoT);
    if (mdt!=0){
        prepareRIO_OnTrackElementLink<const Muon::MdtPrepDataContainer, Muon::MdtDriftCircleOnTrack>(mdt);
        return;
    }
    Muon::CscClusterOnTrack* csc = dynamic_cast<Muon::CscClusterOnTrack*>(RoT);
    if (csc!=0){
        prepareRIO_OnTrackElementLink<const Muon::CscPrepDataContainer, Muon::CscClusterOnTrack>(csc);
        return;
    }
    Muon::RpcClusterOnTrack* rpc = dynamic_cast<Muon::RpcClusterOnTrack*>(RoT);
    if (rpc!=0){
        prepareRIO_OnTrackElementLink<const Muon::RpcPrepDataContainer, Muon::RpcClusterOnTrack>(rpc);
        return;
    }
    Muon::TgcClusterOnTrack* tgc = dynamic_cast<Muon::TgcClusterOnTrack*>(RoT);
    if (tgc!=0){
        prepareRIO_OnTrackElementLink<const Muon::TgcPrepDataContainer, Muon::TgcClusterOnTrack>(tgc);
        return;
    }   
    Muon::sTgcClusterOnTrack* stgc = dynamic_cast<Muon::sTgcClusterOnTrack*>(RoT);
    if (stgc!=0){
        prepareRIO_OnTrackElementLink<const Muon::sTgcPrepDataContainer, Muon::sTgcClusterOnTrack>(stgc);
        return;
    }
    Muon::MMClusterOnTrack* mm = dynamic_cast<Muon::MMClusterOnTrack*>(RoT);
    if (mm!=0){
        prepareRIO_OnTrackElementLink<const Muon::MMPrepDataContainer, Muon::MMClusterOnTrack>(mm);
        return;
    }     
    return;
}

void Muon::MuonEventCnvTool::recreateRIO_OnTrack( Trk::RIO_OnTrack *RoT ) const {
    
    ////std::cout<<"MuonEventCnvTool::recreateRIO_OnTrack"<<std::endl;
    std::pair<const Trk::TrkDetElementBase *, const Trk::PrepRawData *> pair = getLinks( *RoT );
    if (pair.first)
      Trk::ITrkEventCnvTool::setRoT_Values( pair, RoT );    
    return;
}

const Trk::TrkDetElementBase* 
Muon::MuonEventCnvTool::getDetectorElement(const Identifier& id, const IdentifierHash& /**idHash*/) const
{
    return getDetectorElement(id);
}

const Trk::TrkDetElementBase* 
Muon::MuonEventCnvTool::getDetectorElement(const Identifier& id) const
{
    const Trk::TrkDetElementBase* detEl = 0;

    if ( m_muonMgr!=0 ) 
    {
        //TODO Check that these are in the most likely ordering, for speed. EJWM.
      if (m_muonMgr->rpcIdHelper()->is_rpc(id)) {
        detEl =  m_muonMgr->getRpcReadoutElement( id ) ;
      }else if(m_muonMgr->cscIdHelper()->is_csc(id)){
        detEl =  m_muonMgr->getCscReadoutElement( id ) ;
      }else if(m_muonMgr->tgcIdHelper()->is_tgc(id)){
        detEl = m_muonMgr->getTgcReadoutElement( id ) ;
      }else if(m_muonMgr->mdtIdHelper()->is_mdt(id)) {
        detEl =  m_muonMgr->getMdtReadoutElement( id ) ;
      }else if(m_muonMgr->stgcIdHelper()->is_stgc(id)){
        detEl = m_muonMgr->getsTgcReadoutElement( id ) ;
      }else if(m_muonMgr->mmIdHelper()->is_mm(id)){
        detEl = m_muonMgr->getMMReadoutElement( id ) ;
      }
    }

    if (detEl==0) ATH_MSG_ERROR("Apparently could not find detector element for Identifier: "<< id);
    return detEl;
}

const Trk::PrepRawData* 
    Muon::MuonEventCnvTool::rpcClusterLink( const Identifier& id,  const IdentifierHash& idHash  ) const
{
    using namespace Trk;
    using namespace Muon;

    // need to retrieve pointers to collections

    // obviously this can be optimised! EJWM
    const RpcPrepDataContainer* rpcClusCont;
    StatusCode sc = evtStore()->retrieve(rpcClusCont, m_rpcClusContName);
    if (sc.isFailure()){
        ATH_MSG_ERROR("rpc Cluster container not found at "<<m_rpcClusContName);
        return 0;
    }
    else{
        ATH_MSG_DEBUG("rpc Cluster container found");
    }

    RpcPrepDataContainer::const_iterator it = rpcClusCont->indexFind(idHash);
    // if we find PRD, then recreate link
    if (it!=rpcClusCont->end()) 
    {
        //loop though collection to find matching PRD.
        RpcPrepDataCollection::const_iterator collIt = (*it)->begin();
        RpcPrepDataCollection::const_iterator collItEnd = (*it)->end();
        // there MUST be a faster way to do this!!
        for ( ; collIt!=collItEnd; collIt++){
            if ( (*collIt)->identify()==id ) return *collIt;
        }
    }
    ATH_MSG_DEBUG("No matching PRD found");
    return 0;
}

const Trk::PrepRawData* 
    Muon::MuonEventCnvTool::cscClusterLink( const Identifier& id,  const IdentifierHash& idHash  ) const
{
    using namespace Trk;
    using namespace Muon;

    // need to retrieve pointers to collections

    // obviously this can be optimised! EJWM
    const CscPrepDataContainer* cscClusCont;
    StatusCode sc = evtStore()->retrieve(cscClusCont, m_cscClusContName);
    if (sc.isFailure()){
        ATH_MSG_ERROR("csc Cluster container not found at "<<m_cscClusContName);
        return 0;
    }
    else{
        ATH_MSG_DEBUG("csc Cluster container found" );
    }

    CscPrepDataContainer::const_iterator it = cscClusCont->indexFind(idHash);
    // if we find PRD, then recreate link
    if (it!=cscClusCont->end()) 
    {
        //loop though collection to find matching PRD.
        CscPrepDataCollection::const_iterator collIt = (*it)->begin();
        CscPrepDataCollection::const_iterator collItEnd = (*it)->end();
        // there MUST be a faster way to do this!!
        for ( ; collIt!=collItEnd; collIt++){
            if ( (*collIt)->identify()==id ) return *collIt;
        }
    }
    ATH_MSG_DEBUG("No matching PRD found" );
    return 0;
}

const Trk::PrepRawData* 
    Muon::MuonEventCnvTool::tgcClusterLink( const Identifier& id,  const IdentifierHash& idHash  ) const
{
    using namespace Trk;
    using namespace Muon;
    // need to retrieve pointers to collections

    // obviously this can be optimised! EJWM
    const TgcPrepDataContainer* tgcClusCont;
    StatusCode sc = evtStore()->retrieve(tgcClusCont, m_tgcClusContName);
    if (sc.isFailure()){
        ATH_MSG_ERROR("tgc Cluster container not found at "<<m_tgcClusContName);
        return 0;
    }
    else{
        ATH_MSG_DEBUG("tgc Cluster container found");
    }

    TgcPrepDataContainer::const_iterator it = tgcClusCont->indexFind(idHash);
    // if we find PRD, then recreate link
    if (it!=tgcClusCont->end()) 
    {
        //loop though collection to find matching PRD.
        TgcPrepDataCollection::const_iterator collIt = (*it)->begin();
        TgcPrepDataCollection::const_iterator collItEnd = (*it)->end();
        // there MUST be a faster way to do this!!
        for ( ; collIt!=collItEnd; collIt++){
            if ( (*collIt)->identify()==id ) return *collIt;
        }
    }
    ATH_MSG_DEBUG("No matching PRD found" );
    return 0;
}

const Trk::PrepRawData* 
    Muon::MuonEventCnvTool::mdtDriftCircleLink( const Identifier& id,  const IdentifierHash& idHash  ) const
{
    using namespace Trk;
    using namespace Muon;

    // need to retrieve pointers to collections

    // obviously this can be optimised! EJWM
    const MdtPrepDataContainer* mdtClusCont;
    StatusCode sc = evtStore()->retrieve(mdtClusCont, m_mdtClusContName);
    if (sc.isFailure()){
        ATH_MSG_ERROR("mdt Cluster container not found at "<<m_mdtClusContName);
        return 0;
    }
    else{
        ATH_MSG_DEBUG("mdt Cluster container found" );
    }

    MdtPrepDataContainer::const_iterator it = mdtClusCont->indexFind(idHash);
    // if we find PRD, then recreate link
    if (it!=mdtClusCont->end()) 
    {
        //loop though collection to find matching PRD.
        MdtPrepDataCollection::const_iterator collIt = (*it)->begin();
        MdtPrepDataCollection::const_iterator collItEnd = (*it)->end();
        // there MUST be a faster way to do this!!
        for ( ; collIt!=collItEnd; collIt++){
            if ( (*collIt)->identify()==id ) return *collIt;
        }
    }
    ATH_MSG_DEBUG("No matching PRD found");
    return 0;
}

const Trk::PrepRawData* 
    Muon::MuonEventCnvTool::stgcClusterLink( const Identifier& id,  const IdentifierHash& idHash  ) const
{
    using namespace Trk;
    using namespace Muon;
    // need to retrieve pointers to collections

    // obviously this can be optimised! EJWM
    const sTgcPrepDataContainer* stgcClusCont;
    StatusCode sc = evtStore()->retrieve(stgcClusCont, m_stgcClusContName);
    if (sc.isFailure()){
        ATH_MSG_ERROR("stgc Cluster container not found at "<<m_stgcClusContName);
        return 0;
    }
    else{
        ATH_MSG_DEBUG("stgc Cluster container found");
    }

    sTgcPrepDataContainer::const_iterator it = stgcClusCont->indexFind(idHash);
    // if we find PRD, then recreate link
    if (it!=stgcClusCont->end()) 
    {
        //loop though collection to find matching PRD.
        sTgcPrepDataCollection::const_iterator collIt = (*it)->begin();
        sTgcPrepDataCollection::const_iterator collItEnd = (*it)->end();
        // there MUST be a faster way to do this!!
        for ( ; collIt!=collItEnd; collIt++){
            if ( (*collIt)->identify()==id ) return *collIt;
        }
    }
    ATH_MSG_DEBUG("No matching PRD found" );
    return 0;
}

const Trk::PrepRawData* 
    Muon::MuonEventCnvTool::mmClusterLink( const Identifier& id,  const IdentifierHash& idHash  ) const
{
    using namespace Trk;
    using namespace Muon;
    // need to retrieve pointers to collections

    // obviously this can be optimised! EJWM
    const MMPrepDataContainer* mmClusCont;
    StatusCode sc = evtStore()->retrieve(mmClusCont, m_mmClusContName);
    if (sc.isFailure()){
        ATH_MSG_ERROR("Mm Cluster container not found at "<<m_mmClusContName);
        return 0;
    }
    else{
        ATH_MSG_DEBUG("Mm Cluster container found");
    }

    MMPrepDataContainer::const_iterator it = mmClusCont->indexFind(idHash);
    // if we find PRD, then recreate link
    if (it!=mmClusCont->end()) 
    {
        //loop though collection to find matching PRD.
        MMPrepDataCollection::const_iterator collIt = (*it)->begin();
        MMPrepDataCollection::const_iterator collItEnd = (*it)->end();
        // there MUST be a faster way to do this!!
        for ( ; collIt!=collItEnd; collIt++){
            if ( (*collIt)->identify()==id ) return *collIt;
        }
    }
    ATH_MSG_DEBUG("No matching PRD found" );
    return 0;
}



