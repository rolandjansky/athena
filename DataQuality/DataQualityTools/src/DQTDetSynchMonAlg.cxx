/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DQTDetSynchMonAlg.cxx
// PACKAGE:  DataQualityTools
//
// AUTHORS:   Luca Fiorini <Luca.Fiorini@cern.ch>
//            Updated by:
//             Jahred Adelman (jahred.adelman@cern.ch)
//            and Max Baak (mbaakcern.ch)
//            and Sam King (samking@physics.ubc.ca)
//            and Simon Viel (svielcern.ch)
//            and Peter Onyisi
//
// ********************************************************************

#include "DataQualityTools/DQTDetSynchMonAlg.h"
#include "TrigT1Result/CTP_RIO.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"

#include "TProfile.h"
#include "AthenaMonitoringKernel/Monitored.h"


//----------------------------------------------------------------------------------
DQTDetSynchMonAlg::DQTDetSynchMonAlg( const std::string& name,
				      ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name, pSvcLocator)
{
   for (size_t ix = 0; ix < NDETS; ++ix) {
     for (size_t iy = ix+1; iy < NDETS; ++iy) {
       if (m_run2Compat && iy == 6) {
	 m_diffnamevec.push_back("diff_" + std::to_string(iy) + "_" + std::to_string(ix));
       } else {
	 m_diffnamevec.push_back("diff_" + std::to_string(ix) + "_" + std::to_string(iy));
       }
     }
   }
}

//----------------------------------------------------------------------------------
DQTDetSynchMonAlg::~DQTDetSynchMonAlg()
//----------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------
StatusCode DQTDetSynchMonAlg::initialize() {
//----------------------------------------------------------------------------------
  ATH_CHECK( m_InDetTimeCollectionKeys.initialize() );
  ATH_CHECK( m_LArFebHeaderContainerKey.initialize() );
  ATH_CHECK( m_TileDigitsContainerKey.initialize() );
  ATH_CHECK( m_RpcPadContainerKey.initialize() );
  ATH_CHECK( m_fieldCacheCondObjInputKey.initialize() );
  return AthMonitorAlgorithm::initialize();
}

//----------------------------------------------------------------------------------
StatusCode DQTDetSynchMonAlg::fillHistograms( const EventContext& ctx ) const
//----------------------------------------------------------------------------------
{
   ATH_MSG_DEBUG("in DQTDetSynchMonAlg::fillHistograms()");

   std::multiset<uint32_t> sctbcidset;
   std::multiset<uint32_t> trtbcidset;
   std::multiset<uint32_t> larbcidset;
   std::multiset<uint32_t> tilebcidset;
   std::multiset<uint32_t> rpcbcidset;
   std::multiset<uint32_t> pixelbcidset;

   std::multiset<uint32_t> sctl1idset;
   std::multiset<uint32_t> trtl1idset;
   std::multiset<uint32_t> larl1idset;
   std::multiset<uint32_t> tilel1idset;
   std::multiset<uint32_t> rpcl1idset;
   std::multiset<uint32_t> pixell1idset;


   Monitored::Scalar<uint16_t> sctbcid("sctbcid", 0), trtbcid("trtbcid", 0), larbcid("larbcid", 0), tilebcid("tilebcid", 0), rpcbcid("rpcbcid", 0), pixelbcid("pixelbcid", 0);
   Monitored::Scalar<uint32_t> sctl1id("sctl1id", 0), trtl1id("trtl1id", 0), larl1id("larl1id", 0), rpcl1id("rpcl1id", 0), pixell1id("pixell1id", 0);
   uint32_t tilel1id(0); // handled specially

   Monitored::Scalar<float> sctfrac("sctfrac", 0.0), trtfrac("trtfrac", 0.0), larfrac("larfrac", 0.0), tilefrac("tilefrac", 0.0), rpcfrac("rpcfrac", 0.0), pixelfrac("pixelfrac", 0.0);

   auto ctpbcid = Monitored::Scalar("ctpbcid", (uint16_t) 9999);
   uint32_t ctpl1id(9999999); // handled specially

   uint32_t lumi(0), evtNum(0);

   //Retrieve CTP, other things from EventInfo
   SG::ReadHandle<xAOD::EventInfo> thisEventInfo { GetEventInfo(ctx) } ;
   if(! thisEventInfo.isValid())
   {
     ATH_MSG_WARNING( "Could not find EventInfo in evtStore" );
   }
   else
   {
     ctpbcid = thisEventInfo->bcid();
     lumi    = thisEventInfo->lumiBlock() ;
     evtNum  = thisEventInfo->eventNumber();
     ctpl1id = thisEventInfo->extendedLevel1ID();
   }

   auto lb = Monitored::Scalar("LB", lumi);
   auto inDetTimeCollections = m_InDetTimeCollectionKeys.makeHandles(ctx);

   if (inDetTimeCollections[0].isValid()) {
     auto& TRT_BCIDColl(inDetTimeCollections[0]);
     for ( InDetTimeCollection::const_iterator itrt_bcid
	     = TRT_BCIDColl->begin();
	   itrt_bcid != TRT_BCIDColl->end(); ++itrt_bcid ) {
       //Current bcid
       trtbcidset.insert((uint16_t)(*itrt_bcid).second);
     } // End for loop
   }
   else {
     ATH_MSG_WARNING( "Could not get any TRT_BCID containers " );
   }


   if (inDetTimeCollections[1].isValid()) {
     auto& SCT_BCIDColl(inDetTimeCollections[1]);
     for ( InDetTimeCollection::const_iterator isct_bcid
	     = SCT_BCIDColl->begin();
	   isct_bcid != SCT_BCIDColl->end(); ++isct_bcid ) {
       //Current bcid
       sctbcidset.insert((uint16_t)(*isct_bcid).second);
     } // End for loop
   }
   else {
     ATH_MSG_WARNING( "Could not get any SCT_BCID containers " );
   }


   if (inDetTimeCollections[2].isValid()) {
     auto& Pixel_BCIDColl(inDetTimeCollections[2]);
     for ( InDetTimeCollection::const_iterator ipixel_bcid
	     = Pixel_BCIDColl->begin();
	   ipixel_bcid != Pixel_BCIDColl->end(); ++ipixel_bcid ) {
       //Current bcid
       pixelbcidset.insert((uint16_t)(*ipixel_bcid).second);
     } // End for loop
   }
   else {
     ATH_MSG_WARNING( "Could not get any Pixel_BCID containers " );
   }


   if (inDetTimeCollections[3].isValid()) {
     auto& TRT_LVL1IDColl(inDetTimeCollections[3]);
     for ( InDetTimeCollection::const_iterator itrt_lvl1id
	     = TRT_LVL1IDColl->begin();
	   itrt_lvl1id != TRT_LVL1IDColl->end(); ++itrt_lvl1id ) {
       //Current lvl1id
       trtl1idset.insert((uint16_t)(*itrt_lvl1id).second);
     } // End for loop
   }
   else {
     ATH_MSG_WARNING( "Could not get TRT_LVL1ID container " );
   }


   if (inDetTimeCollections[4].isValid()) {
     auto& SCT_LVL1IDColl(inDetTimeCollections[4]);
     for ( InDetTimeCollection::const_iterator isct_lvl1id
	     = SCT_LVL1IDColl->begin();
	   isct_lvl1id != SCT_LVL1IDColl->end(); ++isct_lvl1id ) {
       //Current lvl1id
       sctl1idset.insert((uint16_t)(*isct_lvl1id).second);
     } // End for loop
   }
   else {
     ATH_MSG_WARNING( "Could not get SCT_LVL1ID container " );
   }


   if (inDetTimeCollections[5].isValid()) {
     auto& Pixel_LVL1IDColl(inDetTimeCollections[5]);
     for ( InDetTimeCollection::const_iterator ipixel_lvl1id
	     = Pixel_LVL1IDColl->begin();
	   ipixel_lvl1id != Pixel_LVL1IDColl->end(); ++ipixel_lvl1id ) {
       //Current lvl1id
       pixell1idset.insert((uint16_t)(*ipixel_lvl1id).second);
     } // End for loop
   }
   else {
     ATH_MSG_WARNING( "Could not get Pixel_LVL1ID container " );
   }


   sctbcid=findid(sctbcidset);
   sctfrac=findfrac(sctbcidset,ctpbcid);
   trtbcid=findid(trtbcidset);
   trtfrac=findfrac(trtbcidset,ctpbcid);
   sctl1id=findid(sctl1idset);
   trtl1id=findid(trtl1idset);
   pixell1id=findid(pixell1idset);
   pixelbcid=findid(pixelbcidset);
   pixelfrac=findfrac(pixelbcidset,ctpbcid);

   SG::ReadHandle<LArFebHeaderContainer> hdrCont(m_LArFebHeaderContainerKey, ctx);
   if (! hdrCont.isValid()) {
      ATH_MSG_WARNING( "No LArFEB container found in TDS" );
   }
   else {
      //log << MSG::DEBUG << "LArFEB container found" <<endmsg;
      LArFebHeaderContainer::const_iterator it = hdrCont->begin();
      LArFebHeaderContainer::const_iterator itend = hdrCont->end();
      for ( ; it!=itend;++it) {
	//HWIdentifier febid=(*it)->FEBId();
        unsigned int febid=((*it)->FEBId()).get_identifier32().get_compact();
         if (febid >= 0x38000000 && febid <= 0x3bc60000) {
            larbcidset.insert((uint16_t) (*it)->BCId() );
            larl1idset.insert((uint32_t) (*it)->ELVL1Id() );
         }
      }
   }
   larbcid=findid(larbcidset);
   larfrac=findfrac(larbcidset,larbcid);
   larl1id=findid(larl1idset);

   SG::ReadHandle<TileDigitsContainer> DigitsCnt(m_TileDigitsContainerKey, ctx);
   if (! DigitsCnt.isValid()) {
     ATH_MSG_WARNING( "No Tile Digits container found in TDS" );
   }
   else {
     TileDigitsContainer::const_iterator collItr=DigitsCnt->begin();
     TileDigitsContainer::const_iterator lastColl=DigitsCnt->end();
     for (;collItr!=lastColl;++collItr){
       tilebcidset.insert( (*collItr)->getRODBCID() );
       tilel1idset.insert( (*collItr)->getLvl1Id() );
     }
   }
   tilebcid=findid(tilebcidset);
   tilefrac=findfrac(tilebcidset,tilebcid);
   tilel1id=findid(tilel1idset);

   if (m_doRPC) {
     SG::ReadHandle<RpcPadContainer> rpcRDO(m_RpcPadContainerKey, ctx);
     if (! rpcRDO.isValid()) {
       ATH_MSG_WARNING( "No RPC Pad container found in TDS" );
     } else {
       RpcPadContainer::const_iterator pad = rpcRDO->begin();
       RpcPadContainer::const_iterator endpad = rpcRDO->end();
       for (; pad !=  endpad ; ++pad ) {
	 if ( (*pad) ) {
	   if ( (*pad)->size() > 0 )  {
	     rpcbcidset.insert( (*pad)->bcId() );
	     rpcl1idset.insert( (*pad)->lvl1Id() );
	   }
         }
       }
     }
   }


   rpcbcid=findid(rpcbcidset);
   rpcfrac=findfrac(rpcbcidset,rpcbcid);
   rpcl1id=findid(rpcl1idset);

   auto ctp_l1id16 = Monitored::Scalar<uint32_t>("ctpl1id", ctpl1id & 0xFFFF);
   uint32_t ctp_l1id9 = ctpl1id & 0x1FF;
   auto tile_l1id16 = Monitored::Scalar<uint32_t>("tilel1id", tilel1id & 0xFFFF);
   uint32_t tile_l1id9 = tilel1id & 0x1FF;
   uint32_t sct_l1id9 = sctl1id & 0x1FF;
   uint32_t trt_l1id9 = trtl1id & 0x1FF;
   uint32_t lar_l1id9 = larl1id & 0x1FF;
   uint32_t pixel_l1id9 = pixell1id & 0x1FF;

   std::vector<float> bcidrates_base { sctfrac, trtfrac, larfrac,
       tilefrac, rpcfrac, pixelfrac };
   auto bcidrates = Monitored::Collection( "bcidrates",
					   bcidrates_base );

   std::vector<int> bcidrates_idx_base = { 1, 2, 3, 4, 5, 6 };

   auto bcidrates_idx = Monitored::Collection( "bcidrates_idx",
					       bcidrates_idx_base );


   std::vector<Int_t> bcidvals { ctpbcid, sctbcid, trtbcid, larbcid,
       tilebcid, rpcbcid, pixelbcid };
   std::vector<int> diffx_base, diffy_base;
   // catch if we have a sync problem ...
   assert( bcidvals.size() == NDETS );

   std::vector<Monitored::Scalar<Int_t>> diffvec;
   for (size_t ix = 0; ix < NDETS; ++ix) {
     for (size_t iy = ix+1; iy < NDETS; ++iy) {
       if (m_run2Compat && iy == 6) {
	 diffvec.push_back(Monitored::Scalar<Int_t>(m_diffnamevec[diffvec.size()]));
       } else {
	 diffvec.push_back(Monitored::Scalar<Int_t>(m_diffnamevec[diffvec.size()]));
       }
     }
   }

   for (size_t ix = 0, ivec = 0; ix < NDETS; ++ix) {
     for (size_t iy = ix+1; iy < NDETS; ++iy) {
       Int_t comparison = bcidvals[ix] - bcidvals[iy];
       if (comparison > 0) {
	 diffx_base.push_back(ix); diffy_base.push_back(iy);
       } else if (comparison < 0) {
	 diffx_base.push_back(iy); diffy_base.push_back(ix);
       } else {
	 diffx_base.push_back(ix); diffy_base.push_back(ix);
	 diffx_base.push_back(iy); diffy_base.push_back(iy);
       }
       if (m_run2Compat && iy == 6) {
	 diffvec[ivec] = -comparison;
       } else {
	 diffvec[ivec] = comparison;
       }
       ++ivec;
     }
   }
   for (auto&& var: diffvec) {
     fill("bcid", var, lb);
   }
   if (m_run2Compat) {
     auto diffjunk = Monitored::Scalar("diff_0_6", bcidvals[0]-bcidvals[6]);
     fill("bcid", diffjunk, lb);
   }

   auto diffx = Monitored::Collection("diffx", diffx_base);
   auto diffy = Monitored::Collection("diffy", diffy_base);
   fill("bcid", ctpbcid, sctbcid, trtbcid, larbcid, tilebcid, rpcbcid,
   	pixelbcid, bcidrates, bcidrates_idx, diffx, diffy);


// // Now l1id
   std::vector<UInt_t> l1idvals { ctp_l1id16, sctl1id, trtl1id, larl1id,
       tile_l1id16, rpcl1id, pixell1id };
   // the following for comparison for RPC
   std::vector<UInt_t> l1idvals_d9 { ctp_l1id9, sct_l1id9, trt_l1id9,
       lar_l1id9, tile_l1id9, rpcl1id, pixel_l1id9 };
   // catch if we have a sync problem ...
   assert( l1idvals.size() == NDETS && l1idvals_d9.size() == NDETS );

   diffx_base.clear(); diffy_base.clear();

   for (size_t ix = 0, ivec = 0; ix < NDETS; ++ix) {
     for (size_t iy = ix+1; iy < NDETS; ++iy) {
       UInt_t xl1id, yl1id;
       // RPC (index 5) is an exception
       if (iy == 5) {
	 yl1id = rpcl1id; xl1id = l1idvals_d9[ix];
       } else if (ix == 5) {
	 xl1id = rpcl1id; yl1id = l1idvals_d9[iy];
       } else {
	 xl1id = l1idvals[ix]; yl1id = l1idvals[iy];
       }
       Int_t comparison = xl1id - yl1id;
       if (comparison > 0) {
	 diffx_base.push_back(ix); diffy_base.push_back(iy);
       } else if (comparison < 0) {
	 diffx_base.push_back(iy); diffy_base.push_back(ix);
       } else {
	 diffx_base.push_back(ix); diffy_base.push_back(ix);
	 diffx_base.push_back(iy); diffy_base.push_back(iy);
       }
       if (m_run2Compat && iy == 6) {
	 diffvec[ivec] = -comparison;
       } else {
	 diffvec[ivec] = comparison;
       }
       ++ivec;
     }
   }
   for (auto&& var: diffvec) {
     fill("l1id", var, lb);
   }
   if (m_run2Compat) {
     auto diffjunk = Monitored::Scalar("diff_0_6", bcidvals[0]-bcidvals[6]);
     fill("l1id", diffjunk, lb);
   }
   fill("l1id", ctp_l1id16, sctl1id, trtl1id, larl1id,
	tile_l1id16, rpcl1id, pixell1id, diffx, diffy);

   // B field
   Amg::Vector3D f;
   Amg::Vector3D gP1(m_solenoidPositionX, m_solenoidPositionY, m_solenoidPositionZ);
   MagField::AtlasFieldCache    fieldCache;
   SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
   const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
   if (fieldCondObj == nullptr) {
       ATH_MSG_ERROR("DQTDetSynchMonAlg: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCacheCondObjInputKey.key());
       return StatusCode::FAILURE;
   }
   fieldCondObj->getInitializedCache (fieldCache);
   fieldCache.getField(gP1.data(),f.data());

   // field is in kilotesla (!)
   auto solenoid_bz = Monitored::Scalar("solenoid_bz", f[2]*1000.);

   Amg::Vector3D  gP2(m_toroidPositionX, m_toroidPositionY, m_toroidPositionZ);
   if (fieldCache.useNewBfieldCache()) fieldCache.getField(gP2.data(),f.data());

   auto toroid_bx = Monitored::Scalar("toroid_bx", f[0]*1000.);

   fill("bfield", solenoid_bz, toroid_bx, lb);


   ATH_MSG_VERBOSE( "evt Num  : "  << evtNum    << "\tLumi      : "  << lumi     );
   ATH_MSG_VERBOSE( "evt L1ID : "  << ctpl1id   << "\tevt BCID  : "  << ctpbcid  );
   ATH_MSG_VERBOSE( "CTP L1ID : "  << ctpl1id   << "\tCTP BCID  : "  << ctpbcid  );
   ATH_MSG_VERBOSE( "LAR L1ID : "  << larl1id   << "\tLAR BCID  : "  << larbcid  );
   ATH_MSG_VERBOSE( "TILE L1ID: "  << tilel1id  << "\tTILE BCID : "  << tilebcid );
   ATH_MSG_VERBOSE( "RPC L1ID : "  << rpcl1id   << "\tRPC BCID  : "  << rpcbcid  );
   ATH_MSG_VERBOSE( "Pixel L1ID : "  << pixell1id   << "\tPixel BCID  : "  << pixelbcid  );
   ATH_MSG_VERBOSE( "CTP-RPC L1ID : "  << ctp_l1id9-rpcl1id   << "\tCTP-LAR L1ID  : "  << ctp_l1id16-larl1id  );
   return StatusCode::SUCCESS;
}


//----------------------------------------------------------------------------------
uint32_t DQTDetSynchMonAlg::findid(const std::multiset<uint32_t>& mset) const
//----------------------------------------------------------------------------------
{
   uint32_t id(9999999),refid(9999999);
   int refcount(0),count(0);
   std::multiset<uint32_t>::iterator it = mset.begin();
   std::multiset<uint32_t>::iterator itend = mset.end();

   if (it!=itend && !mset.empty()){
      for (;it!=itend;++it) {
         if ( (*it) != id ) { //skip check if the id value has been already evaluated

            id=*it; count = mset.count(*it);
            ATH_MSG_VERBOSE( "new id found: " << (*it) << " with counts: " << count );
            if ( ( (*it) !=refid )&&(count>refcount) ){ // if count> previous maximum and double check the id value
               refid = (*it);
               refcount=count;
               ATH_MSG_VERBOSE( "new REFERENCE id: " << refid << " with REFERENCE counts: " << refcount );
            }
         }
      }
   }
   ATH_MSG_VERBOSE( "Returning REFERENCE id: " << refid << " with REFERENCE counts: " << refcount );
   return refid;
}

//----------------------------------------------------------------------------------
float DQTDetSynchMonAlg::findfrac(const std::multiset<uint32_t>& mset, uint16_t ctpid) const
//----------------------------------------------------------------------------------
{
  MsgStream log(msgSvc(), name());
  std::multiset<uint32_t>::iterator it = mset.begin();
  std::multiset<uint32_t>::iterator itend = mset.end();

  int totalCounter=0;
  int nonctpIdCounter=0;
  float frac = 0.0;

  if (it!=itend && !mset.empty()){
    for (;it!=itend;++it) {
      totalCounter++;
      if ( (*it) != ctpid ) nonctpIdCounter++;
    }
  }

  if (totalCounter>0)
    frac = ((float) nonctpIdCounter)/((float) totalCounter);
  else
    frac = 1.0;
  return frac;
}
