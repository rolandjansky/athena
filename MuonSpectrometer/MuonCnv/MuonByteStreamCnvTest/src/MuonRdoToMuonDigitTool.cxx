/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Author: Ketevi A. Assamagan
// BNL, January 24 2004

// algorithm to decode RDO into digits

#include "MuonRDO/MdtAmtHit.h"
#include "MuonRDO/MdtCsm.h"
#include "MuonRDO/MdtCsmContainer.h"

#include "MuonRDO/CscRawData.h"
#include "MuonRDO/CscRawDataCollection.h"
#include "MuonRDO/CscRawDataContainer.h"

#include "MuonRDO/RpcFiredChannel.h"
#include "MuonRDO/RpcCoinMatrix.h"
#include "MuonRDO/RpcPad.h"
#include "MuonRDO/RpcPadContainer.h"

#include "MuonRDO/TgcRawData.h"
#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRdoContainer.h"

#include "MuonRDO/STGC_RawData.h"
#include "MuonRDO/STGC_RawDataCollection.h"
#include "MuonRDO/STGC_RawDataContainer.h"

#include "MuonRDO/MM_RawData.h"
#include "MuonRDO/MM_RawDataCollection.h"
#include "MuonRDO/MM_RawDataContainer.h"

#include "MuonDigitContainer/MdtDigit.h"
#include "MuonDigitContainer/MdtDigitCollection.h"
#include "MuonDigitContainer/MdtDigitContainer.h"

#include "MuonDigitContainer/CscDigit.h"
#include "MuonDigitContainer/CscDigitCollection.h"
#include "MuonDigitContainer/CscDigitContainer.h"

#include "MuonDigitContainer/RpcDigit.h"
#include "MuonDigitContainer/RpcDigitCollection.h"
#include "MuonDigitContainer/RpcDigitContainer.h"

#include "MuonDigitContainer/TgcDigit.h"
#include "MuonDigitContainer/TgcDigitCollection.h"
#include "MuonDigitContainer/TgcDigitContainer.h"

#include "MuonDigitContainer/sTgcDigit.h"
#include "MuonDigitContainer/sTgcDigitCollection.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"

#include "MuonDigitContainer/MmDigit.h"
#include "MuonDigitContainer/MmDigitCollection.h"
#include "MuonDigitContainer/MmDigitContainer.h"

#include "MuonByteStreamCnvTest/MuonRdoToMuonDigitTool.h"

#include "TGCcablingInterface/ITGCcablingServerSvc.h"


MuonRdoToMuonDigitTool::MuonRdoToMuonDigitTool(const std::string& type,const std::string& name,const IInterface* pIID) 
  : AthAlgTool(type, name, pIID),
    m_tgcCabling(nullptr),
    m_is12foldTgc(true)
{

  declareInterface<IMuonDigitizationTool>(this);
  
  declareProperty("DecodeMdtRDO", m_decodeMdtRDO = true);
  declareProperty("DecodeCscRDO", m_decodeCscRDO = true);
  declareProperty("DecodeRpcRDO", m_decodeRpcRDO = true);
  declareProperty("DecodeTgcRDO", m_decodeTgcRDO = true);
  declareProperty("DecodeSTGC_RDO", m_decodesTgcRDO = true);
  declareProperty("DecodeMM_RDO", m_decodeMmRDO = true);

  declareProperty("show_warning_level_invalid_TGC_A09_SSW6_hit", m_show_warning_level_invalid_TGC_A09_SSW6_hit = false);
}

StatusCode MuonRdoToMuonDigitTool::initialize() {

  ATH_MSG_DEBUG( " in initialize()"  );
  ATH_CHECK( m_acSvc.retrieve() );
  ATH_CHECK( m_idHelperSvc.retrieve() );

  /** CSC calibratin tool for the Condtiions Data base access */
  ATH_CHECK( m_cscCalibTool.retrieve() );

  ATH_CHECK(m_mdtRdoKey.initialize(m_decodeMdtRDO));
  ATH_CHECK(m_mdtDigitKey.initialize(m_decodeMdtRDO));
  ATH_CHECK(m_cscRdoKey.initialize(m_decodeCscRDO));
  ATH_CHECK(m_cscDigitKey.initialize(m_decodeCscRDO));
  ATH_CHECK(m_rpcRdoKey.initialize(m_decodeRpcRDO));
  ATH_CHECK(m_rpcDigitKey.initialize(m_decodeRpcRDO));
  ATH_CHECK(m_tgcRdoKey.initialize(m_decodeTgcRDO));
  ATH_CHECK(m_tgcDigitKey.initialize(m_decodeTgcRDO));
  ATH_CHECK(m_stgcRdoKey.initialize(m_decodesTgcRDO));
  ATH_CHECK(m_stgcDigitKey.initialize(m_decodesTgcRDO));
  ATH_CHECK(m_mmRdoKey.initialize(m_decodeMmRDO));
  ATH_CHECK(m_mmDigitKey.initialize(m_decodeMmRDO));

  if (m_decodeMdtRDO) ATH_CHECK( m_mdtRdoDecoderTool.retrieve() );
  if (m_decodeCscRDO) ATH_CHECK( m_cscRdoDecoderTool.retrieve() );
  if (m_decodeRpcRDO) ATH_CHECK( m_rpcRdoDecoderTool.retrieve() );
  if (m_decodeTgcRDO) ATH_CHECK( m_tgcRdoDecoderTool.retrieve() );
  if (m_decodesTgcRDO) ATH_CHECK( m_stgcRdoDecoderTool.retrieve() );
  if (m_decodeMmRDO) ATH_CHECK( m_mmRdoDecoderTool.retrieve() );

  ATH_CHECK(m_rpcReadKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::digitize(const EventContext& ctx) {

  ATH_MSG_DEBUG( " *************** in MuonRdoToMuonDigitTool::digitize()"  );
  ATH_MSG_DEBUG( "in digitize()"  );

  // create digit containers and decode

  if (m_decodeMdtRDO){
    SG::WriteHandle<MdtDigitContainer> wh_mdtDigit(m_mdtDigitKey, ctx);
    ATH_CHECK(wh_mdtDigit.record(std::make_unique<MdtDigitContainer>(m_idHelperSvc->mdtIdHelper().module_hash_max())));
    ATH_CHECK( decodeMdtRDO(ctx, wh_mdtDigit.ptr()) );
  }

  if (m_decodeCscRDO){
    SG::WriteHandle<CscDigitContainer> wh_cscDigit(m_cscDigitKey, ctx);
    ATH_CHECK(wh_cscDigit.record(std::make_unique<CscDigitContainer> (m_idHelperSvc->cscIdHelper().module_hash_max())));
    ATH_CHECK( decodeCscRDO(ctx, wh_cscDigit.ptr()) );
  }

  if (m_decodeRpcRDO ){
    SG::WriteHandle<RpcDigitContainer> wh_rpcDigit(m_rpcDigitKey, ctx);
    ATH_CHECK(wh_rpcDigit.record(std::make_unique<RpcDigitContainer> (m_idHelperSvc->rpcIdHelper().module_hash_max())));
    ATH_CHECK( decodeRpcRDO(ctx, wh_rpcDigit.ptr()) );
  }

  if(!m_tgcCabling && getTgcCabling().isFailure()) return StatusCode::FAILURE;
  if (m_decodeTgcRDO && m_tgcCabling){
    SG::WriteHandle<TgcDigitContainer> wh_tgcDigit(m_tgcDigitKey, ctx);
    ATH_CHECK(wh_tgcDigit.record(std::make_unique<TgcDigitContainer> (m_idHelperSvc->tgcIdHelper().module_hash_max())));
    ATH_CHECK( decodeTgcRDO(ctx, wh_tgcDigit.ptr()) );
  }

  if (m_decodesTgcRDO){
    SG::WriteHandle<sTgcDigitContainer> wh_stgcDigit(m_stgcDigitKey, ctx);
    ATH_CHECK(wh_stgcDigit.record(std::make_unique<sTgcDigitContainer> (m_idHelperSvc->stgcIdHelper().module_hash_max())));
    ATH_CHECK( decodeSTGC_RDO(ctx, wh_stgcDigit.ptr()) );
  }

  if (m_decodeMmRDO){
    SG::WriteHandle<MmDigitContainer> wh_mmDigit(m_mmDigitKey, ctx);
    ATH_CHECK(wh_mmDigit.record(std::make_unique<MmDigitContainer> (m_idHelperSvc->mmIdHelper().module_hash_max())));
    ATH_CHECK( decodeMM_RDO(ctx, wh_mmDigit.ptr()) );
  }


  return StatusCode::SUCCESS;
}


StatusCode MuonRdoToMuonDigitTool::decodeMdtRDO(const EventContext& ctx, MdtDigitContainer *mdtContainer) const
{
  ATH_MSG_DEBUG( "Decoding MDT RDO into MDT Digit"  );

  // retrieve the collection of RDO
  SG::ReadHandle<MdtCsmContainer> rdoRH(m_mdtRdoKey, ctx);
  if (!rdoRH.isValid()) {
    ATH_MSG_WARNING( "No MDT RDO container found!"  );
    return StatusCode::SUCCESS;
  }
  const MdtCsmContainer* rdoContainer=rdoRH.cptr();
  ATH_MSG_DEBUG( "Retrieved " << rdoContainer->size() << " MDT RDOs." );

  MdtDigitMap_t mdtDigitMap;
  
  // now decode RDO into digits
  for (const MdtCsm* rdoColl : *rdoContainer)
    {
      ATH_CHECK( this->decodeMdt(*rdoColl, mdtDigitMap) );
    }

  for (auto& p : mdtDigitMap) {
    ATH_CHECK( mdtContainer->addCollection (p.second.release(), p.first) );
  }

  return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::decodeCscRDO(const EventContext& ctx, CscDigitContainer* cscContainer) const
{
  ATH_MSG_DEBUG( "Decoding CSC RDO into CSC Digit"  );

  m_acSvc->setStore( &*evtStore() );

  // retrieve the collection of RDO
  SG::ReadHandle<CscRawDataContainer> rdoRH(m_cscRdoKey, ctx);
  if (!rdoRH.isValid()) {
    ATH_MSG_WARNING( "No CSC RDO container found!"  );
    return StatusCode::SUCCESS;
  }
  const CscRawDataContainer* rdoContainer=rdoRH.cptr();
  ATH_MSG_DEBUG( "Retrieved " << rdoContainer->size() << " CSC RDOs." );

  CscDigitMap_t cscDigitMap;

  // now decode RDO into digits
  for (const CscRawDataCollection* rdoColl : *rdoContainer) {
    ATH_CHECK( this->decodeCsc (*rdoColl, cscDigitMap) );
  }

  for (auto& p : cscDigitMap) {
    ATH_CHECK( cscContainer->addCollection (p.second.release(), p.first) );
  }

  return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::decodeRpcRDO(const EventContext& ctx, RpcDigitContainer *rpcContainer) const
{
    ATH_MSG_DEBUG( "Decoding RPC RDO into RPC Digit"  );

    const RpcPadContainer* rdoContainer;

    // retrieve the collection of RDO
    SG::ReadHandle<RpcPadContainer> rdoRH(m_rpcRdoKey, ctx);
    if (!rdoRH.isValid()) {
      ATH_MSG_WARNING( "No RPC RDO container found!"  );
      return StatusCode::SUCCESS;
    }
    rdoContainer = rdoRH.cptr();
    ATH_MSG_DEBUG( "Retrieved " << rdoContainer->size() << " RPC RDOs." );
    // now decode RDO into digits

    SG::ReadCondHandle<RpcCablingCondData> cablingCondData{m_rpcReadKey, ctx};
    const RpcCablingCondData* rpcCabling{*cablingCondData};

    RpcDigitMap_t rpcDigitMap;

    for (const RpcPad* rpcPad : *rdoContainer)
      {
	if ( !rpcPad->empty() ) {
	  ATH_CHECK( this->decodeRpc(*rpcPad, rpcDigitMap, rpcCabling) );
	}
      }

    for (auto& p : rpcDigitMap) {
      ATH_CHECK( rpcContainer->addCollection (p.second.release(), p.first) );
    }

    return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::decodeTgcRDO(const EventContext& ctx, TgcDigitContainer *tgcContainer) const
{
  ATH_MSG_DEBUG( "Decoding TGC RDO into TGC Digit"  );

  if(!m_tgcCabling) return StatusCode::FAILURE;
  
  // retrieve the collection of RDO
  SG::ReadHandle<TgcRdoContainer> rdoRH(m_tgcRdoKey, ctx);
  if (!rdoRH.isValid()) {
    ATH_MSG_WARNING( "No TGC RDO container found!"  );
    return StatusCode::SUCCESS;
  }

  const TgcRdoContainer* rdoContainer = rdoRH.cptr();
  ATH_MSG_DEBUG( "Retrieved " << rdoContainer->size() << " TGC RDOs." );

  TgcDigitMap_t tgcDigitMap;

  TgcRdoContainer::const_iterator tgcRDO = rdoContainer->begin();
  for (; tgcRDO!=rdoContainer->end();++tgcRDO)
    {
      if ( !(*tgcRDO)->empty() ) {
	ATH_CHECK( this->decodeTgc (**tgcRDO, tgcDigitMap) );
      }
    }
     
  for (auto& p : tgcDigitMap) {
    ATH_CHECK( tgcContainer->addCollection (p.second.release(), p.first) );
  }

  return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::decodeSTGC_RDO(const EventContext& ctx, sTgcDigitContainer* stgcContainer) const
{
  ATH_MSG_DEBUG( "Decoding sTGC RDO into sTGC Digit"  );

  m_acSvc->setStore( &*evtStore() );


  SG::ReadHandle<Muon::STGC_RawDataContainer> rdoRH(m_stgcRdoKey, ctx);
  if (!rdoRH.isValid()) {
    ATH_MSG_WARNING( "No sTGC RDO container found!" );
    return StatusCode::SUCCESS;
  }
  const Muon::STGC_RawDataContainer* sTgcRDO=rdoRH.cptr();
  ATH_MSG_DEBUG( "Retrieved " << sTgcRDO->size() << " sTGC RDOs." );

  sTgcDigitMap_t stgcDigitMap;

  ATH_MSG_DEBUG("Converting sTGC RDOs to Digits");
  for (const Muon::STGC_RawDataCollection * rdoColl : *sTgcRDO) { // Go through RDO container
    ATH_MSG_DEBUG("rdoColl size = " << rdoColl->size() );
    ATH_CHECK( this->decodeSTGC (*rdoColl, stgcDigitMap) );
  }

  for (auto& p : stgcDigitMap) {
    ATH_CHECK( stgcContainer->addCollection (p.second.release(), p.first) );
  }

  return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::decodeMM_RDO(const EventContext& ctx, MmDigitContainer *mmContainer) const
{
  ATH_MSG_DEBUG( "Decoding MM RDO into MM Digit"  );

  m_acSvc->setStore( &*evtStore() );

  SG::ReadHandle<Muon::MM_RawDataContainer> rdoRH(m_mmRdoKey, ctx);
  if (!rdoRH.isValid()) {
    ATH_MSG_WARNING( "No MM RDO container found!" );
    return StatusCode::SUCCESS;
  }
  const Muon::MM_RawDataContainer* MmRDO=rdoRH.cptr();

  ATH_MSG_DEBUG( "Retrieved " << MmRDO->size() << " MM RDOs." );

  MmDigitMap_t mmDigitMap;

  ATH_MSG_DEBUG("Converting MM RDOs to Digits");
  for (const Muon::MM_RawDataCollection * rdoColl : *MmRDO) { // Go through RDO container
    ATH_MSG_DEBUG("rdoColl size = " << rdoColl->size() );
    ATH_CHECK( this->decodeMM (*rdoColl, mmDigitMap) );
  }

  for (auto& p : mmDigitMap) {
    ATH_CHECK( mmContainer->addCollection (p.second.release(), p.first) );
  }

  return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::decodeMdt (const MdtCsm& rdoColl,
                                              MdtDigitMap_t& mdtDigitMap) const
{
    IdContext mdtContext = m_idHelperSvc->mdtIdHelper().module_context();
 
    if ( !rdoColl.empty() ) {
        ATH_MSG_DEBUG( " Number of AmtHit in this Csm " 
                       << rdoColl.size()  );
	
	uint16_t subdetId = rdoColl.SubDetId();
	uint16_t mrodId = rdoColl.MrodId();
	uint16_t csmId = rdoColl.CsmId();
	
	// for each Csm, loop over AmtHit, converter AmtHit to digit
	// retrieve/create digit collection, and insert digit into collection
        for (const MdtAmtHit* amtHit : rdoColl)
	  { 
            std::unique_ptr<MdtDigit> newDigit
              (m_mdtRdoDecoderTool->getDigit(amtHit,subdetId,
                                             mrodId,csmId));
	    
	    if (!newDigit) {
	      ATH_MSG_WARNING( "Error in MDT RDO decoder"  );
              continue;
	    }
	
	    // find here the Proper Digit Collection identifier, using the rdo-hit id
	    // (since RDO collections are not in a 1-to-1 relation with digit collections)
	    Identifier elementId = m_idHelperSvc->mdtIdHelper().elementID(newDigit->identify());
	    IdentifierHash coll_hash;
	    if (m_idHelperSvc->mdtIdHelper().get_hash(elementId, coll_hash, &mdtContext)) {
	      ATH_MSG_WARNING( "Unable to get MDT digit collection hash id " 
                               << "context begin_index = " << mdtContext.begin_index()
                               << " context end_index  = " << mdtContext.end_index()
                               << " the identifier is " );
	      elementId.show();
	    } 

            std::unique_ptr<MdtDigitCollection>& coll = mdtDigitMap[coll_hash];
            if (!coll) {
              coll = std::make_unique<MdtDigitCollection> (elementId, coll_hash);
            }
            coll->push_back (std::move (newDigit));
	  }
    }
    return StatusCode::SUCCESS;
}  

StatusCode MuonRdoToMuonDigitTool::decodeCsc(const CscRawDataCollection& rdoColl,
                                             CscDigitMap_t& cscDigitMap) const
{
      ATH_MSG_DEBUG( " Number of RawData in this rdo " 
                     << rdoColl.size()  );
      
      double samplingTime = rdoColl.rate();
      unsigned int numSamples   = rdoColl.numSamples();

      //      decoder.setParams(samplingTime);

      IdContext cscContext = m_idHelperSvc->cscIdHelper().module_context();

      /** for each Rdo, loop over RawData, converter RawData to digit
	  retrieve/create digit collection, and insert digit into collection */
      for (const CscRawData* data : rdoColl) {
	uint16_t width = data->width();
        //        Identifier stationId = decoder.stationIdentifier(data);
        Identifier stationId = m_cscRdoDecoderTool->stationIdentifier(data);
	for (int j=0; j<width; ++j) {
          //          Identifier channelId = decoder.channelIdentifier(data, j);
          Identifier channelId = m_cscRdoDecoderTool->channelIdentifier(data, j);
	  std::vector<uint16_t> samples;
	  bool extractSamples = data->samples(j, numSamples, samples);
	  if (!extractSamples) {
            ATH_MSG_WARNING( "Unable to extract samples for strip " << j 
                             << " Online Cluster width = " << width 
                             << " for number of Samples = " << numSamples 
                             << " continuing ..."  );
	     continue;
          }	 
	  std::vector<float> charges;
	  bool adctocharge = m_cscCalibTool->adcToCharge(samples, data->hashId(), charges);
	  double charge = 0.0;
	  double time   = 0.0;
	  unsigned int samplingPhase = 0;
          int errorfindCharge = m_cscCalibTool->findCharge(samplingTime, samplingPhase, charges, charge, time);
          // WPARK: Potential problem! error code includes time out of range and aa>0
          // If this is the case, only very nice samples are converted to Digit. Sep 11, 2009
	  if ( !adctocharge || !errorfindCharge ) { 
	    ATH_MSG_WARNING( " CSC conversion ADC to Charge failed " 
                             << "CSC Digit not build ... " 
                             << " or charge finding failed " << " ... skipping "  );
	    continue;
	  }
	  ATH_MSG_DEBUG( "CSC RDO->CscDigit: " << m_idHelperSvc->cscIdHelper().show_to_string(channelId) );
	  int theCharge = static_cast<int>(charge);
          auto newDigit = std::make_unique<CscDigit>(channelId, theCharge, time);
	  ATH_MSG_DEBUG( "CSC RDO->Digit: " << m_idHelperSvc->cscIdHelper().show_to_string(newDigit->identify()) 
                         << " " << newDigit->charge() << " " << charge << " time= " << time  );

	  for (uint16_t i=0; i< samples.size(); ++i) {
	    ATH_MSG_DEBUG( "CSC RDO->Digit: " << samples[i]  );
	  }
          IdentifierHash coll_hash;
          if (m_idHelperSvc->cscIdHelper().get_hash(stationId, coll_hash, &cscContext)) {
            ATH_MSG_WARNING( "Unable to get CSC digiti collection hash id " 
                             << "context begin_index = " << cscContext.begin_index()
                             << " context end_index  = " << cscContext.end_index()
                             << " the identifier is " );
             stationId.show();
          }

          std::unique_ptr<CscDigitCollection>& coll = cscDigitMap[coll_hash];
          if (!coll) {
            coll = std::make_unique<CscDigitCollection> (stationId, coll_hash);
          }
          coll->push_back (std::move (newDigit));
	}
      }
      return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::decodeRpc (const RpcPad& rdoColl,
                                              RpcDigitMap_t& rpcDigitMap,
                                              const RpcCablingCondData* rpcCab) const
{

            IdContext rpcContext = m_idHelperSvc->rpcIdHelper().module_context();

            ATH_MSG_DEBUG( " Number of CMs in this Pad " 
                           << rdoColl.size()  );
	
            Identifier padOfflineId = rdoColl.identify();
            //Get pad online id and sector id
            uint16_t padId     = rdoColl.onlineId(); 
            uint16_t sectorId  = rdoColl.sector(); 

            int stationName = m_idHelperSvc->rpcIdHelper().stationName(padOfflineId);
            int stationEta  = m_idHelperSvc->rpcIdHelper().stationEta(padOfflineId);
            int stationPhi  = m_idHelperSvc->rpcIdHelper().stationPhi(padOfflineId);
            int doubletR    = m_idHelperSvc->rpcIdHelper().doubletR(padOfflineId);
	
            Identifier elementId = m_idHelperSvc->rpcIdHelper().elementID(stationName, stationEta,
                                                          stationPhi, doubletR);
	
            // For each pad, loop on the coincidence matrices
            for (const RpcCoinMatrix* cm : rdoColl) {
                // Get CM online Id
                uint16_t cmaId = cm->onlineId();

                // For each CM, loop on the fired channels
                for (const RpcFiredChannel* rpcChan : *cm) {
                  std::unique_ptr<std::vector<RpcDigit*> > digitVec 
                    (m_rpcRdoDecoderTool->getDigit(rpcChan, sectorId, padId, cmaId, rpcCab));
	    
                    if (!digitVec) {
                        ATH_MSG_FATAL( "Error in the RPC RDO decoder "  );
                        return StatusCode::FAILURE;
                    }

                    // Loop on the digits corresponding to the fired channel
                    for (RpcDigit* newDigit : *digitVec)
                    {
                        elementId = m_idHelperSvc->rpcIdHelper().elementID(newDigit->identify());
                        
                        IdentifierHash coll_hash;
                        if (m_idHelperSvc->rpcIdHelper().get_hash(elementId, coll_hash, &rpcContext)) {
                          ATH_MSG_WARNING( "Unable to get RPC digit collection hash id " 
                                           << "context begin_index = " << rpcContext.begin_index()
                                           << " context end_index  = " << rpcContext.end_index()
                                           << " the identifier is " );
                                  elementId.show();
                        } 

                        m_acSvc->setStore( &*evtStore() );

                        std::unique_ptr<RpcDigitCollection>& coll = rpcDigitMap[coll_hash];
                        if (!coll) {
                          coll = std::make_unique<RpcDigitCollection> (elementId, coll_hash);
                        }
                        coll->push_back (std::move (newDigit));
                    }
                }
            }
    
            return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::decodeTgc (const TgcRdo& rdoColl,
                                              TgcDigitMap_t& tgcDigitMap) const
{
      if(!m_tgcCabling) return StatusCode::FAILURE;

      IdContext tgcContext = m_idHelperSvc->tgcIdHelper().module_context();

      ATH_MSG_DEBUG( "Number of RawData in this rdo " 
                     << rdoColl.size()  );
      // for each Rdo, loop over RawData, converter RawData to digit
      // retrieve/create digit collection, and insert digit into collection
      TgcRdo::const_iterator itD  = rdoColl.begin();
      TgcRdo::const_iterator itDe = rdoColl.end();

      std::map<std::array<uint16_t,5>, uint16_t> stripMap;
      if(m_is12foldTgc) { // Only when 12-fold TGC cabling is used 
	// TGC2 Endcap Strip OR channel treatement preparation start
	// Signals are ORed as follows: 
	// |stationEta|=5, T9, E1 - slbId=16, bit0 of stripSlbBits in this code 

	//                        /
	// |stationEta|=4, T8, E2 - slbId=17, bit1 of stripSlbBits in this code 
	//                        /
	// |stationEta|=3, T7, E3 - slbId=18, bit2 of stripSlbBits in this code 
	//                        /
	// |stationEta|=2, T6, E4 - slbId=19, bit3 of stripSlbBits in this code 
	//                        /
	// |stationEta|=1, T4, E5 - slbId=20, bit4 of stripSlbBits in this code
	for (; itD != itDe; ++itD) {
	  if((*itD)->isCoincidence()) continue; // Require hits
	  if((*itD)->slbType()!=TgcRawData::SLB_TYPE_DOUBLET_STRIP) continue; // Require TGC2 or TGC3
	  if((*itD)->sswId()==7) continue; // Exclude Forward
	  if((*itD)->bitpos()<112 || (*itD)->bitpos()>199) continue; // Require C, D-input
	  
	  std::array<uint16_t,5> stripId;
	  stripId[0] = (*itD)->subDetectorId();
	  stripId[1] = (*itD)->rodId();
	  stripId[2] = (*itD)->sswId();
	  stripId[3] = (*itD)->bitpos();
	  stripId[4] = (*itD)->bcTag();
	  uint16_t stripSlbBits = 0x1 << ((*itD)->slbId()-16);
	  std::map<std::array<uint16_t,5>, uint16_t>::iterator itMap = stripMap.find(stripId); // Find correspond channel 
	  if(itMap==stripMap.end()) { // This is new one
	    stripMap.emplace(stripId, stripSlbBits);
	  } else { // This already exists
	    itMap->second |= stripSlbBits;
	  }
	}
	// Covert to original hit patterns 
        for (auto& jtPair : stripMap) {
	  if(jtPair.second<=31) {
	    // x    : 5-bit variable
	    // f(x) : OR function above, Digit->RDO conversion 
	    // g(x) : originalHitBits which satisfies f(g(f(x))) = f(x), RDO->Digit conversion 
	    static const uint16_t originalHitBits[32] = {
	      //  0   1   2   3   4   5   6   7
	          0,  1,  0,  3,  0,  0,  4,  7,
	      //  8   9  10  11  12  13  14  15
	          0,  0,  0,  0,  8,  9, 12, 15,
	      // 16  17  18  19  20  21  22  23
		  0,  0,  0,  0,  0,  0,  0,  0,
	      // 24  25  26  27  28  29  30  31
		 16, 17,  0, 19, 24, 25, 28, 31
	    };
	    jtPair.second = originalHitBits[jtPair.second]; 
	  } else {
	    jtPair.second = 0;
	  }
	}
	// TGC2 Endcap Strip OR channel treatement preparation end
	itD = rdoColl.begin();
      }

      for (; itD != itDe; ++itD)
        { 
	  // check Hit or Coincidence
	  if ((*itD)->isCoincidence()) continue;
	  
	  if(m_is12foldTgc) { // Only when 12-fold TGC cabling is used 
	    // TGC2 Endcap Strip OR channel treatement start 
	    if((*itD)->slbType()==TgcRawData::SLB_TYPE_DOUBLET_STRIP && // Require TGC2 or TGC3
	       (*itD)->sswId()!=7 && // Exclude Forward
	       (*itD)->bitpos()>=112 && (*itD)->bitpos()<=199 // Require C, D-input
	       ) {
	      std::array<uint16_t,5> stripId;
	      stripId[0] = (*itD)->subDetectorId();
	      stripId[1] = (*itD)->rodId();
	      stripId[2] = (*itD)->sswId();
	      stripId[3] = (*itD)->bitpos();
	      stripId[4] = (*itD)->bcTag();
	      std::map<std::array<uint16_t,5>, uint16_t>::iterator itMap = stripMap.find(stripId); // Find correspond hit
	      if(itMap!=stripMap.end()) {
		uint16_t stripSlbBits = 0x1 << ((*itD)->slbId()-16);
		if(!(itMap->second & stripSlbBits)) continue; // This hit is additional.
	      }
	    }
	    // TGC2 Endcap Strip OR channel treatement end 
	  }

	  // repeat two times for ORed channel
	  for (int iOr=0; iOr<2; ++iOr)
	    {

	      if(m_is12foldTgc) { // Only when 12-fold TGC cabling is used 
		// TGC2 Endcap Strip OR channel is not converted. 
		if(iOr && (*itD)->slbType()==TgcRawData::SLB_TYPE_DOUBLET_STRIP) continue; 
	      }

	      bool orFlag = false;
	      
	      // check if this channel has ORed partner only when 2nd time
	      if (iOr != 0) 
		{
		  bool o_found = m_tgcCabling->isOredChannel((*itD)->subDetectorId(),
							  (*itD)->rodId(),
							  (*itD)->sswId(),
							  (*itD)->slbId(),
							  (*itD)->bitpos());
		  // set OR flag
		  if (o_found)
		    orFlag=true;
		  else
		    continue;
		}
	      
	      // get element ID
	      Identifier elementId;
	      bool e_found = m_tgcCabling->getElementIDfromReadoutID
		(elementId,(*itD)->subDetectorId(),(*itD)->rodId(),
		 (*itD)->sswId(),(*itD)->slbId(),(*itD)->bitpos(),orFlag);
	      
	      if (!e_found) {
		bool show_warning_level = true;
		
		/* One invalid channel in TGC sector A09:
		   sub=103 rod=9 ssw=6 slb=20 bitpos=151 orFlag=0
		   was always seen in 2008 data, at least run 79772 - 91800.
		   bug #48828 */
		if((*itD)->subDetectorId()==103 &&
		   (*itD)->rodId()==9 &&
		   (*itD)->sswId()==6 &&
		   (*itD)->slbId()==20 &&
		   (*itD)->bitpos()==151) {
		  show_warning_level = m_show_warning_level_invalid_TGC_A09_SSW6_hit;
		}
                
		if(show_warning_level || msgLvl(MSG::DEBUG)) {
		  ATH_MSG_WARNING( "ElementID not found for "
                                   << " sub=" << (*itD)->subDetectorId()
                                   << " rod=" << (*itD)->rodId()
                                   << " ssw=" << (*itD)->sswId()
                                   << " slb=" << (*itD)->slbId()
                                   << " bitpos=" << (*itD)->bitpos() 
                                   << " orFlag=" << orFlag  );
		}
		continue;
	      }
	      
	      // convert RawData to Digit
              std::unique_ptr<TgcDigit> newDigit (m_tgcRdoDecoderTool->getDigit(*itD,orFlag));
	      
	      // check if converted correctly
	      if (!newDigit) continue;
	      
	      // check new element or not
              IdentifierHash coll_hash;
              if (m_idHelperSvc->tgcIdHelper().get_hash(elementId, coll_hash, &tgcContext)) {
                ATH_MSG_WARNING( "Unable to get TGC digit collection hash " 
                                 << "context begin_index = " << tgcContext.begin_index()
                                 << " context end_index  = " << tgcContext.end_index()
                                 << " the identifier is " );
                 elementId.show();
              } 

              std::unique_ptr<TgcDigitCollection>& coll = tgcDigitMap[coll_hash];
              if (!coll) {
                coll = std::make_unique<TgcDigitCollection> (elementId, coll_hash);
              }

	      // check duplicate digits
	      bool duplicate = false;
              for (const TgcDigit* digit : *coll) {
		if ((newDigit->identify() == digit->identify()) && 
		    (newDigit->bcTag()    == digit->bcTag())) {
		  duplicate = true;
		  ATH_MSG_DEBUG( "Duplicate TGC Digit removed"   );
		  break;
		}
	      }
	      if (!duplicate) {
		// add the digit to the collection
		coll->push_back(newDigit.release());
	      }
	    }
	}
      return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::decodeSTGC(const Muon::STGC_RawDataCollection& rdoColl,
                                              sTgcDigitMap_t& stgcDigitMap) const
{
      if ( !rdoColl.empty() ) {
      ATH_MSG_DEBUG( " Number of RawData in this rdo " << rdoColl.size()  );
      IdContext stgcContext = m_idHelperSvc->stgcIdHelper().module_context();

      /** for each Rdo, loop over RawData, converter RawData to digit
	  retrieve/create digit collection, and insert digit into collection */
      for (const Muon::STGC_RawData * data : rdoColl){
            std::unique_ptr<sTgcDigit> newDigit (m_stgcRdoDecoderTool->getDigit(data));

	    if (!newDigit) {
	      ATH_MSG_WARNING( "Error in sTGC RDO decoder"  );
              continue;
	    }
	    ATH_MSG_DEBUG( "sTGC RDO->sTGCDigit: " << m_idHelperSvc->stgcIdHelper().show_to_string(newDigit->identify()) );
	    ATH_MSG_DEBUG( "sTGC RDO->Digit: " << m_idHelperSvc->stgcIdHelper().show_to_string(newDigit->identify())
                         << " charge: " << newDigit->charge() << " time: " << newDigit->time()  );

	    Identifier elementId = m_idHelperSvc->stgcIdHelper().elementID(newDigit->identify());
	    IdentifierHash coll_hash;
	    if (m_idHelperSvc->stgcIdHelper().get_hash(elementId, coll_hash, &stgcContext)) {
	      ATH_MSG_WARNING( "Unable to get sTGC digit collection hash id "
                               << "context begin_index = " << stgcContext.begin_index()
                               << " context end_index  = " << stgcContext.end_index()
                               << " the identifier is " );
	      elementId.show();
	    }

            std::unique_ptr<sTgcDigitCollection>& coll = stgcDigitMap[coll_hash];
            if (!coll) {
              coll = std::make_unique<sTgcDigitCollection> (elementId, coll_hash);
            }
            coll->push_back (std::move (newDigit));
      }
    }
    return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::decodeMM(const Muon::MM_RawDataCollection& rdoColl,
                                            MmDigitMap_t& mmDigitMap) const
{
      if ( !rdoColl.empty() ) {
      ATH_MSG_DEBUG( " Number of RawData in this rdo " << rdoColl.size()  );
      IdContext mmContext = m_idHelperSvc->mmIdHelper().module_context();

      /** for each Rdo, loop over RawData, converter RawData to digit
	  retrieve/create digit collection, and insert digit into collection */
      for (const Muon::MM_RawData * data : rdoColl) {
            std::unique_ptr<MmDigit> newDigit
              (m_mmRdoDecoderTool->getDigit(data));

	    if (!newDigit) {
	      ATH_MSG_WARNING( "Error in MM RDO decoder"  );
              continue;
	    }
	    ATH_MSG_DEBUG( "MM RDO->MMDigit: " << m_idHelperSvc->mmIdHelper().show_to_string(newDigit->identify()) );

	    Identifier elementId = m_idHelperSvc->mmIdHelper().elementID(newDigit->identify());
	    IdentifierHash coll_hash;
	    if (m_idHelperSvc->mmIdHelper().get_hash(elementId, coll_hash, &mmContext)) {
	      ATH_MSG_WARNING( "Unable to get MM digit collection hash id "
                               << "context begin_index = " << mmContext.begin_index()
                               << " context end_index  = " << mmContext.end_index()
                               << " the identifier is " );
	      elementId.show();
	    }

            std::unique_ptr<MmDigitCollection>& coll = mmDigitMap[coll_hash];
            if (!coll) {
              coll = std::make_unique<MmDigitCollection> (elementId, coll_hash);
            }
            coll->push_back (std::move (newDigit));
      }
    }
    return StatusCode::SUCCESS;
}
 
StatusCode MuonRdoToMuonDigitTool::getTgcCabling() {
  // get TGC cablingSvc
  ServiceHandle<ITGCcablingServerSvc> TgcCabGet("TGCcablingServerSvc", name());
  ATH_CHECK( TgcCabGet.retrieve() );
  ATH_CHECK(  TgcCabGet->giveCabling(m_tgcCabling) );

  m_is12foldTgc = TgcCabGet->isAtlas();

  return StatusCode::SUCCESS;
}
