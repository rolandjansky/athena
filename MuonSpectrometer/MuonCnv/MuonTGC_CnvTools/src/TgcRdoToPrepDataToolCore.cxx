/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TgcRdoToPrepDataToolCore.h"

#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "TGCcablingInterface/ITGCcablingServerSvc.h"
#include "TGCcablingInterface/TGCIdBase.h"
#include "MuonDigitContainer/TgcDigit.h"
#include "MuonTrigCoinData/TgcCoinData.h"
#include "TrkSurfaces/Surface.h"
#include "MuonCnvToolInterfaces/IDC_Helper.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include <cfloat>
#include <algorithm>

//================ Constructor =================================================

Muon::TgcRdoToPrepDataToolCore::TgcRdoToPrepDataToolCore(const std::string& t, const std::string& n, const IInterface* p)
  : base_class(t, n, p), 
    m_nHitRDOs(0), 
    m_nHitPRDs(0), 
    m_nTrackletRDOs(0), 
    m_nTrackletPRDs(0), 
    m_nTrackletEIFIRDOs(0), 
    m_nTrackletEIFIPRDs(0), 
    m_nHiPtRDOs(0), 
    m_nHiPtPRDs(0), 
    m_nSLRDOs(0), 
    m_nSLPRDs(0), 
    m_outputCoinKeys{"dummy", "dummy", "dummy" },
    m_outputprepdataKeys{"dummy", "dummy", "dummy", "dummy"}
{
  // tools
  declareProperty("TGCHashIdOffset", m_tgcOffset = 26000);  
  declareProperty("FillCoinData", m_fillCoinData = true);
  declareProperty("OutputCollection", m_outputCollectionLocation="TGC_Measurements");
  declareProperty("OutputCoinCollection", m_outputCoinCollectionLocation="TrigT1CoinDataCollection");
  declareProperty("DecodeData", m_decodeData = true);  // !< toggle on/off the decoding of TGC RDO into TgcPrepData 
  declareProperty("show_warning_level_invalid_A09_SSW6_hit", m_show_warning_level_invalid_A09_SSW6_hit = false); 
  declareProperty("dropPrdsWithZeroWidth", m_dropPrdsWithZeroWidth = true);
  declareProperty("outputCoinKey", m_outputCoinKeys);
  declareProperty("prepDataKeys", m_outputprepdataKeys);
  // DataHandle
  declareProperty("RDOContainer",   m_rdoContainerKey = std::string("TGCRDO"),"TgcRdoContainer to retrieve");
}  

//================ Initialization =================================================

StatusCode Muon::TgcRdoToPrepDataToolCore::initialize()
{
  ATH_CHECK(AthAlgTool::initialize());
  ATH_CHECK(m_idHelperSvc.retrieve());

  for(int ibc=0; ibc<NBC+1; ibc++) {      
    int bcTag=ibc+1;
    std::ostringstream location;
    location << m_outputCollectionLocation << (bcTag==TgcDigit::BC_PREVIOUS ? "PriorBC" : "")
	     << (bcTag==TgcDigit::BC_NEXT ? "NextBC" : "") << (bcTag==(NBC+1) ? "AllBCs" : "");    
    m_outputprepdataKeys.at(ibc) = location.str();
  }

  for(int ibc=0; ibc<NBC; ibc++) {
    int bcTag=ibc+1;
    std::ostringstream location;
    location << m_outputCoinCollectionLocation << (bcTag==TgcDigit::BC_PREVIOUS ? "PriorBC" : "")
             << (bcTag==TgcDigit::BC_NEXT ? "NextBC" : "");
    m_outputCoinKeys.at(ibc) = location.str();
  }
  
  ATH_CHECK(m_outputCoinKeys.initialize());
  ATH_CHECK(m_outputprepdataKeys.initialize());
  ATH_CHECK(m_muDetMgrKey.initialize());
 
  // check if initializing of DataHandle objects success
  ATH_CHECK( m_rdoContainerKey.initialize() );
  
  //try to configure the cabling service
  if (!getCabling()) {
    // ??? Is this delayed initialization still needed?
    ATH_MSG_INFO("TGCcablingServerSvc not yet configured; postpone TGCcabling initialization at first event.");
  }
  
  ATH_MSG_INFO("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

//================ Finalization =================================================

StatusCode Muon::TgcRdoToPrepDataToolCore::finalize()
{
  ATH_MSG_INFO("finalize(): input RDOs->output PRDs ["  << 
	       "Hit: "          << m_nHitRDOs          << "->" << m_nHitPRDs          << ", " << 
	       "Tracklet: "     << m_nTrackletRDOs     << "->" << m_nTrackletPRDs     << ", " << 
	       "TrackletEIFI: " << m_nTrackletEIFIRDOs << "->" << m_nTrackletEIFIPRDs << ", " << 
	       "HiPt: "         << m_nHiPtRDOs         << "->" << m_nHiPtPRDs         << ", " << 
	       "SL: "           << m_nSLRDOs           << "->" << m_nSLPRDs           << "]"); 

  return AthAlgTool::finalize();
}

//================ Decoding =================================================
StatusCode Muon::TgcRdoToPrepDataToolCore::decode(std::vector<IdentifierHash>&, std::vector<IdentifierHash>&) const
{
  return StatusCode::FAILURE;
}

void Muon::TgcRdoToPrepDataToolCore::printInputRdo() const
{
  ATH_MSG_INFO("***************** Listing input TgcRdo Collections *****************************************");

  /// TGC context
  IdContext tgcContext = m_idHelperSvc->tgcIdHelper().module_context();

  /// TGC RDO container --- assuming it is available
  auto rdoContainer = SG::makeHandle(m_rdoContainerKey);
  if(!rdoContainer.isValid()) {
    ATH_MSG_WARNING("*** Retrieval of TGC RDO container for debugging purposes failed !");
    return;
  }                                                                

  if(rdoContainer->size()==0) {
    ATH_MSG_INFO("*** No TgcRdo collections were found");
    return;
  }

  int ncoll = 0;
  int nHit = 0;
  int nTracklet = 0;
  int nTrackletEIFI = 0;
  int nHiPt = 0;
  int nSL = 0;
  int nOthers = 0;
  int nTgcRawData = 0;

  TgcRdoContainer::const_iterator rdo_container_it   = rdoContainer->begin();
  TgcRdoContainer::const_iterator rdo_container_it_e = rdoContainer->end();
  for(; rdo_container_it!=rdo_container_it_e; ++rdo_container_it) {
    // loop over all elements of the rdo container 
    const TgcRdo* rdoColl = *rdo_container_it;
    if(rdoColl->empty()) continue;
    
    uint16_t subDetectorId = rdoColl->subDetectorId();
    uint16_t rodId = rdoColl->rodId();
    uint16_t onlineId = rdoColl->calculateOnlineId(subDetectorId, rodId);
    ATH_MSG_INFO("*** TgcRdo : onlineId=" << onlineId 
		 << " subDetectorId=" << subDetectorId
		 << " rodId="<< rodId
		 << " # of TgcRawData inside is "<< rdoColl->size());
    
    IdentifierHash tgcHashId = rdoColl->identifyHash();
    Identifier rdoId;
    int code = m_idHelperSvc->tgcIdHelper().get_id(tgcHashId, rdoId, &tgcContext);
    if(code!=0) { 
      ATH_MSG_INFO("*** A problem in hash -> id conversion for hashId= " << static_cast<int>(tgcHashId)); 
    }
    
    std::string extIdstring = m_idHelperSvc->tgcIdHelper().show_to_string(rdoId);
    ATH_MSG_INFO("*** Offine HashId = " << tgcHashId << " extended = " << extIdstring);
    
    int iHit = 0;
    int iTracklet = 0;
    int iTrackletEIFI = 0;
    int iHiPt = 0;
    int iSL = 0;
    int iOthers = 0;

    // For each rdo, loop on the coincidence matrices
    for (const TgcRawData* rd : *rdoColl) {
      if(!rd->isCoincidence()) {
	iHit++;
      } else {
	if((rd->type()==TgcRawData::TYPE_TRACKLET)) {
	  if((rd->slbType()==TgcRawData::SLB_TYPE_DOUBLET_WIRE) ||
	     (rd->slbType()==TgcRawData::SLB_TYPE_DOUBLET_STRIP)) {
	    iTracklet++;
	  } else if((rd->slbType()==TgcRawData::SLB_TYPE_INNER_WIRE) || 
		    (rd->slbType()==TgcRawData::SLB_TYPE_INNER_STRIP)) {
	    iTrackletEIFI++; 
	  }
	} else if((rd->type()==TgcRawData::TYPE_HIPT) && 
		  (rd->isHipt())) {
	  iHiPt++;
	} else if((rd->type()==TgcRawData::TYPE_SL)) {
	  iSL++;
	} else {
	  iOthers++; 
	}
      }
    } // end loop over TgcRawData

    ATH_MSG_INFO("*** " << ncoll << "-th TgcRdo (onlineId=" << onlineId << ") has " 
		 << iHit          << " Hits / "
		 << iTracklet     << " Tracklets / "
		 << iTrackletEIFI << " TrackletEIFs / "
		 << iHiPt         << " HiPts / "
		 << iSL           << " SLs / " 
		 << iOthers       << " Others");
    ATH_MSG_INFO("--------------------------------------------------------------------------------------------");
    
    ncoll++;
    nHit          += iHit;
    nTracklet     += iTracklet;
    nTrackletEIFI += iTrackletEIFI;
    nHiPt         += iHiPt;
    nSL           += iSL;
    nOthers       += iOthers;
    nTgcRawData += rdoColl->size();
  } // end loop over TgcRdo's
  ATH_MSG_INFO("*** Event Summary: " << ncoll       << " Collections / " << nTgcRawData << " TgcRawData"); 
  ATH_MSG_INFO("***               (" 
	       << nHit          << " Hits / "
	       << nTracklet     << " Tracklets / "
	       << nTrackletEIFI << " TrackletEIFIs / "
	       << nHiPt         << " HiPts / "
	       << nSL           << " SLs / "
	       << nOthers       << " Others)");
  ATH_MSG_INFO("********************************************************************************************");
}

void Muon::TgcRdoToPrepDataToolCore::printPrepDataImpl
  (const TgcPrepDataContainer* const* tgcPrepDataContainer,
   const TgcCoinDataContainer* const* tgcCoinDataContainer) const
{
  // For debugging purpose (start)  
  /*  
      unsigned int nHit[NBC];  
      unsigned int nCoin[NBC];  
      std::cout << "Muon::TgcRdoToPrepDataToolCore::printPrepData";  
      for(int ibc=0; ibc<NBC; ibc++) {  
      nHit[ibc] = 0;  
      IdentifiableContainer<Muon::TgcPrepDataCollection>::const_iterator tgcColli   = tgcPrepDataContainer[ibc]->begin();  
      IdentifiableContainer<Muon::TgcPrepDataCollection>::const_iterator tgcColli_e = tgcPrepDataContainer[ibc]->end();  
      for(; tgcColli!=tgcColli_e; tgcColli++) {  
      const TgcPrepDataCollection* tgcColl = *tgcColli;  
      TgcPrepDataCollection::const_iterator it_tgcPrepData   = tgcColl->begin();   
      TgcPrepDataCollection::const_iterator it_tgcPrepData_e = tgcColl->end();  
      for(; it_tgcPrepData!=it_tgcPrepData_e; it_tgcPrepData++) nHit[ibc]++;  
      }  
      nCoin[ibc] = 0;  
      IdentifiableContainer<Muon::TgcCoinDataCollection>::const_iterator tgcCoinColli   = tgcCoinDataContainer[ibc]->begin();   
      IdentifiableContainer<Muon::TgcCoinDataCollection>::const_iterator tgcCoinColli_e = tgcCoinDataContainer[ibc]->end();  
      for(; tgcCoinColli!=tgcCoinColli_e; tgcCoinColli++) {  
      const TgcCoinDataCollection* tgcCoinColl = *tgcCoinColli;  
      TgcCoinDataCollection::const_iterator it_tgcCoinData   = tgcCoinColl->begin();  
      TgcCoinDataCollection::const_iterator it_tgcCoinData_e = tgcCoinColl->end();  
      for(; it_tgcCoinData!=it_tgcCoinData_e; it_tgcCoinData++) nCoin[ibc]++;  
      }  
      std::cout << " " << nHit[ibc] << " " << nCoin[ibc];  
      }  
      std::cout << std::endl;  
      return;  
  */  
  // For debugging purpose (end)  
  if(!(this->msgLvl(MSG::INFO))) return; 

  ATH_MSG_INFO("**************************************************************************************************");
  ATH_MSG_INFO("************** Listing TgcPrepData collections content *******************************************");

  const TgcPrepDataCollection* tgcColl;
  for(int ibc=0; ibc<NBC; ibc++) {
    if(tgcPrepDataContainer[ibc]->size()==0) ATH_MSG_INFO("No TgcPrepRawData collections found");

    ATH_MSG_INFO("--------------------------------------------------------------------------------------------");
    IdentifiableContainer<Muon::TgcPrepDataCollection>::const_iterator tgcColli = tgcPrepDataContainer[ibc]->begin();
    IdentifiableContainer<Muon::TgcPrepDataCollection>::const_iterator tgcColli_e = tgcPrepDataContainer[ibc]->end();
    for(; tgcColli!=tgcColli_e; ++tgcColli) {
      tgcColl = *tgcColli;

      ATH_MSG_INFO("TgcPrepDataContainer of "
		   << (ibc==0 ? "PriorBC" : "")
		   << (ibc==1 ? "CurrentBC" : "")
		   << (ibc==2 ? "NextBC" : ""));

      ATH_MSG_INFO("PrepData Collection ID "
		   << m_idHelperSvc->tgcIdHelper().show_to_string(tgcColl->identify()));

      for (const TgcPrepData* tgcPrepData : *tgcColl) {
	ATH_MSG_INFO("PrepData Offline ID "
		     << m_idHelperSvc->tgcIdHelper().show_to_string(tgcPrepData->identify()));
      }
    }
  }

  const TgcCoinDataCollection* tgcCoinColl;
  for(int ibc=0; ibc<NBC; ibc++) {
    if(tgcCoinDataContainer[ibc]->size()==0) ATH_MSG_INFO("No TgcCoinData collections found");

    ATH_MSG_INFO("--------------------------------------------------------------------------------------------");

    IdentifiableContainer<Muon::TgcCoinDataCollection>::const_iterator tgcCoinColli 
      = tgcCoinDataContainer[ibc]->begin();
    IdentifiableContainer<Muon::TgcCoinDataCollection>::const_iterator tgcCoinColli_e 
      = tgcCoinDataContainer[ibc]->end();
    for(; tgcCoinColli!=tgcCoinColli_e; ++tgcCoinColli) {
      tgcCoinColl = *tgcCoinColli;

      ATH_MSG_INFO("TgcCoinDataContainer of "
		   << (ibc==0 ? "PriorBC" : "")
		   << (ibc==1 ? "CurrentBC" : "")
		   << (ibc==2 ? "NextBC" : ""));

      ATH_MSG_INFO("CoinData Collection ID "
		   << m_idHelperSvc->tgcIdHelper().show_to_string(tgcCoinColl->identify()));

      for (const TgcCoinData* tgcCoinData : *tgcCoinColl) {
	ATH_MSG_INFO("CoinData Offline ID "
		     << m_idHelperSvc->tgcIdHelper().show_to_string(tgcCoinData->identify()));
      }
    }
  }
}

void Muon::TgcRdoToPrepDataToolCore::selectDecoder(getHitCollection_func& getHitCollection,
                                                   getCoinCollection_func& getCoinCollection,
                                                   const TgcRawData& rd,
                                                   const TgcRdo* rdoColl) const
{
  StatusCode status = StatusCode::SUCCESS;
  if(!status.isSuccess()) return;

  const CablingInfo* cinfo = getCabling();
  if (!cinfo) {
    return;
  }

  if(!rd.isCoincidence()) {
    status = decodeHits(getHitCollection, rd);
    if(!status.isSuccess()) {
      ATH_MSG_WARNING("Cannot decode TGC Hits");
    }
  } else if(rd.isCoincidence() && m_fillCoinData && cinfo->m_is12fold) { // coincidence start

    if((rd.type()==TgcRawData::TYPE_TRACKLET)) {
      if((rd.slbType()==TgcRawData::SLB_TYPE_DOUBLET_WIRE) || 
	 (rd.slbType()==TgcRawData::SLB_TYPE_DOUBLET_STRIP)) { 
	status = decodeTracklet(getCoinCollection, rd);
      } else if((rd.slbType()==TgcRawData::SLB_TYPE_INNER_WIRE) ||
		(rd.slbType()==TgcRawData::SLB_TYPE_INNER_STRIP)) {
	status = decodeTrackletEIFI(getCoinCollection, rd);
      }
    } else if((rd.type()==TgcRawData::TYPE_HIPT) && 
	      (rd.isHipt())) {
      status = decodeHiPt(getCoinCollection, rd);
    } else if((rd.type()==TgcRawData::TYPE_HIPT) && 
	      ((rd.sector() & 4) != 0)) { // inner
      status = decodeInner(getCoinCollection, rd);
    } else if((rd.type()==TgcRawData::TYPE_SL)) {
      status = decodeSL(getCoinCollection, rd, rdoColl);
    }
    if(!status.isSuccess()) {
      ATH_MSG_WARNING("Cannot decode TGC Coincidences");
    }
  }
}

StatusCode Muon::TgcRdoToPrepDataToolCore::decodeHits(getHitCollection_func& getHitCollection,
                                                      const TgcRawData& rd) const
{
  m_nHitRDOs++; // Count the number of input Hit RDOs.
  bool isConverted = false;
  bool isDuplicated = false; 
  bool isInvalid = false; 

  ATH_MSG_DEBUG("for debugging "
		<< " sub=" << rd.subDetectorId()
		<< " rod=" << rd.rodId()
		<< " ssw=" << rd.sswId()
		<< " slb=" << rd.slbId()
		<< " bitpos=" << rd.bitpos());

  const CablingInfo* cinfo = getCabling();
  if (!cinfo) {
    return StatusCode::FAILURE;
  }
  const ITGCcablingSvc* tgcCabling = cinfo->m_tgcCabling;

  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();

  TgcPrepDataCollection* collection = nullptr;
  TgcPrepDataCollection* collectionAllBcs = nullptr;
  IdentifierHash tgcHashId;
  IdContext tgcContext = m_idHelperSvc->tgcIdHelper().module_context();
  
  // select current Hits, =0 for backward compatibility
  // BC_CURRENT=2, BC_UNDEFINED=0
  int locId = (rd.bcTag()==TgcDigit::BC_CURRENT || rd.bcTag()==TgcDigit::BC_UNDEFINED) 
    ? 1 : rd.bcTag()-1;
  
  // repeat two times for ORed channel
  for(int iOr=0; iOr<2; iOr++) {
    bool orFlag = false;
    // check if this channel has ORed partner only when 2nd time
    if(iOr != 0) {
      bool o_found = tgcCabling->isOredChannel(rd.subDetectorId(),
                                               rd.rodId(),
                                               rd.sswId(),
                                               rd.slbId(),
                                               rd.bitpos());
      // set OR flag
      if(o_found) orFlag = true;
      else continue;
    }
    
    // get element ID
    Identifier elementId;
    bool e_found = tgcCabling->getElementIDfromReadoutID(elementId,
                                                         rd.subDetectorId(),
                                                         rd.rodId(),
                                                         rd.sswId(),
                                                         rd.slbId(),
                                                         rd.bitpos(), 
                                                         orFlag);
    if(!e_found) {
      if(!orFlag && cinfo->m_is12fold) {
	bool show_warning_level = true;

	/* One invalid channel in sector A09: 
	   sub=103 rod=9 ssw=6 slb=20 bitpos=151 +offset=0 orFlag=0
	   was always seen in 2008 data, at least run 79772 - 91800.
	   bug #48828 */
	/* One invalid channel in sector A11: 
	   sub=103 rod=11 ssw=2 slb=8 bitpos=41 orFlag=0 
	   was seen 5 times in 1,059,867 events of run 159179. */ 
	/* EIFI of MC ByteStream without correction issue : bug 57051 */ 
	if((rd.subDetectorId()==103 &&
	    rd.rodId()==9 &&
	    rd.sswId()==6 && 
	    rd.slbId()==20 && 
	    rd.bitpos()==151) || 
	   (rd.subDetectorId()==103 &&
            rd.rodId()==11 &&
            rd.sswId()==2 &&
            rd.slbId()==8 &&
            rd.bitpos()==41) ||
	   (rd.rodId()%3==2 &&
	    rd.sswId()==8)
	   ) {
	  show_warning_level = m_show_warning_level_invalid_A09_SSW6_hit;
	  isInvalid = true; 
	} 

	msg(show_warning_level ? MSG::WARNING : MSG::DEBUG) 
	  << "ElementID not found for "
	  << " sub=" << rd.subDetectorId()
	  << " rod=" << rd.rodId()
	  << " ssw=" << rd.sswId()
	  << " slb=" << rd.slbId()
	  << " bitpos=" << rd.bitpos() 
	  << " orFlag=" << orFlag 
	  << endmsg;
      }
      continue;
    }
    
    if(m_idHelperSvc->tgcIdHelper().get_hash(elementId, tgcHashId, &tgcContext)) {
      ATH_MSG_WARNING("Unable to get TGC hash id from TGC RDO collection "
		      << "context begin_index = " << tgcContext.begin_index()
		      << " context end_index  = " << tgcContext.end_index()
		      << " the identifier is ");
      elementId.show();
    }

    collection = getHitCollection (elementId, locId);

    // convert RawData to PrepRawData
    Identifier channelId;    
    bool c_found = tgcCabling->getOfflineIDfromReadoutID(channelId,
                                                         rd.subDetectorId(),
                                                         rd.rodId(),
                                                         rd.sswId(),
                                                         rd.slbId(),
                                                         rd.bitpos(),
                                                         orFlag);
    if(!c_found) {
      if(!orFlag) {
	ATH_MSG_WARNING("OfflineID not found for "
			<< " sub=" << rd.subDetectorId()<< " rod=" << rd.rodId()
			<< " ssw=" << rd.sswId()<< " slb=" << rd.slbId()
			<< " bitpos=" << rd.bitpos()<< " orFlag=" << orFlag);
      }
      continue;
    }

    // Check the hit is duplicated or not 
    bool duplicate = false;
    for (const TgcPrepData* tgcPrepData : *collection) {
      if(channelId==tgcPrepData->identify()) {
	duplicate = true;
	ATH_MSG_DEBUG("Duplicated TgcPrepData(removed) = "
		      << m_idHelperSvc->tgcIdHelper().show_to_string(channelId));
	break;
      }
    }
    if(duplicate) {
      isDuplicated = true; // A converted PRD of this RDO is duplicated. 
      continue;
    }
    
    const MuonGM::TgcReadoutElement* descriptor = muDetMgr->getTgcReadoutElement(channelId);
    if(!isOfflineIdOKForTgcReadoutElement(descriptor, channelId)) {
      ATH_MSG_WARNING("decodeHits: MuonGM::TgcReadoutElement is invalid.");
      continue;
    }
    ATH_MSG_DEBUG("TGC RDO->PrepRawdata: " << m_idHelperSvc->tgcIdHelper().show_to_string(channelId));
    
    std::vector<Identifier> identifierList;
    identifierList.push_back(channelId);
    
    Amg::Vector3D position = descriptor->channelPos(channelId);
    Amg::Vector2D hitPos; 
    bool onSurface = descriptor->surface(channelId).globalToLocal(position,position,hitPos);
    // the globalToLocal should not fail, if it does produce a WARNING
    if(!onSurface) {
      ATH_MSG_WARNING("Muon::TgcRdoToPrepDataToolCore::decodeHits Amg::Vector2D* hitPos is null."); 
      continue; 
    }
    
    int gasGap  = m_idHelperSvc->tgcIdHelper().gasGap(channelId);
    int channel = m_idHelperSvc->tgcIdHelper().channel(channelId);
    double width = 0.;
    if(!static_cast<bool>(m_idHelperSvc->tgcIdHelper().isStrip(channelId))) { // wire
      width = getWidthWire(descriptor, gasGap, channel);
    } else { // strip
      width = getWidthStrip(descriptor, gasGap, channel, channelId);
    }
    if(width<s_cutDropPrdsWithZeroWidth && m_dropPrdsWithZeroWidth) { // Invalid PRD's whose widths are zero are dropped. 
      ATH_MSG_WARNING("decodeHits: width= " << width << " is smaller than s_cutDropPrdsWithZeroWidth= " << s_cutDropPrdsWithZeroWidth);
      continue; 
    }
    double errPos = width/sqrt(12.);
    
    Amg::MatrixX mat(1,1);
    mat.setIdentity();
    mat *= errPos*errPos;
    auto errHitPos = Amg::MatrixX(mat);

    // add the digit to the collection
    // new TgcPrepRawData
    TgcPrepData* newPrepData = new TgcPrepData(channelId, // Readout ID -> Offline ID
					       tgcHashId, // Readout ID -> Element ID -> Hash 
					       hitPos, // determined from channelId
					       identifierList, // holds channelId only
					       errHitPos, // determined from channelId
					       descriptor); // determined from channelId
    newPrepData->setHashAndIndex(collection->identifyHash(), collection->size());
    collection->push_back(newPrepData);
    isConverted = true; // This RDO is converted to at least one PRD.  

    // for All Bcs
    collectionAllBcs = getHitCollection (elementId, NBC);

    uint16_t bcBitMap = 0;
    if (locId == 0)      bcBitMap = TgcPrepData::BCBIT_PREVIOUS;
    else if (locId == 1) bcBitMap = TgcPrepData::BCBIT_CURRENT;
    else if (locId == 2) bcBitMap = TgcPrepData::BCBIT_NEXT;

    bool duplicateInAllBCs = false;
    TgcPrepDataCollection::iterator it_tgcPrepData   = collectionAllBcs->begin();
    TgcPrepDataCollection::iterator it_tgcPrepData_e = collectionAllBcs->end();
    for(; it_tgcPrepData!=it_tgcPrepData_e; ++it_tgcPrepData) {
      if(channelId==(*it_tgcPrepData)->identify()) {
        duplicateInAllBCs = true;
        break;
      }
    }
    if(duplicateInAllBCs) {
      TgcPrepData *prd = *it_tgcPrepData;
      uint16_t bcBitMap_tmp = prd->getBcBitMap();
      prd->setBcBitMap(bcBitMap_tmp | bcBitMap);
    } else {
      auto errHitPosAllBcs = Amg::MatrixX(errHitPos);
      TgcPrepData* newPrepDataAllBcs = new TgcPrepData(channelId, // Readout ID -> Offline ID
						       tgcHashId, // Readout ID -> Element ID -> Hash 
						       hitPos, // determined from channelId
						       identifierList, // holds channelId only
						       errHitPosAllBcs, // determined from channelId
						       descriptor,
						       bcBitMap);
      newPrepDataAllBcs->setHashAndIndex(collectionAllBcs->identifyHash(), collectionAllBcs->size());
      collectionAllBcs->push_back(newPrepDataAllBcs);
    }
  }

  if(isConverted) m_nHitPRDs++; // Count the number of output Hit PRDs.
  else if(isDuplicated || isInvalid) m_nHitRDOs--; // Reduce the number of input RDOs. 

  return StatusCode::SUCCESS;
}

StatusCode Muon::TgcRdoToPrepDataToolCore::decodeTracklet(getCoinCollection_func& getCoinCollection,
                                                          const TgcRawData& rd) const
{
  m_nTrackletRDOs++; // Count the number of input Tracklet RDOs. 

  const CablingInfo* cinfo = getCabling();
  if (!cinfo) {
    return StatusCode::FAILURE;
  }
  const ITGCcablingSvc* tgcCabling = cinfo->m_tgcCabling;

  bool found = false;
  
  //*** Get OfflineId of pivot plane (TGC3) start ***//
  Identifier channelIdOut;
  found = tgcCabling->getOfflineIDfromLowPtCoincidenceID(channelIdOut, 
                                                         rd.subDetectorId(), 
                                                         rd.rodId(),
                                                         rd.sswId(), 
                                                         rd.slbId(), 
                                                         rd.subMatrix(), 
                                                         rd.position(), 
                                                         false);
  if(!found) {
    ATH_MSG_DEBUG("decodeTracklet: can't get the OfflineIdOut");
    return StatusCode::SUCCESS;
  }
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();

  //*** Get OfflineId of pivot plane (TGC3) end ***//
  
  //*** Get OfflineId of non-pivot plane (TGC2) start ***//
  int tmp_slbId = 0;
  int tmp_subMatrix = 0;
  int tmp_position = 0;
  found = getTrackletInfo(rd, tmp_slbId, tmp_subMatrix, tmp_position);
  if(!found) {
    return StatusCode::SUCCESS;
  }
  Identifier channelIdIn;
  found = tgcCabling->getOfflineIDfromLowPtCoincidenceID(channelIdIn, 
                                                         rd.subDetectorId(), 
                                                         rd.rodId(),
                                                         rd.sswId(), 
                                                         tmp_slbId, 
                                                         tmp_subMatrix, 
                                                         tmp_position, 
                                                         true);		  
  if(!found) {
    ATH_MSG_DEBUG("decodeTracklet: can't get the OfflineIdIn");
    return StatusCode::SUCCESS;
  }
  //*** Get OfflineId of non-pivot plane (TGC2) end ***//
  
  IdentifierHash tgcHashId;
  IdContext tgcContext = m_idHelperSvc->tgcIdHelper().module_context();
  Identifier elementId = m_idHelperSvc->tgcIdHelper().elementID(channelIdOut);
  if(m_idHelperSvc->tgcIdHelper().get_hash(elementId, tgcHashId, &tgcContext)) {
    ATH_MSG_WARNING("Unable to get TGC hash id from TGC RDO collection "
		    << "context begin_index = " << tgcContext.begin_index()
		    << " context end_index  = " << tgcContext.end_index()
		    << " the identifier is ");
    elementId.show();
  }

  int locId = (rd.bcTag()==TgcDigit::BC_CURRENT || rd.bcTag()==TgcDigit::BC_UNDEFINED) 
    ? 1 : rd.bcTag()-1;

  TgcCoinDataCollection* coincollection = getCoinCollection (elementId, locId);

  int subMatrix = static_cast<int>(rd.subMatrix());
  int trackletId = static_cast<int>(2*rd.slbId()+subMatrix); 
  int delta = static_cast<int>(rd.delta());

  // Check duplicate digits
  for (const TgcCoinData* tgcCoinData : *coincollection) {
    if((TgcCoinData::TYPE_TRACKLET==tgcCoinData->type()) && // coincidence type
       (channelIdOut==tgcCoinData->identify()) && // channelIdOut, identify returns channelIdOut for Tracklet
       (channelIdIn==tgcCoinData->channelIdIn()) &&  // channelIdIn
       (trackletId==tgcCoinData->trackletId()) && // trackletId
       (delta==tgcCoinData->delta()) && // delta
       (subMatrix==tgcCoinData->sub()) // subMatrix 
       ) {
      ATH_MSG_DEBUG("Duplicated TgcCoinData (Tracklet) = "
		    << m_idHelperSvc->tgcIdHelper().show_to_string(channelIdIn));
      return StatusCode::SUCCESS;
    }
  }

  ATH_MSG_DEBUG("TGC RDO->Coindata for LowPT: " << m_idHelperSvc->tgcIdHelper().show_to_string(channelIdOut)); 

  //*** Get geometry of pivot plane (TGC3) start ***//
  const MuonGM::TgcReadoutElement* descriptor_o = muDetMgr->getTgcReadoutElement(channelIdOut);
  if(!isOfflineIdOKForTgcReadoutElement(descriptor_o, channelIdOut)) {
    return StatusCode::SUCCESS;
  }
  
  int gasGap_o = m_idHelperSvc->tgcIdHelper().gasGap(channelIdOut);
  int channel_o = m_idHelperSvc->tgcIdHelper().channel(channelIdOut);
  double width_o = 0.;
  if(!(static_cast<bool> (m_idHelperSvc->tgcIdHelper().isStrip(channelIdOut)))) { // wire 
    width_o = getWidthWire(descriptor_o, gasGap_o, channel_o);
  } else { // strip
    width_o = getWidthStrip(descriptor_o, gasGap_o, channel_o, channelIdOut);
  }
  if(width_o<s_cutDropPrdsWithZeroWidth && m_dropPrdsWithZeroWidth) { // Invalid PRD's whose widths are zero are dropped.
    return StatusCode::SUCCESS;
  }
  
  Amg::Vector3D position_o = descriptor_o->channelPos(channelIdOut);
  Amg::Vector2D hitPos_o;
  bool onSurface_o = descriptor_o->surface(channelIdOut).globalToLocal(position_o,position_o,hitPos_o);
  // the globalToLocal should not fail, if it does produce a WARNING
  if(!onSurface_o) {
    ATH_MSG_WARNING("Muon::TgcRdoToPrepDataToolCore::decodeTracklet Amg::Vector2D* hitPos_o is null.");
    return StatusCode::SUCCESS;
  }  
  //*** Get geometry of pivot plane (TGC3) end ***//
  
  //*** Get geometry of non-pivot plane (TGC2) start ***//
  const MuonGM::TgcReadoutElement* descriptor_i = muDetMgr->getTgcReadoutElement(channelIdIn);
  if(!isOfflineIdOKForTgcReadoutElement(descriptor_i, channelIdIn)) {
    return StatusCode::SUCCESS;
  }
  
  int gasGap_i = m_idHelperSvc->tgcIdHelper().gasGap(channelIdIn);
  int channel_i = m_idHelperSvc->tgcIdHelper().channel(channelIdIn);
  double width_i = 0.;
  if(!(static_cast<bool>(m_idHelperSvc->tgcIdHelper().isStrip(channelIdIn)))) { // wire 
    width_i = getWidthWire(descriptor_i, gasGap_i, channel_i);
  } else { // strip
    width_i = getWidthStrip(descriptor_i, gasGap_i, channel_i, channelIdIn);
  }
  if(width_i<s_cutDropPrdsWithZeroWidth && m_dropPrdsWithZeroWidth) { // Invalid PRD's whose widths are zero are dropped.
    return StatusCode::SUCCESS;
  }

  Amg::Vector3D position_i = descriptor_i->channelPos(channelIdIn);
  Amg::Vector2D hitPos_i;
  bool onSurface_i = descriptor_i->surface(channelIdIn).globalToLocal(position_i,position_i,hitPos_i);
  // the globalToLocal should not fail, if it does produce a WARNING
  if(!onSurface_i) {
    ATH_MSG_WARNING("Muon::TgcRdoToPrepDataToolCore::decodeTracklet Amg::Vector2D* hitPos_i is null."); 
    return StatusCode::SUCCESS;
  }  
  //*** Get geometry of non-pivot plane (TGC2) end ***//
  const Amg::Vector2D* hitPosition_o = new Amg::Vector2D(hitPos_o);
  const Amg::Vector2D* hitPosition_i = new Amg::Vector2D(hitPos_i);
  // Add the digit to the collection
  TgcCoinData* newCoinData = new TgcCoinData(channelIdIn,
					     channelIdOut,
					     tgcHashId, // determined from channelIdOut
					     descriptor_i, // determined from channelIdIn
					     descriptor_o, // determined from channelIdOut
					     TgcCoinData::TYPE_TRACKLET, 
					     (rd.subDetectorId()==ASIDE), // isAside
					     static_cast<int>(m_idHelperSvc->tgcIdHelper().stationPhi(channelIdOut)), // phi
                                             0, // isInner
					     (rd.sswId()==7||rd.sswId()==2), // isForward
					     static_cast<bool>(m_idHelperSvc->tgcIdHelper().isStrip(channelIdOut)), // isStrip
					     trackletId, // trackletId
					     hitPosition_i, // determined from channelIdIn
					     hitPosition_o, // determined from channelIdOut
					     width_i, // determined from channelIdIn 
					     width_o, // determined from channelIdOut
					     delta, // delta
					     subMatrix,
                                             0); // subMatrix
  newCoinData->setHashAndIndex(coincollection->identifyHash(), coincollection->size());  
  coincollection->push_back(newCoinData);

  ATH_MSG_DEBUG("coincollection->push_back done (for LowPT)");

  m_nTrackletPRDs++; // Count the number of output Tracklet PRDs. 
  
  return StatusCode::SUCCESS;
}

StatusCode Muon::TgcRdoToPrepDataToolCore::decodeTrackletEIFI(getCoinCollection_func& getCoinCollection,
                                                              const TgcRawData& rd) const
{
  // Count the number of input TrackletEIFI RDOs. 
  m_nTrackletEIFIRDOs++; 

  const CablingInfo* cinfo = getCabling();
  if (!cinfo) {
    return StatusCode::FAILURE;
  }
  const ITGCcablingSvc* tgcCabling = cinfo->m_tgcCabling;

  // Determine chamber type 
  bool isStrip = (rd.slbType()==TgcRawData::SLB_TYPE_INNER_STRIP);
  bool isAside = (rd.subDetectorId()==ASIDE); 
  // https://twiki.cern.ch/twiki/pub/Main/TgcDocument/EIFI_PSB_SSW_ConnectionTable_v20080808.pdf
  bool isForward = (rd.slbId()%2==0);
  // Assuming RXID in the above file is equal to slbId 
  // rodId:     2                 5                 8                11
  // slbId FI:  0  2  4  6  8 10  0  2  4  6  8 10  0  2  4  6  8 10  0  2  4  6  8 10 
  //       EI:  1  3  5  7  9 11  1  3  5 XX  9 11  1  3  5  7  9 XX  1  3  5 XX  9 11  
  // slot:     24  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
  // stationPhi
  //       FI: 24  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
  //       EI: 21  1  2  3  4  5  6  7  8 XX  9 10 11 12 13 14 15 XX 16 17 18 XX 19 20  
  int slot = ((rd.slbId()/2) + (rd.rodId()-2)*2 + 23)%24 + 1; 
  // isBackward is determined according to MuonSpectrometer/MuonCablings/TGCcablingInterface/src/TGCIdBase.cxx
  // http://atlas-proj-tgc.web.cern.ch/atlas-proj-tgc/docs/numbering.pdf Information for FI is incorrect.  
  bool isBackward = false; 
  if(!isForward) { // EI
    // Special case of EI11
    if(slot==15) {
      isBackward = !isAside;
    } else if(slot==16) {
      isBackward =  isAside;
    } else {
      //  A-side phi0 F: phi1 F: phi2 B
      //  C-side phi0 B: phi1 B: phi2 F
      if(isAside) isBackward = (slot%3==2);
      else        isBackward = (slot%3!=2);
    }
  } else { // FI
    isBackward = isAside;
  }

  // Determine bitpos based on wire/strip and subMatrix 
  // Input is chosen so that gasGap is 1 and channel is 4,12,20,28.  
  // One subMatrix covers 8 channels (=BIT_POS_ASD_SIZE/2)
  uint16_t bitpos = 0;
  if(!isStrip) { // Wire
    // For wires, D-Input is gasGap=1 

    int tmpsubMatrix = static_cast<int>(rd.subMatrix());
    if(!isForward) { // If an edge channel of EI fires, subMatrix of the tracklet can be greater than one of the hit channel.  
      if(tmpsubMatrix==3) {
	if(            slot== 1 || 
                       slot== 3 || slot== 4 || slot== 5 || 
                       slot== 6 || slot== 7 || slot== 8 || 
	               slot==10 || slot==11 ||
	               slot==13 || 
                       slot==18 || slot==19 || slot==20) {
	  // These slots have only 24 wire channels (=3 submatrixes)  
	  tmpsubMatrix = 2;
	}
      } else if(tmpsubMatrix==2) {
	if(slot==24 ||             slot== 2 || 
	   slot==12 ||             slot==14 ||
	   slot==15 || slot==16 || 
           slot==22 || slot==23) {
	  // These slots have only 16 wire channels (=2 submatrixes)  
	  tmpsubMatrix = 1;
	}
      }
    }

    bitpos = BIT_POS_D_INPUT_ORIGIN - BIT_POS_INPUT_SIZE + 1 + BIT_POS_ASD_SIZE/4*(tmpsubMatrix*2+1); 
  } else { // Strip
    if(isBackward) {
      // For Backward chambers, B-Input is gasGap 1
      bitpos = BIT_POS_B_INPUT_ORIGIN - BIT_POS_INPUT_SIZE + (isAside ? 1 : 0) + BIT_POS_ASD_SIZE/4*(rd.subMatrix()*2+1); 
    } else {
      // For Forward  chambers, A-Input is gasGap 1
      bitpos = BIT_POS_A_INPUT_ORIGIN - BIT_POS_INPUT_SIZE + (isAside ? 0 : 1) + BIT_POS_ASD_SIZE/4*(rd.subMatrix()*2+1); 
    }
  }

  // Retrieve OfflineID from ReadoutID
  Identifier channelIdIn;
  bool o_found = tgcCabling->getOfflineIDfromReadoutID(channelIdIn,
                                                       rd.subDetectorId(),
                                                       rd.rodId(),
                                                       rd.sswId(),
                                                       rd.slbId(),
                                                       bitpos,
                                                       false/*orflag*/);
  if(!o_found) {
    ATH_MSG_WARNING("Muon::TgcRdoToPrepDataToolCore::decodeTrackletEIFI OfflineID not found for "
		    << " subDetectorId=" << rd.subDetectorId()
		    << " rodId=" << rd.rodId()
		    << " sswId=" << rd.sswId()
		    << " slbId=" << rd.slbId() 
		    << " slbType=" << rd.slbType() 
		    << " subMatrix=" << rd.subMatrix() 
		    << " bitpos=" << bitpos
		    << " isStrip=" << isStrip
		    << " isAside=" << isAside
		    << " isForward=" << isForward
		    << " slot=" << slot
		    << " isBackward=" << isBackward);
    return StatusCode::SUCCESS;
  }

  // Retrieve ElementID from OfflineID
  Identifier elementId = m_idHelperSvc->tgcIdHelper().elementID(channelIdIn);

  // Retrieve Hash from ElementID
  IdentifierHash tgcHashId;
  IdContext tgcContext = m_idHelperSvc->tgcIdHelper().module_context();
  if(m_idHelperSvc->tgcIdHelper().get_hash(elementId, tgcHashId, &tgcContext)) {
    ATH_MSG_WARNING("Unable to get TGC hash id from TGC RDO collection "
                    << "context begin_index = " << tgcContext.begin_index()
                    << " context end_index  = " << tgcContext.end_index()
                    << " the identifier is ");
    elementId.show();
  }

  // Index is determined based on bcTag. 
  int locId = (rd.bcTag()==TgcDigit::BC_CURRENT || rd.bcTag()==TgcDigit::BC_UNDEFINED)
    ? 1 : rd.bcTag()-1;

  // Get corresponding collection 
  TgcCoinDataCollection* coincollection = getCoinCollection (elementId, locId);

  // Check duplicate digits
  for (const TgcCoinData* tgcCoinData : *coincollection) {
    if((TgcCoinData::TYPE_TRACKLET_EIFI==tgcCoinData->type()) && // coincidence type
       (channelIdIn==tgcCoinData->channelIdIn()) && // channelIdIn
       (static_cast<int>(rd.subMatrix())==tgcCoinData->sub())) { // sub
      ATH_MSG_DEBUG("Duplicated TgcCoinData (TrackletEIFI) = "
		    << m_idHelperSvc->tgcIdHelper().show_to_string(channelIdIn));
      return StatusCode::SUCCESS;
    }
  }

  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();
  // Get MuonGM::TgcReadoutElement from channelIdIn
  const MuonGM::TgcReadoutElement* descriptor = muDetMgr->getTgcReadoutElement(channelIdIn);
  if(!isOfflineIdOKForTgcReadoutElement(descriptor, channelIdIn)) {
    ATH_MSG_WARNING("Muon::TgcRdoToPrepDataToolCore::decodeTrackletEIFI descriptor doesn't contain " 
		    << m_idHelperSvc->tgcIdHelper().show_to_string(channelIdIn));
    return StatusCode::SUCCESS;
  }

  // Get Amg::Vector2D from channelIdIn //here I am
  Amg::Vector3D position = descriptor->channelPos(channelIdIn);
  Amg::Vector2D hitPos;
  bool onSurface = descriptor->surface(channelIdIn).globalToLocal(position,position,hitPos);
  // the globalToLocal should not fail, if it does produce a WARNING
  if(!onSurface) {
    ATH_MSG_WARNING("Muon::TgcRdoToPrepDataToolCore::decodeTrackletEIFI Amg::Vector2D* hitPos is null.");
    return StatusCode::SUCCESS;
  }

  // Get width from channelIdIn, one subMatrix covers 8 channels per gasGap 
  double width = 0.;
  int gasGap = m_idHelperSvc->tgcIdHelper().gasGap(channelIdIn); // 1
  int channel = m_idHelperSvc->tgcIdHelper().channel(channelIdIn); // 4, 12, 20, 28
  if(isStrip) { // Strip
    double localZ = (descriptor->transform(channelIdIn).inverse()*descriptor->channelPos(channelIdIn)).z();
    double stripMaxX  = descriptor->stripMaxX(gasGap, channel+4, localZ);
    double stripMinX  = descriptor->stripMinX(gasGap, channel-3, localZ);
    width = stripMaxX - stripMinX;
  } else { // Wire
    int positiveOffset = +4;
    if(isForward && (slot%3==2) && channel==28) positiveOffset = +2; // T10S has only 30 channels.  
    double gangMaxZ = descriptor->gangMaxZ(gasGap, channel+positiveOffset);
    double gangMinZ = descriptor->gangMinZ(gasGap, channel-3);
    width = gangMaxZ - gangMinZ;
  }
  if(width<s_cutDropPrdsWithZeroWidth) {
    ATH_MSG_WARNING("Muon::TgcRdoToPrepDataToolCore::decodeTrackletEIFI Width for " << (isStrip ? "strip" : "wire") 
		    << "is invalid : gasGap = " << gasGap << " channel = " << channel << " width = " << width);
    return StatusCode::SUCCESS;
  }

  const Amg::Vector2D* hitPosition = new Amg::Vector2D(hitPos);
  // Add the digit to the collection
  TgcCoinData* newCoinData = new TgcCoinData(channelIdIn, 
					     tgcHashId, // determined from channelIdIn
                                             descriptor, // determined from channelIdIn
					     TgcCoinData::TYPE_TRACKLET_EIFI,
                                             isAside, 
                                             m_idHelperSvc->tgcIdHelper().stationPhi(channelIdIn), 
                                             isForward, 
                                             isStrip, 
                                             hitPosition, // determined from channelIdIn
                                             width, // determined from channelIdIn
                                             static_cast<int>(rd.subMatrix())); // subMatrix
  newCoinData->setHashAndIndex(coincollection->identifyHash(), coincollection->size());  
  coincollection->push_back(newCoinData);

  // Count the number of output Tracklet EIFI PRDs.
  m_nTrackletEIFIPRDs++; 
  return StatusCode::SUCCESS;
}

StatusCode Muon::TgcRdoToPrepDataToolCore::decodeHiPt(getCoinCollection_func& getCoinCollection,
                                                      const TgcRawData& rd) const
{
  m_nHiPtRDOs++; // Count the number of input HiPt RDOs. 

  const CablingInfo* cinfo = getCabling();
  if (!cinfo) {
    return StatusCode::FAILURE;
  }
  const ITGCcablingSvc* tgcCabling = cinfo->m_tgcCabling;

  // Protection against invalid subDetectorId and isForward
  if((rd.subDetectorId()!=ASIDE && rd.subDetectorId()!=CSIDE)) {
    ATH_MSG_DEBUG("TgcRdoToPrepDataToolCore::decodeHiPt::Unknown subDetectorId!!");
    return StatusCode::SUCCESS;
  }

  // Protection against invalid hitId
  if(rd.hitId()==0) {
    ATH_MSG_DEBUG("Invalid hitId_rdo_hipt, hitId == 0!! skip to convert this RDO to PRD");
    return StatusCode::SUCCESS;
  }
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();

  TgcCoinDataCollection* coincollection = nullptr;
  IdentifierHash tgcHashId;
  IdContext tgcContext = m_idHelperSvc->tgcIdHelper().module_context();
  
  int slbsubMatrix = 0;
  bool isBackward = isBackwardBW(rd);  // Backward or Forward
  int DeltaBeforeConvert = getDeltaBeforeConvert(rd);
  
  bool found = false;
  
  Identifier channelIdOut_tmp;
  Identifier channelIdOut[2];
  int bitpos_o[2] = {0, 0};
  int slbchannel_o[2] = {0, 0};
  int sswId_o = 0;
  int sbLoc_o = 0;
  int slbId_o = 0;
  
  Identifier channelIdIn_tmp;
  Identifier channelIdIn[4];
  int bitpos_i[4] = {0, 0, 0, 0};
  int slbchannel_i[4] = {0, 0, 0, 0};
  int sswId_i = 0;
  int sbLoc_i = 0;
  int slbId_in[4] = {0, 0, 0, 0}; // only used for wire
  int sbLoc_in[4] = {0, 0, 0, 0}; // only used for wire

  int gasGap_i[4] = {0, 0, 0, 0}; 
  int channel_i[4] = {0, 0, 0, 0}; 
  double width_i = 0.;
  double hit_position_i = 0.;
  Amg::Vector2D tmp_hitPos_i(0., 0.);

  int gasGap_o[2] = {0, 0}; 
  int channel_o[2] = {0, 0};
  double width_o = 0.;
  double hit_position_o = 0.;
  Amg::Vector2D tmp_hitPos_o(0., 0.);

  const MuonGM::TgcReadoutElement* descriptor_ii = nullptr;
  const MuonGM::TgcReadoutElement* descriptor_oo = nullptr;

  //*** TGC3 start ***//
  // RDOHighPtID --> (Sim)HighPtID --> OfflineID --> ReadoutID --> getSLBID
  found = getHiPtIds(rd, sswId_o, sbLoc_o, slbId_o);
  if(!found) {
    return StatusCode::SUCCESS;
  }

  // get the OfflineID of cernter of ROI of TGC3
  if(!(rd.isStrip())) { // wire
    getBitPosOutWire(rd, slbsubMatrix, bitpos_o);
  } else { // strip
    getBitPosOutStrip(rd, slbsubMatrix, bitpos_o);
  }
  for(int i=0; i<2; i++) {
    found = tgcCabling->getOfflineIDfromReadoutID(channelIdOut[i], rd.subDetectorId(), rd.rodId(), 
                                                  sswId_o, sbLoc_o, bitpos_o[i]);
    if(!found ) {
      ATH_MSG_DEBUG("Failed to get OfflineID from ReadoutID for Pivot "
                    << (rd.isStrip() ? "Strip" : "Wire") << ".");
      return StatusCode::SUCCESS;
    }
  }
   
  //}
  //*** TGC3 end ***//

  //*** TGC1 start ***//
  // get the OfflineID of cernter of ROI of TGC1
  if(!(rd.isStrip())) { // wire
    getBitPosInWire(rd, DeltaBeforeConvert, bitpos_i, slbchannel_i, slbId_in, sbLoc_in, sswId_i, bitpos_o, 
                    slbchannel_o, slbId_o);
  } else { // strip
    getBitPosInStrip(rd, DeltaBeforeConvert, bitpos_i, slbchannel_i, sbLoc_i, sswId_i, bitpos_o, slbchannel_o);
  }
  for(int i=0; i<4; i++) {
    found = tgcCabling->getOfflineIDfromReadoutID(channelIdIn[i], rd.subDetectorId(), rd.rodId(), sswId_i,
                                                  (rd.isStrip() ? sbLoc_i : sbLoc_in[i]), 
                                                  bitpos_i[i]);
    if(!found) {
      ATH_MSG_DEBUG("Failed to get OfflineID from ReadoutID for Pivot "
                    << (rd.isStrip() ? "Strip" : "Wire") << ".");
      return StatusCode::SUCCESS;
    }
  }
  //}
  //*** TGC1 end ***//
  
  ATH_MSG_DEBUG("TGC RDO->Coindata for HIPT: " << m_idHelperSvc->tgcIdHelper().show_to_string(channelIdOut[1])); 
  
  Identifier elementId = m_idHelperSvc->tgcIdHelper().elementID(channelIdOut[1]);
  if(m_idHelperSvc->tgcIdHelper().get_hash(elementId, tgcHashId, &tgcContext)) {
    ATH_MSG_WARNING("Unable to get TGC hash id from TGC RDO collection "
		    << "context begin_index = " << tgcContext.begin_index()
		    << " context end_index  = " << tgcContext.end_index()
		    << " the identifier is ");
    elementId.show();
  }

  int locId = (rd.bcTag()==TgcDigit::BC_CURRENT || rd.bcTag()==TgcDigit::BC_UNDEFINED) 
    ? 1 : rd.bcTag()-1;

  coincollection = getCoinCollection (elementId, locId);
  
  //*** TGC3 start ***// 
  // Get geometry of pivot plane 
  const MuonGM::TgcReadoutElement* descriptor_o[2] = {muDetMgr->getTgcReadoutElement(channelIdOut[0]), 
                                                      muDetMgr->getTgcReadoutElement(channelIdOut[1])};
  for(int i=0; i<2; i++) {
    if(!isOfflineIdOKForTgcReadoutElement(descriptor_o[i], channelIdOut[i])) {
      return StatusCode::SUCCESS;
    }
  }

  for(int i=0; i<2; i++) { 
    gasGap_o[i] = m_idHelperSvc->tgcIdHelper().gasGap(channelIdOut[i]);
    channel_o[i] = m_idHelperSvc->tgcIdHelper().channel(channelIdOut[i]);
  }

  if(!(rd.isStrip())) { // wire
    found = getPosAndIdWireOut(descriptor_o, channelIdOut,
                               gasGap_o, channel_o,
                               width_o, hit_position_o, tmp_hitPos_o,
                               channelIdOut_tmp);
  } else { //strip
    found = getPosAndIdStripOut(descriptor_o, channelIdOut,
                                gasGap_o, channel_o,
                                width_o, hit_position_o, tmp_hitPos_o,
                                channelIdOut_tmp,
                                isBackward, (rd.subDetectorId()==ASIDE));
  }
  if(!found) {
    return StatusCode::SUCCESS;
  }
  if(width_o<s_cutDropPrdsWithZeroWidth && m_dropPrdsWithZeroWidth) { // Invalid PRD's whose widths are zero are dropped.
    return StatusCode::SUCCESS;
  }

  descriptor_oo = muDetMgr->getTgcReadoutElement(channelIdOut_tmp);
  if(!isOfflineIdOKForTgcReadoutElement(descriptor_oo, channelIdOut_tmp)) {
    return StatusCode::SUCCESS;
  }
  //}
  //*** TGC3 end ***// 
  
  //*** TGC1 start ***// 
  // Get geometry of non-pivot plane 
  const MuonGM::TgcReadoutElement* descriptor_i[4] = {muDetMgr->getTgcReadoutElement(channelIdIn[0]), 
                                                      muDetMgr->getTgcReadoutElement(channelIdIn[1]), 
                                                      muDetMgr->getTgcReadoutElement(channelIdIn[2]), 
                                                      muDetMgr->getTgcReadoutElement(channelIdIn[3])};
  for(int i=0; i<4; i++) {
    if(!isOfflineIdOKForTgcReadoutElement(descriptor_i[i], channelIdIn[i])) {
      return StatusCode::SUCCESS;
    }
  }
  for(int i=0; i<4; i++) { 
    gasGap_i[i] = m_idHelperSvc->tgcIdHelper().gasGap(channelIdIn[i]); 
    channel_i[i] = m_idHelperSvc->tgcIdHelper().channel(channelIdIn[i]); 
  }
    
  if(!rd.isStrip()) { // WIRE
    found = getPosAndIdWireIn(descriptor_i, channelIdIn,
                              gasGap_i, channel_i,
                              width_i, hit_position_i, tmp_hitPos_i,
                              channelIdIn_tmp);
  } else { // STRIP
    found = getPosAndIdStripIn(descriptor_i, channelIdIn,
                               gasGap_i, channel_i,
                               width_i, hit_position_i, tmp_hitPos_i,
                               channelIdIn_tmp,
                               isBackward, (rd.subDetectorId()==ASIDE));
  }
  if(!found) {
    return StatusCode::SUCCESS;
  }
  if(width_i<s_cutDropPrdsWithZeroWidth && m_dropPrdsWithZeroWidth) { // Invalid PRD's whose widths are zero are dropped.
    return StatusCode::SUCCESS;
  }

  descriptor_ii = muDetMgr->getTgcReadoutElement(channelIdIn_tmp);
  if(!isOfflineIdOKForTgcReadoutElement(descriptor_ii, channelIdIn_tmp)) {
    return StatusCode::SUCCESS;
  }
  //}
  //*** TGC1 end ***// 

  int trackletId = 2*sbLoc_o + slbsubMatrix;
  int delta = static_cast<int>(rd.delta());
  int hsub = static_cast<int>(rd.hsub());
  int inner = static_cast<int>(rd.inner());

  // check duplicate digits
  for (const TgcCoinData* tgcCoinData : *coincollection) {
    if((TgcCoinData::TYPE_HIPT==tgcCoinData->type()) && // Coincidence type
       (channelIdOut_tmp==tgcCoinData->identify()) && // channelIdOut, identify returns channelIdOut for HiPt
       (channelIdIn_tmp==tgcCoinData->channelIdIn()) && // channelIdIn
       (trackletId==tgcCoinData->trackletId()) && // trackletId 
       (delta==tgcCoinData->delta()) && // delta
       (hsub==tgcCoinData->sub()) && // hsub
       (inner==tgcCoinData->inner())
       ) { 
      if(38<=trackletId && trackletId<=41) {
	// This drop is most probably due to the fix of the HiPt Endcap Strip Board bug. 
	m_nHiPtRDOs--; // Reduce the number of input RDOs. 
      }
      ATH_MSG_DEBUG("Duplicated TgcCoinData (HiPt) = "
		    << m_idHelperSvc->tgcIdHelper().show_to_string(channelIdOut_tmp));
      return StatusCode::SUCCESS;
    }
  }
  
  const Amg::Vector2D* hitPos_o = new Amg::Vector2D(tmp_hitPos_o);
  if(!hitPos_o) ATH_MSG_WARNING("Muon::TgcRdoToPrepDataToolCore::decodeHiPt Amg::Vector2D* hitPos_o is null");
  const Amg::Vector2D* hitPos_i = new Amg::Vector2D(tmp_hitPos_i);
  if(!hitPos_i) ATH_MSG_WARNING("Muon::TgcRdoToPrepDataToolCore::decodeHiPt Amg::Vector2D* hitPos_i is null");
 
  TgcCoinData* newCoinData = new TgcCoinData(channelIdIn_tmp,
					     channelIdOut_tmp,
					     tgcHashId, // determined from channelIdOut[1]
					     descriptor_ii, // determined from channelIdIn_tmp
					     descriptor_oo, // determined from channelIdOut_tmp
					     TgcCoinData::TYPE_HIPT, // Coincidence type
					     (rd.subDetectorId()==ASIDE), // isAside
					     static_cast<int>(m_idHelperSvc->tgcIdHelper().stationPhi(channelIdOut_tmp)), // phi
                                             0, // isInner
					     static_cast<bool>(rd.isForward()), // isForward
					     static_cast<bool>(rd.isStrip()), // isStrip
					     trackletId, // trackletId
					     hitPos_i, 
					     hitPos_o,
					     width_i,
					     width_o,
					     delta, // delta
					     hsub,  // hsub
                                             inner);
  // add the digit to the collection
  newCoinData->setHashAndIndex(coincollection->identifyHash(), coincollection->size());  
  coincollection->push_back(newCoinData);

  ATH_MSG_DEBUG("coincollection->push_back done (for HIPT)");

  m_nHiPtPRDs++; // Count the number of output HiPt PRDs. 
  
  return StatusCode::SUCCESS;
}

StatusCode Muon::TgcRdoToPrepDataToolCore::decodeInner(getCoinCollection_func& getCoinCollection,
                                                       const TgcRawData& rd) const
{
  m_nHiPtRDOs++; // Count the number of input HiPt RDOs. 

  const CablingInfo* cinfo = getCabling();
  if (!cinfo) {
    return StatusCode::FAILURE;
  }
  const ITGCcablingSvc* tgcCabling = cinfo->m_tgcCabling;

  int subDetectorId = rd.subDetectorId();
  // Protection against invalid subDetectorId and isForward
  if(subDetectorId!=ASIDE && subDetectorId!=CSIDE) {
    ATH_MSG_DEBUG("TgcRdoToPrepDataToolCore::decodeHiPt::Unknown subDetectorId!!");
    return StatusCode::SUCCESS;
  }

  bool isInner = ((rd.sector() & 4) != 0 ); // Inner flag for EIFI and Tilecal

  TgcCoinDataCollection* coincollection = nullptr;
  IdentifierHash tgcHashId;
  IdContext tgcContext = m_idHelperSvc->tgcIdHelper().module_context();
 
  Identifier channelIdIn; 
  Identifier channelIdOut; 
  int sswId_o = 9;
  int sbLoc_o = rd.sector() & 3;
  int inner   = rd.inner();
  
  int phi = 0; bool isAside = false; bool isEndcap;
  tgcCabling->getSLIDfromReadoutID(phi, isAside, isEndcap, subDetectorId, 
      rd.rodId(), sswId_o, sbLoc_o);
  
  int locId = (rd.bcTag()==TgcDigit::BC_CURRENT || rd.bcTag()==TgcDigit::BC_UNDEFINED) 
    ? 1 : rd.bcTag()-1;
  
  Amg::Vector2D tmp_hitPos_i(0., 0.);
  Amg::Vector2D tmp_hitPos_o(0., 0.);
  const Amg::Vector2D* hitPos_o = new Amg::Vector2D(tmp_hitPos_o);
  const Amg::Vector2D* hitPos_i = new Amg::Vector2D(tmp_hitPos_i);
  
  const MuonGM::TgcReadoutElement* descriptor_ii = nullptr;
  const MuonGM::TgcReadoutElement* descriptor_oo = nullptr;

  std::string stationName = "T3E"; 
  int stationEta = isAside ? 1 : -1;
  int stationPhi = phi;

  Identifier elementId = m_idHelperSvc->tgcIdHelper().elementID(stationName, stationEta, stationPhi, true);
  if(m_idHelperSvc->tgcIdHelper().get_hash(elementId, tgcHashId, &tgcContext)) {
    ATH_MSG_WARNING("Unable to get TGC hash id from TGC RDO collection "
		    << "context begin_index = " << tgcContext.begin_index()
		    << " context end_index  = " << tgcContext.end_index()
		    << " the identifier is ");
    elementId.show();
  }

  coincollection = getCoinCollection (elementId, locId);
  
  ATH_MSG_DEBUG("Inner Data Word, phi: " << phi << " isAside: " << isAside << " isEndcap: " << isEndcap
                << " subDetectorId: " << subDetectorId << " isStrip: " << rd.isStrip()
                << " rodId: " << rd.rodId() << " slbId: " << sbLoc_o << " inner:"<< rd.inner());

  TgcCoinData* newCoinData = new TgcCoinData(channelIdIn,  // empty
					     channelIdOut, // empty
					     tgcHashId, // determined from channelIdOut[1]
					     descriptor_ii, // determined from channelIdIn_tmp
					     descriptor_oo, // determined from channelIdOut_tmp
					     TgcCoinData::TYPE_HIPT, // Coincidence type
					     isAside, // isAside
					     phi, // phi
                                             isInner,
					     0, // isForward
					     static_cast<bool>(rd.isStrip()), // isStrip, Inner or Tile
					     0, // trackletId
					     hitPos_i, 
					     hitPos_o,
					     0., // width_i,
					     0., // width_o,
					     0, // delta,
					     0, // hsub,
                                             inner);
  // add the digit to the collection
  newCoinData->setHashAndIndex(coincollection->identifyHash(), coincollection->size());  
  coincollection->push_back(newCoinData);

  ATH_MSG_DEBUG("coincollection->push_back done (for Inner)");

  m_nHiPtPRDs++; // Count the number of output HiPt PRDs. 
  
  return StatusCode::SUCCESS;
}

StatusCode Muon::TgcRdoToPrepDataToolCore::decodeSL(getCoinCollection_func& getCoinCollection,
                                                    const TgcRawData& rd,
                                                    const TgcRdo* rdoColl) const
{
  m_nSLRDOs++; // Count the number of input SL RDOs. 

  // Protection against invalid subDetectorId
  if((rd.subDetectorId()!=ASIDE && rd.subDetectorId()!=CSIDE)) {
    ATH_MSG_DEBUG("TgcRdoToPrepDataToolCore::decodeSL::Unknown subDetectorId!!");
    return StatusCode::SUCCESS;
  }
  
  TgcCoinDataCollection* coincollection = nullptr;
  IdentifierHash tgcHashId;
  IdContext tgcContext = m_idHelperSvc->tgcIdHelper().module_context();
  
  bool found = false;

  Identifier channelId_wire[3];
  int index_w = 0;
  int chip_w = 0;
  int hitId_w = 0;
  int sub_w = 0;
  int sswId_w = 0;
  int sbLoc_w = 0;
  int subMatrix_w = 0;
  int bitpos_w[3] = {0, 0, 0};

  //***  get OfflineID, center of ROI of R (wire) ***//
  found = getSLIds(false, rd, channelId_wire, index_w, chip_w, hitId_w, sub_w, sswId_w, sbLoc_w, 
		   subMatrix_w, bitpos_w);
  if(!found) {
    return StatusCode::SUCCESS;
  }

  Identifier channelId_strip[3];
  int index_s = 0;
  int chip_s = 0;
  int hitId_s = 0;
  int sub_s = 0;
  int sswId_s = 0;
  int sbLoc_s = 0;
  int subMatrix_s = 0;
  int bitpos_s[3] = {0, 0, 0};

  //***  get OfflineID, center of ROI of phi (strip) ***//
  found = getSLIds(true, rd, channelId_strip, index_s, chip_s, hitId_s, sub_s, sswId_s, sbLoc_s, subMatrix_s, 
		   bitpos_s, isIncludedInChamberBoundary(rd), rdoColl, index_w, chip_w, hitId_w, sub_w);
  if(!found) {
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG("TGC RDO->TgcCoindata(SL): " << m_idHelperSvc->tgcIdHelper().show_to_string(channelId_wire[1])); 
  
  Identifier elementId = m_idHelperSvc->tgcIdHelper().elementID(channelId_wire[1]);
  if(m_idHelperSvc->tgcIdHelper().get_hash(elementId, tgcHashId, &tgcContext)) {
    ATH_MSG_WARNING("Unable to get TGC hash id from TGC RDO collection "
		    << "context begin_index = " << tgcContext.begin_index()
		    << " context end_index  = " << tgcContext.end_index()
		    << " the identifier is ");
    elementId.show();
  }

  int locId = (rd.bcTag()==TgcDigit::BC_CURRENT || rd.bcTag()==TgcDigit::BC_UNDEFINED) 
    ? 1 : rd.bcTag()-1;

  coincollection = getCoinCollection (elementId, locId);
  
  int trackletId = 2*sbLoc_w + subMatrix_w;
  int trackletIdStrip = 2*sbLoc_s + subMatrix_s;
  int roi = static_cast<int>(rd.roi());
  int pt = static_cast<int>(rd.threshold());
  bool veto = rd.isVeto();
  bool isPositiveDeltaR = rd.isMuplus(); // Current SL sets isMuplus flag based on sign of deltaR. 
                                              // Postive deltaR gives isMuplus=true.
  // check duplicate digits
  for (const TgcCoinData* tgcCoinData : *coincollection) {
    if((TgcCoinData::TYPE_SL==tgcCoinData->type()) &&
       (channelId_wire[2]==tgcCoinData->identify()) &&
       (trackletId==tgcCoinData->trackletId()) &&
       (trackletIdStrip==tgcCoinData->trackletIdStrip()) &&
       (roi==tgcCoinData->roi()) && 
       (pt==tgcCoinData->pt()) && 
       (veto==tgcCoinData->veto()) && 
       (isPositiveDeltaR==tgcCoinData->isPositiveDeltaR()) 
       ) {
      ATH_MSG_DEBUG("Duplicated TgcCoinData (SL) = "
		    << m_idHelperSvc->tgcIdHelper().show_to_string(channelId_wire[2]));
      return StatusCode::SUCCESS;
    }
  }
 
  //*** R (wire) start ***//
  double width_w = 0.;
  double tmp_r = 0.;
  double tmp_wire_z = 0.;
  found = getSLWireGeometry(channelId_wire, width_w, tmp_r, tmp_wire_z);
  if(!found) {
    return StatusCode::SUCCESS;
  }
  if(width_w<s_cutDropPrdsWithZeroWidth && m_dropPrdsWithZeroWidth) { // Invalid PRD's whose widths are zero are dropped.
    return StatusCode::SUCCESS;
  }

  double tmp_eta = 0.; 
  bool isGoodEta = getEtafromRZ(tmp_r, tmp_wire_z, tmp_eta); 
  if(!isGoodEta) { 
    ATH_MSG_WARNING("Conversion from r and z to eta by Muon::TgcRdoToPrepDataToolCore::getEtafromRZ failed."); 
    return StatusCode::SUCCESS; 
  } 
  if(tmp_wire_z<0.) tmp_eta *= -1.; 
  //*** R (wire) end ***//

  //*** Phi (strip) start ***//
  double width_s = 0.;
  double tmp_phi = 0.;
  found = getSLStripGeometry(channelId_strip, isBackwardBW(rd), (rd.subDetectorId()==ASIDE), width_s, tmp_phi);
  if(!found) {
    return StatusCode::SUCCESS;
  }
  if(width_s<s_cutDropPrdsWithZeroWidth && m_dropPrdsWithZeroWidth) { // Invalid PRD's whose widths are zero are dropped.
    return StatusCode::SUCCESS;
  }
  //*** Phi (strip) end ***//
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();
  const MuonGM::TgcReadoutElement* descriptor_w2 = muDetMgr->getTgcReadoutElement(channelId_wire[2]);
  if(!isOfflineIdOKForTgcReadoutElement(descriptor_w2, channelId_wire[2])) {
    return StatusCode::SUCCESS;
  }
  
  Amg::Vector3D tmp_gp(tmp_r*cos(tmp_phi), tmp_r*sin(tmp_phi), tmp_wire_z); 
  Amg::Vector2D tmp_hitPos;
  bool onSurface = descriptor_w2->surface(channelId_wire[2]).globalToLocal(tmp_gp,tmp_gp,tmp_hitPos);
    // If TGC A-lines with rotations are used, the z-coordinate of a chamber depends on position. 
    // In this case, the global to local conversion fails. 
    // Obtain the local position in a different way. 
  const Amg::Vector2D* hitPos = !onSurface ? new Amg::Vector2D(tmp_hitPos) : getSLLocalPosition(descriptor_w2, channelId_wire[2], tmp_eta, tmp_phi);

//  if(!hitPos) {  //No need to check should not be possible to fail
//    ATH_MSG_WARNING("Muon::TgcRdoToPrepDataToolCore::decodeSL Amg::Vector2D* hitPos is null.");  
//  } 

  Amg::MatrixX mat(2,2);
  mat.setIdentity();
  mat(0,0) = width_w;
  mat(1,1) = width_s;
  const Amg::MatrixX* errMat = new Amg::MatrixX(mat);


  // new TgcCoinData		  
  TgcCoinData* newCoinData = new TgcCoinData(channelId_wire[2],
					     tgcHashId, // determined from channelId_wire[1]
					     descriptor_w2, // determined from channelId_wire[2]
					     TgcCoinData::TYPE_SL, // Coincidence type
					     static_cast<bool>(rd.subDetectorId()==ASIDE), // isAside
					     static_cast<int>(m_idHelperSvc->tgcIdHelper().stationPhi(channelId_wire[2])), // phi
					     static_cast<bool> (rd.isForward()), // isForward
					     trackletId, // trackletId 
					     trackletIdStrip, // trackletIdStrip
					     hitPos,
					     errMat,
					     roi, // roi from RDO
					     pt, // threshold from RDO
					     veto, // veto flag from RDO
					     isPositiveDeltaR); // isMuplus from RDO

  // add the digit to the collection
  newCoinData->setHashAndIndex(coincollection->identifyHash(), coincollection->size());  
  coincollection->push_back(newCoinData);
  ATH_MSG_DEBUG("coincollection->push_back done (for SL)");

  m_nSLPRDs++; // Count the number of output SL PRDs.
  return StatusCode::SUCCESS;
}

int Muon::TgcRdoToPrepDataToolCore::getbitpos(int channel, TgcRawData::SlbType slbType) const
{
  int bitpos = -BIT_POS_INPUT_SIZE +1;
  if(slbType==TgcRawData::SLB_TYPE_TRIPLET_WIRE) {
    if(channel<0 || channel>=WT_MAP_SIZE) return -1; // Invalid channel

    if(     channel%3==0) bitpos += BIT_POS_C_INPUT_ORIGIN; // C-Input
    else if(channel%3==1) bitpos += BIT_POS_B_INPUT_ORIGIN; // B-Input
    else                  bitpos += BIT_POS_A_INPUT_ORIGIN; // A-Input
    return bitpos + channel/3;
  } else if(slbType==TgcRawData::SLB_TYPE_TRIPLET_STRIP || 
	    slbType==TgcRawData::SLB_TYPE_DOUBLET_STRIP) {
    if(channel<0 || channel>=ST_MAP_SIZE) return -1; // Invalid channel, SD_MAP_SIZE is equal to ST_MAP_SIZE. 

    if(channel%2==0) bitpos += BIT_POS_B_INPUT_ORIGIN; // B-Input
    else             bitpos += BIT_POS_A_INPUT_ORIGIN; // A-Input
    return bitpos + channel/2;
  } else if(slbType==TgcRawData::SLB_TYPE_DOUBLET_WIRE) {
    if(channel<0 || channel>=WD_MAP_SIZE) return -1; // Invalid channel

    if(channel%2==0) bitpos += BIT_POS_B_INPUT_ORIGIN; // B-Input
    else             bitpos += BIT_POS_A_INPUT_ORIGIN; // A-Input
    return bitpos + channel/2;
  } else {
    return -1;
  }
}

int Muon::TgcRdoToPrepDataToolCore::getchannel(int bitpos, TgcRawData::SlbType slbType) const
{
  int input = -1;
  if(     (bitpos<=BIT_POS_A_INPUT_ORIGIN) && (bitpos>BIT_POS_A_INPUT_ORIGIN-BIT_POS_INPUT_SIZE)) input = 2; // A-Input
  else if((bitpos<=BIT_POS_B_INPUT_ORIGIN) && (bitpos>BIT_POS_B_INPUT_ORIGIN-BIT_POS_INPUT_SIZE)) input = 1; // B-Input
  else if((bitpos<=BIT_POS_C_INPUT_ORIGIN) && (bitpos>BIT_POS_C_INPUT_ORIGIN-BIT_POS_INPUT_SIZE)) input = 0; // C-Input
  // D-Input is not implemented yet. 
  else return -1; // Invalid bitpos
  if(input==0 && slbType!=TgcRawData::SLB_TYPE_TRIPLET_WIRE) return -1; // Only Wire Triplet has C-input.
  
  int channel = 1-BIT_POS_INPUT_SIZE;
  if(slbType==TgcRawData::SLB_TYPE_TRIPLET_WIRE) {
    if(     input==2) channel += BIT_POS_A_INPUT_ORIGIN; // A-input
    else if(input==1) channel += BIT_POS_B_INPUT_ORIGIN; // B-input
    else              channel += BIT_POS_C_INPUT_ORIGIN; // C-input
    channel = 3*(bitpos - channel) + input;
    return channel; // C(0)->B(1)->A(2)->C(3)->B(4)->A(5)-> ... ->C(96-3)->B(96-2)->A(96-1)
  } else if(slbType==TgcRawData::SLB_TYPE_TRIPLET_STRIP ||
	    slbType==TgcRawData::SLB_TYPE_DOUBLET_WIRE  ||
	    slbType==TgcRawData::SLB_TYPE_DOUBLET_STRIP) {
    if(input==2) channel += BIT_POS_A_INPUT_ORIGIN; // A-input
    else         channel += BIT_POS_B_INPUT_ORIGIN; // B-input
    channel = 2*(bitpos - channel) + input-1;
    return channel; // B(0)->A(1)->B(2)->A(3)-> ... ->B(64-2)->A(64-1)
  } else {
    return -1;
  }
}

bool Muon::TgcRdoToPrepDataToolCore::getRPhiEtafromXYZ(const double x, const double y, const double z, 
						   double& r, double& phi, double& eta) const
{
  if((fabs(x)<DBL_MIN) && (fabs(y)<DBL_MIN)) return false; 

  r = sqrt(x*x + y*y);
  phi = atan2(y,x);
 
  double z_tmp = fabs(z);
  eta = fabs(atan2(r,z_tmp)); // theta
  eta = tan(eta/2.); // tan(theta/2)
  eta = -log(eta); // pseudo-rapidity=-log(tan(theta/2))
  return true;

}

bool Muon::TgcRdoToPrepDataToolCore::getRfromEtaZ(const double eta, const double z, double& r) const
{
  r = exp(-eta); // tan(theta/2)
  r = atan(r); // theta/2
  r = tan(2.*r); // tan(theta)
  r *= fabs(z); // r=|z|*tan(theta)

  if(r < 0.) return false;
  else return true;
}

bool Muon::TgcRdoToPrepDataToolCore::getEtafromRZ(const double r, const double z, double& eta) const
{
  double r_tmp = fabs(r);
  double z_tmp = fabs(z);

  if((r_tmp<DBL_MIN) && (z_tmp<DBL_MIN)) return false; 

  eta = fabs(atan2(r_tmp,z_tmp)); // theta
  eta = tan(eta/2.); // tan(theta/2)
  eta = -log(eta); // rapidity=-log(tan(theta/2))
  return true;
}

bool Muon::TgcRdoToPrepDataToolCore::isAlreadyConverted(const std::vector<const TgcRdo*>& decodedRdoCollVec,
                                                        const std::vector<const TgcRdo*>& rdoCollVec, 
                                                        const TgcRdo* rdoColl) const
{
  return
    (std::find (rdoCollVec.begin(), rdoCollVec.end(), rdoColl)
     != rdoCollVec.end()) ||
    (std::find (decodedRdoCollVec.begin(), decodedRdoCollVec.end(), rdoColl)
     != decodedRdoCollVec.end());
}

bool Muon::TgcRdoToPrepDataToolCore::isRequested(const std::vector<IdentifierHash>& requestedIdHashVect, 
                                                 IdentifierHash tgcHashId) const
{
  return
    (std::find (requestedIdHashVect.begin(), requestedIdHashVect.end(), tgcHashId)
     != requestedIdHashVect.end());
}

bool Muon::TgcRdoToPrepDataToolCore::isOfflineIdOKForTgcReadoutElement(const MuonGM::TgcReadoutElement* descriptor, 
								   const Identifier channelId) const
{
  if(!descriptor || !descriptor->containsId(channelId)) {
    ATH_MSG_DEBUG("Illegal OfflineID for TgcReadoutElement" << m_idHelperSvc->tgcIdHelper().show_to_string(channelId));
    return false;
  }
  return true;
}

void Muon::TgcRdoToPrepDataToolCore::showIdentifierHashVector(const State& state,
                                                              std::vector<IdentifierHash>& idHashVect) const
{
  if(!msgLvl(MSG::DEBUG)) return;

  unsigned int nc = 0;
  for (IdentifierHash offlineCollHash : idHashVect) {
    nc++;
    if(isIdentifierHashFoundInAnyTgcPrepDataContainer(state, offlineCollHash) ||
       isIdentifierHashFoundInAnyTgcCoinDataContainer(state, offlineCollHash)) {
      ATH_MSG_DEBUG("A collection exists in the container for offline id hash n. "
		    << nc << " = " << static_cast<int>(offlineCollHash));
    }
  }
}

void Muon::TgcRdoToPrepDataToolCore::fillIdentifierHashVector(const State& state,
                                                              std::vector<IdentifierHash>& selectedIdHashVect) const
{  
  // Fill the hashes with hits or coincidences   
  unsigned int HashId_max = m_idHelperSvc->tgcIdHelper().module_hash_max();  
  for(unsigned int HashId=0; HashId<HashId_max; HashId++) {  
    IdentifierHash offlineCollHash(static_cast<IdentifierHash>(HashId));  
    if(isIdentifierHashFoundInAnyTgcPrepDataContainer(state, offlineCollHash) ||  
       isIdentifierHashFoundInAnyTgcCoinDataContainer(state, offlineCollHash)) {  
      if(find(selectedIdHashVect.begin(), selectedIdHashVect.end(), offlineCollHash)==selectedIdHashVect.end()) {  
        // Avoid duplication  
        selectedIdHashVect.push_back(offlineCollHash);  
      }  
    }  
  }  
}  

void Muon::TgcRdoToPrepDataToolCore::showIdentifierHash(const State& state) const
{
  if(!(this->msgLvl(MSG::DEBUG))) return;

  unsigned int nc = 0;
  unsigned int HashId_max = m_idHelperSvc->tgcIdHelper().module_hash_max();
  for(unsigned int HashId=0; HashId<HashId_max; HashId++) {
    nc++;
    IdentifierHash offlineCollHash(static_cast<IdentifierHash>(HashId));
    if(isIdentifierHashFoundInAnyTgcPrepDataContainer(state, offlineCollHash) ||
       isIdentifierHashFoundInAnyTgcCoinDataContainer(state, offlineCollHash)) {
      ATH_MSG_DEBUG("A collection exists in the container for offline id hash n. "
		    << nc << " = " << static_cast<int>(offlineCollHash));
    }
  }
}

bool Muon::TgcRdoToPrepDataToolCore::isIdentifierHashFoundInAnyTgcPrepDataContainer
 (const State& state, const IdentifierHash Hash) const
{
  for(int ibc=0; ibc<NBC+1; ibc++) {
    if(state.m_tgcPrepDataContainer[ibc]->indexFindPtr(Hash) != nullptr) {
      return true;
    }
  }
  return false;
}

bool Muon::TgcRdoToPrepDataToolCore::isIdentifierHashFoundInAnyTgcCoinDataContainer
  (const State& state, const IdentifierHash Hash) const
{
  for(int ibc=0; ibc<NBC; ibc++) {
    if(state.m_tgcCoinDataContainer[ibc]->indexFindPtr(Hash) != nullptr) {
      return true;
    }
  }
  return false;
}

bool Muon::TgcRdoToPrepDataToolCore::getTrackletInfo(const TgcRawData& rd,
                                                 int& tmp_slbId, int& tmp_subMatrix, int& tmp_position) const
{
  tmp_subMatrix = rd.subMatrix();
  if(tmp_subMatrix!=0 && tmp_subMatrix!=1) {
    ATH_MSG_DEBUG("getTrackletInfo: subMatrix " << tmp_subMatrix << " is invalid.");
    return false;
  }
  
  int tmp_sswId = rd.sswId();
  tmp_slbId = rd.slbId();
  tmp_position = rd.position();
  
  int tmp_position_delta = tmp_position + rd.delta();
  int tmp_slbType = rd.slbType();

  if(tmp_position_delta>=BIT_POS_INPUT_SIZE) {
    tmp_position = tmp_position_delta-BIT_POS_INPUT_SIZE;
    if(tmp_subMatrix==1) {
      if(tmp_slbType==TgcRawData::SLB_TYPE_DOUBLET_STRIP) {
	tmp_position = BIT_POS_INPUT_SIZE-1;
	ATH_MSG_DEBUG("Expected TGC2 Strip position (" << tmp_position_delta <<  
		      ") does not exist and is changed to the edge position " << tmp_position);
      } else if((tmp_slbType==TgcRawData::SLB_TYPE_DOUBLET_WIRE) && (tmp_sswId==7) && ((tmp_slbId==3) || (tmp_slbId==11))) { 
	// upper edge SLB of FWD:sbLoc3,11
	ATH_MSG_DEBUG("sbLoc " << tmp_slbId+1 << " doesn't exist for FWD!! (upper edge SLB of FWD:sbLoc3,11)");
        return false;
      } else if((tmp_slbType==TgcRawData::SLB_TYPE_DOUBLET_WIRE) && (tmp_sswId!=7) && (tmp_slbId==9)) { 
	// upper edge SLB of EWD:sbLoc9
	ATH_MSG_DEBUG("sbLoc " << tmp_slbId+1 << " doesn't exist for EWD!! (upper edge SLB of EWD:sbLoc9)");
        return false;
      } else { 
	// Valid sbLoc,delta,blockpos
        tmp_subMatrix = 0;
        tmp_slbId++;
      }
    } else {
      tmp_subMatrix = 1;
    }
  } else if(tmp_position_delta<0) {
    tmp_position = tmp_position_delta+BIT_POS_INPUT_SIZE;
    if(tmp_subMatrix==0) { 
      if(tmp_slbType==TgcRawData::SLB_TYPE_DOUBLET_STRIP) {
	tmp_position = 0;
	ATH_MSG_DEBUG("Expected TGC2 Strip position (" << tmp_position_delta <<  
		      ") does not exist and is changed to the edge position " << tmp_position);
      } else if((tmp_slbType==TgcRawData::SLB_TYPE_DOUBLET_WIRE) && (tmp_sswId==7) && ((tmp_slbId==0) || (tmp_slbId==8))) { 
	// bottom edge SLB of FWD:sbLoc0,8
	if(tmp_position_delta==-1) { // This case is valid. 
	  tmp_position = 0; 
	} else { 
	  ATH_MSG_DEBUG("sbLoc " << tmp_slbId-1 << " doesn't exist for FWD!! (bottom edge SLB of FWD:sbLoc0,8)");
	  return false;
	}
      } else if((tmp_slbType==TgcRawData::SLB_TYPE_DOUBLET_WIRE) &&(tmp_sswId!=7) && (tmp_slbId==0)) { 
	// bottom edge SLB of EWD:sbLoc0
	if(tmp_position_delta==-1) { // This case is valid. 
	  tmp_position = 0; 
	} else { 
	  ATH_MSG_DEBUG("sbLoc " << tmp_slbId-1 << " doesn't exist for EWD!! (bottom edge SLB of EWD:sbLoc0)");
	  return false;
	}
      } else { 
	// Valid sbLoc,delta,
        tmp_subMatrix = 1;
        tmp_slbId--;
      }
    } else {
      tmp_subMatrix = 0;
    }
  } else {
    tmp_position = tmp_position_delta;
  }

  return true;
}

int Muon::TgcRdoToPrepDataToolCore::getRoiRow(const TgcRawData& rd) const
{
  int RoiRow = static_cast<int>(rd.roi()/4);
  return RoiRow;
}

bool Muon::TgcRdoToPrepDataToolCore::isIncludedInChamberBoundary(const TgcRawData& rd) const
{
  int RoiRow = getRoiRow(rd);

  if(!rd.isForward() && 
     (RoiRow== 3 || RoiRow== 4 || // SSC 2 : ROI 12-19
      RoiRow== 7 || RoiRow== 8 || // SSC 4 : ROI 28-35
      RoiRow==11 || RoiRow==12 || // SSC 6 : ROI 44-51
      RoiRow==23 || RoiRow==24    // SSC12 : ROI 92-99
      )) {
    return true;
  }
  return false;
}

void Muon::TgcRdoToPrepDataToolCore::getBitPosOutWire(const TgcRawData& rd, int& slbsubMatrix, int* bitpos_o) const
{
  // This method is used by decodeHiPt 
  if((rd.hitId()%2)==1) { // 1,3,5
    slbsubMatrix = 0;
    if((rd.hsub())==0) {
      bitpos_o[0] = BIT_POS_B_INPUT_LARGE_R_CH15; //  78
      bitpos_o[1] = BIT_POS_A_INPUT_LARGE_R_CH08; //  49
    } else if((rd.hsub())==1) {
      bitpos_o[0] = BIT_POS_B_INPUT_LARGE_R_CH07; //  86
      bitpos_o[1] = BIT_POS_A_INPUT_LARGE_R_CH00; //  57
    }
  } else { // 2,4,6
    slbsubMatrix = 1;
    if((rd.hsub())==0) {
      bitpos_o[0] = BIT_POS_B_INPUT_SMALL_R_CH15; //  94
      bitpos_o[1] = BIT_POS_A_INPUT_SMALL_R_CH08; //  65
    } else if((rd.hsub())==1) {
      bitpos_o[0] = BIT_POS_B_INPUT_SMALL_R_CH07; // 102
      bitpos_o[1] = BIT_POS_A_INPUT_SMALL_R_CH00; //  73
    }
  }

  // In case of WIRE, there are no bit assign.(EWD0 EWD4 FWD0 FWD1)-----------------------------
  // EWD0 sbLoc = 0     bitpos = 102 : does not exist and has 6 channels only (the largest  R, RoiRow== 0).
  // EWD4 sbLoc = 9     bitpos =  73 : does not exist and has 4 channels only (the smallest R, RoiRow==36). 
  // FWD0 sbLoc = 0, 8  bitpos =  78 : does not exist and has 5 channels only (the largest  R, RoiRow== 0). 
  // FWD1 sbLoc = 3, 11 bitpos =  73 : does not exist and has 5 channels only (the smallest R, RoiTow==15).
  // fixed it by following description.
  if(!rd.isForward()) { // Endcap
    if((rd.chip()==0) && (rd.hitId()==1) && (rd.hsub()==1)) { 
      // chip 0 should have these values and means EWD0's position.
      slbsubMatrix = 1;
      bitpos_o[0] = BIT_POS_B_INPUT_SMALL_R_CH05; // 104
      bitpos_o[1] = BIT_POS_A_INPUT_SMALL_R_CH00; //  73
    } else if((rd.chip()==3) && (rd.hitId()==6) && (rd.hsub()==1)) { // EWD4's position
      bitpos_o[0] = BIT_POS_B_INPUT_SMALL_R_CH07; // 102
      bitpos_o[1] = BIT_POS_A_INPUT_SMALL_R_CH04; //  69 
    }
  } else { // Forward
    if((rd.chip()==0) && (rd.hitId()==1) && (rd.hsub()==0)) { // FWD0
      bitpos_o[0] = BIT_POS_B_INPUT_LARGE_R_CH12; //  81
      bitpos_o[1] = BIT_POS_A_INPUT_LARGE_R_CH08; //  49
    } else if((rd.chip()==1) && (rd.hitId()==2) && (rd.hsub()==1)) { //FWD1
      bitpos_o[0] = BIT_POS_B_INPUT_SMALL_R_CH07; // 102
      bitpos_o[1] = BIT_POS_A_INPUT_SMALL_R_CH03; //  70
    }
  }// end of Forward
  // end of fixed----------------------------------------------------------------------------------
}

void Muon::TgcRdoToPrepDataToolCore::getBitPosInWire(const TgcRawData& rd, const int DeltaBeforeConvert,
                                                 int* bitpos_i, int* slbchannel_i, int* slbId_in, int* sbLoc_in, 
						 int& sswId_i,
                                                 const int* bitpos_o, int* slbchannel_o, const int slbId_o) const
{
  // This method is used by decodeHiPt 
  const int NUM_SLBID_SBLOC_OFFSET_WT = 8; // (see https://twiki.cern.ch/twiki/pub/Main/TgcDocument/sbloc-070701.xls)

  /*** get bitpos for TGC1 Station ***/

  int rdochIn_max = 0;
  int rdochIn_min = 0;
  int offset_dt = 0;
  if(!rd.isForward()) { // EWT
    rdochIn_max = 665;
    rdochIn_min = 78;
    offset_dt = 32;
  } else { // FWT
    rdochIn_max = 312;
    rdochIn_min = 0;
    offset_dt = 0;
  }

  int tmp_rdochannel_i = 0;
  int tmp_rdochannel_i2 = 0;
  for(int i=0; i<2; i++) {
    slbchannel_o[i] = getchannel(bitpos_o[i], TgcRawData::SLB_TYPE_DOUBLET_WIRE);
    tmp_rdochannel_i = WD_MAP_SIZE*slbId_o + slbchannel_o[i] + DeltaBeforeConvert + offset_dt; 
    if(tmp_rdochannel_i>rdochIn_max) {
      tmp_rdochannel_i = rdochIn_max;
    } else if(tmp_rdochannel_i<rdochIn_min) {
      tmp_rdochannel_i = rdochIn_min;
    }

    //                               Large R <-----------------> Small R
    // tmp_rdochannel_i           :  0  1  2  3  4  5  6  7  8  9 10 ...
    //------------------------------------------------------------------- 
    // tmp_rdochannel_i/3         :  0  0  0  1  1  1  2  2  2  3  3 ...
    // (tmp_rdochannel_i/3)*3     :  0  0  0  3  3  3  6  6  6  9  9 ...
    // tmp_rdochannel_i2 (i==0)   :  2  2  2  5  5  5  8  8  8 11 11 ...
    //-------------------------------------------------------------------
    // tmp_rdochannel_i+1         :  1  2  3  4  5  6  7  8  9 10 11 ...
    // (tmp_rdochannel_i+1)/3     :  0  0  1  1  1  2  2  2  3  3  3 ...
    // ((tmp_rdochannel_i+1)/3)*3 :  0  0  3  3  3  6  6  6  9  9  9 ...
    // tmp_rdochannel_i2 (i==1)   : -1 -1  2  2  2  5  5  5  8  8  8 ... 
    
    if(i==0) { // get the lower R channel on L3 nearest from bitpos_o[0]
      tmp_rdochannel_i2 = (tmp_rdochannel_i/3)*3 + 2; 
    } else { // get the higher R channel on L3 nearest from bitpos_o[1] 
      tmp_rdochannel_i2 = ((tmp_rdochannel_i + 1)/3)*3 - 1; 
    }
    
    if(tmp_rdochannel_i2>rdochIn_max) {
      tmp_rdochannel_i2 = rdochIn_max;
    } else if(tmp_rdochannel_i2<rdochIn_min) {
      tmp_rdochannel_i2 = rdochIn_min + 2; // 2 is added for L3 
    }

    slbId_in[i]   = tmp_rdochannel_i /WT_MAP_SIZE;
    slbId_in[i+2] = tmp_rdochannel_i2/WT_MAP_SIZE;

    sbLoc_in[i]   = slbId_in[i];
    sbLoc_in[i+2] = slbId_in[i+2];
    if(rd.sector()%2==1) { 
      // phi1 and phi3 have offset (see https://twiki.cern.ch/twiki/pub/Main/TgcDocument/sbloc-070701.xls) 
      // Endcap sector=1, 3 are phi1 and phi3, and Forward sector=1 is phi2.
      sbLoc_in[i]   += NUM_SLBID_SBLOC_OFFSET_WT;
      sbLoc_in[i+2] += NUM_SLBID_SBLOC_OFFSET_WT;
    }

    slbchannel_i[i]   = tmp_rdochannel_i %WT_MAP_SIZE;
    slbchannel_i[i+2] = tmp_rdochannel_i2%WT_MAP_SIZE;

    bitpos_i[i]   = getbitpos(slbchannel_i[i],   TgcRawData::SLB_TYPE_TRIPLET_WIRE);
    bitpos_i[i+2] = getbitpos(slbchannel_i[i+2], TgcRawData::SLB_TYPE_TRIPLET_WIRE);
  }

  if(!rd.isForward()) { // EWT
    sswId_i = static_cast<int>(rd.sector()/2);
  } else { // FWT
    sswId_i = 2;
  }
}

void Muon::TgcRdoToPrepDataToolCore::getBitPosOutStrip(const TgcRawData& rd, int& slbsubMatrix, int* bitpos_o) const
{
  // This method is used by decodeHiPt
  if((rd.hitId()%2)==1) { // 1,3,5::hitId:1-6 for EC, 2-3 for Fw
    slbsubMatrix = 0;
    if((rd.hsub())==0) {
      bitpos_o[0] = BIT_POS_B_INPUT_LARGE_PHI_FOR_A_FWD_C_BWD_CH15; //  78 
      bitpos_o[1] = BIT_POS_A_INPUT_LARGE_PHI_FOR_A_FWD_C_BWD_CH08; //  49
    } else if((rd.hsub())==1) {
      bitpos_o[0] = BIT_POS_B_INPUT_LARGE_PHI_FOR_A_FWD_C_BWD_CH07; //  86
      bitpos_o[1] = BIT_POS_A_INPUT_LARGE_PHI_FOR_A_FWD_C_BWD_CH00; //  57
    }
  } else { // 2,4,6
    slbsubMatrix = 1;
    if((rd.hsub())==0) {
      bitpos_o[0] = BIT_POS_B_INPUT_SMALL_PHI_FOR_A_FWD_C_BWD_CH15; //  94
      bitpos_o[1] = BIT_POS_A_INPUT_SMALL_PHI_FOR_A_FWD_C_BWD_CH08; //  65
    } else if((rd.hsub())==1) {
      bitpos_o[0] = BIT_POS_B_INPUT_SMALL_PHI_FOR_A_FWD_C_BWD_CH07; // 102
      bitpos_o[1] = BIT_POS_A_INPUT_SMALL_PHI_FOR_A_FWD_C_BWD_CH00; //  73
    }
  }
}

void Muon::TgcRdoToPrepDataToolCore::getBitPosInStrip(const TgcRawData& rd, const int DeltaBeforeConvert,
                                                  int* bitpos_i, int* slbchannel_i, int& sbLoc_i, int& sswId_i,
                                                  const int* bitpos_o, int* slbchannel_o) const
{
  // This method is used by decodeHiPt 
  //*** get bitpos for TGC1 Station ***//
  // ST
  int rdochIn_max = ST_MAP_SIZE-1;
  int rdochIn_min = 0;

  if((rd.sector()%2)==0) { 
    if(rd.chip()==0) {
      sbLoc_i = 16; // EST0 (phi0 and phi2) or FST0 (phi0)
    } else if(rd.chip()==1) {
      sbLoc_i = 17; // EST1 (phi0 and phi2)
    }
  } else {
    if(rd.chip()==0) {
      sbLoc_i = 24; // EST0 (phi1 and phi3) or FST0 (phi2)
    } else if(rd.chip()==1) {
      sbLoc_i = 25; // EST1 (phi1 and phi3)
    }
  }

  for(int i=0; i<2; i++) {
    slbchannel_o[i] = getchannel(bitpos_o[i], TgcRawData::SLB_TYPE_DOUBLET_STRIP);
    slbchannel_i[i] = slbchannel_o[i] + DeltaBeforeConvert;
    if(slbchannel_i[i]>rdochIn_max) {
      slbchannel_i[i] = rdochIn_max;
    } else if(slbchannel_i[i]<rdochIn_min) {
      slbchannel_i[i] = rdochIn_min;
    }

    if(i==0) {
      slbchannel_i[i+2] = slbchannel_i[i] + 1;
    } else {
      slbchannel_i[i+2] = slbchannel_i[i] - 1;
    }

    if(slbchannel_i[i+2] > rdochIn_max) {
      slbchannel_i[i+2] = rdochIn_max;
    } else if(slbchannel_i[i+2] < rdochIn_min) {
      slbchannel_i[i+2] = rdochIn_min;
    }

    bitpos_i[i]   = getbitpos(slbchannel_i[i],   TgcRawData::SLB_TYPE_TRIPLET_STRIP);
    bitpos_i[i+2] = getbitpos(slbchannel_i[i+2], TgcRawData::SLB_TYPE_TRIPLET_STRIP);
  }

  if(!rd.isForward()) { // EST
    sswId_i = static_cast<int>(rd.sector()/2);
  } else { // FST
    sswId_i = 2;
  }
}

void Muon::TgcRdoToPrepDataToolCore::getBitPosWire(const TgcRawData& rd, const int hitId_w, const int sub_w,
                                                   int& subMatrix_w, int* bitpos_w) const
{
  // This method is used by getSLIds 
  // This method assumes sub_w is 0 or 1.
  // Protection for other possibility is needed.

  // 0 : Index for the largest R channel 
  // 1 : Index for the smallest R channel 
  // 2 : Index for the "center" channel

  int RoiRow = getRoiRow(rd);
  bool isForward = rd.isForward();

  if(RoiRow==0 && !isForward) { // EWD0,SLB0 exception (It has 6 channels only, the largest R)
    subMatrix_w = 1;
    bitpos_w[0] = BIT_POS_B_INPUT_SMALL_R_CH05; // 104 
    bitpos_w[1] = BIT_POS_A_INPUT_SMALL_R_CH04; //  69
    bitpos_w[2] = BIT_POS_A_INPUT_SMALL_R_CH00; //  73
  } else if(RoiRow==36 && !isForward) { // EWD4,SLB1 exception (It has 4 channels only, the smallest R)
    subMatrix_w = 1;
    bitpos_w[0] = BIT_POS_B_INPUT_SMALL_R_CH07; // 102
    bitpos_w[1] = BIT_POS_A_INPUT_SMALL_R_CH04; //  69
    bitpos_w[2] = BIT_POS_A_INPUT_SMALL_R_CH04; //  69
  } else if(RoiRow==0 && isForward) { // FWD0,SLB0 exception (It has 5 channels only, the largest R)
    subMatrix_w = 0;
    bitpos_w[0] = BIT_POS_B_INPUT_LARGE_R_CH12; //  81
    bitpos_w[1] = BIT_POS_A_INPUT_LARGE_R_CH12; //  45
    bitpos_w[2] = BIT_POS_A_INPUT_LARGE_R_CH08; //  49
  } else if(RoiRow==15 && isForward) { // FWD1,SLB1 exception (It has 5 channels only, the smallest R)
    subMatrix_w = 1;
    bitpos_w[0] = BIT_POS_B_INPUT_SMALL_R_CH07; // 102
    bitpos_w[1] = BIT_POS_A_INPUT_SMALL_R_CH04; //  69
    bitpos_w[2] = BIT_POS_A_INPUT_SMALL_R_CH03; //  70
  } else {
    if((hitId_w%2)==0) { // 0, 2, 4
      subMatrix_w = 0;
      if(sub_w==0) {
        bitpos_w[0] = BIT_POS_B_INPUT_LARGE_R_CH15; //  78 
	bitpos_w[1] = BIT_POS_A_INPUT_LARGE_R_CH12; //  45 
	bitpos_w[2] = BIT_POS_A_INPUT_LARGE_R_CH08; //  49
      } else if(sub_w==1) {
        bitpos_w[0] = BIT_POS_B_INPUT_LARGE_R_CH07; //  86
	bitpos_w[1] = BIT_POS_A_INPUT_LARGE_R_CH04; //  53
	bitpos_w[2] = BIT_POS_A_INPUT_LARGE_R_CH00; //  57
      }
    } else { // 1, 3, 5
      subMatrix_w = 1;
      if(sub_w==0) {
        bitpos_w[0] = BIT_POS_B_INPUT_SMALL_R_CH15; //  94 
	bitpos_w[1] = BIT_POS_A_INPUT_SMALL_R_CH12; //  61 
	bitpos_w[2] = BIT_POS_A_INPUT_SMALL_R_CH08; //  65
      } else if(sub_w==1) {
        bitpos_w[0] = BIT_POS_B_INPUT_SMALL_R_CH07; // 102 
	bitpos_w[1] = BIT_POS_A_INPUT_SMALL_R_CH04; //  69
	bitpos_w[2] = BIT_POS_A_INPUT_SMALL_R_CH00; //  73
      }
    }
  }
}

void Muon::TgcRdoToPrepDataToolCore::getBitPosStrip(const int hitId_s, const int sub_s, int& subMatrix_s, int* bitpos_s) const
{
  // This method is used by getSLIds 
  // 0 : Index for the largest phi (for A-side forward and C-side backward) channel 
  // 1 : Index for the smallest phi (for A-side forward and C-side backward) channel 
  // 2 : Index for the "center" channel

  if((hitId_s%2)==0) { // 0, 2, 4
    subMatrix_s = 0;
    if(sub_s==0) {
      bitpos_s[0] = BIT_POS_B_INPUT_LARGE_PHI_FOR_A_FWD_C_BWD_CH15; //  78 
      bitpos_s[1] = BIT_POS_A_INPUT_LARGE_PHI_FOR_A_FWD_C_BWD_CH12; //  45
      bitpos_s[2] = BIT_POS_A_INPUT_LARGE_PHI_FOR_A_FWD_C_BWD_CH08; //  49
    } else if(sub_s==1) {
      bitpos_s[0] = BIT_POS_B_INPUT_LARGE_PHI_FOR_A_FWD_C_BWD_CH07; //  86
      bitpos_s[1] = BIT_POS_A_INPUT_LARGE_PHI_FOR_A_FWD_C_BWD_CH04; //  53
      bitpos_s[2] = BIT_POS_A_INPUT_LARGE_PHI_FOR_A_FWD_C_BWD_CH00; //  57
    }
  } else if((hitId_s%2)==1) { // 1, 3, 5
    subMatrix_s = 1;
    if(sub_s==0) {
      bitpos_s[0] = BIT_POS_B_INPUT_SMALL_PHI_FOR_A_FWD_C_BWD_CH15; //  94
      bitpos_s[1] = BIT_POS_A_INPUT_SMALL_PHI_FOR_A_FWD_C_BWD_CH12; //  61
      bitpos_s[2] = BIT_POS_A_INPUT_SMALL_PHI_FOR_A_FWD_C_BWD_CH08; //  65
    } else if(sub_s==1) {
      bitpos_s[0] = BIT_POS_B_INPUT_SMALL_PHI_FOR_A_FWD_C_BWD_CH07; // 102 
      bitpos_s[1] = BIT_POS_A_INPUT_SMALL_PHI_FOR_A_FWD_C_BWD_CH04; //  69 
      bitpos_s[2] = BIT_POS_A_INPUT_SMALL_PHI_FOR_A_FWD_C_BWD_CH00; //  73
    }
  }
}

int Muon::TgcRdoToPrepDataToolCore::getDeltaBeforeConvert(const TgcRawData& rd) const
{
  int DeltaBeforeConvert = 0;

  if(rd.isStrip()) {// strip
    switch(rd.delta()) {
    case  5: DeltaBeforeConvert =   6; break;
    case  6: DeltaBeforeConvert =   8; break;
    case  7: DeltaBeforeConvert =  10; break;
    case -4: DeltaBeforeConvert =  -5; break;
    case -5: DeltaBeforeConvert =  -7; break;
    case -6: DeltaBeforeConvert =  -9; break;
    case -7: DeltaBeforeConvert = -12; break;
    default: DeltaBeforeConvert = rd.delta(); break;
    }
  } else {// wire
    switch (rd.delta()) {
    case  11: DeltaBeforeConvert =  12; break;
    case  12: DeltaBeforeConvert =  14; break;
    case  13: DeltaBeforeConvert =  16; break;
    case  14: DeltaBeforeConvert =  18; break;
    case  15: DeltaBeforeConvert =  20; break;
    case -12: DeltaBeforeConvert = -13; break;
    case -13: DeltaBeforeConvert = -15; break;
    case -14: DeltaBeforeConvert = -17; break;
    case -15: DeltaBeforeConvert = -19; break;
    default:  DeltaBeforeConvert = rd.delta(); break;
    }
  }

  return DeltaBeforeConvert;
}

bool Muon::TgcRdoToPrepDataToolCore::isBackwardBW(const TgcRawData& rd) const
{
  bool isBackward = false;

  if(!rd.isForward()) { // Endcap
    if(((rd.subDetectorId()==ASIDE) && (rd.sector()%2==1)) ||
       ((rd.subDetectorId()==CSIDE) && (rd.sector()%2==0))) {
      // Aside,phi_odd::Backward
      // Cside,phi_even::Backward
      isBackward = true;
    } else {
      // Aside,phi_even::Forward
      // Cside,phi_odd::Forward
      isBackward = false;
    }
  } else { // Forward
    // Aide::Backward
    // Cside::Forward
    isBackward = (rd.subDetectorId()==ASIDE);
  }

  return isBackward;
}

double Muon::TgcRdoToPrepDataToolCore::getWidthWire(const MuonGM::TgcReadoutElement* descriptor, 
						const int gasGap, const int channel) const
{
  double gangMaxZ = descriptor->gangMaxZ(gasGap, channel);
  double gangMinZ = descriptor->gangMinZ(gasGap, channel);
  double width = gangMaxZ - gangMinZ;
  if(width<s_cutDropPrdsWithZeroWidth) {
    ATH_MSG_DEBUG("Width for wire is invalid :" 
		  << " gasGap = " << gasGap 
		  << " channel = " << channel 
		  << " gangMaxZ = " << gangMaxZ 
		  << " gangMinZ = " << gangMinZ 
		  << " width = " << width);
  }
  return width;
}

double Muon::TgcRdoToPrepDataToolCore::getWidthStrip(const MuonGM::TgcReadoutElement* descriptor, 
						 const int gasGap, const int channel,
                                                 const Identifier channelId) const
{
  Amg::Vector3D localPos = descriptor->transform(channelId).inverse()*descriptor->channelPos(channelId);
  double stripMaxX = descriptor->stripMaxX(gasGap, channel, localPos.z());
  double stripMinX = descriptor->stripMinX(gasGap, channel, localPos.z());
  double width = stripMaxX - stripMinX;
  if(width<s_cutDropPrdsWithZeroWidth) {
    ATH_MSG_DEBUG("Width for strip is invalid :"
		  << " gasGap = " << gasGap
		  << " channel = " << channel
		  << " stripMaxX = " << stripMaxX
		  << " stripMinX = " << stripMinX 
		  << " width = " << width);
  }
  return width;
}

bool Muon::TgcRdoToPrepDataToolCore::getSLWireGeometry(const Identifier* channelId_wire, 
						   double& width_wire, double& r_wire, double& z_wire) const
{
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();
  const MuonGM::TgcReadoutElement* descriptor_w[3] = {muDetMgr->getTgcReadoutElement(channelId_wire[0]), 
						      muDetMgr->getTgcReadoutElement(channelId_wire[1]), 
						      muDetMgr->getTgcReadoutElement(channelId_wire[2])};
  for(int i=0; i<3; i++) {
    if(!isOfflineIdOKForTgcReadoutElement(descriptor_w[i], channelId_wire[i])) {
      return false;
    }
  }

  Amg::Vector3D position_w = Amg::Vector3D(descriptor_w[2]->channelPos(channelId_wire[2]));
  Amg::Vector2D loc_hitPos_w;
  bool onSurface_w = descriptor_w[2]->surface(channelId_wire[2]).globalToLocal(position_w,position_w,loc_hitPos_w);
  if(!onSurface_w) { 
    ATH_MSG_WARNING("Muon::TgcRdoToPrepDataToolCore::getSLWireGeometry Amg::Vector2D* loc_hitPos_w is null."); 
    return false; 
  } 
  Amg::Vector2D tmp_hitPos_w(0., 0.);

  int gasGap_w[3] = {0, 0, 0};
  int channel_w[3] = {0, 0, 0};
  for(int i=0; i<3; i++) {
    gasGap_w[i] = m_idHelperSvc->tgcIdHelper().gasGap(channelId_wire[i]);
    channel_w[i] = m_idHelperSvc->tgcIdHelper().channel(channelId_wire[i]);
  }
  double tmp_r_w[3] = {0., 0., 0.};
  double tmp_phi_w[3] = {0., 0., 0.};
  double tmp_eta_w[3] = {0., 0., 0.};
  
  Amg::Vector3D tmp_position_w[3];
  for(int i=0; i<3; i+=2) { // i=0 and 2
    tmp_position_w[i] = Amg::Vector3D(descriptor_w[i]->channelPos(channelId_wire[i]));
    bool flag_geteta_w = getRPhiEtafromXYZ(tmp_position_w[i].x(), tmp_position_w[i].y(), tmp_position_w[i].z(), 
					   tmp_r_w[i], tmp_phi_w[i], tmp_eta_w[i]);
    if(!flag_geteta_w) {
      ATH_MSG_DEBUG("TgcRdoToPrepDataToolCore::getSLWireGeometry::failed to getRPhiEta!!");
      return false;
    }

    double half_width = getWidthWire(descriptor_w[i], gasGap_w[i], channel_w[i])/2.;
    if(half_width<s_cutDropPrdsWithZeroWidth/2. && m_dropPrdsWithZeroWidth) { // Invalid PRD's whose widths are zero are dropped.
      return false;
    }
    // add half widths of edge channels
    if(i==0) {
      tmp_r_w[0] += half_width;
    } else if(i==2) {
      tmp_r_w[2] -= half_width;
    }
  }
  
  bool flag_geteta_w = getEtafromRZ(tmp_r_w[0], tmp_position_w[0].z(), tmp_eta_w[0]);
  bool flag_getr_w = getRfromEtaZ(tmp_eta_w[0], tmp_position_w[2].z(), tmp_r_w[0]);
  if(!flag_geteta_w || !flag_getr_w) {
    ATH_MSG_DEBUG("TgcRdoToPrepDataToolCore::getSLWireGeometry::failed to getR on L7!!");
    return false;
  }  
  width_wire = tmp_r_w[0] - tmp_r_w[2];
  double gang = descriptor_w[2]->gangMinZ(gasGap_w[2],channel_w[2]) + width_wire/2.;
  tmp_hitPos_w[Trk::locX] = gang;
  tmp_hitPos_w[Trk::locY] = ((loc_hitPos_w)[Trk::loc2]);

  Amg::Vector3D tmp_wire_gp;
  descriptor_w[2]->surface(channelId_wire[2]).localToGlobal(tmp_hitPos_w,tmp_wire_gp,tmp_wire_gp);
  z_wire = tmp_wire_gp.z();
  r_wire = tmp_r_w[2] + width_wire/2.;

  return true;
}

bool Muon::TgcRdoToPrepDataToolCore::getSLStripGeometry(const Identifier* channelId_strip, 
						    const bool isBackward, const bool isAside, 
						    double& width_strip, double& theta_strip) const
{
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();
  const MuonGM::TgcReadoutElement* descriptor_s[3] = {muDetMgr->getTgcReadoutElement(channelId_strip[0]), 
						      muDetMgr->getTgcReadoutElement(channelId_strip[1]), 
						      muDetMgr->getTgcReadoutElement(channelId_strip[2])};
  for(int i=0; i<3; i++) {
    if(!isOfflineIdOKForTgcReadoutElement(descriptor_s[i], channelId_strip[i])) {
      return false;
    }
  }
  
  Amg::Vector3D position_s = Amg::Vector3D(descriptor_s[1]->channelPos(channelId_strip[1]));
  Amg::Vector2D loc_hitPos_s;
  bool onSurface_s = descriptor_s[1]->surface(channelId_strip[1]).globalToLocal(position_s,position_s,loc_hitPos_s);
  if(!onSurface_s) { 
    ATH_MSG_WARNING("Muon::TgcRdoToPrepDataToolCore::getSLStripGeometry Amg::Vector2D* loc_hitPos_s is null."); 
    return false; 
  } 
  Amg::Vector2D tmp_hitPos_s(0., 0.);
  
  int gasGap_s[3] = {0, 0, 0};
  int channel_s[3] = {0, 0, 0};
  for(int i=0; i<3; i++) {
    gasGap_s[i] = m_idHelperSvc->tgcIdHelper().gasGap(channelId_strip[i]);
    channel_s[i] = m_idHelperSvc->tgcIdHelper().channel(channelId_strip[i]);
  }

  Amg::Vector3D localPos[3];
  for(int i=0; i<3; i+=2) { // i=0 and 2
    localPos[i] = descriptor_s[i]->transform(channelId_strip[i]).inverse()
      *descriptor_s[i]->channelPos(channelId_strip[i]);
  }
  
  bool flag_reverse = false;
  int index_strip[2] = {0, 0};

  if(!isBackward) { // Forward chamber
    if(isAside) { // Aside/Forward Chamber
      index_strip[0] = 2; index_strip[1] = 0; flag_reverse = true;
    } else { // Cside/Forward Chamber
      index_strip[0] = 0; index_strip[1] = 2; flag_reverse = false;
    }
  } else { // Backward chamber   
    if(isAside) { // Aside/Backward Chamber
      index_strip[0] = 0; index_strip[1] = 2; flag_reverse = true;
    } else { // Cside/Backward Chamber
      index_strip[0] = 2; index_strip[1] = 0; flag_reverse = false;
    }
  }
  
  double stripMaxX = descriptor_s[index_strip[0]]->stripMaxX(gasGap_s[index_strip[0]], channel_s[index_strip[0]], 
							     localPos[index_strip[0]].z());
  double stripMinX = descriptor_s[index_strip[1]]->stripMinX(gasGap_s[index_strip[1]], channel_s[index_strip[1]], 
							     localPos[index_strip[1]].z());
  width_strip = stripMaxX - stripMinX;
  double strip = stripMinX +  width_strip/2.;
  if(flag_reverse) strip *= -1.;
  
  tmp_hitPos_s[Trk::locX] = strip;
  tmp_hitPos_s[Trk::locY] = loc_hitPos_s[Trk::loc2];
  
  int index_strip_gp = 0;
  if(!isBackward) index_strip_gp = 0;
  else index_strip_gp = 2;
  Amg::Vector3D tmp_strip_gp; 
  descriptor_s[index_strip_gp]->surface(channelId_strip[index_strip_gp]).localToGlobal(tmp_hitPos_s,tmp_strip_gp,tmp_strip_gp);
  theta_strip = atan2(tmp_strip_gp.y(), tmp_strip_gp.x()); 

  
  return true;
}

bool Muon::TgcRdoToPrepDataToolCore::getPosAndIdWireOut(const MuonGM::TgcReadoutElement** descriptor_o, 
						    const Identifier* channelIdOut,
                                                    const int* gasGap_o, const int* channel_o,
                                                    double& width_o, double& hit_position_o, Amg::Vector2D& tmp_hitPos_o,
                                                    Identifier& channelIdOut_tmp) const
{
  // This method is used by decodeHiPt
  Amg::Vector3D position_o[2];
  double tmp_phi_o[2] = {0., 0.};
  double tmp_eta_o[2] = {0., 0.};
  double tmp_r_o[2] = {0., 0.};
  
  for(int i=0; i<2; i++) {
    position_o[i] = Amg::Vector3D(descriptor_o[i]->channelPos(channelIdOut[i]));
    bool flag_geteta_o = getRPhiEtafromXYZ(position_o[i].x(), position_o[i].y(), position_o[i].z(), 
					   tmp_r_o[i], tmp_phi_o[i], tmp_eta_o[i]);
    if(!flag_geteta_o) {
      ATH_MSG_DEBUG("TgcRdoToPrepDataToolCore::getPosAndIdWireOut::failed to getRPhiEta!!");
      return false;
    }
    // add half widths of edge channels
    double half_width = getWidthWire(descriptor_o[i], gasGap_o[i], channel_o[i])/2.;
    if(half_width<s_cutDropPrdsWithZeroWidth/2. && m_dropPrdsWithZeroWidth) { // Invalid PRD's whose widths are zero are dropped.
      return false;
    }
    if(i==0) tmp_r_o[0] += half_width;
    else     tmp_r_o[1] -= half_width;
  }
  
  bool flag_geteta_o = getEtafromRZ(tmp_r_o[0], position_o[0].z(), tmp_eta_o[0]);
  bool flag_getr_o = getRfromEtaZ(tmp_eta_o[0], position_o[1].z(), tmp_r_o[0]);
  if(!flag_geteta_o || !flag_getr_o) {
    ATH_MSG_DEBUG("TgcRdoToPrepDataToolCore::getPosAndIdWireOut::failed to getR on L7!!");
    return false;
  }
  
  width_o = tmp_r_o[0] - tmp_r_o[1];
  // X-coordinate
  hit_position_o = descriptor_o[1]->gangMinZ(gasGap_o[1], channel_o[1]) + width_o/2;
  tmp_hitPos_o[Trk::locX] = (hit_position_o); 
  // Y-coordinate
  Amg::Vector3D position_out = Amg::Vector3D(descriptor_o[1]->channelPos(channelIdOut[1]));
  // dummy global pos
  Amg::Vector2D loc_hitPos_o;
  bool onSurface_o = descriptor_o[1]->surface(channelIdOut[1]).globalToLocal(position_out,position_out,loc_hitPos_o); 
  if(!onSurface_o) { 
    ATH_MSG_WARNING("Muon::TgcRdoToPrepDataToolCore::getPosAndIdWireOut Amg::Vector2D* loc_hitPos_o is null."); 
    return false; 
  } 
  tmp_hitPos_o[Trk::locY] = loc_hitPos_o[Trk::loc2];
  
  channelIdOut_tmp = channelIdOut[1];
  
  return true;
}

bool Muon::TgcRdoToPrepDataToolCore::getPosAndIdStripOut(const MuonGM::TgcReadoutElement** descriptor_o, 
						     const Identifier* channelIdOut,
                                                     const int* gasGap_o, const int* channel_o,
                                                     double& width_o, double& hit_position_o, Amg::Vector2D& tmp_hitPos_o,
                                                     Identifier& channelIdOut_tmp,
                                                     const bool isBackward, const bool isAside) const
{
  // This method is used by decodeHiPt
  // Currently, this method always returns true.
  Amg::Vector3D localpos_o[2];
  for(int i=0; i<2; i++) {
    localpos_o[i] = descriptor_o[i]->transform(channelIdOut[i]).inverse()*descriptor_o[i]->channelPos(channelIdOut[i]);
  }
  
  int index[3] = {0, 0, 0};
  bool flag_reverse = false;
  if(!isBackward) { // Forward chamber
    index[2] = 0;
    if(isAside) { // Aside/Forward Chamber
      index[0] = 1; index[1] = 0; flag_reverse = true;
    } else { // Cside/Forward Chamber
      index[0] = 0; index[1] = 1; flag_reverse = false;
    }
  } else { // Backward chamber   
    index[2] = 1;
    if(isAside) { // Aside/Backward Chamber
      index[0] = 0; index[1] = 1; flag_reverse = true;
    } else { // Cside/Backward Chamber
      index[0] = 1; index[1] = 0; flag_reverse = false;
    }
  }
  
  double stripMax = descriptor_o[index[0]]->stripMaxX(gasGap_o[index[0]], channel_o[index[0]], 
						      localpos_o[index[0]].z());
  double stripMin = descriptor_o[index[1]]->stripMinX(gasGap_o[index[1]], channel_o[index[1]], 
						      localpos_o[index[1]].z());
  width_o = stripMax - stripMin;
  // X-coordinate
  hit_position_o = stripMin + width_o/2.;
  if(flag_reverse) hit_position_o *= -1.;
  tmp_hitPos_o[Trk::locX] = hit_position_o;
  // Y-coordinate
  Amg::Vector3D position_out = Amg::Vector3D(descriptor_o[1]->channelPos(channelIdOut[1]));
  // dummy global pos
  Amg::Vector2D loc_hitPos_o;
  bool onSurface_o = descriptor_o[1]->surface(channelIdOut[1]).globalToLocal(position_out,position_out,loc_hitPos_o); 
  if(!onSurface_o) { 
    ATH_MSG_WARNING("Muon::TgcRdoToPrepDataToolCore::getPosAndIdStripOut Amg::Vector2D* loc_hitPos_o is null."); 
    return false; 
  } 
  tmp_hitPos_o[Trk::locY] = loc_hitPos_o[Trk::loc2];
  
  channelIdOut_tmp = channelIdOut[index[2]];
  
  return true;
}

bool Muon::TgcRdoToPrepDataToolCore::getPosAndIdWireIn(const MuonGM::TgcReadoutElement** descriptor_i, 
						   const Identifier* channelIdIn,
                                                   const int* gasGap_i, const int* channel_i,
                                                   double& width_i, double& hit_position_i, Amg::Vector2D& tmp_hitPos_i,
                                                   Identifier& channelIdIn_tmp) const
{
  // This method is used by decodeHiPt
  int flag_boundary_i = 0;
  if(descriptor_i[1]->getReadoutType()==descriptor_i[3]->getReadoutType()) { 
    // in case lower edge is not chamber boundary
    if(gasGap_i[1]==gasGap_i[3]) { // in case that edge channel is on L3; use [1]
      flag_boundary_i = 1;
    } else { // in case that edge channel is not on L3; use [3]
      flag_boundary_i = 3;
    }
  } else if(descriptor_i[0]->getReadoutType()==descriptor_i[2]->getReadoutType()) { 
    // in case higher edge is not chamber boundary
    if(gasGap_i[0]==gasGap_i[2]) { // in case that edge channel is on L3; use [0]
      flag_boundary_i = 0;
    } else { // in case that edge channel is not on L3; use [2]
      flag_boundary_i = 2;
    }
  } else {
    ATH_MSG_DEBUG("TgcRdoToPrepDataToolCore::getPosAndIdWireIn::ROI has 3 readout elements!!");
    return false;
  }
  
  channelIdIn_tmp = channelIdIn[flag_boundary_i];
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();
  const MuonGM::TgcReadoutElement* descriptor_iw = muDetMgr->getTgcReadoutElement(channelIdIn_tmp);
  if(!isOfflineIdOKForTgcReadoutElement(descriptor_iw, channelIdIn_tmp)) {
    return false;
  }
  
  double tmp_r_i[3] = {0., 0., 0.};
  double tmp_phi_i[3] = {0., 0., 0.};
  double tmp_eta_i[3] = {0., 0., 0.};
  
  Amg::Vector3D position_i[3] = {Amg::Vector3D(descriptor_i[0]->channelPos(channelIdIn[0])), 
                                 Amg::Vector3D(descriptor_i[1]->channelPos(channelIdIn[1])), 
                                 Amg::Vector3D(descriptor_iw->channelPos(channelIdIn_tmp))};

  for(int i=0; i<3; i++) {
    bool flag_getrphi_i = getRPhiEtafromXYZ(position_i[i].x(), position_i[i].y(), position_i[i].z(), 
					    tmp_r_i[i], tmp_phi_i[i], tmp_eta_i[i]);
    
    if(!flag_getrphi_i) {
      ATH_MSG_DEBUG("TgcRdoToPrepDataToolCore::getPosAndIdWireIn::failed to getRPhiEtaIn!!");
      return false;
    }
    
    if(i<2) {
      // add half widths of edge channels
      double half_width = getWidthWire(descriptor_i[i], gasGap_i[i], channel_i[i])/2.;
      if(half_width<s_cutDropPrdsWithZeroWidth/2. && m_dropPrdsWithZeroWidth) { // Invalid PRD's whose widths are zero are dropped.
	return false;
      }
      if(i==0) tmp_r_i[0] += half_width;
      else     tmp_r_i[1] -= half_width;
      
      bool flag_geteta_i = getEtafromRZ(tmp_r_i[i], position_i[i].z(), tmp_eta_i[i]);
      bool flag_getr_i = getRfromEtaZ(tmp_eta_i[i], position_i[2].z(), tmp_r_i[i]); 
      
      if(!flag_geteta_i || !flag_getr_i) {
	ATH_MSG_DEBUG("TgcRdoToPrepDataToolCore::getPosAndIdWireIn::failed to getRIn" << i << " on L3!!");
	return false;
      }
    }
  }
  
  width_i = tmp_r_i[0] - tmp_r_i[1];
  if(width_i<0.) {
    ATH_MSG_DEBUG("TgcRdoToPrepDataToolCore::getPosAndIdWireIn::minus value width_i = " << width_i);
    return false;
  }
  
  int gasGap_tmp = m_idHelperSvc->tgcIdHelper().gasGap(channelIdIn_tmp);
  int channel_tmp = m_idHelperSvc->tgcIdHelper().channel(channelIdIn_tmp);
  double half_width = getWidthWire(descriptor_iw, gasGap_tmp, channel_tmp)/2.;
  if(half_width<s_cutDropPrdsWithZeroWidth/2. && m_dropPrdsWithZeroWidth) { // Invalid PRD's whose widths are zero are dropped.
    return false;
  }
  if((flag_boundary_i%2)==1) { // in case lower edge is not chamber boundary
    tmp_r_i[2] -= half_width;
    hit_position_i = descriptor_iw->gangMinZ(gasGap_tmp, channel_tmp)
      - (tmp_r_i[2] - tmp_r_i[1]) + width_i/2.;
  } else { // in case higher edge is not chamber boundary
    tmp_r_i[2] += half_width;
    hit_position_i = descriptor_iw->gangMaxZ(gasGap_tmp, channel_tmp)
      + (tmp_r_i[0] - tmp_r_i[2]) - width_i/2.;
  }
  
  // X-coordinate
  tmp_hitPos_i[Trk::locX] = hit_position_i;
  Amg::Vector3D position_in = Amg::Vector3D(descriptor_i[1]->channelPos(channelIdIn[1]));
  // dummy global pos
  Amg::Vector2D loc_hitPos_i;
  bool onSurface_i = descriptor_i[1]->surface(channelIdIn[1]).globalToLocal(position_in,position_in,loc_hitPos_i); 
  if(!onSurface_i) { 
    ATH_MSG_WARNING("Muon::TgcRdoToPrepDataToolCore::getPosAndIdWireIn Amg::Vector2D* loc_hitPos_i is null."); 
    return false; 
  } 
  // Y-coordinate
  tmp_hitPos_i[Trk::locY] = loc_hitPos_i[Trk::loc2];
  
  return true;
}

bool Muon::TgcRdoToPrepDataToolCore::getPosAndIdStripIn(const MuonGM::TgcReadoutElement** descriptor_i, 
						    const Identifier* channelIdIn,
                                                    const int* gasGap_i, const int* channel_i,
                                                    double& width_i, double& hit_position_i, Amg::Vector2D& tmp_hitPos_i,
                                                    Identifier& channelIdIn_tmp,
                                                    const bool isBackward, const bool isAside) const
{
  // This method is used by decodeHiPt
  int flag_isL3 = -1;
  // which bitpos is Layer3?
  for(int i=0; i<4; i++) {
    if(gasGap_i[i]==3) {
      flag_isL3 = i;
      break;
    }
  }
  if(flag_isL3<0 || flag_isL3>=4) {
    ATH_MSG_DEBUG("getPosAndIdStripIn: Any bitpos is not at Layer3!");
    return false;
  }
  
  channelIdIn_tmp = channelIdIn[flag_isL3];
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> muDetMgrHandle{m_muDetMgrKey};
  const MuonGM::MuonDetectorManager* muDetMgr = muDetMgrHandle.cptr();
  const MuonGM::TgcReadoutElement* descriptor_is = muDetMgr->getTgcReadoutElement(channelIdIn_tmp);
  if(!isOfflineIdOKForTgcReadoutElement(descriptor_is, channelIdIn_tmp)) {
    return true;
  }
  
  double tmp_r_i[3] = {0., 0., 0.};
  double tmp_phi_i[3] = {0., 0., 0.};
  double tmp_eta_i[3] = {0., 0., 0.};
  Amg::Vector3D position_is[3];
  for(int i=0; i<3; i++) {
    if(i<2) {
      position_is[i] = Amg::Vector3D(descriptor_i[i]->channelPos(channelIdIn[i]));
    } else {
      position_is[i] = Amg::Vector3D(descriptor_is->channelPos(channelIdIn_tmp));
    }
    bool flag_getrphi_is = getRPhiEtafromXYZ(position_is[i].x(), position_is[i].y(), position_is[i].z(), 
					     tmp_r_i[i], tmp_phi_i[i], tmp_eta_i[i]);
    if(!flag_getrphi_is) {
      ATH_MSG_DEBUG("TgcRdoToPrepDataToolCore::getPosAndIdStripIn::failed to getRPhiEtaIn!!");
      return false;
    }
  }
  
  int index[2] = {0, 0};
  bool flag_reverse = false;
  if(!isBackward) { // Forward chamber
    if(isAside) { // Aside/Forward Chamber
      index[0] = 1; index[1] = 0; flag_reverse = true;
    } else { // Cside/Forward Chamber
      index[0] = 0; index[1] = 1; flag_reverse = false;
    }
  } else { // Backward chamber
    if(isAside) { // Aside/Backward Chamber
      index[0] = 0; index[1] = 1; flag_reverse = true;
    } else { // Cside/Backward Chamber
      index[0] = 1; index[1] = 0; flag_reverse = false;
    }
  }
  
  Amg::Vector3D localpos_i[2];
  for(int i=0; i<2; i++) {
    localpos_i[i] = descriptor_i[i]->transform(channelIdIn[i]).inverse()*descriptor_i[i]->channelPos(channelIdIn[i]);
  }
  double stripMaxX = descriptor_i[index[0]]->stripMaxX(gasGap_i[index[0]], channel_i[index[0]], 
						       localpos_i[index[0]].z());
  double stripMinX = descriptor_i[index[1]]->stripMinX(gasGap_i[index[1]], channel_i[index[1]], 
						       localpos_i[index[1]].z());
  width_i = stripMaxX - stripMinX;
  // X-coordinate
  hit_position_i = stripMinX +  width_i/2.;
  if(flag_reverse) hit_position_i *= -1.;
  tmp_hitPos_i[Trk::locX] = hit_position_i;
  // Y-coordinate
  Amg::Vector3D position_in = Amg::Vector3D(descriptor_i[1]->channelPos(channelIdIn[1]));
  // dummy global pos
  Amg::Vector2D loc_hitPos_i;
  bool onSurface_i = descriptor_i[1]->surface(channelIdIn[1]).globalToLocal(position_in,position_in,loc_hitPos_i); 
  if(!onSurface_i) { 
    ATH_MSG_WARNING("Muon::TgcRdoToPrepDataToolCore::getPosAndIdStripIn Amg::Vector2D* loc_hitPos_i is null."); 
    return false; 
  } 
  tmp_hitPos_i[Trk::locY] = loc_hitPos_i[Trk::loc2];
  
  return true;
}

// RDOHighPtID --> (Sim)HighPtID --> OfflineID --> ReadoutID --> getSLBID
bool Muon::TgcRdoToPrepDataToolCore::getHiPtIds(const TgcRawData& rd, int& sswId_o, int& sbLoc_o, int& slbId_o) const
{
  const CablingInfo* cinfo = getCabling();
  if (!cinfo) {
    return false;
  }
  const ITGCcablingSvc* tgcCabling = cinfo->m_tgcCabling;

  int index = static_cast<int>(rd.index());
  int chip = static_cast<int>(rd.chip());
  int hitId = static_cast<int>(rd.hitId());
  
  // getSimHighPtIDfromRDOHighPtID changes index, chip and hitId.
  bool found = tgcCabling->getSimHighPtIDfromRDOHighPtID(rd.isForward(), rd.isStrip(), index, chip, hitId);
  if(!found) {
    ATH_MSG_DEBUG("Failed to get SimHighPtID from RDOHighPtID for Pivot "
		  << (rd.isStrip() ? "Strip" : "Wire"));
    return false;
  }
  
  Identifier dummyId;
  found = tgcCabling->getOfflineIDfromHighPtID(dummyId,
                                               rd.subDetectorId(), rd.rodId(), rd.sector(),
                                               rd.isStrip(), rd.isForward(), index,
                                               chip, hitId, rd.hsub());
  if(!found) {
    ATH_MSG_DEBUG("Failed to get offlineID from HighPtID for Pivot "
		  << (rd.isStrip() ? "Strip" : "Wire"));
    return false;
  }
  
  int dummy_i[3] = {0, 0, 0};
  found = tgcCabling->getReadoutIDfromOfflineID(dummyId, dummy_i[0], dummy_i[1], sswId_o, sbLoc_o, dummy_i[2]);
  if(!found) {
    ATH_MSG_DEBUG("Failed to get ReadoutID from OfflineID for Pivot "
		  << (rd.isStrip() ? "Strip" : "Wire"));
    return false;
  }
  
  int i_o[2] = {0, 0};
  bool b_o[2] = {false, false};
  found = tgcCabling->getSLBIDfromReadoutID(i_o[0], b_o[0], b_o[1], i_o[1], slbId_o, 
                                            rd.subDetectorId(), rd.rodId(), sswId_o, sbLoc_o);
  if(!found) {
    ATH_MSG_DEBUG("Failed to get SLBID from ReadoutID for Pivot "
		  << (rd.isStrip() ? "Strip" : "Wire"));
    return false;
  }

  return true;
}

bool Muon::TgcRdoToPrepDataToolCore::getSLIds(const bool isStrip,
                                              const TgcRawData& rd,
                                              Identifier* channelId, 
					  int& index, int& chip, int& hitId, int& sub, int& sswId, int& sbLoc, 
					  int& subMatrix, int* bitpos,
					  const bool isBoundary, const TgcRdo* rdoColl, 
					  const int index_w, const int chip_w, const int hitId_w, const int sub_w) const

{
  const CablingInfo* cinfo = getCabling();
  if (!cinfo) {
    return false;
  }
  const ITGCcablingSvc* tgcCabling = cinfo->m_tgcCabling;

  bool found = tgcCabling->getHighPtIDfromROINumber(rd.roi(),
                                                    rd.isForward(),
                                                    isStrip, // get HitID of HPT Board
                                                    index,
                                                    chip,
                                                    hitId,
                                                    sub);
  if(!found) {
    ATH_MSG_DEBUG("Failed to get HighPtID from ROINumber for "
		  << (!isStrip ? "Wire" : "Strip"));
    return false;
  }
  
  found = tgcCabling->getSimHighPtIDfromRDOHighPtID(rd.isForward(),
                                                    isStrip,
                                                    index,
                                                    chip,
                                                    hitId);
  if(!found) {
    ATH_MSG_DEBUG("Failed to get SimHighPtID from RDOHighPtID for "
		  << (!isStrip ? "Wire" : "Strip"));
    return false;
  }
  
  Identifier offlineId;
  found = tgcCabling->getOfflineIDfromHighPtID(offlineId, 
                                               rd.subDetectorId(),
                                               rd.rodId(),
                                               rd.sector(),
                                               isStrip,
                                               rd.isForward(),
                                               index,
                                               chip,
                                               hitId,
                                               sub);
  if(!found) {
    ATH_MSG_DEBUG("Failed to get OfflineID from HighPtID for "
		  << (!isStrip ? "Wire" : "Strip"));
    return false;
  }
    
  if(!isStrip || !isBoundary) { // Wire or strip whose ROI not including chamber boundary
    channelId[1] = offlineId;
    int dummy_i[3] = {0, 0, 0};
    found = tgcCabling->getReadoutIDfromOfflineID(channelId[1], 
                                                  dummy_i[0],
                                                  dummy_i[1],
                                                  sswId,
                                                  sbLoc,
                                                  dummy_i[2]);
    if(!found) {
      ATH_MSG_DEBUG("Failed to get ReadoutID from OfflineID for "
		    << (!isStrip ? "Wire" : "Strip"));
      return false;
    }
  } else { // --> solving the chamber boundary of strip 
    sswId = rd.sector()+3; // SSW3: M3-EC phi0, SSW4: M3-EC phi1, SSW5: M3-EC phi2, SSW6: M3-EC phi3 

    // Loop over all HiPt Strip to find an associated one to obtain sbLoc
    bool exist_hipt_s = getSbLocOfEndcapStripBoundaryFromHiPt(rd, sbLoc, rdoColl, index_w, chip_w, hitId_w, sub_w);
    if(!exist_hipt_s) { 
      // Loop over all Tracklet Strip to find an associated one to obtain sbLoc 
      bool exist_tracklet_s = getSbLocOfEndcapStripBoundaryFromTracklet(rd, sbLoc, rdoColl, index_w, chip_w, hitId_w, sub_w);
      if(!exist_tracklet_s) { 
	ATH_MSG_DEBUG("Failed to find correspond Tracklet_strip for SL!!");
	return false;
      }
    }
  }

  if(!isStrip) { // wire
    getBitPosWire(rd, hitId, sub, subMatrix, bitpos);
  } else { // strip
    getBitPosStrip(hitId, sub, subMatrix, bitpos);
  } 
  
  for(int i=0; i<3; i++) { 
    if(i==1 && (!isStrip || !isBoundary)) continue; 
    
    found = tgcCabling->getOfflineIDfromReadoutID(channelId[i],
                                                  rd.subDetectorId(),
                                                  rd.rodId(),
                                                  sswId,
                                                  sbLoc,
                                                  bitpos[i]);
    if(!found) {
      ATH_MSG_DEBUG("Failed to get OfflineID from ReadoutID for " 
		    << (!isStrip ? "Wire" : "Strip"));
      if(!isStrip || i==1) {
	ATH_MSG_DEBUG("subDetectorId = " << rd.subDetectorId()
		      << ", rodId = " << rd.rodId()
		      << ", sswId = " << sswId
		      << ", slbId = " << sbLoc
		      << ", bitpos_" << (!isStrip ? "w" : "s") 
		      << "[" << i << "] = " << bitpos[i]);
      }
      return false;
    }
  }

  return true;
}

bool Muon::TgcRdoToPrepDataToolCore::getSbLocOfEndcapStripBoundaryFromHiPt(const TgcRawData& rd, int& sbLoc,
								       const TgcRdo* rdoColl, 
								       const int index_w, const int chip_w, const int hitId_w, const int sub_w) const
{ 
  const CablingInfo* cinfo = getCabling();
  if (!cinfo) {
    return false;
  }
  const ITGCcablingSvc* tgcCabling = cinfo->m_tgcCabling;

  bool exist_hipt_s = false;
  
  // Get trackletIds of three candidates 
  int trackletIdStripFirst  = -1;
  int trackletIdStripSecond = -1;
  int trackletIdStripThird  = -1;
  getEndcapStripCandidateTrackletIds(static_cast<int>(rd.roi()), trackletIdStripFirst, 
				     trackletIdStripSecond, trackletIdStripThird); 
  
  // Loop over all HiPt Strip to find an associated one
  for (const TgcRawData* rdH : *rdoColl) {
    if((rdH->type()==TgcRawData::TYPE_HIPT) && // HiPt
       (rdH->isHipt()) && // HiPt flag is required
       (rdH->isStrip()) && // Strip
       (!rdH->isForward()) && // Endcap
       (rdH->bcTag()==rd.bcTag()) && // The same timing 
       (rdH->subDetectorId()==rd.subDetectorId()) && // The same side
       (rdH->rodId()==rd.rodId()) && // The same ROD 
       (rdH->sector()==rd.sector()) // The same sector (1/48 phi)
       ) {

      // Get sbLoc
      int sswId_o = 0;
      int sbLoc_o = 0;
      int slbId_o = 0;
      bool found = getHiPtIds(*rdH, sswId_o, sbLoc_o, slbId_o);
      if(!found) continue;

      // Get subMatrix
      int slbsubMatrix = 0;
      int bitpos_o[2] = {0, 0};
      getBitPosOutStrip(*rdH, slbsubMatrix, bitpos_o);

      // Get trackletId 
      int trackletIdStrip = 2*sbLoc_o + slbsubMatrix;

      // Compare trackletIds 
      if(trackletIdStrip!=trackletIdStripFirst && 
	 trackletIdStrip!=trackletIdStripSecond && 
	 trackletIdStrip!=trackletIdStripThird) continue; 

      // The third candidate is used only if any corresponding HiPt Strip is found so far.  
      if(exist_hipt_s && trackletIdStrip==trackletIdStripThird) continue;

      // getRDOHighPtIDfromSimHighPtID overwrites index, chip and hitId. 
      int index_w_tmp = index_w;
      int chip_w_tmp = chip_w;
      int hitId_w_tmp = hitId_w;
      // Get RDO HighPt ID from SimHighPtID for wire 
      found = tgcCabling->getRDOHighPtIDfromSimHighPtID(false, // false for endcap
                                                        false, // wire
                                                        index_w_tmp,
                                                        chip_w_tmp,
                                                        hitId_w_tmp);
      if(!found) {
	ATH_MSG_DEBUG("Failed to get RDOHighPtID from SimHighPtID for Wire");
	continue;
      }

      // RDO High Pt ID of Strip 
      int chip_s = static_cast<int>(rdH->chip());
      int hitId_s = static_cast<int>(rdH->hitId());
      int hsub_s = static_cast<int>(rdH->hsub());

      int roi = 0;
      found = tgcCabling->getROINumberfromHighPtID(roi,
                                                   false, // false for Endcap
                                                   index_w_tmp, // hpb_wire (not used)
                                                   chip_w_tmp, // chip_wire
                                                   hitId_w_tmp, // hitId_wire
                                                   sub_w, // sub_wire
                                                   chip_s, // chip_strip (not used)
                                                   hitId_s, // hitId_strip 
                                                   hsub_s); // sub_strip
      if(!found) {
        ATH_MSG_DEBUG("Failed to get ROINumber from HighPtID for Strip");
        continue;
      }
      
      if(roi==rd.roi()) { 
	sbLoc = sbLoc_o;
        exist_hipt_s = true;
        if(trackletIdStrip==trackletIdStripFirst) break; // If the first candidate is found, exit from this for loop
      }
    }
  }

  return exist_hipt_s;
}

bool Muon::TgcRdoToPrepDataToolCore::getSbLocOfEndcapStripBoundaryFromTracklet(const TgcRawData& rd, int& sbLoc,
									   const TgcRdo* rdoColl,
									   const int index_w, const int chip_w, 
									   const int hitId_w, const int sub_w) const
{
  const CablingInfo* cinfo = getCabling();
  if (!cinfo) {
    return false;
  }
  const ITGCcablingSvc* tgcCabling = cinfo->m_tgcCabling;

  bool exist_tracklet_s = false;
  
  // Get trackletIds of three candidates 
  int trackletIdStripFirst  = -1;
  int trackletIdStripSecond = -1;
  int trackletIdStripThird  = -1;
  getEndcapStripCandidateTrackletIds(static_cast<int>(rd.roi()), trackletIdStripFirst, 
				     trackletIdStripSecond, trackletIdStripThird); 
  
  // Loop over all Tracklet Strip to find an associated one
  for (const TgcRawData* rdS : *rdoColl) {
    bool isForward_s = (rdS->sswId()==7); // Doublet, Forward
    if(isForward_s) continue; // Chamber boundaries exist in endcap only
    
    if((rdS->type()==TgcRawData::TYPE_TRACKLET) &&
       (rdS->slbType()==TgcRawData::SLB_TYPE_DOUBLET_STRIP) &&
       (rdS->bcTag()==rd.bcTag()) &&
       (rdS->subDetectorId()==rd.subDetectorId()) &&
       (rdS->rodId()==rd.rodId()) && 
       (rdS->sswId()-3==rd.sector()) // SSW3: M3-EC phi0, SSW4: M3-EC phi1, SSW5: M3-EC phi2, SSW6: M3-EC phi3 
       ) {
      
      // Compare trackletIds 
      int trackletIdStrip = static_cast<int>(2*rdS->slbId()+rdS->subMatrix()); // trackletId of Tracklet Strip
      if(trackletIdStrip!=trackletIdStripFirst && 
	 trackletIdStrip!=trackletIdStripSecond && 
	 trackletIdStrip!=trackletIdStripThird) continue; 

      // The third candidate is used only if any corresponding Tracklet Strip is found so far.
      if(exist_tracklet_s && trackletIdStrip==trackletIdStripThird) continue;
      
      Identifier offlineId;
      bool found = tgcCabling->getOfflineIDfromLowPtCoincidenceID(offlineId, rdS->subDetectorId(), rdS->rodId(),
                                                                  rdS->sswId(), rdS->slbId(), rdS->subMatrix(), 
                                                                  rdS->position(), false);
      if(!found) {
	ATH_MSG_DEBUG("Failed to get OfflineID from LowPtCoincidenceID for Strip");
	continue;
      }
      
      int i[7] = {0, 0, 0, 0, 0, 0, 0};
      bool b[2] = {false, false};
      found = tgcCabling->getHighPtIDfromOfflineID(offlineId,i[0],i[1],i[2],b[0],b[1],i[3],i[4],i[5],i[6]);
      // i[0] subDetectorID, i[1] rodID, i[2] sectorInReadout, b[0] isStrip, 
      // b[1] isForward, i[3] hpb, i[4] chip, i[5] hitID, i[6] pos
      
      if(!found) {
	ATH_MSG_DEBUG("Failed to get HighPtID from OfflineID for Strip");
	continue;
      }

      // getRDOHighPtIDfromSimHighPtID overwrites index, chip and hitId. 
      int index_w_tmp = index_w;
      int chip_w_tmp = chip_w;
      int hitId_w_tmp = hitId_w;
      found = tgcCabling->getRDOHighPtIDfromSimHighPtID(rd.isForward(), // false for endcap
                                                        false, // wire
                                                        index_w_tmp, // hpb-index
                                                        chip_w_tmp, // chip-chip
                                                        hitId_w_tmp); // hitID-hitId
      if(!found) {
	ATH_MSG_DEBUG("Failed to get RDOHighPtID from SimHighPtID for Wire");
	continue;
      }
      
      found = tgcCabling->getRDOHighPtIDfromSimHighPtID(rd.isForward(), // false for endcap
                                                        true, // strip
                                                        i[3], // hpb-index
                                                        i[4], // chip-chip
                                                        i[5]); // hitID-hitId
      if(!found) {
	ATH_MSG_DEBUG("Failed to get RDOHighPtID from SimHighPtID for Strip");
	continue;
      }
      
      int roi = 0;
      found = tgcCabling->getROINumberfromHighPtID(roi, 
                                                   rd.isForward(), // false for endcap
                                                   index_w_tmp, // hpb_wire (not used)
                                                   chip_w_tmp, // chip_wire
                                                   hitId_w_tmp, // hitId_wire
                                                   sub_w, // sub_wire
                                                   i[4], // chip_strip (not used)
                                                   i[5], // hitId_strip
                                                   i[6]); // sub_strip
      if(!found) {
	ATH_MSG_DEBUG("Failed to get ROINumber from HighPtID for Strip");
	continue;
      }
      
      if(roi==rd.roi()) { 
	sbLoc = rdS->slbId(); 
	exist_tracklet_s = true;
	if(trackletIdStrip==trackletIdStripFirst) break; // If the first candidate is found, exit from this for loop 
      }
    }
  }

  return exist_tracklet_s;
}

void Muon::TgcRdoToPrepDataToolCore::getEndcapStripCandidateTrackletIds(const int roi, int &trackletIdStripFirst, 
								    int &trackletIdStripSecond, int &trackletIdStripThird) const {
  constexpr int T9SscMax =  2; // SSC 0 to SSC 2
  constexpr int T8SscMax =  4; // SSC 3 to SSC 4
  constexpr int T7SscMax =  6; // SSC 5 to SSC 6
  constexpr int T6SscMax = 12; // SSC 7 to SSC12
  constexpr int T5SscMax = 18; // SSC13 to SSC18

  constexpr int T9Offset = 32 + 0; 
  constexpr int T8Offset = 32 + 2; 
  constexpr int T7Offset = 32 + 4; 
  constexpr int T6Offset = 32 + 6; 
  constexpr int T5Offset = 32 + 8; 
  
  // ROI     :  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 ...
  // SSC     :  0  0  0  0  1  1  1  1  1  1  1  1  2  2  2  2  2  2 ...
  // Half SSC:  0  0  1  1  0  0  1  1  0  0  1  1  0  0  1  1  0  0 ... 
  int ssc = (roi+4)/8; 
  int halfSsc = (roi%4)/2; 
  
  if(       ssc< T9SscMax) { // SSC 0 to SSC 1 
    trackletIdStripFirst  = T9Offset + halfSsc; // T9
    trackletIdStripSecond = -1;
    trackletIdStripThird  = -1;
  } else if(ssc==T9SscMax) { // SSC 2 (boundary)
    trackletIdStripFirst  = T8Offset + halfSsc; // T8
    trackletIdStripSecond = T9Offset + halfSsc; // T9
    trackletIdStripThird  = -1;
  } else if(ssc< T8SscMax) { // SSC 3
    trackletIdStripFirst  = T8Offset + halfSsc; // T8
    trackletIdStripSecond = -1;
    trackletIdStripThird  = -1;
  } else if(ssc==T8SscMax) { // SSC 4 (boundary)
    trackletIdStripFirst  = T7Offset + halfSsc; // T7
    trackletIdStripSecond = T8Offset + halfSsc; // T8
    trackletIdStripThird  = -1;
  } else if(ssc< T7SscMax) { // SSC 5
    trackletIdStripFirst  = T7Offset + halfSsc; // T7
    trackletIdStripSecond = -1;
    trackletIdStripThird  = -1;
  } else if(ssc==T7SscMax) { // SSC 6 (boundary)
    trackletIdStripFirst  = T6Offset + halfSsc; // T6
    trackletIdStripSecond = T7Offset + halfSsc; // T7
    trackletIdStripThird  = T5Offset + halfSsc; // T5, HiPt Endcap Strip board bug
  } else if(ssc< T6SscMax) { // SSC 7 to SSC11 
    trackletIdStripFirst  = T6Offset + halfSsc; // T6
    trackletIdStripSecond = T5Offset + halfSsc; // T5, HiPt Endcap Strip board bug 
    trackletIdStripThird  = -1;
  } else if(ssc==T6SscMax) { // SSC12 (boundary)
    trackletIdStripFirst  = T6Offset + halfSsc; // T6
    trackletIdStripSecond = T5Offset + halfSsc; // T5
    trackletIdStripThird  = -1;
  } else if(ssc<=T5SscMax) { // SSC13 to SSC18 
    trackletIdStripFirst  = T5Offset + halfSsc; // T5
    trackletIdStripSecond = T6Offset + halfSsc; // T6, HiPt Endcap Strip board bug 
    trackletIdStripThird  = -1;
  } else {
    trackletIdStripFirst  = -1;
    trackletIdStripSecond = -1;
    trackletIdStripThird  = -1;
  }
}

const Muon::TgcRdoToPrepDataToolCore::CablingInfo*
Muon::TgcRdoToPrepDataToolCore::getCabling() const
{
  if (m_cablingInfo.isValid()) {
    return m_cablingInfo.ptr();
  }

  // get TGC cablingSvc
  ServiceHandle<ITGCcablingServerSvc> TgcCabGet ("TGCcablingServerSvc", name());
  if (TgcCabGet.retrieve().isFailure()) {
    ATH_MSG_ERROR ("Could not get TGCcablingServerSvc !");
    return nullptr;
  }

  CablingInfo cinfo;
  
  if (TgcCabGet->giveCabling(cinfo.m_tgcCabling).isFailure()) {
    ATH_MSG_ERROR( "Could not get ITGCcablingSvc from Server!" );
    return nullptr;
  }

  cinfo.m_is12fold = TgcCabGet->isAtlas();

  if(cinfo.m_is12fold) {  
    ATH_MSG_DEBUG(cinfo.m_tgcCabling->name() << " is OK");  
  } else {  
    ATH_MSG_DEBUG("TGCcablingSvc (octant segmentation) OK");  
  }

  if(m_fillCoinData && !cinfo.m_is12fold) { 
    ATH_MSG_INFO("Input RDO is produced with 8-fold TGC cabling, m_fillCoinData considered false " <<
		 "to disable the conversion from TGC coincidence RDO to TGC coincidence PRD."); 
  }

  // check the relation between hash and onlineId (onlineId depends on cabling)  
  unsigned int hashId_max = m_idHelperSvc->tgcIdHelper().module_hash_max();  
  cinfo.m_hashToOnlineId.reserve(hashId_max);  
  IdContext tgcContext = m_idHelperSvc->tgcIdHelper().module_context(); // TGC context   
  Identifier elementId;  
  int subDetectorId = 0; // 103(=0x67) for A side, 104(=0x68) for C side  
  int rodId = 0; // 1 to 12 (12-fold cabling), 0 to 7 (8-fold cabling)   
  TgcRdo tgcRdo; // For onlineId conversion  
  for(unsigned int hashId=0; hashId<hashId_max; hashId++) {  
    IdentifierHash hash(hashId);  
    m_idHelperSvc->tgcIdHelper().get_id(hash, elementId, &tgcContext);  
    cinfo.m_tgcCabling->getReadoutIDfromElementID(elementId, subDetectorId, rodId);  
    // onlineId: 0 to 11 on A side and 12 to 23 on C side (12-fold cabling)  
    //           0 to  7 on A side and  8 to 15 on C side (8-fold cabling)  
    uint16_t onlineId = tgcRdo.calculateOnlineId(subDetectorId, rodId);   
    cinfo.m_hashToOnlineId.push_back(onlineId);  
  } 

  // initialize with false  
  cinfo.m_MAX_N_ROD = (cinfo.m_is12fold ? 2*12 : 2*8);

  m_cablingInfo.set (std::move (cinfo));
  return m_cablingInfo.ptr();
}

const Amg::Vector2D* Muon::TgcRdoToPrepDataToolCore::getSLLocalPosition(const MuonGM::TgcReadoutElement* readout, const Identifier identify,  
                                                                    const double eta, const double phi) const { 
  if(!readout) return nullptr;  
  
  // Obtain the local coordinate by the secant method
  constexpr double length = 100.; // 100 mm
  constexpr unsigned int nRep = 10; // 10 repetitions
  constexpr double dRAccuracy = 1.0E-20; // recuqired dR accuracy
  constexpr double maxLocR = 10000.; // 10 m
  
  double locX = 0.;  
  double locY = 0.;  
  for(unsigned int iRep=0; iRep<nRep; iRep++) { 
    Amg::Vector2D loc_hitPos_c(locX, locY); // center or current position 
    Amg::Vector3D tmp_glob_hitPos_c;
    readout->surface(identify).localToGlobal(loc_hitPos_c,tmp_glob_hitPos_c,tmp_glob_hitPos_c);
    const Amg::Vector3D &glob_hitPos_c  = tmp_glob_hitPos_c;

    double glob_eta_c = glob_hitPos_c.eta();
    double glob_phi_c = glob_hitPos_c.phi();

    Amg::MatrixX vector(2, 1);
    vector(0,0) = eta - glob_eta_c;
    vector(1,0) = phi - glob_phi_c;
    while(vector(1,0)>+M_PI) vector(1,0) -= 2.*M_PI;
    while(vector(1,0)<-M_PI) vector(1,0) += 2.*M_PI;
      
    double dR = sqrt(vector(0,0)*vector(0,0) + vector(1,0)*vector(1,0));
    if(dR<dRAccuracy) {
	break;
    }
      
    Amg::Vector2D loc_hitPos_w(locX+length, locY       ); // slightly shifted position in the wire direction 
    Amg::Vector2D loc_hitPos_s(locX,        locY+length); // slightly shifted position in the strip direction 
    Amg::Vector3D tmp_glob_hitPos_w; 
    readout->surface(identify).localToGlobal(loc_hitPos_w,tmp_glob_hitPos_w,tmp_glob_hitPos_w);
    const Amg::Vector3D &glob_hitPos_w = tmp_glob_hitPos_w;
    Amg::Vector3D tmp_glob_hitPos_s; 
    readout->surface(identify).localToGlobal(loc_hitPos_s,tmp_glob_hitPos_s,tmp_glob_hitPos_s);
    const Amg::Vector3D &glob_hitPos_s = tmp_glob_hitPos_s;
      
    Amg::MatrixX matrix(2, 2);
    matrix(0,0) = glob_hitPos_w.eta() - glob_eta_c;
    matrix(1,0) = glob_hitPos_w.phi() - glob_phi_c;
    while(matrix(1,0)>+M_PI) matrix(1,0) -= 2.*M_PI;
    while(matrix(1,0)<-M_PI) matrix(1,0) += 2.*M_PI;
	
    matrix(0,1) = glob_hitPos_s.eta() - glob_eta_c;
    matrix(1,1) = glob_hitPos_s.phi() - glob_phi_c;
    while(matrix(1,1)>+M_PI) matrix(1,1) -= 2.*M_PI;
    while(matrix(1,1)<-M_PI) matrix(1,1) += 2.*M_PI;

    bool invertible = matrix.determinant();
    if(invertible) {
      Amg::MatrixX invertedMatrix = matrix.inverse();
      Amg::MatrixX solution = invertedMatrix * vector;
      locX += length * solution(0,0);
      locY += length * solution(1,0);
	  
      double locR = sqrt(locX*locX + locY*locY);
      if(locR>maxLocR) {
        locX *= maxLocR/locR;
        locY *= maxLocR/locR;
      }
    }
  }
  
  return new Amg::Vector2D(locX, locY); 
} 

const double Muon::TgcRdoToPrepDataToolCore::s_cutDropPrdsWithZeroWidth = 0.1; // 0.1 mm 
