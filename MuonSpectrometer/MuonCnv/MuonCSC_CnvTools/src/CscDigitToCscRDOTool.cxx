/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"

#include "CLHEP/Random/RandomEngine.h"
//#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandFlat.h"

#include "MuonDigitContainer/CscDigitContainer.h"
#include "MuonDigitContainer/CscDigitCollection.h"
#include "MuonDigitContainer/CscDigit.h"

#include "CscRODReadOut.h"

#include "CscDigitToCscRDOTool.h"

#include <math.h>
#include <cassert>

const uint16_t MAX_AMPL = 4095; // 12-bit ADC

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

CscDigitToCscRDOTool::CscDigitToCscRDOTool
(const std::string& type,const std::string& name,const IInterface* pIID)
  : AthAlgTool(type, name, pIID), 
    m_cscCablingSvc("CSCcablingSvc", name),
    m_cscCalibTool("CscCalibTool"),
    m_rndmSvc("AtRndmGenSvc", name ),
    m_rndmEngine(0),
    m_rndmEngineName("CscDigitToCscRDOTool")
 {

  declareInterface<IMuonDigitizationTool>(this);

  declareProperty("NumSamples",   m_numSamples = 4);
  declareProperty("Latency",      m_latency = 0);
  declareProperty("RndmSvc", 	  m_rndmSvc, "Random Number Service used for CscDigitToCscRDOTool" );
  declareProperty("RndmEngine",   m_rndmEngineName, "Random engine name for CscDigitToCscRDOTool");
  declareProperty("addNoise",     m_addNoise =true );
  declareProperty("cscCalibTool", m_cscCalibTool);
 }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode CscDigitToCscRDOTool::initialize()
{

  ATH_MSG_DEBUG ( " in initialize()" );

  if ( m_cscCablingSvc.retrieve().isFailure() )  {
    ATH_MSG_ERROR ( " Cannot get CSC Cabling Service " );
    return StatusCode::FAILURE;
  }
    
  if ( detStore()->retrieve(m_cscHelper, "CSCIDHELPER").isFailure()) {
    ATH_MSG_FATAL ( "Could not get CscIdHelper !" );
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_DEBUG ( " Found the CscIdHelper. " );
  }
  
  
  /** CSC calibration tool for the Condtiions Data base access */
  if ( m_cscCalibTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR ( "Can't get handle on CSC calibration tools" );
    return StatusCode::FAILURE;
  }

  m_startTime   = m_cscCalibTool->getTimeOffset();//   StartTime=46.825,
  ATH_MSG_INFO (" m_startTims is set to be " << m_startTime << "from cscCalibTool->getTimeOffset()" );

  //random number initialization
  if ( m_rndmSvc.retrieve().isFailure() ) {
    ATH_MSG_ERROR ( " Could not initialize Random Number Service" );
  }      
  
  // getting our random numbers stream
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if (m_rndmEngine==0) {
    ATH_MSG_ERROR ( "Could not find RndmEngine : " << m_rndmEngineName );
    return StatusCode::FAILURE;
  }
  

  /** initialization of CSC ROD Decoder */
  m_samplingTime = static_cast<uint16_t>( m_cscCalibTool->getSamplingTime() ); // FIXME
  m_samplingRate = int(1000/m_samplingTime);
  m_signalWidth = m_cscCalibTool->getSignalWidth(); 

  m_numberOfIntegration =
    static_cast<uint16_t>(m_cscCalibTool->getNumberOfIntegration()); //12

  ATH_CHECK( m_rdoContainerKey.initialize() );
  ATH_CHECK( m_digitContainerKey.initialize() );
  
  return StatusCode::SUCCESS;
}
 
StatusCode CscDigitToCscRDOTool::digitize()
{
  ATH_MSG_DEBUG ( "in execute()" );

  if ( fill_CSCdata().isFailure() )
    ATH_MSG_ERROR ( " CscDigitToCscRdo failed to execute " );

  ATH_MSG_DEBUG ( "done execute()" );
  return StatusCode::SUCCESS;
}


StatusCode CscDigitToCscRDOTool::fill_CSCdata()
{
  ATH_MSG_DEBUG ( "fill_CSCdata" );

  // according to cosmic data ~33% is phase =1 peak close to 3rd...
  // but let's start with half and half

  
  CscRODReadOut rodReadOut(m_startTime, m_samplingTime, m_signalWidth, m_numberOfIntegration);
  // initialization but it will be updated per channel later 12/03/2009 WP
  // m_startTime is not related to rodReadOut at all but doesn't matter it's not used....by resetting later...

  rodReadOut.set(m_cscHelper);
  rodReadOut.setChamberBitVaue(1);


  SG::WriteHandle<CscRawDataContainer> rdoContainer(m_rdoContainerKey);
  ATH_CHECK( rdoContainer.record(std::make_unique<CscRawDataContainer>()) );

  /** initialization of data collection map */
  m_cscRdoMap.clear();

  /** type definition for collection iterators */
  typedef CscDigitContainer::const_iterator collection_iterator;
  typedef CscDigitCollection::const_iterator digit_iterator;

  /** Retrieve the digit container */
  SG::ReadHandle<CscDigitContainer> container(m_digitContainerKey);

  /** loop over digit collections from the simulation */
  collection_iterator it_coll   = container->begin();
  collection_iterator it_coll_e = container->end();  

  for ( ; it_coll != it_coll_e; ++it_coll) {
    const CscDigitCollection* cscCollection = *it_coll;
    
    /** current wire layer identifier */
    int oldLayer = 0;
    
    /** the online address of the first RDO */
    uint32_t address = 0x0;
    
    /** consider one cluster per layer: no clusterization done in the simulation */
    uint16_t width = 0x0;
    
    /** the ADC samples */
    std::vector<uint16_t> samples;
    samples.clear();

    /** Hash identifier of the first RDO which has offline convention*/
    IdentifierHash cscRawDataOfflineHashId;

    /** SPU number for this RDO
	there 10 SPU - Sparsifier Processing Units, one for each gas layer, except
	for the non-precision strips where all the layers map to one SPU */
    uint16_t spuID = 0x0;

    /** Iterate on the digits of this collection */
    digit_iterator it_dig   = cscCollection->begin();
    digit_iterator it_dig_e = cscCollection->end();

    /** some counters */
    int oldStrip = -1;
    unsigned int count = 0;
    unsigned int size  = cscCollection->size();

    for ( ; it_dig != it_dig_e; ++it_dig) {

      const CscDigit* cscDigit = *it_dig;

      bool IsNewEDM = (cscDigit->sampleCharges()).size() ? true : false;

      count++;

      // There is no difference between online and offline except wheel A(+Z) Phi strips...
      // Where address is made, onlineId should be used......
      Identifier offlineChannelId = cscDigit->identify();
      IdentifierHash cscOfflineChannelHashId;
      IdContext cscContext = m_cscHelper->channel_context();
      if (!m_cscHelper->get_hash(offlineChannelId, cscOfflineChannelHashId, &cscContext)) {
        ATH_MSG_DEBUG ( "HashId for CscDigit (offline) is " << cscOfflineChannelHashId
                        << " for " << m_cscHelper->show_to_string(offlineChannelId,&cscContext) );
      }

      int currentStrip = m_cscHelper->strip(offlineChannelId);

      /** the RDO collection or create it if it does not exits */
      int currentLayer = m_cscHelper->wireLayer(offlineChannelId);
      int measuresPhi  = m_cscHelper->measuresPhi(offlineChannelId);
      int eta = m_cscHelper->stationEta(offlineChannelId);
      int phi = m_cscHelper->stationPhi(offlineChannelId);
      int stationId = m_cscHelper->stationName(offlineChannelId);
      uint16_t subDetectorId = (eta==-1) ? 0x6A : 0x69;
      uint16_t rodId         = 0xFFFF;
      if(m_cscCablingSvc->nROD()==16){
         if(stationId==0x32) rodId = (0x10 | (phi-1));
         if(stationId==0x33) rodId = (0x18 | (phi-1));
      } else {
         rodId = uint16_t (phi-1);
      }
      CscRawDataCollection * cscRdoCollection = this->cscRdo(subDetectorId,rodId);
      
      if (IsNewEDM) {
        if (cscCollection->samplingPhase()) {
          ATH_MSG_DEBUG("There is OddPhase DigitCollection");
          cscRdoCollection->set_samplingPhase();
        }
      } else {
        double flat = CLHEP::RandFlat::shoot(m_rndmEngine, 0.0,1.0);                 // for other particles
        if (flat < 0.5)
          cscRdoCollection->set_samplingPhase();
      }
      

      /** build the event type for the ROD - all other information is 0 */
      uint8_t calLayer     = 0x0;
      bool sparsified      = true;
      bool neutron         = false;
      uint8_t calAmplitude = 0x0;
      bool enableCal       = false;
      
      uint32_t eventType = (m_numSamples & 0xff)         |
                           ((m_latency & 0xff) << 8)     |
                           ((calLayer & 0x3f) <<16)      | 
                           ((sparsified & 1) << 22)      |
                           ((neutron & 1) << 23)         |
                           ((calAmplitude & 0x3f)<<24)   |
                           ((enableCal & 1) << 30)       |
                           ((m_samplingRate==20?0:1)<<31);

      cscRdoCollection->set_eventType( eventType );

      /** the online identifier of this collection */
      uint16_t collId  = cscRdoCollection->identify(); 
	
      /** find the online address - one per plane 
	  for the non-precision strips, the 4 planes all go into one RPU */
      //if ( oldLayer != currentLayer ) {
	//oldLayer = currentLayer;
      //}

      if ( currentStrip != (oldStrip+1) || currentLayer != oldLayer ) {
        
        if ( currentLayer != oldLayer ) oldLayer = currentLayer;

	/** Create a cscRawData object and save it */
        if ( width > 0 ) {

          int mphi  = ( (address & 0x00000100) >>  8);
          int zEta  =  ( ((address & 0x00001000) >> 12 ) == 0x0) ? -1 : 1;

          if (zEta > 0 && mphi == 1) {

            int istat     = ( (address & 0x00010000) >> 16 ) + 50;
            int phisector = ( (address & 0x0000E000) >> 13 ) + 1;
            int chamLayer = ( (address & 0x00000800) >> 11 ) + 1;
            int wlayer    = ( (address & 0x00000600) >>  9 ) + 1;

            int beforestrip  = (address & 0x000000FF)+1; 
            int afterstrip = beforestrip - width + 1;

            Identifier newOnlineChannelId
              = m_cscHelper->channelID(istat,zEta,phisector,chamLayer,wlayer,mphi,afterstrip);
            address = rodReadOut.address(newOnlineChannelId,zEta,phisector);

          }


          ATH_MSG_DEBUG ( "At Creation of CscRawData, SPU ID = " << spuID );
	  CscRawData* rawData = new CscRawData(samples, address, collId, spuID, width);
          uint32_t hashId = static_cast<uint32_t>(cscRawDataOfflineHashId);
	  rawData->setHashID( hashId );
	  cscRdoCollection->push_back(rawData);
        }
 	
        /** station identifier to calcuate the SPU ID */
        int stationName = m_cscHelper->stationName(offlineChannelId);
        /** there 10 SPU - Sparsifier Processing Units, one for each gas layer, except
            for the non-precision strips where all the layers map to one SPU
            note that the "-50" is because stationName = 50 (CSS) or 51 (CSL) */
        if ( measuresPhi == 0 ) spuID = static_cast<uint16_t>( (stationName-50)*5 + currentLayer-1);
        else spuID = static_cast<uint16_t>( ( (stationName-50)+1)*5 - 1);
        ATH_MSG_DEBUG ( "SPU ID = " << spuID );

        
        // There is no difference between online and offline except wheel A(+Z) Phi strips...
        // Where address is made, onlineId should be used......
        // Let's make online hashId first and then convert it into online identifier....
        Identifier onlineChannelId =offlineChannelId; // onlineChannelId is only needed to get address....
        if (eta > 0 && measuresPhi == 1) {
          int chamberLayer = m_cscHelper->chamberLayer(offlineChannelId); // Either 1 or 2 (but always 1)
          int strip        = 49 -currentStrip;

          onlineChannelId = m_cscHelper->channelID(stationName,eta,phi,chamberLayer,currentLayer,measuresPhi,strip);
        }

        /** The strip online address */  // this registers the first one...
	address = rodReadOut.address(onlineChannelId,eta,phi);

        /** Strip hash identifier is from offline convention for CscRawData first strip...*/
        IdContext cscContext = m_cscHelper->channel_context();
        if (!m_cscHelper->get_hash(offlineChannelId, cscRawDataOfflineHashId, &cscContext)) {
          ATH_MSG_DEBUG ( "HashId off CscRawData (still offline hashId) is " << cscRawDataOfflineHashId
                          << " for " << m_cscHelper->show_to_string(offlineChannelId,&cscContext) );
        }

        /** clear for the next CscRawData */
	width = 0x0;
	samples.clear();
      }
      
      /** simulation data conversion to ADC counts */
      ATH_MSG_DEBUG ( "CSC Digit->RDO: Digit offline info " << m_cscHelper->show_to_string(offlineChannelId)
                      << " " << cscDigit->charge() );	  

      int zsec = m_cscHelper->stationEta(offlineChannelId);
      int phisec = m_cscHelper->stationPhi(offlineChannelId);
      int istation = m_cscHelper->stationName(offlineChannelId) - 49;
      int sector = zsec*(2*phisec-istation+1);
      int wlay = m_cscHelper->wireLayer(offlineChannelId);
      int measphi = m_cscHelper->measuresPhi(offlineChannelId);
      int istrip = m_cscHelper->strip(offlineChannelId);

      // false will return value in ADC counts - true in number of electrons
      //      double noise    = m_cscCalibTool->stripNoise( cscOfflineChannelHashId, true );
      double noiseADC    = m_cscCalibTool->stripNoise( cscOfflineChannelHashId, false );
      //      double pedestal = m_cscCalibTool->stripPedestal( cscOfflineChannelHashId, true );
      double pedestalADC = m_cscCalibTool->stripPedestal( cscOfflineChannelHashId, false );
      double phaseOffset = (cscRdoCollection->samplingPhase()) ? 25 : 0;


      if (IsNewEDM) {
        std::vector<float> cscDigitSamples = cscDigit->sampleCharges();        

        for (int i=0; i<m_numSamples; i++) {
          //          double samplingTime = (i+1)*(1000/m_samplingRate) + m_startTime;// considered in CSC_Digitization
	  double theNoise = (m_addNoise) ? CLHEP::RandGaussZiggurat::shoot(m_rndmEngine, 0.0, noiseADC) : 0.0;
	  double rawAmpl = cscDigitSamples[i];
          double ampl = rawAmpl;
          double charge_to_adcCount  = m_cscCalibTool->numberOfElectronsToADCCount(cscOfflineChannelHashId, ampl) + theNoise + pedestalADC;
          
          if ( charge_to_adcCount > MAX_AMPL ) charge_to_adcCount = MAX_AMPL-1.0; 
          if ( charge_to_adcCount < 0 ) charge_to_adcCount = 0;
          //          if (m_addNoise) {
          //            ampl = ampl + theNoise + pedestal;
          //          }
          //          if (ampl < 0) ampl = 0.0;
          uint16_t adcCount = (uint16_t) rint(charge_to_adcCount);
          //          if (ampl > (1.0*MAX_AMPL)) {
          //            ATH_MSG_WARNING ( "value out of range: " << ampl << " " 
          //                              << " Setting it to max value = " << MAX_AMPL
          //                              << " Identifier is " << m_cscHelper->show_to_string(offlineChannelId) );
          //            adcCount = MAX_AMPL;
          //          }
          
          samples.push_back(adcCount); 
          
          ATH_MSG_DEBUG ( "amplitude :: index =  "     << (i+1) 
                          //                          << " NA/sampling time (ns) = "     << samplingTime
                          << " charge to ADC = " << charge_to_adcCount
                          //                          << " amplitude (double) = "     << ampl
                          << " raw amplitude (double) = " << rawAmpl
                          << " theNoise (ADC) = "      << theNoise    );
        }
      } else {
        double charge_to_adcCount  = m_cscCalibTool->numberOfElectronsToADCCount(cscOfflineChannelHashId, cscDigit->charge());
        //        if ( charge_to_adcCount > MAX_AMPL/2.0 ) charge_to_adcCount = MAX_AMPL/2.0-1.0; 

        rodReadOut.setParams(cscDigit->time()+phaseOffset, m_samplingTime, m_signalWidth); 
        
        for (int i=0; i<m_numSamples; i++) {
          double samplingTime = (i+1)*(1000/m_samplingRate) + m_startTime;
	  double theNoise = (m_addNoise) ? CLHEP::RandGaussZiggurat::shoot(m_rndmEngine, 0.0, noiseADC) : 0.0;
          double rawAmpl = rodReadOut.signal_amplitude(samplingTime); //FIXME - to be updated still!
          double ampl = charge_to_adcCount*rawAmpl + theNoise + pedestalADC;

          if ( ampl > MAX_AMPL ) ampl = MAX_AMPL-1.0; 

          //          if (m_addNoise) {
          //            ampl = charge_to_adcCount*rawAmpl + theNoise + pedestal;
          //          }
          //          if (ampl < 0) ampl = 0.0;
          uint16_t adcCount = (uint16_t) rint(ampl);
          //          if (ampl > (1.0*MAX_AMPL)) {
          //            ATH_MSG_WARNING ( "value out of range: " << ampl << " " 
          //                              << " Setting it to max value = " << MAX_AMPL
          //                              << " Identifier is " << m_cscHelper->show_to_string(offlineChannelId) );
          //            adcCount = MAX_AMPL;
          //          }
          
          samples.push_back(adcCount); 
          
          ATH_MSG_DEBUG ( "amplitude :: index =  "     << (i+1) 
                          << " sampling time (ns) = "     << samplingTime
                          << " charge to ADC (double) = " << charge_to_adcCount
                          << " amplitude (double) = "     << ampl
                          << " raw amplitude (double) = " << rawAmpl
                          << " theNoise (double) = "      << theNoise    );
          
          // DEBUG purpose....
          //          samplesOrig.push_back(float(adcCount));
          //          uint16_t adcNoMix   =(uint16_t) rint(charge_to_adcCount*rawAmpl);
          //          uint16_t adcPlusPed =(uint16_t) rint(charge_to_adcCount*rawAmpl+pedestal);
          //          samplesNoMix.push_back(float(adcNoMix));
          //          samplesPlusPed.push_back(float(adcPlusPed));
        }

        // DEBUG ->
        //        int error0, error1, error2;
        //        double charge0, time0, charge1, time1, charge2, time2;
        
        //        error0 = m_cscCalibTool->findCharge(float(samplingTime), phase, samplesOrig, charge0, time0);
        //        error1 = m_cscCalibTool->findCharge(float(samplingTime), phase, samplesNoMix, charge1, time1);
        //        error2 = m_cscCalibTool->findCharge(float(samplingTime), phase, samplesPlusPed, charge2, time2);
        
      } // isNewEDM

      
      ATH_MSG_DEBUG ( "CSC Digit zsec:phisec:station:sector:measphi:wlay:istrip:charge "
                      //                      << run << " " << evt << " "
                      << zsec << " " << phisec << " " << istation << " " << sector << " "
                      << measphi << " " <<  wlay << " " << istrip << " "
                      //                      << int (cscDigit->charge()+1) << " " << charge_to_adcCount << " "
                      //                      << charge0-pedestal << " " << charge1 << " " << charge2-pedestal
                      //                      << pedestal << " "
                      << samples[0] << " "
                      << samples[1] << " "
                      << samples[2] << " "
                      << samples[3] << " "
                      ) ;
      // DEBUG <-//

      
      /** increase the width and the currentStrip */
      width++;
      oldStrip = currentStrip;

      if ( count == size ) {  // digicount in digiCollection equals to size of digicollection...

        /** increament the RPU counts */
        ATH_MSG_DEBUG ( "End of DigitCollection : SPU ID = " << spuID ); 
        uint16_t rpuID = 0x0; 
        if ( spuID <= 4 ) rpuID = 5;
        else if ( spuID > 4 && spuID <= 9 ) rpuID = 11;
        else ATH_MSG_ERROR ( "SPU ID out of range " << spuID );
        cscRdoCollection->addRPU( rpuID );
        cscRdoCollection->addDataType( 0x0 );
        ATH_MSG_DEBUG ( "RPU id = " << rpuID );


        int mphi  = ( (address & 0x00000100) >>  8);
        int zEta  =  ( ((address & 0x00001000) >> 12 ) == 0x0) ? -1 : 1;
        
        if (zEta > 0 && mphi == 1) {
          
          int istat     = ( (address & 0x00010000) >> 16 ) + 50;
          int phisector = ( (address & 0x0000E000) >> 13 ) + 1;
          int chamLayer = ( (address & 0x00000800) >> 11 ) + 1;
          int wlayer    = ( (address & 0x00000600) >>  9 ) + 1;
          
          int beforestrip  = (address & 0x000000FF)+1; 
          int afterstrip = beforestrip - width + 1;
          
          Identifier newOnlineChannelId
            = m_cscHelper->channelID(istat,zEta,phisector,chamLayer,wlayer,mphi,afterstrip);
          address = rodReadOut.address(newOnlineChannelId,zEta,phisector);
          
        }



        /** Also Create one last CscRawData on the last element */
	uint32_t hashId = static_cast<uint32_t>(cscRawDataOfflineHashId);
	CscRawData* rawData = new CscRawData(samples, address, collId, spuID, width);
	rawData->setHashID( hashId );
	cscRdoCollection->push_back(rawData);
      }

    } // Loop over CscDigit
  } // Loop over CscDigitCollection...
  
  ATH_MSG_DEBUG ( "Adding RDOs to the RdoContainer" );
  /** Add RDOs to the RdoContainer */
  std::map<uint16_t, CscRawDataCollection *>::iterator itM  =m_cscRdoMap.begin();
  std::map<uint16_t, CscRawDataCollection *>::iterator itM_e=m_cscRdoMap.end();  
  for (; itM != itM_e; ++itM) {
     
      /** we need to get the cluster counts in each SPU */
      uint16_t clusterCounts[] = {0,0,0,0,0,0,0,0,0,0};
      CscRawDataCollection::const_iterator itD   = (itM->second)->begin();
      CscRawDataCollection::const_iterator itD_e = (itM->second)->end();
      for (; itD!=itD_e; ++itD)
      {
        uint16_t spuID = (*itD)->rpuID();
        clusterCounts[spuID] += 1;
      }
      for (unsigned int i=0; i<10; ++i) (itM->second)->set_spuCount(i,clusterCounts[i]);

      /** save collections into StoreGate */
      ATH_CHECK( rdoContainer->addCollection(itM->second, itM->first) );
  }
  ATH_MSG_DEBUG ( "Added RDOs to the RdoContainer" );
  
  return StatusCode::SUCCESS;
}

CscRawDataCollection * CscDigitToCscRDOTool::cscRdo(uint16_t subDetectorId, 
						    uint16_t rodId) {
  ATH_MSG_DEBUG ( "cscRdo Begin!" );

  uint16_t onlineColId = m_cscCablingSvc->collectionId(subDetectorId, rodId);
  ATH_MSG_DEBUG ( "This collection online identifier is " << onlineColId );

  //assert (onlineColId <= 15);

  std::map<uint16_t, CscRawDataCollection *>::iterator it = 
    m_cscRdoMap.find(onlineColId);
  if (it != m_cscRdoMap.end()){
    return (*it).second;
  }

  /** create new CscRdo */
  CscRawDataCollection *rdo = new CscRawDataCollection (onlineColId);
  m_cscRdoMap[onlineColId] = rdo;

  /** set SubDetectorID and ROD ID */
  rdo->setSubDetectorId (subDetectorId);
  uint16_t onlineRodId = 0x0;
  bool check = m_cscCablingSvc->onlineId(rodId, onlineRodId);
  if (!check) ATH_MSG_ERROR ( "Enable to convert offline ROD Id to online ROD id" );
  ATH_MSG_DEBUG ( "online ROD id = " << onlineRodId << " offline ROD id " << rodId );  
  assert ( m_cscCablingSvc->is_rodId(onlineRodId) );
  rdo->setRodId(onlineRodId);
    
  return rdo;
}

