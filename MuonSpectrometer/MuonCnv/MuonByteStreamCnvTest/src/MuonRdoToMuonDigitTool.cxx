/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Author: Ketevi A. Assamagan
// BNL, January 24 2004

// algorithm to decode RDO into digits

#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "MuonMDT_CnvTools/IMDT_RDO_Decoder.h"
#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"
#include "MuonRPC_CnvTools/IRPC_RDO_Decoder.h"
#include "MuonTGC_CnvTools/ITGC_RDO_Decoder.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"

//#include "MDTcabling/MDTcablingSvc.h"
#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "TGCcablingInterface/ITGCcablingServerSvc.h"

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

#include "MuonByteStreamCnvTest/MuonRdoToMuonDigitTool.h"

namespace std { template<typename _Tp> class auto_ptr; }

MuonRdoToMuonDigitTool::MuonRdoToMuonDigitTool(const std::string& type,const std::string& name,const IInterface* pIID) 
  : AthAlgTool(type, name, pIID),
    m_acSvc("ActiveStoreSvc", name),
    m_cscCalibTool("CscCalibTool"),
    m_mdtRdoDecoderTool("Muon::MdtRDO_Decoder"),
    m_cscRdoDecoderTool("Muon::CscRDO_Decoder"),
    m_rpcRdoDecoderTool("Muon::RpcRDO_Decoder"),
    m_tgcRdoDecoderTool("Muon::TgcRDO_Decoder"),
    m_mdtHelper(0), m_cscHelper(0), 
    m_rpcHelper(0), m_tgcHelper(0), 
    m_tgcCabling(0), m_mdtContainer(0), m_cscContainer(0),
    m_rpcContainer(0), m_tgcContainer(0), 
    m_is12foldTgc(true)
{

  declareInterface<IMuonDigitizationTool>(this);
  
  declareProperty("DecodeMdtRDO", m_decodeMdtRDO = true);
  declareProperty("DecodeCscRDO", m_decodeCscRDO = true);
  declareProperty("DecodeRpcRDO", m_decodeRpcRDO = true);
  declareProperty("DecodeTgcRDO", m_decodeTgcRDO = true);
  
  declareProperty("RetrievePrivateCopy", m_retrievePrivateCopy=false);
  declareProperty("cscCalibTool",  m_cscCalibTool);
  declareProperty("mdtRdoDecoderTool",  m_mdtRdoDecoderTool);
  declareProperty("cscRdoDecoderTool",  m_cscRdoDecoderTool);
  declareProperty("rpcRdoDecoderTool",  m_rpcRdoDecoderTool);
  declareProperty("tgcRdoDecoderTool",  m_tgcRdoDecoderTool);

  declareProperty("show_warning_level_invalid_TGC_A09_SSW6_hit", m_show_warning_level_invalid_TGC_A09_SSW6_hit = false);
}

MuonRdoToMuonDigitTool::~MuonRdoToMuonDigitTool()  
{}

StatusCode MuonRdoToMuonDigitTool::initialize() {

  ATH_MSG_DEBUG( " in initialize()"  );
  ATH_CHECK( m_acSvc.retrieve() );

  // initialize the identifier helpers

  ATH_CHECK( detStore()->retrieve(m_mdtHelper, "MDTIDHELPER") );
  ATH_CHECK( detStore()->retrieve(m_cscHelper, "CSCIDHELPER") );
  ATH_CHECK( detStore()->retrieve(m_rpcHelper, "RPCIDHELPER") );
  ATH_CHECK( detStore()->retrieve(m_tgcHelper, "TGCIDHELPER") );

  // get MDT cablingSvc
  //  status = service("MDTcablingSvc", m_mdtCabling);
  //
  //if (status.isFailure()) {
  // ATH_MSG_WARNING( "Could not get MDTcablingSvc !"  );
  //m_mdtCabling = 0;
  //return StatusCode::SUCCESS;
  //} 
  //  else {
  //ATH_MSG_DEBUG( " Found the MDTcablingSvc. "  );
  //}

  // get RPC cablingSvc
  ServiceHandle<IRPCcablingServerSvc> RpcCabGet ("RPCcablingServerSvc", name());
  ATH_CHECK( RpcCabGet.retrieve() );
  ATH_CHECK( RpcCabGet->giveCabling(m_rpcCabling) );

  /** CSC calibratin tool for the Condtiions Data base access */
  ATH_CHECK( m_cscCalibTool.retrieve() );

  // create empty digit containers ready for filling

  try{
    m_mdtContainer = new MdtDigitContainer(m_mdtHelper->module_hash_max());
  } catch(const std::bad_alloc&){
    ATH_MSG_FATAL( "Could not create a new MdtDigitContainer!" );
    return StatusCode::FAILURE;
  }
  m_mdtContainer->addRef();

  try{
    m_cscContainer = new CscDigitContainer(m_cscHelper->module_hash_max());
  } catch(const std::bad_alloc&){
    ATH_MSG_FATAL( "Could not create a new CscDigitContainer!" );
    return StatusCode::FAILURE;
  }
  m_cscContainer->addRef();

  try{
    m_rpcContainer = new RpcDigitContainer(m_rpcHelper->module_hash_max());
  } catch(const std::bad_alloc&){
    ATH_MSG_FATAL( "Could not create a new RpcDigitContainer!" );
    return StatusCode::FAILURE;
  }
  m_rpcContainer->addRef();

  try{
    m_tgcContainer = new TgcDigitContainer(m_tgcHelper->module_hash_max());
  } catch(const std::bad_alloc&){
    ATH_MSG_FATAL( "Could not create a new TgcDigitContainer!" );
    return StatusCode::FAILURE;
  }
  m_tgcContainer->addRef();

  ATH_CHECK( m_mdtRdoDecoderTool.retrieve() );
  ATH_CHECK( m_cscRdoDecoderTool.retrieve() );
  ATH_CHECK( m_rpcRdoDecoderTool.retrieve() );
  ATH_CHECK( m_tgcRdoDecoderTool.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::digitize() {

  ATH_MSG_DEBUG( " *************** in MuonRdoToMuonDigitTool::digitize()"  );
  ATH_MSG_DEBUG( "in digitize()"  );

  if (m_decodeMdtRDO) ATH_CHECK( decodeMdtRDO() );
  if (m_decodeCscRDO) ATH_CHECK( decodeCscRDO() );
  if (m_decodeRpcRDO && m_rpcCabling) ATH_CHECK( decodeRpcRDO() );

  if(!m_tgcCabling && getTgcCabling().isFailure()) return StatusCode::FAILURE;

  if (m_decodeTgcRDO && m_tgcCabling) ATH_CHECK( decodeTgcRDO() );

  return StatusCode::SUCCESS;
}


StatusCode MuonRdoToMuonDigitTool::finalize() {
    
   ATH_MSG_DEBUG( "finalize."  );

   if(m_mdtContainer) m_mdtContainer->release();
   if(m_cscContainer) m_cscContainer->release();
   if(m_rpcContainer) m_rpcContainer->release();
   if(m_tgcContainer) m_tgcContainer->release();

   return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::decodeMdtRDO() {

  ATH_MSG_DEBUG( "Decoding MDT RDO into MDT Digit"  );

  // clean up the digit container
  m_mdtContainer->cleanup();

  // retrieve the collection of RDO
  Identifier oldId;
  MdtDigitCollection * collection = 0;
  const MdtCsmContainer* rdoContainer;

  if ( !m_retrievePrivateCopy ) {
     m_acSvc->setStore( &*evtStore() );
     StatusCode status = evtStore()->retrieve( rdoContainer, "MDTCSM" );
     if (status.isFailure()) {
        ATH_MSG_WARNING( "No MDT RDO container in StoreGate!"  );
        return StatusCode::SUCCESS;
     }
     ATH_MSG_DEBUG( "Retrieved " << rdoContainer->size() << " MDT RDOs." );
     
     // now decode RDO into digits
     m_acSvc->setStore( &*evtStore() );
     MdtCsmContainer::const_iterator mdtCSM = rdoContainer->begin();
       
     for (; mdtCSM!=rdoContainer->end();++mdtCSM)
     {
         status = this->decodeMdt( *mdtCSM, collection, oldId); 
         if ( status.isFailure() ) return status;
     }
  } else {
     m_acSvc->setStore( &*evtStore() );
     std::auto_ptr<MdtCsmContainer> cdata(evtStore()->retrievePrivateCopy<MdtCsmContainer>("MDTCSM"));
     if( !cdata.get() ) {
       ATH_MSG_WARNING( "Could not get real data MDT RDO container \"" << " MDTCSM " << "\""  );
       return StatusCode::SUCCESS;
     }

     MdtCsmContainer::const_iterator it_rdoColl  = (cdata.get())->begin();
     MdtCsmContainer::const_iterator lastRdoColl = (cdata.get())->end();
     for (; it_rdoColl!=lastRdoColl; ++it_rdoColl)
     {
        const MdtCsm * rdoColl = *it_rdoColl;
        StatusCode status = this->decodeMdt( rdoColl, collection, oldId );
        if ( status.isFailure() ) return status;
     }
  } 

  // record the container in storeGate
  std::string key = "MDT_DIGITS";
  m_acSvc->setStore( &*evtStore() );
  ATH_CHECK( evtStore()->record(m_mdtContainer,key) );

  return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::decodeCscRDO() {

  ATH_MSG_DEBUG( "Decoding CSC RDO into CSC Digit"  );

  //  CscRDO_Decoder decoder;
  //  decoder.set(m_cscHelper);

  m_acSvc->setStore( &*evtStore() );

  // clean up the digit container
  m_cscContainer->cleanup();

  Identifier oldId;
  CscDigitCollection * collection = 0;
  const CscRawDataContainer* rdoContainer;

  // retrieve the collection of RDO
  if ( !m_retrievePrivateCopy ) {
     m_acSvc->setStore( &*evtStore() );
     StatusCode status = evtStore()->retrieve( rdoContainer, "CSCRDO" );
     if (status.isFailure()) {
       ATH_MSG_WARNING( "No CSC RDO container in StoreGate!"  );
       return StatusCode::SUCCESS;
     }
     ATH_MSG_DEBUG( "Retrieved " << rdoContainer->size() << " CSC RDOs." );
     // now decode RDO into digits
     m_acSvc->setStore( &*evtStore() );
     const DataHandle<CscRawDataCollection> rdoColl;
     const DataHandle<CscRawDataCollection> lastRdoColl;
     m_acSvc->setStore( &*evtStore() );
     if (StatusCode::SUCCESS != evtStore()->retrieve(rdoColl,lastRdoColl)) {
       ATH_MSG_DEBUG( "Cannot retrieve CscRawDataCollection"  );
     }
  
     for (; rdoColl!=lastRdoColl; ++rdoColl) {
       //        status = this->decodeCsc ( rdoColl, collection, oldId, decoder );
       status = this->decodeCsc ( rdoColl, collection, oldId);
        if ( status.isFailure() ) return status;
     }
  } else {
     m_acSvc->setStore( &*evtStore() );
     std::auto_ptr<CscRawDataContainer> cdata(evtStore()->retrievePrivateCopy<CscRawDataContainer>("CSCRDO"));
     if( !cdata.get() ) {
       ATH_MSG_WARNING( "Could not get real data CSC RDO container \"" << " CSCRDO " << "\""  );
         return StatusCode::SUCCESS;
     }
     CscRawDataContainer::const_iterator it_rdoColl  = (cdata.get())->begin();
     CscRawDataContainer::const_iterator lastRdoColl = (cdata.get())->end();
     for (; it_rdoColl!=lastRdoColl; ++it_rdoColl)
     {
        const CscRawDataCollection * rdoColl = *it_rdoColl;
        //        status = this->decodeCsc( rdoColl, collection, oldId, decoder );
        StatusCode status = this->decodeCsc( rdoColl, collection, oldId);
        if ( status.isFailure() ) return status;
     }
  } 
  /** record the container in storeGate */
  std::string key = "CSC_DIGITS";
  m_acSvc->setStore( &*evtStore() );
  ATH_CHECK( evtStore()->record(m_cscContainer,key) );
  ATH_CHECK( evtStore()->setConst(m_cscContainer) );

  return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::decodeRpcRDO() {

    ATH_MSG_DEBUG( "Decoding RPC RDO into RPC Digit"  );

    // clean up the digit container
    m_rpcContainer->cleanup();

    RpcDigitCollection * collection = 0;
    const RpcPadContainer* rdoContainer;

    // retrieve the collection of RDO
    if ( !m_retrievePrivateCopy ) {
       m_acSvc->setStore( &*evtStore() );
       StatusCode status = evtStore()->retrieve( rdoContainer, "RPCPAD" );
       if (status.isFailure()) {
          ATH_MSG_WARNING( "No RPC RDO container in StoreGate!"  );
          return StatusCode::SUCCESS;
       }
       ATH_MSG_DEBUG( "Retrieved " << rdoContainer->size() << " RPC RDOs." );
       // now decode RDO into digits
       m_acSvc->setStore( &*evtStore() );
       RpcPadContainer::const_iterator rpcPAD = rdoContainer->begin();
       
       for (; rpcPAD!=rdoContainer->end();++rpcPAD)
       {
          if ( (*rpcPAD)->size() > 0 ) {
             status = this->decodeRpc ( *rpcPAD, collection ); 
             if ( status.isFailure() ) return status;
          }
       }
    } else {
      m_acSvc->setStore( &*evtStore() );
      std::auto_ptr<RpcPadContainer> cdata(evtStore()->retrievePrivateCopy<RpcPadContainer>("RPCPAD"));
      if( !cdata.get() ) {
        ATH_MSG_WARNING( "Could not get real data CSC RDO container \"" << " RPCPAD " << "\""  );
         return StatusCode::SUCCESS;
      }
      RpcPadContainer::const_iterator it_rdoColl  = (cdata.get())->begin();
      RpcPadContainer::const_iterator lastRdoColl = (cdata.get())->end();
      for (; it_rdoColl!=lastRdoColl; ++it_rdoColl)
      {
         const RpcPad * rdoColl = *it_rdoColl;
         StatusCode status = this->decodeRpc( rdoColl, collection );
         if ( status.isFailure() ) return status;
      }
    } 

    // record the container in storeGate
    std::string key = "RPC_DIGITS";
    m_acSvc->setStore( &*evtStore() );
    ATH_CHECK( evtStore()->record(m_rpcContainer,key) );

    return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::decodeTgcRDO() {

  ATH_MSG_DEBUG( "Decoding TGC RDO into TGC Digit"  );

  if(!m_tgcCabling && getTgcCabling().isFailure()) return StatusCode::FAILURE;
  
  // clean up the digit container
  m_tgcContainer->cleanup();
  
  Identifier oldElementId;

  // retrieve the collection of RDO
  if ( !m_retrievePrivateCopy ) {
     m_acSvc->setStore( &*evtStore() );
     const TgcRdoContainer* rdoContainer;
     StatusCode status = evtStore()->retrieve( rdoContainer, "TGCRDO" );
     if (status.isFailure())
     {
        ATH_MSG_WARNING( "No TGC RDO container in StoreGate!"  );
        return StatusCode::SUCCESS;
     }  
     ATH_MSG_DEBUG( "Retrieved " << rdoContainer->size() << " TGC RDOs." );

     m_acSvc->setStore( &*evtStore() );
     TgcRdoContainer::const_iterator tgcRDO = rdoContainer->begin();
       
     for (; tgcRDO!=rdoContainer->end();++tgcRDO)
     {
         if ( (*tgcRDO)->size() > 0 ) {
             status = this->decodeTgc ( *tgcRDO, oldElementId ); 
             if ( status.isFailure() ) return status;
         }
     }
     
  } else {
     m_acSvc->setStore( &*evtStore() );
     std::auto_ptr<TgcRdoContainer> cdata(evtStore()->retrievePrivateCopy<TgcRdoContainer>("TGCRDO"));
     if( !cdata.get() ) {
        ATH_MSG_WARNING( "Could not get real data TGC RDO container \"" << " TGCRDO " << "\""  );
         return StatusCode::SUCCESS;
     }
     TgcRdoContainer::const_iterator it_rdoColl  = (cdata.get())->begin();
     TgcRdoContainer::const_iterator lastRdoColl = (cdata.get())->end();
     for (; it_rdoColl!=lastRdoColl; ++it_rdoColl)
     {
        const TgcRdo * rdoColl = *it_rdoColl;
        StatusCode status = this->decodeTgc( rdoColl, oldElementId );
        if ( status.isFailure() ) return status;
     }
  } 
  // record the container in storeGate
  std::string key = "TGC_DIGITS";
  m_acSvc->setStore( &*evtStore() );
  ATH_CHECK( evtStore()->record(m_tgcContainer,key) );

  return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::decodeMdt( const MdtCsm * rdoColl, MdtDigitCollection*& collection, Identifier& oldId ) {

    IdContext mdtContext = m_mdtHelper->module_context();
 
    if ( rdoColl->size() > 0 ) {
        ATH_MSG_DEBUG( " Number of AmtHit in this Csm " 
                       << rdoColl->size()  );
	
	uint16_t subdetId = rdoColl->SubDetId();
	uint16_t mrodId = rdoColl->MrodId();
	uint16_t csmId = rdoColl->CsmId();
	
	// for each Csm, loop over AmtHit, converter AmtHit to digit
	// retrieve/create digit collection, and insert digit into collection
	MdtCsm::const_iterator itD   = rdoColl->begin(); 
	MdtCsm::const_iterator itD_e = rdoColl->end();
	
	for (; itD!=itD_e; ++itD)
	  { 
	    const MdtAmtHit * amtHit = (*itD);
	    MdtDigit * newDigit = m_mdtRdoDecoderTool->getDigit(amtHit,subdetId,
                                                                mrodId,csmId);
	    
	    if (newDigit==NULL) {
	      ATH_MSG_WARNING( "Error in MDT RDO decoder"  );
              continue;
	    }
	
	    // find here the Proper Digit Collection identifier, using the rdo-hit id
	    // (since RDO collections are not in a 1-to-1 relation with digit collections)
	    Identifier elementId = m_mdtHelper->elementID(newDigit->identify());
	    IdentifierHash coll_hash;
	    if (m_mdtHelper->get_hash(elementId, coll_hash, &mdtContext)) {
	      ATH_MSG_WARNING( "Unable to get MDT digit collection hash id " 
                               << "context begin_index = " << mdtContext.begin_index()
                               << " context end_index  = " << mdtContext.end_index()
                               << " the identifier is " );
	      elementId.show();
	    } 

     
	    if (oldId != elementId) {
              m_acSvc->setStore( &*evtStore() );
              MdtDigitContainer::const_iterator it_coll = m_mdtContainer->indexFind(coll_hash);
              if (m_mdtContainer->end() ==  it_coll) {
		MdtDigitCollection * newCollection = 
		  new MdtDigitCollection(elementId,coll_hash);
		newCollection->push_back(newDigit);
		collection = newCollection;
                m_acSvc->setStore( &*evtStore() );
		StatusCode status = m_mdtContainer->addCollection(newCollection, coll_hash);
		if (status.isFailure())
		  ATH_MSG_WARNING( "Couldn't record MdtDigitCollection with key=" << coll_hash 
                                   << " in StoreGate!"  );
	      } else {  
		MdtDigitCollection * oldCollection = const_cast<MdtDigitCollection*>( *it_coll );
		oldCollection->push_back(newDigit);
		collection = oldCollection;
	      }
	      oldId = elementId;
	    } else {
	      collection->push_back(newDigit);
	    }
	  }
    }
    return StatusCode::SUCCESS;
}  

StatusCode MuonRdoToMuonDigitTool::decodeCsc( const CscRawDataCollection * rdoColl, CscDigitCollection*& collection, 
                                              Identifier& oldId){// , CscRDO_Decoder& decoder) {

      ATH_MSG_DEBUG( " Number of RawData in this rdo " 
                     << rdoColl->size()  );
      
      double samplingTime = rdoColl->rate();
      unsigned int numSamples   = rdoColl->numSamples();

      //      decoder.setParams(samplingTime);

      IdContext cscContext = m_cscHelper->module_context();

      /** for each Rdo, loop over RawData, converter RawData to digit
	  retrieve/create digit collection, and insert digit into collection */
      CscRawDataCollection::const_iterator itD   = rdoColl->begin(); 
      CscRawDataCollection::const_iterator itD_e = rdoColl->end();
      for (; itD!=itD_e; ++itD) { 
	const CscRawData * data = (*itD);
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
	  if ( !adctocharge || errorfindCharge ) { 
	    ATH_MSG_WARNING( " CSC conversion ADC to Charge failed " 
                             << "CSC Digit not build ... " 
                             << " or charge finding failed " << " ... skipping "  );
	    continue;
	  }
	  ATH_MSG_DEBUG( "CSC RDO->CscDigit: " << m_cscHelper->show_to_string(channelId) );
	  int theCharge = static_cast<int>(charge);
	  CscDigit * newDigit = new CscDigit(channelId, theCharge, time);
	  ATH_MSG_DEBUG( "CSC RDO->Digit: " << m_cscHelper->show_to_string(newDigit->identify()) 
                         << " " << newDigit->charge() << " " << charge << " time= " << time  );

	  for (uint16_t i=0; i< samples.size(); ++i) {
	    ATH_MSG_DEBUG( "CSC RDO->Digit: " << samples[i]  );
	  }
          IdentifierHash coll_hash;
          if (m_cscHelper->get_hash(stationId, coll_hash, &cscContext)) {
            ATH_MSG_WARNING( "Unable to get CSC digiti collection hash id " 
                             << "context begin_index = " << cscContext.begin_index()
                             << " context end_index  = " << cscContext.end_index()
                             << " the identifier is " );
             stationId.show();
          } 

	  if (oldId != stationId) {
            m_acSvc->setStore( &*evtStore() );
            CscDigitContainer::const_iterator it_coll = m_cscContainer->indexFind(coll_hash);
            if (m_cscContainer->end() ==  it_coll) {
		CscDigitCollection * newCollection = new CscDigitCollection(stationId, coll_hash);
		newCollection->push_back(newDigit);
		collection = newCollection;
                m_acSvc->setStore( &*evtStore() );
		StatusCode status = m_cscContainer->addCollection(newCollection, coll_hash);
		if (status.isFailure())
		  ATH_MSG_WARNING( "Couldn't record CscDigitCollection with key=" << coll_hash 
                                   << " in StoreGate!"  );
	    } else {  
	      CscDigitCollection * oldCollection = const_cast<CscDigitCollection*>( *it_coll );
	      oldCollection->push_back(newDigit);
	      collection = oldCollection;
	    }
	    oldId = stationId;
	  } else {
	    collection->push_back(newDigit);
	  }
	}
      }
      return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::decodeRpc( const RpcPad * rdoColl, RpcDigitCollection*& collection ) {

            IdContext rpcContext = m_rpcHelper->module_context();

            ATH_MSG_DEBUG( " Number of CMs in this Pad " 
                           << rdoColl->size()  );
	
            Identifier padOfflineId = rdoColl->identify();
            //Get pad online id and sector id
            uint16_t padId     = rdoColl->onlineId(); 
            uint16_t sectorId  = rdoColl->sector(); 

            int stationName = m_rpcHelper->stationName(padOfflineId);
            int stationEta  = m_rpcHelper->stationEta(padOfflineId);
            int stationPhi  = m_rpcHelper->stationPhi(padOfflineId);
            int doubletR    = m_rpcHelper->doubletR(padOfflineId);
	
            Identifier elementId = m_rpcHelper->elementID(stationName, stationEta,
                                                          stationPhi, doubletR);
	
            // For each pad, loop on the coincidence matrices
            RpcPad::const_iterator itCM   = rdoColl->begin();
            RpcPad::const_iterator itCM_e = rdoColl->end();
	
            for (; itCM != itCM_e ; ++itCM) {
	  
                // Get CM online Id
                uint16_t cmaId = (*itCM)->onlineId();

                // For each CM, loop on the fired channels
                RpcCoinMatrix::const_iterator itD   = (*itCM)->begin();
                RpcCoinMatrix::const_iterator itD_e = (*itCM)->end();
	  
                for (; itD != itD_e ; ++itD) {

                    const RpcFiredChannel * rpcChan = (*itD);
                    std::vector<RpcDigit*>* digitVec = 
                        m_rpcRdoDecoderTool->getDigit(rpcChan, sectorId, padId, cmaId);
	    
                    if (digitVec==NULL) {
                        ATH_MSG_FATAL( "Error in the RPC RDO decoder "  );
                        return StatusCode::FAILURE;
                    }

                    // Loop on the digits corresponding to the fired channel
                    std::vector<RpcDigit*>::const_iterator itVec   = digitVec->begin();
                    std::vector<RpcDigit*>::const_iterator itVec_e = digitVec->end();
		
                    for ( ; itVec!=itVec_e; ++itVec) 
                    {
                        collection = 0;
                        RpcDigit* newDigit = (*itVec);
                        elementId = m_rpcHelper->elementID(newDigit->identify());
                        
                        IdentifierHash coll_hash;
                        if (m_rpcHelper->get_hash(elementId, coll_hash, &rpcContext)) {
                          ATH_MSG_WARNING( "Unable to get RPC digit collection hash id " 
                                           << "context begin_index = " << rpcContext.begin_index()
                                           << " context end_index  = " << rpcContext.end_index()
                                           << " the identifier is " );
                                  elementId.show();
                        } 

                        m_acSvc->setStore( &*evtStore() );
                        RpcDigitContainer::const_iterator it_coll = m_rpcContainer->indexFind(coll_hash);
                        if (m_rpcContainer->end() ==  it_coll) {
                            RpcDigitCollection * newCollection = new RpcDigitCollection(elementId, coll_hash);
                            newCollection->push_back(newDigit);
                            collection = newCollection;
                            m_acSvc->setStore( &*evtStore() );
                            StatusCode status = m_rpcContainer->addCollection(newCollection, coll_hash);
                            if (status.isFailure())
                              ATH_MSG_WARNING( "Couldn't record RpcDigitCollection with key=" 
                                               << coll_hash << " in StoreGate!"  );
                        }
                        else
                        {
	                   RpcDigitCollection * oldCollection = const_cast<RpcDigitCollection*>( *it_coll );
	                   oldCollection->push_back(newDigit);
                           collection = oldCollection;
                        }
                    }
                    delete digitVec; 
                }
            }
    
            return StatusCode::SUCCESS;
}

StatusCode MuonRdoToMuonDigitTool::decodeTgc( const TgcRdo *rdoColl, 
                                              Identifier& oldElementId ) { 
      TgcDigitCollection* collection = 0; 

      if(!m_tgcCabling && getTgcCabling().isFailure()) return StatusCode::FAILURE;

      IdContext tgcContext = m_tgcHelper->module_context();

      ATH_MSG_DEBUG( "Number of RawData in this rdo " 
                     << rdoColl->size()  );
      // for each Rdo, loop over RawData, converter RawData to digit
      // retrieve/create digit collection, and insert digit into collection
      TgcRdo::const_iterator itD  = rdoColl->begin();
      TgcRdo::const_iterator itDe = rdoColl->end();

      std::map<std::vector<uint16_t>, uint16_t> stripMap;
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
	  
	  std::vector<uint16_t> stripId(5, 0);
	  stripId.at(0) = (*itD)->subDetectorId();
	  stripId.at(1) = (*itD)->rodId();
	  stripId.at(2) = (*itD)->sswId();
	  stripId.at(3) = (*itD)->bitpos();
	  stripId.at(4) = (*itD)->bcTag();
	  uint16_t stripSlbBits = 0x1 << ((*itD)->slbId()-16);
	  std::map<std::vector<uint16_t>, uint16_t>::iterator itMap = stripMap.find(stripId); // Find correspond channel 
	  if(itMap==stripMap.end()) { // This is new one
	    stripMap.insert(std::map<std::vector<uint16_t>, uint16_t>::value_type(stripId, stripSlbBits));
	  } else { // This already exists
	    itMap->second |= stripSlbBits;
	  }
	}
	// Covert to original hit patterns 
	std::map<std::vector<uint16_t>, uint16_t>::iterator jtMap   = stripMap.begin();
	std::map<std::vector<uint16_t>, uint16_t>::iterator jtMap_e = stripMap.end();
	for(; jtMap!=jtMap_e; jtMap++) {
	  if(jtMap->second<=31) {
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
	    jtMap->second = originalHitBits[jtMap->second]; 
	  } else {
	    jtMap->second = 0;
	  }
	}
	// TGC2 Endcap Strip OR channel treatement preparation end
	itD = rdoColl->begin();
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
	      std::vector<uint16_t> stripId(5, 0);
	      stripId.at(0) = (*itD)->subDetectorId();
	      stripId.at(1) = (*itD)->rodId();
	      stripId.at(2) = (*itD)->sswId();
	      stripId.at(3) = (*itD)->bitpos();
	      stripId.at(4) = (*itD)->bcTag();
	      std::map<std::vector<uint16_t>, uint16_t>::iterator itMap = stripMap.find(stripId); // Find correspond hit
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
              if (m_tgcHelper->get_hash(elementId, coll_hash, &tgcContext)) {
                ATH_MSG_WARNING( "Unable to get TGC digit collection hash " 
                                 << "context begin_index = " << tgcContext.begin_index()
                                 << " context end_index  = " << tgcContext.end_index()
                                 << " the identifier is " );
                 elementId.show();
              } 

	      if (elementId != oldElementId)
		{
		  // get collection
                  m_acSvc->setStore( &*evtStore() );
                  TgcDigitContainer::const_iterator it_coll = m_tgcContainer->indexFind(coll_hash);
                  if (m_tgcContainer->end() !=  it_coll) {
                       collection = const_cast<TgcDigitCollection*>( *it_coll );
		    }
		  else
		    {
		      // create new collection
		      ATH_MSG_DEBUG( "Created a new digit collection : " << coll_hash  );
		      collection = new TgcDigitCollection(elementId, coll_hash);
                      m_acSvc->setStore( &*evtStore() );
		      ATH_CHECK( m_tgcContainer->addCollection(collection,coll_hash) );
		    }
		  
		  oldElementId = elementId;
		}

	      if(!collection) {
		ATH_MSG_WARNING( "MuonRdoToMuonDigitTool::decodeTgc TgcDigitCollection* collection is null."  );
		return StatusCode::SUCCESS;
	      }
	      
	      // check duplicate digits
	      TgcDigitCollection::const_iterator it_tgcDigit;
	      bool duplicate = false;
	      for (it_tgcDigit=collection->begin(); it_tgcDigit != collection->end(); it_tgcDigit++) {
		if ((newDigit->identify() == (*it_tgcDigit)->identify()) && 
		    (newDigit->bcTag()    == (*it_tgcDigit)->bcTag())) {
		  duplicate = true;
		  ATH_MSG_DEBUG( "Duplicate TGC Digit removed"   );
		  break;
		}
	      }
	      if (!duplicate) {
		// add the digit to the collection
		collection->push_back(newDigit.release());
	      }
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
