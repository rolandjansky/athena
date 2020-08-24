/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonEventCnvTool.h"

#include "GaudiKernel/MsgStream.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkDetElementBase/TrkDetElementBase.h"

#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"

#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MMClusterOnTrack.h"
#include "MuonRIO_OnTrack/sTgcClusterOnTrack.h"

#include <vector>
#include <cassert>
#include <iostream>

#include "EventPrimitives/EventPrimitives.h"


Muon::MuonEventCnvTool::MuonEventCnvTool(
    const std::string& t,
    const std::string& n,
    const IInterface*  p )
    :
    base_class(t,n,p),
    m_muonMgr(nullptr)
{
}

StatusCode Muon::MuonEventCnvTool::initialize()
{

  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_rpcPrdKey.initialize());
  ATH_CHECK(m_cscPrdKey.initialize(!m_cscPrdKey.empty())); // check for layouts without CSCs
  ATH_CHECK(m_tgcPrdKey.initialize());
  ATH_CHECK(m_mdtPrdKey.initialize());
  ATH_CHECK(m_mmPrdKey.initialize(!m_mmPrdKey.empty())); // check for layouts without MicroMegas
  ATH_CHECK(m_stgcPrdKey.initialize(!m_stgcPrdKey.empty())); // check for layouts without STGCs
  ATH_CHECK(detStore()->retrieve(m_muonMgr));

  return StatusCode::SUCCESS;
}

void 
    Muon::MuonEventCnvTool::checkRoT( const Trk::RIO_OnTrack& rioOnTrack ) const
{
    MuonConcreteType type=TypeUnknown;
    if (dynamic_cast<const Muon::RpcClusterOnTrack*>(&rioOnTrack) )        type = RPC;
    if (dynamic_cast<const Muon::CscClusterOnTrack*>(&rioOnTrack) )        type = CSC;
    if (dynamic_cast<const Muon::TgcClusterOnTrack*>(&rioOnTrack) )        type = TGC;
    if (dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(&rioOnTrack) )    type = MDT;
    if (dynamic_cast<const Muon::MMClusterOnTrack*>(&rioOnTrack) )         type = MM;
    if (dynamic_cast<const Muon::sTgcClusterOnTrack*>(&rioOnTrack) )       type = STGC;
    if (type==TypeUnknown) {
        ATH_MSG_ERROR("Type does not match known concrete type of MuonSpectrometer! Dumping RoT:"<<rioOnTrack);
    }else{
      ATH_MSG_VERBOSE("Type = "<<type<<"(RPC="<<RPC<<", CSC="<<CSC<<", TGC="<<TGC<<"MDT="<<MDT<<", STGC="<<STGC<<"MM="<<MM<<")");
    }

    return;
}



std::pair<const Trk::TrkDetElementBase*, const Trk::PrepRawData*> 
    Muon::MuonEventCnvTool::getLinks( Trk::RIO_OnTrack& rioOnTrack ) const
{
    using namespace Trk;
    using namespace MuonGM;

    const TrkDetElementBase* detEl = 0;
    const PrepRawData*       prd   = 0;
    const Identifier& id           = rioOnTrack.identify();

    if (m_muonMgr) {
        //TODO Check that these are in the most likely ordering, for speed. EJWM.
      if (m_idHelperSvc->isRpc(id)){
        detEl =  m_muonMgr->getRpcReadoutElement( id ) ;
        if (m_manuallyFindPRDs) prd = rpcClusterLink(id, rioOnTrack.idDE());
      } else if(m_idHelperSvc->isCsc(id)){
        detEl =  m_muonMgr->getCscReadoutElement( id ) ;
        if (m_manuallyFindPRDs) prd = cscClusterLink(id, rioOnTrack.idDE());
      } else if(m_idHelperSvc->isTgc(id)){
        detEl = m_muonMgr->getTgcReadoutElement( id ) ;
        if ( m_manuallyFindPRDs) prd = tgcClusterLink(id, rioOnTrack.idDE());
        if ( m_fixTGCs && !rioOnTrack.prepRawData() ) {
          // Okay, so we might have hit the nasty issue that the TGC EL is broken in some samples
          // Need to fix by pointing to the key defined here (assumung it has been configured correctly for this sample)
          Muon::TgcClusterOnTrack* tgc = dynamic_cast<Muon::TgcClusterOnTrack*>(&rioOnTrack);
          ElementLinkToIDC_TGC_Container& el = tgc->m_rio;
          el.resetWithKeyAndIndex(m_tgcPrdKey.key(), el.index());
        }
      }else if(m_idHelperSvc->isMdt(id)){
        detEl =  m_muonMgr->getMdtReadoutElement( id ) ;
        if (m_manuallyFindPRDs) prd = mdtDriftCircleLink(id, rioOnTrack.idDE());
      } else if(m_idHelperSvc->isMM(id)){
        detEl = m_muonMgr->getMMReadoutElement( id ) ;
        if (m_manuallyFindPRDs) prd = mmClusterLink(id, rioOnTrack.idDE());
      } else if(m_idHelperSvc->issTgc(id)){
        detEl = m_muonMgr->getsTgcReadoutElement( id ) ;
        if (m_manuallyFindPRDs) prd = stgcClusterLink(id, rioOnTrack.idDE());
      }else{
        ATH_MSG_WARNING( "Unknown type of Muon detector from identifier :"<< id);
      }
    }
    if (detEl==nullptr) ATH_MSG_ERROR( "Apparently could not find detector element for "<< id);
    ATH_MSG_VERBOSE("Found PRD at : "<<prd);    
    return std::pair<const Trk::TrkDetElementBase*, const Trk::PrepRawData*>(detEl,prd); 
}

void Muon::MuonEventCnvTool::prepareRIO_OnTrack( Trk::RIO_OnTrack *RoT ) const {
    
    Muon::MdtDriftCircleOnTrack* mdt = dynamic_cast<Muon::MdtDriftCircleOnTrack*>(RoT);
    if (mdt){
        prepareRIO_OnTrackElementLink<const Muon::MdtPrepDataContainer, Muon::MdtDriftCircleOnTrack>(mdt);
        return;
    }
    Muon::CscClusterOnTrack* csc = dynamic_cast<Muon::CscClusterOnTrack*>(RoT);
    if (csc){
        prepareRIO_OnTrackElementLink<const Muon::CscPrepDataContainer, Muon::CscClusterOnTrack>(csc);
        return;
    }
    Muon::RpcClusterOnTrack* rpc = dynamic_cast<Muon::RpcClusterOnTrack*>(RoT);
    if (rpc){
        prepareRIO_OnTrackElementLink<const Muon::RpcPrepDataContainer, Muon::RpcClusterOnTrack>(rpc);
        return;
    }
    Muon::TgcClusterOnTrack* tgc = dynamic_cast<Muon::TgcClusterOnTrack*>(RoT);
    if (tgc){
        prepareRIO_OnTrackElementLink<const Muon::TgcPrepDataContainer, Muon::TgcClusterOnTrack>(tgc);
        return;
    }   
    Muon::sTgcClusterOnTrack* stgc = dynamic_cast<Muon::sTgcClusterOnTrack*>(RoT);
    if (stgc){
        prepareRIO_OnTrackElementLink<const Muon::sTgcPrepDataContainer, Muon::sTgcClusterOnTrack>(stgc);
        return;
    }
    Muon::MMClusterOnTrack* mm = dynamic_cast<Muon::MMClusterOnTrack*>(RoT);
    if (mm){
        prepareRIO_OnTrackElementLink<const Muon::MMPrepDataContainer, Muon::MMClusterOnTrack>(mm);
        return;
    }     
    return;
}

void
Muon::MuonEventCnvTool::prepareRIO_OnTrackLink( const Trk::RIO_OnTrack *RoT,
                                                ELKey_t& key,
                                                ELIndex_t& index) const
{
    const Muon::MdtDriftCircleOnTrack* mdt = dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(RoT);
    if (mdt){
        prepareRIO_OnTrackElementLink<const Muon::MdtPrepDataContainer, Muon::MdtDriftCircleOnTrack>(mdt, key, index);
        return;
    }
    const Muon::CscClusterOnTrack* csc = dynamic_cast<const Muon::CscClusterOnTrack*>(RoT);
    if (csc){
        prepareRIO_OnTrackElementLink<const Muon::CscPrepDataContainer, Muon::CscClusterOnTrack>(csc, key, index);
        return;
    }
    const Muon::RpcClusterOnTrack* rpc = dynamic_cast<const Muon::RpcClusterOnTrack*>(RoT);
    if (rpc){
        prepareRIO_OnTrackElementLink<const Muon::RpcPrepDataContainer, Muon::RpcClusterOnTrack>(rpc, key, index);
        return;
    }
    const Muon::TgcClusterOnTrack* tgc = dynamic_cast<const Muon::TgcClusterOnTrack*>(RoT);
    if (tgc){
        prepareRIO_OnTrackElementLink<const Muon::TgcPrepDataContainer, Muon::TgcClusterOnTrack>(tgc, key, index);
        return;
    }   
    const Muon::sTgcClusterOnTrack* stgc = dynamic_cast<const Muon::sTgcClusterOnTrack*>(RoT);
    if (stgc){
        prepareRIO_OnTrackElementLink<const Muon::sTgcPrepDataContainer, Muon::sTgcClusterOnTrack>(stgc, key, index);
        return;
    }
    const Muon::MMClusterOnTrack* mm = dynamic_cast<const Muon::MMClusterOnTrack*>(RoT);
    if (mm){
        prepareRIO_OnTrackElementLink<const Muon::MMPrepDataContainer, Muon::MMClusterOnTrack>(mm, key, index);
        return;
    }     
    return;
}

void Muon::MuonEventCnvTool::recreateRIO_OnTrack( Trk::RIO_OnTrack *RoT ) const {
    
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
    const Trk::TrkDetElementBase* detEl = nullptr;

    if (m_muonMgr) 
    {
        //TODO Check that these are in the most likely ordering, for speed. EJWM.
      if (m_idHelperSvc->isRpc(id)) {
        detEl =  m_muonMgr->getRpcReadoutElement( id ) ;
      }else if(m_idHelperSvc->isCsc(id)){
        detEl =  m_muonMgr->getCscReadoutElement( id ) ;
      }else if(m_idHelperSvc->isTgc(id)){
        detEl = m_muonMgr->getTgcReadoutElement( id ) ;
      }else if(m_idHelperSvc->isMdt(id)) {
        detEl =  m_muonMgr->getMdtReadoutElement( id ) ;
      }else if(m_idHelperSvc->issTgc(id)){
        detEl = m_muonMgr->getsTgcReadoutElement( id ) ;
      }else if(m_idHelperSvc->isMM(id)){
        detEl = m_muonMgr->getMMReadoutElement( id ) ;
      }
    }

    if (!detEl) ATH_MSG_ERROR("Apparently could not find detector element for Identifier: "<< id);
    return detEl;
}

template<class CONT>
const Trk::PrepRawData* 
    Muon::MuonEventCnvTool::getLink( const Identifier& id,  const IdentifierHash& idHash, const SG::ReadHandleKey<CONT>& prdKey ) const
{  
  SG::ReadHandle<CONT> handle(prdKey);
  
  if (!handle.isValid()){
      ATH_MSG_ERROR("PRD container not found at "<<prdKey);
      return 0;
  }
  else{
      ATH_MSG_DEBUG("PRD Cluster container found at "<<prdKey);
  }

  auto ptr = handle->indexFindPtr(idHash);
  // if we find PRD, then recreate link
  if (ptr!=nullptr) 
  {
      //loop though collection to find matching PRD.
      auto collIt = ptr->begin();
      auto collItEnd = ptr->end();
      // there MUST be a faster way to do this!!
      for ( ; collIt!=collItEnd; collIt++){
          if ( (*collIt)->identify()==id ) return *collIt;
      }
  }
  ATH_MSG_DEBUG("No matching PRD found");
  return 0;
}

const Trk::PrepRawData* 
    Muon::MuonEventCnvTool::rpcClusterLink( const Identifier& id,  const IdentifierHash& idHash  ) const
{
    return getLink(id, idHash, m_rpcPrdKey);
}

const Trk::PrepRawData* 
    Muon::MuonEventCnvTool::cscClusterLink( const Identifier& id,  const IdentifierHash& idHash  ) const
{
  return getLink(id, idHash, m_cscPrdKey);
}

const Trk::PrepRawData* 
    Muon::MuonEventCnvTool::tgcClusterLink( const Identifier& id,  const IdentifierHash& idHash  ) const
{
  return getLink(id, idHash, m_tgcPrdKey);
}

const Trk::PrepRawData* 
    Muon::MuonEventCnvTool::mdtDriftCircleLink( const Identifier& id,  const IdentifierHash& idHash  ) const
{
  return getLink(id, idHash, m_mdtPrdKey);
}

const Trk::PrepRawData* 
    Muon::MuonEventCnvTool::stgcClusterLink( const Identifier& id,  const IdentifierHash& idHash  ) const
{
  return getLink(id, idHash, m_stgcPrdKey);
}

const Trk::PrepRawData* 
    Muon::MuonEventCnvTool::mmClusterLink( const Identifier& id,  const IdentifierHash& idHash  ) const
{
  return getLink(id, idHash, m_mmPrdKey);
}



