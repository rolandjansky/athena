/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CscOverlay/CscOverlay.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"
#include "MuonIdHelpers/CscIdHelper.h"

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGauss.h"

#include <iostream>
#include <typeinfo>

const uint16_t MAX_AMPL = 4095; // 12-bit ADC

//================================================================
CscOverlay::CscOverlay(const std::string &name, ISvcLocator *pSvcLocator) :
  MuonOverlayBase(name, pSvcLocator),
  m_storeGateTemp("StoreGateSvc/BkgEvent_1_SG", name),
  m_cscHelper(0),
  m_cscCalibTool( "CscCalibTool", this),
  m_digTool("CscDigitizationTool", this ),
  m_rdoTool2("CscDigitToCscRDOTool2", this ),
  m_rdoTool4("CscDigitToCscRDOTool4", this ),
  m_cscRdoDecoderTool ("Muon::CscRDO_Decoder"),
  m_rndmSvc("AtRndmGenSvc", name ),
  m_rndmEngine(0),
  m_rndmEngineName("CscOverlay")
{

  /** Event DAta Store keys for the 2 data streams to overlay
      - modifiable in job options */
  declareProperty("TempStore", m_storeGateTemp, "help");
  declareProperty("mainInputCSC_Name", m_mainInputCSC_Name="CSCRDO");
  declareProperty("overlayInputCSC_Name", m_overlayInputCSC_Name="CSCRDO");
  declareProperty("DigitizationTool", m_digTool);
  declareProperty("MakeRDOTool2", m_rdoTool2);
  declareProperty("MakeRDOTool4", m_rdoTool4);
  declareProperty("CscRdoDecoderTool",   m_cscRdoDecoderTool );
  declareProperty("IsByteStream", m_isByteStream = false ); 
  declareProperty("RndmSvc", 	     m_rndmSvc, "Random Number Service used for CscDigitToCscRDOTool" );
  declareProperty("RndmEngine",      m_rndmEngineName, "Random engine name for CscDigitToCscRDOTool");

}

//================================================================
StatusCode CscOverlay::overlayInitialize()
{
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO <<"CscOverlay initialized"<<endmsg;

  if (m_storeGateTemp.retrieve().isFailure()) {
    msg<< MSG::FATAL << "SaveInDetObjects::initialize): StoreGate[temp] service not found !" << endmsg;
     return StatusCode::FAILURE;
  }

  /** initialize the detectore store service */
  StoreGateSvc* detStore=0;
  StatusCode sc = serviceLocator()->service("DetectorStore", detStore);
  if (sc.isFailure()) {
    msg<< MSG::FATAL << "DetectorStore service not found !" << endmsg;
    return StatusCode::FAILURE;
  }

  /** access to the CSC Identifier helper */
  sc = detStore->retrieve(m_cscHelper, "CSCIDHELPER");
  if (sc.isFailure()) {
    msg<< MSG::FATAL << "Could not get CscIdHelper !" << endmsg;
    return StatusCode::FAILURE;
  } 
  else {
    msg<<MSG::DEBUG<<" Found the CscIdHelper. "<<endmsg;
  }

  /** CSC calibratin tool for the Condtiions Data base access */
  sc = m_cscCalibTool.retrieve();
  if (sc.isFailure()) {
      msg<<MSG::ERROR<< "Can't get handle on CSC calibration tools" << endmsg;
      return StatusCode::FAILURE;
  }

  // get cscRdoDecoderTool
  if (m_cscRdoDecoderTool.retrieve().isFailure()){
    ATH_MSG_ERROR ( "Can't get handle on CSC RDO Decoder tool" );
    return StatusCode::FAILURE;
  } 
 
  if (m_digTool.retrieve().isFailure()) {
    msg<< MSG::FATAL << "Could not retrieve CSC Digitization Tool!" << endmsg;
    return StatusCode::FAILURE;
  }
  msg<<MSG::DEBUG<<"Retrieved CSC Digitization Tool."<<endmsg;
  
  if (m_rdoTool2.retrieve().isFailure()) {
    msg<< MSG::FATAL << "Could not retrieve CSC Digit -> RDO Tool 2!" << endmsg;
    return StatusCode::FAILURE;
  }
  msg<<MSG::DEBUG<<"Retrieved CSC Digit -> RDO Tool 2."<<endmsg;
  
  if (m_rdoTool4.retrieve().isFailure()) {
    msg<< MSG::FATAL << "Could not retrieve CSC Digit -> RDO Tool 4!" << endmsg;
    return StatusCode::FAILURE;
  }
  msg<<MSG::DEBUG<<"Retrieved CSC Digit -> RDO Tool 4."<<endmsg;

  //random number initialization
  if (!m_rndmSvc.retrieve().isSuccess()) {
     msg<<MSG::ERROR<< " Could not initialize Random Number Service" << endmsg;
  }      
    
  // getting our random numbers stream
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if (m_rndmEngine==0) {
    msg<<MSG::ERROR<< "Could not find RndmEngine : " << m_rndmEngineName << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//================================================================
StatusCode CscOverlay::overlayFinalize() 
{
  MsgStream msg(msgSvc(), name());
  msg <<MSG::INFO << "CscOverlay finalized" << endmsg;
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode CscOverlay::overlayExecute() {
  MsgStream msg(msgSvc(), name());
  msg<<MSG::DEBUG<<"CscOverlay::execute() begin"<<endmsg;

  //----------------------------------------------------------------
  unsigned int numsamples=0;//to be determined from the data
  std::auto_ptr<CscRawDataContainer> cdata(0);
  const CscRawDataContainer* data_input_CSC(0);
  if ( !m_isByteStream ) {
     cdata = m_storeGateData->retrievePrivateCopy<CscRawDataContainer>(m_mainInputCSC_Name);
     if( !cdata.get() ) {
       msg << MSG::WARNING << "Could not get real data CSC RDO container \"" << m_mainInputCSC_Name << "\"" << endmsg;
       return StatusCode::SUCCESS;
     }
     if ((cdata->begin()==cdata->end()) || !(cdata->begin()->cptr())){
       msg << MSG::WARNING << "Could not get nsamples, cdata empty?"<< endmsg;
       //return StatusCode::SUCCESS;
     }
     else {numsamples=cdata->begin()->cptr()->numSamples();}
  } else {
    if(! (m_storeGateData->retrieve(data_input_CSC, m_mainInputCSC_Name).isSuccess()) ) {
      msg << MSG::WARNING << "Could not get real data CSC RDO container \"" << m_mainInputCSC_Name << "\"" << endmsg;
      return StatusCode::SUCCESS;
    }
    if ((data_input_CSC->begin()==data_input_CSC->end()) || !(data_input_CSC->begin()->cptr())){
      msg << MSG::WARNING << "Could not get nsamples, data_input_CSC empty?"<< endmsg;
      //return StatusCode::SUCCESS;
    }
    else {numsamples=data_input_CSC->begin()->cptr()->numSamples();}
  }

  /** in the simulation stream, run digitization of the fly
      and make RDO - this will be used as input to the overlay job */
  if ( m_digTool->digitize().isFailure() ) {
     msg << MSG::WARNING << "On the fly CSC digitization failed " << endmsg;
     return StatusCode::SUCCESS;
  }
  
  if (numsamples==2){
    if ( m_rdoTool2->digitize().isFailure() ) {
      msg << MSG::WARNING << "On the fly CSC Digit -> RDO 2 failed " << endmsg;
      return StatusCode::SUCCESS;
    }
    msg << MSG::DEBUG << "Digitizing with 2 samples" << endmsg;
  }
  else if (numsamples==4){
    if ( m_rdoTool4->digitize().isFailure() ) {
      msg << MSG::WARNING << "On the fly CSC Digit -> RDO 4 failed " << endmsg;
      return StatusCode::SUCCESS;
    }
    msg << MSG::DEBUG << "Digitizing with 4 samples" << endmsg;
  }
  else{
    msg << MSG::WARNING << "On the fly CSC Digit -> RDO failed - not 2 or 4 samples!" << endmsg;
    //return StatusCode::SUCCESS;
  }
  
  if (numsamples>0){
    msg << MSG::DEBUG << "Retrieving MC input CSC container" << endmsg;
    const CscRawDataContainer* ovl_input_CSC(0);
    if(! (m_storeGateMC->retrieve(ovl_input_CSC, m_overlayInputCSC_Name).isSuccess()) ) {
      msg << MSG::WARNING << "Could not get CSC RDO from the simulation stream ... " << m_overlayInputCSC_Name << endmsg;
      return StatusCode::SUCCESS;
    }

    /* now do the overlay - reading real data from the data stream
       and reading simulated RDO produced in the previous steps 
       from the simulation stream */
    if ( !m_isByteStream ) this->overlayContainer(cdata.get(), ovl_input_CSC);
    else this->overlayContainer(const_cast<CscRawDataContainer*>(data_input_CSC), ovl_input_CSC); 
  }

  /* record the overlay data to the output stream */
  if ( !m_isByteStream ) {
     if ( m_storeGateOutput->record(cdata, m_mainInputCSC_Name).isFailure() ) {
        msg << MSG::WARNING << "Failed to record CSC overlay container to output store " << endmsg;
     } 
  } else {
     if ( m_storeGateOutput->record(data_input_CSC, m_mainInputCSC_Name).isFailure() ) {
        msg << MSG::WARNING << "Failed to record CSC overlay container to output store " << endmsg;
     }
  }

  //----------------
  // This kludge is a work around for problems created by another kludge:
  // Digitization algs keep a pointer to their output Identifiable Container and reuse
  // the same object over and other again.   So unlike any "normal" per-event object
  // this IDC is not a disposable one, and we should not delete it.
  cdata.release();

  //----------------------------------------------------------------
  msg<<MSG::DEBUG<<"CscOverlay::execute() end"<<endmsg;
  return StatusCode::SUCCESS;
}

//================================================================
void CscOverlay::overlayContainer(CscRawDataContainer *main,
                                  const CscRawDataContainer *overlay)
{
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "overlayContainer<>() begin" << endmsg;

  /** Add data from the ovl container to the main one */
  CscRawDataContainer::const_iterator p_ovl = overlay->begin(); 
  CscRawDataContainer::const_iterator p_ovl_end = overlay->end();

  for(; p_ovl != p_ovl_end; ) {

    /** retrieve the ovl collection by calling cptr() */
    const CscRawDataCollection *coll_ovl = p_ovl->cptr();

    uint16_t coll_id = (*p_ovl)->identify();

    /** The newly created stuff will go to the data EventStore SG */
    CscRawDataCollection *out_coll = new CscRawDataCollection( coll_id );

    /** Look for the same ID in the main StoreGate EventStore */ 
    CscRawDataContainer::const_iterator q = main->indexFind( coll_id );

    if( q != main->end() ) {
      /** Need to merge the collections
          Retrieve q */

      const CscRawDataCollection *coll_data = q->cptr();

      /** copy a few things to the new collection */
      out_coll->set_eventType( coll_data->eventType() );
      out_coll->setRodId ( coll_data->rodId() );
      out_coll->setSubDetectorId ( coll_data->subDetectorId() );
      if ( coll_data->samplingPhase() ) out_coll->set_samplingPhase();
      if ( coll_data->triggerType() ) out_coll->set_triggerType();
      out_coll->set_firstBitSummary( coll_data->firstBitSummary() );
      out_coll->set_scaAddress( coll_data->scaAddress() );
      const std::vector<uint8_t> dataType = coll_data->dataType();
      for ( unsigned int i=0; i<dataType.size(); ++i ) out_coll->addDataType( dataType[i] );

      this->mergeCollections(out_coll, coll_data, coll_ovl);

      /** Here the new collection is created, but not yet registered. 
	  Put it in IDC in place of the original collection.
       */

      main->removeCollection(p_ovl.hashId());

      if ( main->addCollection(out_coll, p_ovl.hashId()).isFailure() ) {
	msg << MSG::WARNING << "addCollection failed " << endmsg; 
      }
      else {
	msg << MSG::DEBUG << "overlayContainer() added overlaid RDO" << endmsg;
      }

    }
    else {
      /** Copy the complete collection from ovl to data, 
          hopefully preserving the "most derived" type of its raw data */ 

      out_coll->set_eventType( coll_ovl->eventType() );
      out_coll->setRodId ( coll_ovl->rodId() );
      out_coll->setSubDetectorId ( coll_ovl->subDetectorId() );
      if ( coll_ovl->samplingPhase() ) out_coll->set_samplingPhase();
      if ( coll_ovl->triggerType() ) out_coll->set_triggerType();
      out_coll->set_firstBitSummary( coll_ovl->firstBitSummary() );
      out_coll->set_scaAddress( coll_ovl->scaAddress() );
      const std::vector<uint8_t> dataType = coll_ovl->dataType();
      for ( unsigned int i=0; i<dataType.size(); ++i ) out_coll->addDataType( dataType[i] );

      /** Copy the complete collection from ovl to data, 
          hopefully preserving the "most derived" type of its raw data */
      unsigned int numSamples = coll_ovl->numSamples();
      for(CscRawDataCollection::const_iterator i=coll_ovl->begin(); i!=coll_ovl->end(); i++) {

	/** Put Digit into Collection */
	const CscRawData *data = ( *i );
	if( !data ) {
	  msg << MSG::WARNING << "NULL pointer to Digit!" << endmsg;
	  continue;
	}	
	else{
	  /** The new RDO goes to m_storeGateData - no need to mess with the pedestal, but add in noise */
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
		msg << MSG::WARNING  << "Unable to extract samples for strip " << j 
                                     << " Online Cluster width = " << width 
                                     << " for number of Samples = " << numSamples 
                                     << " continuing ..." << endmsg; 
              } else {
                for (unsigned int k=0; k<samples.size(); ++k) {
		  double theNoise = CLHEP::RandGauss::shoot(m_rndmEngine, 0.0, noise);
		  float adcCount = samples[k] + theNoise;
		  if ( adcCount > MAX_AMPL ) {
		    msg<<MSG::DEBUG<< "value out of range (copying over signal): " << adcCount << " " 
		       << " Setting it to max value = " << MAX_AMPL
		       << " IdentifierHash is " << stripHash <<endmsg;
		    adcCount = MAX_AMPL;
		  } 
		  all_samples.push_back( (uint16_t) rint(adcCount) );
                }
              }
          }  
          CscRawData *rdo = new CscRawData( all_samples, data->address(), data->identify(), data->rpuID(), data->width() );
          rdo->setHashID( data->hashId() );//ACH - was "width()" ???
	  rdo->setTime( data->time() );//ACH - was absent

	  //perform some checks
	  bool good=true;
	  for (unsigned int j=0; j<width; ++j) {
	    const Identifier channelId = m_cscRdoDecoderTool->channelIdentifier(rdo, j);
	    if(!(m_cscHelper->valid(channelId))) {
	      msg << MSG::WARNING << "Invalid CSC Identifier! - skipping " << channelId << endmsg;
	      good=false;
	    }
	  }
	  if (good){
	    out_coll->push_back( rdo );
	  }
	  else{
	    delete rdo;//coverity demands it
	  }

	}
      }
          
      /** The new collection goes to m_storeGateData */

      if(main->addCollection(out_coll, out_coll->identify()).isFailure()) {
	msg << MSG::WARNING << "overlayContainer(): Problem in main->addCollection(Identifier)" << endmsg;
      }
      else {
	msg << MSG::DEBUG << "overlayContainer() added new RDO" << endmsg;
      }
    }

    ++p_ovl;
  }
  
  msg << MSG::DEBUG << "overlayContainer<>() end" << endmsg;
}

void CscOverlay::spuData( const CscRawDataCollection * coll, const uint16_t spuID, std::vector<const CscRawData*>& data) {
  MsgStream msg(msgSvc(), name());
  data.clear();  if ( !coll ) return; 
  CscRawDataCollection::const_iterator idata = coll->begin();
  CscRawDataCollection::const_iterator edata = coll->end();
  for ( ; idata != edata; ++idata ) {
      if ( (*idata)->rpuID() == spuID ) data.push_back( *idata );
  }
  msg <<MSG::DEBUG<<"spuData(): made data vector of size "<<data.size()<<" for SPU "<<spuID<<endmsg;
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

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "mergeCollection<>() begin" << endmsg;

  // number of ADC samples in  the both data stream
  unsigned int nSigSamples  = data_coll->numSamples();
  unsigned int nOvlSamples  = ovl_coll->numSamples();

  // sampling times in both data streams 
  unsigned int dataSamplingTime = data_coll->rate();
  unsigned int ovlSamplingTime  = ovl_coll->rate();

  if ( dataSamplingTime != ovlSamplingTime ) {
     msg<< MSG::WARNING  << "Overlay of inconsistent data - sampling times not the same "
                         << dataSamplingTime << " ns " << ovlSamplingTime << " ns" << endmsg;
     return;
  }

  if ( nSigSamples != nOvlSamples ) {
     msg<< MSG::WARNING  << "Overlay of inconsistent data - number of samples not the same "
                         << nSigSamples << " " << nOvlSamples << endmsg;
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
       	 msg <<MSG::DEBUG<<"Looking for overlap of hashes and addresses within witdths because needtoflip"<<endmsg;
	 msg <<MSG::VERBOSE ;
	 std::set<int> sig;  int lastindex=-1;
	 for (std::map< int,std::vector<uint16_t> >::const_iterator si=sigSamples.begin(); si!=sigSamples.end(); ++si) {
	   if (si!=sigSamples.begin() && si->first-lastindex!=1) break;
	   lastindex=si->first;
	   sig.insert(si->first); msg << si->first << " ";
	 }
	 msg<<endmsg;
	 bool overlap=false;
	 msg <<MSG::VERBOSE ;
	 for (std::map< int,std::vector<uint16_t> >::const_iterator so=ovlSamples.begin(); so!=ovlSamples.end(); ++so) {
	   //add 1 to beginning and end of list because adjacent counts as overlap
	   msg << (so->first)-1 << " ";
	   if (sig.find((so->first)-1)!=sig.end()) {overlap=true; msg << "!!";}
	   msg << (so->first) << " ";
	   if (sig.find((so->first))!=sig.end()) {overlap=true; msg << "!!";}
	   msg << (so->first)+1 << " ";
	   if (sig.find((so->first)+1)!=sig.end()) {overlap=true; msg << "!!";}
	 }
	 msg<<endmsg;
	 if (!overlap){
	   msg <<MSG::DEBUG<<"Taking max of hashes and addresses because needtoflip and no overlap"<<endmsg;
	   hash      = std::max( sigHash, ovlHash );
	   address   = std::max( sigAddress, ovlAddress );
	 }
       }

       //for checks
       std::set<int> insertedstrips, readstrips;
       for (std::map< int,std::vector<uint16_t> >::const_iterator s=sigSamples.begin(); s!=sigSamples.end(); ++s){readstrips.insert(s->first);}
       for (std::map< int,std::vector<uint16_t> >::const_iterator si=ovlSamples.begin(); si!=ovlSamples.end(); ++si){readstrips.insert(si->first);}
       
       std::vector<CscRawData*> datums = this->overlay(sigSamples, ovlSamples,address, spuID, out_coll->identify(), hash );
       if ( datums.size()==0 ) { 	 msg<<MSG::WARNING<<"datums is size 0!"<<endmsg;       }
       for (unsigned int di=0; di<datums.size(); ++di){
	 CscRawData* datum=datums[di];
	 hash = datum->hashId();
	 address = datum->address();
	 int stripstart        = (  address & 0x000000FF) + 1 + 0;
	 msg <<MSG::DEBUG<< "Datum in layer="<<j<<" has hash="<<hash<<" address="<<address<<" stripstart="<<stripstart<<", "<< *datum <<endmsg;
	 if (datum->width()==0) {
	   msg<<MSG::WARNING<< "Datum has 0 width!" <<endmsg;
	   continue;
	 }

	  //perform some checks
	  int stationName =  ( ( address & 0x00010000) >> 16 ) + 50;
	  int stationEta  =  ( ((address & 0x00001000) >> 12 ) == 0x0) ? -1 : 1;
	  int stationPhi  =  ( ( address & 0x0000E000) >> 13 ) + 1;
	  Identifier me= m_cscHelper->elementID(stationName,stationEta,stationPhi);
	  msg<<MSG::VERBOSE<<"stationName,Eta,Phi="<<stationName<<","<<stationEta<<","<<stationPhi<<" - me="<<me<<endmsg;
	  bool good=true;
	  for (unsigned int j=0; j<datum->width(); ++j) {
	    int chamberLayer = ( (address & 0x00000800) >> 11) + 0;
	    std::string det=m_cscCalibTool->getDetDescr();
	    if ( det.find ("ATLAS-") != std::string::npos )
	      chamberLayer = ( (address & 0x00000800) >> 11) + 1;
 	    int wireLayer    = ( (address & 0x00000600) >>  9) + 1;
	    int measuresPhi  = ( (address & 0x00000100) >>  8);
	    int strip        = (  address & 0x000000FF) + 1 + j; 
	    msg<<MSG::VERBOSE<<"det,chamberlayer,wirelayer,measuresphi,strip="<<det<<","<<chamberLayer<<","<<wireLayer<<","<<measuresPhi<<","<<strip<<endmsg;
	    // Added to Online -> Offline id  in A side number is opposite bug#56002
	    if (measuresPhi) {
	      int stationEta  =  ( ((address & 0x00001000) >> 12 ) == 0x0) ? -1 : 1;
	      if (stationEta>0) {
		strip = 49-strip;
		msg<<MSG::VERBOSE<<"FLIP strip, now strip="<<strip<<endmsg;
	      }
	    }
	    insertedstrips.insert(strip);//for checks
	    Identifier mechan= m_cscHelper->channelID(me,chamberLayer,wireLayer,measuresPhi,strip);
	    msg<<MSG::VERBOSE<<"mechan="<<mechan<<endmsg;
	    const Identifier channelId = m_cscRdoDecoderTool->channelIdentifier(datum, j);
	    if(!(m_cscHelper->valid(channelId))) {
	      msg << MSG::WARNING << "Invalid CSC Identifier in merge! - skipping " << channelId << endmsg;
	      good=false;
	    }
	    else{msg << MSG::DEBUG << "Valid CSC Identifier in merge " << channelId << endmsg;}
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
	 msg << MSG::WARNING << "Readstrips != Insertedstrips: "<<endmsg;
	 for (std::set<int>::const_iterator i = readstrips.begin(); i!=readstrips.end(); ++i){std::cout<<*i<<" ";} std::cout<<std::endl;
	 for (std::set<int>::const_iterator i = insertedstrips.begin(); i!=insertedstrips.end(); ++i){std::cout<<*i<<" ";} std::cout<<std::endl;
       }

    } 
  } 
  for (unsigned int i=0; i<10; ++i) out_coll->set_spuCount(i,clusterCounts[i]);
  for (unsigned int i=0; i<2; ++i)  { if (rpuCount[i] != 0) out_coll->addRPU(rpuCount[i]); }
  // FIXME --- need to be able to reset the dataType - should add a new method to CscRawDataCollection for this
  msg<<MSG::DEBUG<<"mergeCollection<>() end "<<endmsg;
}
 
uint32_t CscOverlay::stripData ( const std::vector<const CscRawData*>& data,
                                 const unsigned int numSamples, 
                                 std::map< int,std::vector<uint16_t> >& samples,
                                 uint32_t& hash,
                                 const uint16_t spuID,
                                 const int gasLayer, bool isdata)
{
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "stripData<>() begin: gasLayer="<<gasLayer<<" spuID="<<spuID<<" isdata="<<isdata<<endmsg;

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
      //msg << MSG::DEBUG<<"Not precision or non_precision, skipping layer="<<layer<<", gasLayer="<<gasLayer<<", spuID="<<spuID<<endmsg;
      continue;
    }

    //ACH - move down here after layer continue...
    unsigned int newaddress = datum->address();
    //if we're going to later flip the data strip for bug#56002
    if (isdata && needtoflip(newaddress)) {
      msg << MSG::VERBOSE << "needtoflip in stripdata, newaddress was = "<<newaddress<<", strip was = "<<strip<<endmsg;

      //old way
      //newaddress= newaddress- (width-1);//apparently need to shift the address to the highest strip

      //new way
      uint32_t oldFirstStrip  = uint32_t (newaddress & 0x000000FF);
      uint32_t newFirstStrip  = uint32_t (47-oldFirstStrip) - width +1;//starts at 0
      newaddress=newaddress - oldFirstStrip + newFirstStrip;

      uint32_t oldStrip  = uint32_t (strip & 0x000000FF);
      uint32_t newStrip  = uint32_t (49-oldStrip);//starts at 1
      strip=strip - oldStrip + newStrip;
      
      msg << MSG::VERBOSE << "needtoflip in stripdata, newaddress now = "<<newaddress<<", strip now = "<<strip<<endmsg;
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

    msg << MSG::DEBUG << "stripData(): width="<<width<<" hashOffset="<<hashOffset<<" datumaddress="<<datum->address()<<" layer="<<layer<<" strip="<<strip<<", hash="<<hash<<" address="<<address<<endmsg;

    for (unsigned int j=0; j<width; ++j) {
       std::vector<uint16_t> adcs;
       bool extractSamples = datum->samples(j, numSamples, adcs);
       if ( !extractSamples ) {
	 msg << MSG::WARNING  << "Unable to extract samples for strip " << j 
                               << " Online Cluster width = " << width << " for number of Samples = " << numSamples << endmsg; 
       } 
       else {   
	 int newstrip = (strip+j);
	 if (false && isdata && needtoflip(address)){
	   newstrip = strip-j;
	   msg << MSG::VERBOSE << "needtoflip in stripdata, newstrip is "<<newstrip<<endmsg;
	 }
	 samples.insert ( std::make_pair( newstrip, adcs) );    
       }
    }
  }

  msg << MSG::DEBUG << "stripData<>() end: hash=" << hash << " address=" << address << endmsg;
  return address;
}

std::vector<CscRawData*> CscOverlay::overlay( const std::map< int,std::vector<uint16_t> >& sigSamples,
                                  const std::map< int,std::vector<uint16_t> >& ovlSamples,
                                  const uint32_t address, 
                                  const uint16_t spuID, 
                                  const uint16_t collId,
                                  const uint32_t hash )
{
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "overlay<>() begin: hash="<<hash<<" address="<<address<<endmsg;
  std::vector<CscRawData*> datas;
  CscRawData * rawData = 0;
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
      msg << MSG::VERBOSE << "data only for i="<<i<<endmsg;
      for ( unsigned int j=0; j<(*sig).second.size(); ++j ) {
	samples.push_back( (*sig).second.at(j) );
	assert((*sig).second.at(j)<=MAX_AMPL);
      }
      width++; used=true;
    }
    else if ( sig == sigSamples.end() && ovl != ovlSamples.end() ) { // simulation only
      msg << MSG::VERBOSE << "simulation only for i="<<i<<endmsg;
      int myhashw=myhash+width; if (needtoflip(myaddress)) {myhashw=myhash-width;}
      double noise = m_cscCalibTool->stripNoise( (myhashw), false );//in ADC counts
       for ( unsigned int j=0; j<(*ovl).second.size(); ++j ) {
	  double theNoise = CLHEP::RandGauss::shoot(m_rndmEngine, 0.0, noise);
          float adcCount = (*ovl).second.at(j) + theNoise ;//add noise
          if ( adcCount > MAX_AMPL ) {
	    msg << MSG::DEBUG << "value out of range (adding noise): " << adcCount << " "
		<< " Setting it to max value = " << MAX_AMPL
		<< " IdentifierHash is " << (myhashw) <<endmsg;
	    adcCount = MAX_AMPL;
          }
          samples.push_back( (uint16_t) rint(adcCount) );
       }
       width++; used=true;
    }
    else if ( sig != sigSamples.end() && ovl != ovlSamples.end() ) { // real data + MC
      msg << MSG::VERBOSE << "data and simulation for i="<<i<<endmsg;
      int myhashw=myhash+width; if (needtoflip(myaddress)) {myhashw=myhash-width;}
      double pedestal = m_cscCalibTool->stripPedestal( (myhashw), false );//in ADC counts
       for ( unsigned int j=0; j<(*sig).second.size(); ++j ) {
          float adcCount = (*sig).second.at(j) + (*ovl).second.at(j) - pedestal ;//subtract pedestal only (data already has noise)
          if ( adcCount > MAX_AMPL ) {
	    msg << MSG::DEBUG << "value out of range (adding data+MC samples - pedestal): " << adcCount << " "
		<< " Setting it to max value = " << MAX_AMPL
		<< " IdentifierHash is " << (myhashw) <<endmsg;
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
      msg << MSG::DEBUG << "overlay<>() add datum: hash="<<myhash<<" address="<<myaddress<<" width="<<width<<endmsg;
      samples.clear();
      if (!needtoflip(myaddress)) {myhash+=width; myaddress+=width;}
      width=0;
    }
    
  }
  msg << MSG::DEBUG << "overlay<>() end: CscRawDatas size="<<datas.size()<<endmsg;
  return datas;
}
