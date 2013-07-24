/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArCaliWaveSelector.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "AthenaKernel/errorcheck.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArTools/LArCablingService.h"

#include "LArRawConditions/LArCaliWaveContainer.h"

#include "LArRawConditions/LArWaveHelper.h"


typedef LArCaliWaveContainer::ConstConditionsMapIterator   CaliMapIt;
typedef LArCaliWaveContainer::LArCaliWaves::const_iterator CaliWaveIt;

LArCaliWaveSelector::LArCaliWaveSelector(const std::string& name, ISvcLocator* pSvcLocator) 
 : Algorithm(name, pSvcLocator),
   m_cellID(0),
   m_gmask(0),
   m_groupingType("ExtendedFeedThrough") 
{
 declareProperty("SelectionList",    m_selectionList); // Vector of strings of det/lay/gain/DAC -> "EMB/0/0/200"
                                                       // Known detectors EMB, EMEC, HEC, FCAL
                                                       // Available layers 0-3, gains 0-2, DAC 0-65000
 declareProperty("KeyList",          m_keyList);
 declareProperty("KeyOutput",        m_outputKey); 
 declareProperty("GroupingType",     m_groupingType);
}

LArCaliWaveSelector::~LArCaliWaveSelector() 
{}

StatusCode LArCaliWaveSelector::initialize()
{ 
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " in initialize.." << endreq;

  // Default list
  if (!m_keyList.size()) { m_keyList.push_back("LArCaliWave"); }
  parseSelection(&log);

  // Identifier helpers
  // retrieve DetectorStore
  StoreGateSvc*  detStore;
  StatusCode sc = service("DetectorStore", detStore);
  if (sc.isFailure()) {
        log<< MSG::ERROR << "Unable to retrieve pointer to DetectorStore " << endreq;
        return sc;
  }

  // retrieve CaloMgr
  const CaloIdManager* caloMgr;
  sc = detStore->retrieve(caloMgr);
  if (sc.isFailure()) {
        log<< MSG::ERROR << "Unable to retrieve CaloIdManager from DetectorStore" << endreq;
        return sc;
  }

  m_cellID = caloMgr->getCaloCell_ID();
  if (!m_cellID) {
       log<< MSG::ERROR << "Could not access CaloCell_ID helper" << endreq;
       return sc;
  }

  return StatusCode::SUCCESS;
}


StatusCode LArCaliWaveSelector::stop()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " in stop.." << endreq;

  StoreGateSvc* detStore; 
  StatusCode sc = service("DetectorStore",detStore);
  if ( sc != StatusCode::SUCCESS ) {
    log << MSG::ERROR << "Cannot get DetectorStore!" << endreq;
    return sc;
  }

  // Get the cabling service
  LArCablingService * cabling = 0;
  CHECK( toolSvc()->retrieveTool("LArCablingService",cabling) );

  // create empty LArCaliWaveContainer to store DAC selected LArCaliWave
  // make it view container
//  LArCaliWaveContainer* selectedCaliWaveContainer = new LArCaliWaveContainer(SG::VIEW_ELEMENTS);
  LArCaliWaveContainer* selectedCaliWaveContainer = new LArCaliWaveContainer();
  CHECK( selectedCaliWaveContainer->setGroupingType(m_groupingType,log));
  CHECK( selectedCaliWaveContainer->initialize());

  std::vector<std::string>::const_iterator key_it   = m_keyList.begin();
  std::vector<std::string>::const_iterator key_it_e = m_keyList.end();

  for (;key_it!=key_it_e; ++key_it) {
    const LArCaliWaveContainer* larCaliWaveContainer;    
    sc = detStore->retrieve(larCaliWaveContainer,*key_it);
    if (sc.isFailure()) {
         log << MSG::WARNING << "No LArCaliWaveContainer in StoreGate with key = " << *key_it << endreq;
         continue; 
    } else {
         log << MSG::INFO << "Reading LArCaliWaveContainer with key = " << *key_it << endreq;
    }

    int det, lay;
    for ( unsigned gain = CaloGain::LARHIGHGAIN ; gain < CaloGain::LARNGAIN ; ++gain ) {
         if(!( (0x1<<gain) & m_gmask) ) continue;

         CaliMapIt cali_it = larCaliWaveContainer->begin(gain) ;
         CaliMapIt cali_it_e = larCaliWaveContainer->end(gain) ;

         if ( cali_it == cali_it_e ) {
               log << MSG::DEBUG << "No wave with gain = " << gain << " in container with key = " << *key_it << endreq;
               continue;
         } else {
               log << MSG::DEBUG << "processing gain = " << gain << endreq;
         }
         for ( ; cali_it != cali_it_e; ++cali_it) {
            
            CaliWaveIt wave_it   = cali_it->begin();
	    CaliWaveIt wave_it_e = cali_it->end();
            if ( wave_it == wave_it_e ) {
	       log << MSG::DEBUG << "Empty channel found..." << endreq ;
	       continue; 
            } 
	
	   HWIdentifier chid = cali_it.channelId();
           Identifier cell_id = cabling->cnvToIdentifier(chid);
           if(m_cellID->is_em_barrel(cell_id)){ det = 0;
           } else if(m_cellID->is_em_endcap(cell_id)) { det = 1;
           } else if(m_cellID->is_hec(cell_id)) { det = 2;
           } else if(m_cellID->is_fcal(cell_id)) { det = 3;
           } else {
              log << MSG::ERROR << "Wrong ID in LArCaliWaveContainer: "<<m_cellID->print_to_string(cell_id)<<endreq; 
              continue;
           }
           lay = m_cellID->sampling(cell_id);
           DetGain dg = std::make_pair( std::make_pair(det,lay), gain);
           if( m_mapDAC.find(dg) == m_mapDAC.end() ) continue;
        
           for ( ; wave_it != wave_it_e; ++wave_it) {
              if(wave_it->getDAC() == m_mapDAC[dg]) {
                 (selectedCaliWaveContainer->get(chid,gain)).push_back(*wave_it);
                 break;
              }
           }
         }
    }
  } 

  if ( StatusCode::SUCCESS != detStore->record(selectedCaliWaveContainer,m_outputKey)) {
    log << MSG::ERROR << "Cannot store selected LArCaliWaveContainer with key '" << m_outputKey << "' !!!!" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

void LArCaliWaveSelector::parseSelection(MsgStream *log)
{
  int det,lay,gain,DAC;
  std::string selstr;
  for(unsigned i=0; i<m_selectionList.size(); ++i) {
     std::string::size_type det_delimiter( m_selectionList[i].find('/') );
     if( std::string::npos == det_delimiter ){
        (*log) << MSG::WARNING << "Bad selection string: "<< m_selectionList[i] << " skipped...." << endreq;
        continue;
     }
     std::string det_string = m_selectionList[i].substr(0, det_delimiter);
     if(det_string=="EMB") {
        det = 0;
     } else if (det_string == "EMEC") {
        det = 1;
     } else if (det_string == "HEC") {
        det = 2;
     } else if (det_string == "FCAL") {
        det = 3;
     } else {
        (*log) << MSG::WARNING << "Bad detector substring: " << det_string << " skipped ... " << endreq;
        continue;
     }
     std::string::size_type lay_delimiter( m_selectionList[i].find('/',det_delimiter+1) );
     lay = atoi(m_selectionList[i].substr(det_delimiter+1, lay_delimiter).c_str());
     if(lay < 0 || lay > 3) {
        (*log) << MSG::WARNING << "Bad layer: " << lay << " skipped..." << endreq;
        continue;
     }
     std::string::size_type gain_delimiter( m_selectionList[i].find('/',lay_delimiter+1) );
     gain = atoi(m_selectionList[i].substr(lay_delimiter+1, gain_delimiter).c_str());
     if(gain < 0 || gain > 2) {
        (*log) << MSG::WARNING << "Bad gain: " << gain << " skipped..." << endreq;
        continue;
     }
     DAC = atoi(m_selectionList[i].substr(gain_delimiter+1).c_str());
     if(DAC < 0 || DAC > 65000) {
        (*log) << MSG::WARNING << "Bad DAC: " << DAC << " skipped..." << endreq;
        continue;
     }

     // Now we have all numbers
     m_mapDAC[std::make_pair( std::make_pair(det,lay), gain)] = DAC;
     m_gmask |= (0x1<<gain);
  }
}
