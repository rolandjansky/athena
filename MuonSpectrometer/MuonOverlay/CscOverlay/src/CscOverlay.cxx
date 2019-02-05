/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CscOverlay/CscOverlay.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "CxxUtils/make_unique.h"

#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"
#include "MuonIdHelpers/CscIdHelper.h"

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGauss.h"

#include <iostream>
#include <typeinfo>

constexpr uint16_t MAX_AMPL = 4095; // 12-bit ADC

//================================================================
CscOverlay::CscOverlay(const std::string &name, ISvcLocator *pSvcLocator) :
  MuonOverlayBase(name, pSvcLocator),
  m_cscHelper(nullptr),
  m_cscCalibTool( "CscCalibTool", this),
  m_digTool("CscDigitizationTool", this ),
  m_rdoTool2("CscDigitToCscRDOTool2", this ),
  m_rdoTool4("CscDigitToCscRDOTool4", this ),
  m_cscRdoDecoderTool ("Muon::CscRDO_Decoder"),
  m_rndmSvc("AtRndmGenSvc", name ),
  m_rndmEngine(nullptr),
  m_rndmEngineName("CscOverlay")
{

  /** Event DAta Store keys for the 2 data streams to overlay
      - modifiable in job options */
  declareProperty("DigitizationTool", m_digTool);
  declareProperty("MakeRDOTool2", m_rdoTool2);
  declareProperty("MakeRDOTool4", m_rdoTool4);
  declareProperty("CscRdoDecoderTool",   m_cscRdoDecoderTool );
  declareProperty("RndmSvc", 	     m_rndmSvc, "Random Number Service used for CscDigitToCscRDOTool" );
  declareProperty("RndmEngine",      m_rndmEngineName, "Random engine name for CscDigitToCscRDOTool");

}

//================================================================
StatusCode CscOverlay::overlayInitialize()
{
  ATH_MSG_INFO("CscOverlay initialized");

  /** access to the CSC Identifier helper */
  ATH_CHECK(detStore()->retrieve(m_cscHelper, "CSCIDHELPER"));
  ATH_MSG_DEBUG(" Found the CscIdHelper. ");
  
  /** CSC calibratin tool for the Condtiions Data base access */
  ATH_CHECK(m_cscCalibTool.retrieve());  

  // get cscRdoDecoderTool
  ATH_CHECK(m_cscRdoDecoderTool.retrieve());

  ATH_CHECK(m_digTool.retrieve());
  ATH_MSG_DEBUG("Retrieved CSC Digitization Tool.");

  ATH_CHECK(m_rdoTool2.retrieve());
  ATH_MSG_DEBUG("Retrieved CSC Digit -> RDO Tool 2.");

  ATH_CHECK(m_rdoTool4.retrieve());
  ATH_MSG_DEBUG("Retrieved CSC Digit -> RDO Tool 4.");

  //random number initialization
  ATH_CHECK(m_rndmSvc.retrieve());
    
  // getting our random numbers stream
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if (!m_rndmEngine) {
    ATH_MSG_ERROR("Could not find RndmEngine : " << m_rndmEngineName);
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_inputDataRDOKey.initialize() );
  ATH_CHECK( m_inputOverlayRDOKey.initialize() );
  ATH_CHECK( m_outputContainerKey.initialize() );

  return StatusCode::SUCCESS;
}

//================================================================
StatusCode CscOverlay::overlayFinalize() 
{
  ATH_MSG_INFO("CscOverlay finalized");  
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode CscOverlay::overlayExecute() {
  ATH_MSG_DEBUG("CscOverlay::execute() begin");
  //----------------------------------------------------------------
  unsigned int numsamples=0;//to be determined from the data
  SG::ReadHandle<CscRawDataContainer> inputDataRDO(m_inputDataRDOKey);           
  if(!inputDataRDO.isValid()) {
    ATH_MSG_WARNING("Could not get data CscRawDataContainer  \"" << inputDataRDO.name() << "\" in " << inputDataRDO.store());
    return StatusCode::SUCCESS;
  }
  ATH_MSG_VERBOSE("Found CscRawDataContainer \"" << inputDataRDO.name() << "\" in " << inputDataRDO.store());
  if ((inputDataRDO->begin()==inputDataRDO->end()) || !*(inputDataRDO->begin())){
	ATH_MSG_WARNING("Could not get nsamples, inputDataRDO empty?");
  }
  else{
    numsamples=inputDataRDO->begin()->numSamples();
  }

  /** in the simulation stream, run digitization of the fly
      and make RDO - this will be used as input to the overlay job */
  if ( m_digTool->digitize().isFailure() ) {
     ATH_MSG_WARNING("On the fly CSC digitization failed ");
     return StatusCode::SUCCESS;
  }
  
  if (numsamples==2) {
    if ( m_rdoTool2->digitize().isFailure() ) {
      ATH_MSG_WARNING("On the fly CSC Digit -> RDO 2 failed ");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Digitizing with 2 samples");
  }
  else if (numsamples==4) {
    if ( m_rdoTool4->digitize().isFailure() ) {
      ATH_MSG_WARNING("On the fly CSC Digit -> RDO 4 failed ");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Digitizing with 4 samples");
  }
  else{
    ATH_MSG_WARNING("On the fly CSC Digit -> RDO failed - not 2 or 4 samples!");
    //return StatusCode::SUCCESS;
  }
  
  if (numsamples>0) {
    ATH_MSG_DEBUG("Retrieving MC input CSC container");
    SG::ReadHandle<CscRawDataContainer> inputOverlayRDO(m_inputOverlayRDOKey);
    if(!inputOverlayRDO.isValid()) {
      ATH_MSG_WARNING("Could not get overlay CscRawDataContainer \"" << inputOverlayRDO.name() << "\" in " << inputOverlayRDO.store());
      return StatusCode::SUCCESS;
    }
    ATH_MSG_VERBOSE("Found CscRawOverlayContainer \"" << inputOverlayRDO.name() << "\" in " << inputOverlayRDO.store());
    
    /* now do the overlay - reading real data from the data stream
       and reading simulated RDO produced in the previous steps 
       from the simulation stream */
    this->overlayContainer(inputDataRDO.cptr(), inputOverlayRDO.cptr());
  }
  
  //----------------------------------------------------------------
  ATH_MSG_DEBUG("CscOverlay::execute() end");
  return StatusCode::SUCCESS;
}

//================================================================
void CscOverlay::overlayContainer(const CscRawDataContainer *main,
                                  const CscRawDataContainer *overlay)
{
  ATH_MSG_DEBUG("overlayContainer<>() begin");

  SG::WriteHandle<CscRawDataContainer> outputContainer(m_outputContainerKey);
  if (outputContainer.record(std::make_unique<CscRawDataContainer>()).isFailure()) {
    ATH_MSG_ERROR("Failed to record " << m_outputContainerKey);
  }

  /** Add data from the main container to the output one */
  CscRawDataContainer::const_iterator p_main = main->begin(); 
  CscRawDataContainer::const_iterator p_main_end = main->end();
  for(; p_main != p_main_end; ) {
    const CscRawDataCollection& mainColl(**p_main);
    //Deep-copy CscRawDataCollection
    //FIXME Write a copy-constructor for CscRawDataCollection!!
    auto p_newColl = std::make_unique<CscRawDataCollection>(mainColl.identify());
    this->copyCscRawDataCollectionProperties(mainColl, *p_newColl);
    for(auto rawData : **p_main) {
      auto newData = std::make_unique<CscRawData>(*rawData); // TODO confirm implicit copy-constructor works as expected
      p_newColl->push_back(newData.release());
    }
    if ( outputContainer->addCollection(p_newColl.release(), p_main.hashId()).isFailure() ) {
      ATH_MSG_WARNING("addCollection failed for main"); 
    }
    else {
      ATH_MSG_DEBUG("data overlayContainer() added overlaid RDO");
    }
    ++p_main;
  }
  
  /** Add data from the ovl container to the output one */
  CscRawDataContainer::const_iterator p_ovl = overlay->begin(); 
  CscRawDataContainer::const_iterator p_ovl_end = overlay->end();

  for(; p_ovl != p_ovl_end; ) {

    const CscRawDataCollection *coll_ovl = *p_ovl;
    const uint16_t coll_id = (*p_ovl)->identify();

    /** The newly created stuff will go to the output EventStore SG */
    auto out_coll = std::make_unique<CscRawDataCollection>( coll_id );

    /** Look for the same ID in the main StoreGate EventStore */ 
    CscRawDataContainer::const_iterator q = outputContainer->indexFind(coll_id);

    if( q != outputContainer->end() ) {
      /** Need to merge the collections
          Retrieve q */

      const CscRawDataCollection *coll_data = *q;
      this->copyCscRawDataCollectionProperties(*coll_data, *out_coll);
      this->mergeCollections(out_coll.get(), coll_data, coll_ovl);

      /** Here the new collection is created, but not yet registered. 
	  Put it in IDC in place of the original collection.
       */

      outputContainer->removeCollection(p_ovl.hashId());
      if(outputContainer->addCollection(out_coll.release(), p_ovl.hashId()).isFailure()) {      
	ATH_MSG_WARNING("addCollection failed ");
      }
      else {
	ATH_MSG_DEBUG("overlayContainer() added overlaid RDO");
      }
    }
    else {
      /** Copy the complete collection from ovl to output, 
          hopefully preserving the "most derived" type of its raw data */ 
      this->copyCscRawDataCollectionProperties(*coll_ovl, *out_coll);

      /** Copy the complete collection from ovl to output, 
          hopefully preserving the "most derived" type of its raw data */
      unsigned int numSamples = coll_ovl->numSamples();
      for(CscRawDataCollection::const_iterator i=coll_ovl->begin(); i!=coll_ovl->end(); i++) {

	/** Put Digit into Collection */
	const CscRawData *data = ( *i );
	if( !data ) {
	  ATH_MSG_WARNING("NULL pointer to Digit!");
	  continue;
	}	
	else{
	  /** The new RDO goes to m_storeGateOutput - no need to mess with the pedestal, but add in noise */
          uint16_t width = data->width();
          uint32_t hashOffset = data->hashId();
          std::vector<uint16_t> all_samples; 
          for (unsigned int j=0; j<width; ++j) {
	    uint32_t stripHash = hashOffset+j;
	    double noise    = m_cscCalibTool->stripNoise( stripHash, false );
	    //double pedestal = m_cscCalibTool->stripPedestal( stripHash, false );
	    std::vector<uint16_t> samples;
	    bool extractSamples = data->samples(j, numSamples, samples);
	    if (!extractSamples) {
	      ATH_MSG_WARNING("Unable to extract samples for strip " << j 
			      << " Online Cluster width = " << width 
			      << " for number of Samples = " << numSamples 
			      << " continuing ..."); 
	    } 
	    else {
	      for (unsigned int k=0; k<samples.size(); ++k) {
		double theNoise = CLHEP::RandGauss::shoot(m_rndmEngine, 0.0, noise);
		float adcCount = samples[k] + theNoise;
		if ( adcCount > MAX_AMPL ) {
		  ATH_MSG_DEBUG("value out of range (copying over signal): " << adcCount << " " 
				<< " Setting it to max value = " << MAX_AMPL
				<< " IdentifierHash is " << stripHash);
		  adcCount = MAX_AMPL;
		} 
		all_samples.push_back( (uint16_t) rint(adcCount) );
	      }
	    }
          }  
	  auto rdo = std::make_unique<CscRawData>( all_samples, data->address(), data->identify(), data->rpuID(), data->width() );
          rdo->setHashID( data->hashId() );//ACH - was "width()" ???
	  rdo->setTime( data->time() );//ACH - was absent

	  //perform some checks
	  bool good=true;
	  for (unsigned int j=0; j<width; ++j) {
	    const Identifier channelId = m_cscRdoDecoderTool->channelIdentifier(rdo.get(), j);
	    if(!(m_cscHelper->valid(channelId))) {
	      ATH_MSG_WARNING("Invalid CSC Identifier! - skipping " << channelId);
	      good=false;
	    }
	  }
	  if (good){
	    out_coll->push_back( rdo.release() );
	  }
	}
      }
      
      /** The new collection goes to m_storeGateOutput */
      if(outputContainer->addCollection(out_coll.release(), out_coll->identify()).isFailure()) {
	ATH_MSG_WARNING("overlayContainer(): Problem in outputContainer->addCollection(Identifier)");
      }
      else {
	ATH_MSG_DEBUG("overlayContainer() added new RDO");
      }
    }
    
    ++p_ovl;
  }
  
  ATH_MSG_DEBUG("overlayContainer<>() end");
}
 
// Copying CscRawDataCollection properties
 void CscOverlay::copyCscRawDataCollectionProperties(const CscRawDataCollection& sourceColl, CscRawDataCollection& outColl) const {
   /** copy a few things to the new collection */
   outColl.setIdentifyHash( sourceColl.identifyHash() );
   outColl.set_eventType( sourceColl.eventType() );
   outColl.setRodId ( sourceColl.rodId() );
   outColl.setSubDetectorId ( sourceColl.subDetectorId() );
   if (sourceColl.samplingPhase()) { outColl.set_samplingPhase(); }
   if (sourceColl.triggerType()) { outColl.set_triggerType(); }
   outColl.set_firstBitSummary( sourceColl.firstBitSummary() );
   outColl.set_scaAddress( sourceColl.scaAddress() );
   for (auto dataType : sourceColl.dataType()) { outColl.addDataType( dataType ); }
 }

void CscOverlay::spuData( const CscRawDataCollection * coll, const uint16_t spuID, std::vector<const CscRawData*>& data) {
  data.clear();  if ( !coll ) return; 
  CscRawDataCollection::const_iterator idata = coll->begin();
  CscRawDataCollection::const_iterator edata = coll->end();
  for ( ; idata != edata; ++idata ) {
      if ( (*idata)->rpuID() == spuID ) data.push_back( *idata );
  }
  ATH_MSG_DEBUG("spuData(): made data vector of size "<<data.size()<<" for SPU "<<spuID);
}

bool CscOverlay::needtoflip(const int address) const {
	 int measuresPhi  = ( (address & 0x00000100) >>  8);
	 if (address<2147483640 && measuresPhi) {
	   int stationEta  =  ( ((address & 0x00001000) >> 12 ) == 0x0) ? -1 : 1;
	   if (stationEta>0) {	     return true;	   }
	 }
	 return false;
}

//
//
//
//
//================================================================
void CscOverlay::mergeCollections(CscRawDataCollection *out_coll,
                                  const CscRawDataCollection *data_coll,
                                  const CscRawDataCollection *ovl_coll)
{
  ATH_MSG_DEBUG("mergeCollection<>() begin");

  // number of ADC samples in  the both data stream
  unsigned int nSigSamples  = data_coll->numSamples();
  unsigned int nOvlSamples  = ovl_coll->numSamples();

  // sampling times in both data streams 
  unsigned int dataSamplingTime = data_coll->rate();
  unsigned int ovlSamplingTime  = ovl_coll->rate();

  if ( dataSamplingTime != ovlSamplingTime ) {
    ATH_MSG_WARNING("Overlay of inconsistent data - sampling times not the same "
		    << dataSamplingTime << " ns " << ovlSamplingTime << " ns");
     return;
  }

  if ( nSigSamples != nOvlSamples ) {
    ATH_MSG_WARNING("Overlay of inconsistent data - number of samples not the same "
		    << nSigSamples << " " << nOvlSamples);
     return;
  }

  /** loop over the SPU - collecting thr data by layer
      do the overlay by igas layer in a chamber */ 
  uint16_t clusterCounts[] = {0,0,0,0,0,0,0,0,0,0};
  uint16_t rpuCount[] = {0,0};
  for ( uint16_t spuID=0; spuID<10; ++spuID) {

    std::vector<const CscRawData*> sigData; // real data
    this->spuData(data_coll, spuID, sigData);

    std::vector<const CscRawData*> ovlData; // simulation
    this->spuData(ovl_coll, spuID, ovlData);

    /** for the non-precision strips, all the 4 layers in a chamber
        are in the same SPU, we need to recover the data by chamber layer */ 
    int layer = 0;
    if ( spuID == 4 || spuID == 9 ) layer=4;
    for ( int j=0; j<=layer; ++j ) {
       std::map< int,std::vector<uint16_t> > sigSamples;
       std::map< int,std::vector<uint16_t> > ovlSamples;
       uint32_t sigHash;
       uint32_t ovlHash;
       uint32_t sigAddress = this->stripData( sigData, nSigSamples, sigSamples, sigHash, spuID, j , true); // real data
       uint32_t ovlAddress = this->stripData( ovlData, nOvlSamples, ovlSamples, ovlHash, spuID, j , false); // simulation
       if (sigSamples.size()==0 && ovlSamples.size()==0) continue;
 
       uint32_t hash      = std::min( sigHash, ovlHash );
       uint32_t address   = std::min( sigAddress, ovlAddress );
       if (sigSamples.size()!=0 && ovlSamples.size()!=0 && needtoflip(address)){
       	 ATH_MSG_DEBUG("Looking for overlap of hashes and addresses within witdths because needtoflip");
	 msg() << MSG::VERBOSE ;
	 std::set<int> sig;  int lastindex=-1;
	 for (std::map< int,std::vector<uint16_t> >::const_iterator si=sigSamples.begin(); si!=sigSamples.end(); ++si) {
	   if (si!=sigSamples.begin() && si->first-lastindex!=1) break;
	   lastindex=si->first;
	   sig.insert(si->first); msg() << si->first << " ";
	 }
	 msg()<<endmsg;
	 bool overlap=false;
	 msg() <<MSG::VERBOSE ;
	 for (std::map< int,std::vector<uint16_t> >::const_iterator so=ovlSamples.begin(); so!=ovlSamples.end(); ++so) {
	   //add 1 to beginning and end of list because adjacent counts as overlap
	   msg() << (so->first)-1 << " ";
	   if (sig.find((so->first)-1)!=sig.end()) {overlap=true; msg() << "!!";}
	   msg() << (so->first) << " ";
	   if (sig.find((so->first))!=sig.end()) {overlap=true; msg() << "!!";}
	   msg() << (so->first)+1 << " ";
	   if (sig.find((so->first)+1)!=sig.end()) {overlap=true; msg() << "!!";}
	 }
	 msg()<<endmsg;
	 if (!overlap){
	   ATH_MSG_DEBUG("Taking max of hashes and addresses because needtoflip and no overlap");
	   hash      = std::max( sigHash, ovlHash );
	   address   = std::max( sigAddress, ovlAddress );
	 }
       }

       //for checks
       std::set<int> insertedstrips, readstrips;
       for (std::map< int,std::vector<uint16_t> >::const_iterator s=sigSamples.begin(); s!=sigSamples.end(); ++s){readstrips.insert(s->first);}
       for (std::map< int,std::vector<uint16_t> >::const_iterator si=ovlSamples.begin(); si!=ovlSamples.end(); ++si){readstrips.insert(si->first);}
       
       std::vector<CscRawData*> datums = this->overlay(sigSamples, ovlSamples,address, spuID, out_coll->identify(), hash );
       if ( datums.size()==0 ) { 	 ATH_MSG_WARNING("datums is size 0!");       }
       for (unsigned int di=0; di<datums.size(); ++di){
	 CscRawData* datum=datums[di];
	 hash = datum->hashId();
	 address = datum->address();
	 int stripstart        = (  address & 0x000000FF) + 1 + 0;
	 ATH_MSG_DEBUG("Datum in layer="<<j<<" has hash="<<hash<<" address="<<address<<" stripstart="<<stripstart<<", "<< *datum );
	 if (datum->width()==0) {
	   ATH_MSG_WARNING("Datum has 0 width!");
	   continue;
	 }

	  //perform some checks
	  int stationName =  ( ( address & 0x00010000) >> 16 ) + 50;
	  int stationEta  =  ( ((address & 0x00001000) >> 12 ) == 0x0) ? -1 : 1;
	  int stationPhi  =  ( ( address & 0x0000E000) >> 13 ) + 1;
	  Identifier me= m_cscHelper->elementID(stationName,stationEta,stationPhi);
	  ATH_MSG_VERBOSE("stationName,Eta,Phi="<<stationName<<","<<stationEta<<","<<stationPhi<<" - me="<<me);
	  bool good=true;
	  for (unsigned int j=0; j<datum->width(); ++j) {
	    int chamberLayer = ( (address & 0x00000800) >> 11) + 0;
	    std::string det=m_cscCalibTool->getDetDescr();
	    if ( det.find ("ATLAS-") != std::string::npos )
	      chamberLayer = ( (address & 0x00000800) >> 11) + 1;
 	    int wireLayer    = ( (address & 0x00000600) >>  9) + 1;
	    int measuresPhi  = ( (address & 0x00000100) >>  8);
	    int strip        = (  address & 0x000000FF) + 1 + j; 
	    ATH_MSG_VERBOSE("det,chamberlayer,wirelayer,measuresphi,strip="<<det<<","<<chamberLayer<<","<<wireLayer<<","<<measuresPhi<<","<<strip);
	    // Added to Online -> Offline id  in A side number is opposite bug#56002
	    if (measuresPhi) {
	      int stationEta  =  ( ((address & 0x00001000) >> 12 ) == 0x0) ? -1 : 1;
	      if (stationEta>0) {
		strip = 49-strip;
		ATH_MSG_VERBOSE("FLIP strip, now strip="<<strip);
	      }
	    }
	    insertedstrips.insert(strip);//for checks
	    Identifier mechan= m_cscHelper->channelID(me,chamberLayer,wireLayer,measuresPhi,strip);
	    ATH_MSG_VERBOSE("mechan="<<mechan);
	    const Identifier channelId = m_cscRdoDecoderTool->channelIdentifier(datum, j);
	    if(!(m_cscHelper->valid(channelId))) {
	      ATH_MSG_WARNING("Invalid CSC Identifier in merge! - skipping " << channelId );
	      good=false;
	    }
	  else{ATH_MSG_DEBUG("Valid CSC Identifier in merge " << channelId);}
	  }
	  if (good){	    out_coll->push_back(datum);	  }
	  else{	    continue;	  }
	  
	  //keep count
          clusterCounts[spuID] += 1;
          if ( spuID <= 4 ) rpuCount[0] = 5;
          else if ( spuID > 4 && spuID <= 9 ) rpuCount[1] = 11;
       }//loop over datum
       
       //check
       if (readstrips!=insertedstrips){
	 ATH_MSG_WARNING("Readstrips != Insertedstrips: ");
	 for (std::set<int>::const_iterator i = readstrips.begin(); i!=readstrips.end(); ++i){std::cout<<*i<<" ";} std::cout<<std::endl;
	 for (std::set<int>::const_iterator i = insertedstrips.begin(); i!=insertedstrips.end(); ++i){std::cout<<*i<<" ";} std::cout<<std::endl;
       }

    } 
  } 
  for (unsigned int i=0; i<10; ++i) out_coll->set_spuCount(i,clusterCounts[i]);
  for (unsigned int i=0; i<2; ++i)  { if (rpuCount[i] != 0) out_coll->addRPU(rpuCount[i]); }
  // FIXME --- need to be able to reset the dataType - should add a new method to CscRawDataCollection for this
  ATH_MSG_DEBUG("mergeCollection<>() end ");
}
 
uint32_t CscOverlay::stripData ( const std::vector<const CscRawData*>& data,
                                 const unsigned int numSamples, 
                                 std::map< int,std::vector<uint16_t> >& samples,
                                 uint32_t& hash,
                                 const uint16_t spuID,
                                 const int gasLayer, bool isdata)
{
  ATH_MSG_DEBUG("stripData<>() begin: gasLayer="<<gasLayer<<" spuID="<<spuID<<" isdata="<<isdata);

  samples.clear();
  IdContext context = m_cscHelper->channel_context();

  uint32_t maxInt  = 2147483640;
  uint32_t address = maxInt;
  hash             = maxInt;

  //int max = 192;
  //if ( spuID == 4 || spuID == 9 ) max = 48;

  /** loop over the data in the SPU */
  std::vector<const CscRawData*>::const_iterator idata = data.begin();
  std::vector<const CscRawData*>::const_iterator edata = data.end();
  for ( ; idata != edata; ++idata ) {
    const CscRawData * datum = *idata;
    uint32_t hashOffset = datum->hashId();

    /** find the strip Identifier given the strip hash ID */
    Identifier stripId;
    m_cscHelper->get_id(hashOffset, stripId, &context);
    unsigned int strip = static_cast<unsigned int> ( m_cscHelper->strip( stripId ) );
    int layer          = m_cscHelper->wireLayer( stripId );
    uint16_t width = datum->width();

    /** create the map only layer by layer 
        for the precision strip, we set gasLayer=0 because the spuID tells you the gas layer 
        for the non-precision strips, we need to explicitly get the gas layer number form the Identifier */ 
    bool non_precision = (gasLayer==layer) && (spuID==4 || spuID==9);
    bool precision     = (gasLayer==0) && (!(spuID==4 || spuID==9));
    bool check = precision || non_precision; 
    if ( !check ) {
      //ATH_MSG_DEBUG("Not precision or non_precision, skipping layer="<<layer<<", gasLayer="<<gasLayer<<", spuID="<<spuID);
      continue;
    }

    //ACH - move down here after layer continue...
    unsigned int newaddress = datum->address();
    //if we're going to later flip the data strip for bug#56002
    if (isdata && needtoflip(newaddress)) {
      ATH_MSG_VERBOSE("needtoflip in stripdata, newaddress was = "<<newaddress<<", strip was = "<<strip);

      //old way
      //newaddress= newaddress- (width-1);//apparently need to shift the address to the highest strip

      //new way
      uint32_t oldFirstStrip  = uint32_t (newaddress & 0x000000FF);
      uint32_t newFirstStrip  = uint32_t (47-oldFirstStrip) - width +1;//starts at 0
      newaddress=newaddress - oldFirstStrip + newFirstStrip;

      uint32_t oldStrip  = uint32_t (strip & 0x000000FF);
      uint32_t newStrip  = uint32_t (49-oldStrip);//starts at 1
      strip=strip - oldStrip + newStrip;
      
      ATH_MSG_VERBOSE("needtoflip in stripdata, newaddress now = "<<newaddress<<", strip now = "<<strip);
    }

    if ( needtoflip(newaddress) ){
      if (hash == maxInt) hash=0;
      if (address == maxInt) address=0;
      if ( hashOffset > hash ) hash = hashOffset;
      if ( newaddress > address ) address = newaddress;
    }
    else{
      if ( hashOffset < hash ) hash = hashOffset;
      if ( newaddress < address ) address = newaddress;
    }

    ATH_MSG_DEBUG("stripData(): width="<<width<<" hashOffset="<<hashOffset<<" datumaddress="<<datum->address()<<" layer="<<layer<<" strip="<<strip<<", hash="<<hash<<" address="<<address);

    for (unsigned int j=0; j<width; ++j) {
       std::vector<uint16_t> adcs;
       bool extractSamples = datum->samples(j, numSamples, adcs);
       if ( !extractSamples ) {
	 ATH_MSG_WARNING("Unable to extract samples for strip " << j 
			 << " Online Cluster width = " << width << " for number of Samples = " << numSamples); 
       } 
       else {   
	 int newstrip = (strip+j);
	 if (false && isdata && needtoflip(address)){
	   newstrip = strip-j;
	   ATH_MSG_VERBOSE("needtoflip in stripdata, newstrip is "<<newstrip);
	 }
	 samples.insert ( std::make_pair( newstrip, adcs) );    
       }
    }
  }

  ATH_MSG_DEBUG("stripData<>() end: hash=" << hash << " address=" << address);
  return address;
}

std::vector<CscRawData*> CscOverlay::overlay( const std::map< int,std::vector<uint16_t> >& sigSamples,
                                  const std::map< int,std::vector<uint16_t> >& ovlSamples,
                                  const uint32_t address, 
                                  const uint16_t spuID, 
                                  const uint16_t collId,
                                  const uint32_t hash )
{
  ATH_MSG_DEBUG("overlay<>() begin: hash="<<hash<<" address="<<address);
  std::vector<CscRawData*> datas;
  CscRawData * rawData = nullptr;
  int max = 192;
  if ( spuID == 4 || spuID == 9 ) max = 48;
  std::vector<uint16_t> samples;
  std::map< int,std::vector<uint16_t> >::const_iterator sig;
  std::map< int,std::vector<uint16_t> >::const_iterator ovl;
  uint16_t width = 0;
  int myhash=hash; int myaddress=address;

  max+=1;//go one past the end, to get the clusters ending right
  for ( int i=1; i<=max; ++i) {
    sig = sigSamples.find(i);
    ovl = ovlSamples.find(i);
    bool used=false;

    if ( sig != sigSamples.end() && ovl == ovlSamples.end() ) { // real data only
      ATH_MSG_VERBOSE("data only for i="<<i);
      for ( unsigned int j=0; j<(*sig).second.size(); ++j ) {
	samples.push_back( (*sig).second.at(j) );
	assert((*sig).second.at(j)<=MAX_AMPL);
      }
      width++; used=true;
    }
    else if ( sig == sigSamples.end() && ovl != ovlSamples.end() ) { // simulation only
      ATH_MSG_VERBOSE("simulation only for i="<<i);
      int myhashw=myhash+width; if (needtoflip(myaddress)) {myhashw=myhash-width;}
      double noise = m_cscCalibTool->stripNoise( (myhashw), false );//in ADC counts
       for ( unsigned int j=0; j<(*ovl).second.size(); ++j ) {
	  double theNoise = CLHEP::RandGauss::shoot(m_rndmEngine, 0.0, noise);
          float adcCount = (*ovl).second.at(j) + theNoise ;//add noise
          if ( adcCount > MAX_AMPL ) {
	    ATH_MSG_DEBUG("value out of range (adding noise): " << adcCount << " "
		<< " Setting it to max value = " << MAX_AMPL
			  << " IdentifierHash is " << (myhashw));
	    adcCount = MAX_AMPL;
          }
          samples.push_back( (uint16_t) rint(adcCount) );
       }
       width++; used=true;
    }
    else if ( sig != sigSamples.end() && ovl != ovlSamples.end() ) { // real data + MC
      ATH_MSG_VERBOSE("data and simulation for i="<<i);
      int myhashw=myhash+width; if (needtoflip(myaddress)) {myhashw=myhash-width;}
      double pedestal = m_cscCalibTool->stripPedestal( (myhashw), false );//in ADC counts
       for ( unsigned int j=0; j<(*sig).second.size(); ++j ) {
          float adcCount = (*sig).second.at(j) + (*ovl).second.at(j) - pedestal ;//subtract pedestal only (data already has noise)
          if ( adcCount > MAX_AMPL ) {
	    ATH_MSG_DEBUG("value out of range (adding data+MC samples - pedestal): " << adcCount << " "
		<< " Setting it to max value = " << MAX_AMPL
			  << " IdentifierHash is " << (myhashw));
	    adcCount = MAX_AMPL;
          }
          samples.push_back( (uint16_t) rint(adcCount) );
       }
       width++; used=true;
    }
    
    if ( used==false && datas.size()>0 ){
      if (needtoflip(myaddress)) {myhash-=1; myaddress-=1;}
      else {myhash+=1; myaddress+=1;}
    }
  
    //If a break is detected in the strip cluster, start a new CscRawData object...
    //and adjust the hash and address, etc.
    if ( (used==false||i==max) && samples.size()>0){
      if (datas.size()>0 && needtoflip(myaddress)) {myhash-=width; myaddress-=width;}
      rawData = new CscRawData( samples, myaddress, collId, spuID, width );
      rawData->setHashID(myhash); 
      rawData->setTime(0);//ACH - TODO: should be made significantly more clever!
      datas.push_back(rawData);
      ATH_MSG_DEBUG("overlay<>() add datum: hash="<<myhash<<" address="<<myaddress<<" width="<<width);
      samples.clear();
      if (!needtoflip(myaddress)) {myhash+=width; myaddress+=width;}
      width=0;
    }
    
  }
  ATH_MSG_DEBUG("overlay<>() end: CscRawDatas size="<<datas.size());
  return datas;
}
