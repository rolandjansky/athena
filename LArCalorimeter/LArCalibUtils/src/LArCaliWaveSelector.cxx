/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArCaliWaveSelector.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "AthenaKernel/errorcheck.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloGain.h"

#include "LArRawConditions/LArCaliWaveContainer.h"

#include "LArRawConditions/LArWaveHelper.h"


typedef LArCaliWaveContainer::ConstConditionsMapIterator   CaliMapIt;
typedef LArCaliWaveContainer::LArCaliWaves::const_iterator CaliWaveIt;

LArCaliWaveSelector::LArCaliWaveSelector(const std::string& name, ISvcLocator* pSvcLocator) 
 : AthAlgorithm(name, pSvcLocator),
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
  ATH_MSG_INFO ( " in initialize.." );

  // Default list
  if (!m_keyList.size()) { m_keyList.push_back("LArCaliWave"); }
  parseSelection();

  ATH_CHECK( detStore()->retrieve (m_cellID, "CaloCell_ID") );
  if (!m_cellID) {
    ATH_MSG_ERROR ( "Could not access CaloCell_ID helper" );
    return StatusCode::FAILURE;
  }

  ATH_CHECK(  m_cablingKey.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode LArCaliWaveSelector::stop()
{
  ATH_MSG_INFO ( " in stop.." );

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling){
     ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key() );
     return StatusCode::FAILURE;
  }

  // create empty LArCaliWaveContainer to store DAC selected LArCaliWave
  // make it view container
//  LArCaliWaveContainer* selectedCaliWaveContainer = new LArCaliWaveContainer(SG::VIEW_ELEMENTS);
  auto selectedCaliWaveContainer = std::make_unique<LArCaliWaveContainer>();
  CHECK( selectedCaliWaveContainer->setGroupingType(m_groupingType,msg()));
  CHECK( selectedCaliWaveContainer->initialize());

  std::vector<std::string>::const_iterator key_it   = m_keyList.begin();
  std::vector<std::string>::const_iterator key_it_e = m_keyList.end();

  for (;key_it!=key_it_e; ++key_it) {
    const LArCaliWaveContainer* larCaliWaveContainer;    
    StatusCode sc = detStore()->retrieve(larCaliWaveContainer,*key_it);
    if (sc.isFailure()) {
      ATH_MSG_WARNING ( "No LArCaliWaveContainer in StoreGate with key = " << *key_it );
      continue; 
    } else {
      ATH_MSG_INFO ( "Reading LArCaliWaveContainer with key = " << *key_it );
    }

    int det, lay;
    for ( unsigned gain = CaloGain::LARHIGHGAIN ; gain < CaloGain::LARNGAIN ; ++gain ) {
         if(!( (0x1<<gain) & m_gmask) ) continue;

         CaliMapIt cali_it = larCaliWaveContainer->begin(gain) ;
         CaliMapIt cali_it_e = larCaliWaveContainer->end(gain) ;

         if ( cali_it == cali_it_e ) {
           ATH_MSG_DEBUG ( "No wave with gain = " << gain << " in container with key = " << *key_it );
           continue;
         } else {
           ATH_MSG_DEBUG ( "processing gain = " << gain );
         }
         for ( ; cali_it != cali_it_e; ++cali_it) {
            
            CaliWaveIt wave_it   = cali_it->begin();
	    CaliWaveIt wave_it_e = cali_it->end();
            if ( wave_it == wave_it_e ) {
               ATH_MSG_DEBUG ( "Empty channel found..." );
	       continue; 
            } 
	
	   HWIdentifier chid = cali_it.channelId();
           Identifier cell_id = cabling->cnvToIdentifier(chid);
           if(m_cellID->is_em_barrel(cell_id)){ det = 0;
           } else if(m_cellID->is_em_endcap(cell_id)) { det = 1;
           } else if(m_cellID->is_hec(cell_id)) { det = 2;
           } else if(m_cellID->is_fcal(cell_id)) { det = 3;
           } else {
             ATH_MSG_ERROR ( "Wrong ID in LArCaliWaveContainer: "<<m_cellID->print_to_string(cell_id));
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

  ATH_CHECK( detStore()->record(std::move(selectedCaliWaveContainer),m_outputKey) );
  return StatusCode::SUCCESS;
}

void LArCaliWaveSelector::parseSelection()
{
  int det,lay,gain,DAC;
  std::string selstr;
  for(unsigned i=0; i<m_selectionList.size(); ++i) {
     std::string::size_type det_delimiter( m_selectionList[i].find('/') );
     if( std::string::npos == det_delimiter ){
       ATH_MSG_WARNING ( "Bad selection string: "<< m_selectionList[i] << " skipped...." );
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
       ATH_MSG_WARNING ( "Bad detector substring: " << det_string << " skipped ... " );
       continue;
     }
     std::string::size_type lay_delimiter( m_selectionList[i].find('/',det_delimiter+1) );
     lay = atoi(m_selectionList[i].substr(det_delimiter+1, lay_delimiter).c_str());
     if(lay < 0 || lay > 3) {
       ATH_MSG_WARNING ( "Bad layer: " << lay << " skipped..." );
       continue;
     }
     std::string::size_type gain_delimiter( m_selectionList[i].find('/',lay_delimiter+1) );
     gain = atoi(m_selectionList[i].substr(lay_delimiter+1, gain_delimiter).c_str());
     if(gain < 0 || gain > 2) {
       ATH_MSG_WARNING ( "Bad gain: " << gain << " skipped..." );
       continue;
     }
     DAC = atoi(m_selectionList[i].substr(gain_delimiter+1).c_str());
     if(DAC < 0 || DAC > 65000) {
       ATH_MSG_WARNING ( "Bad DAC: " << DAC << " skipped..." );
       continue;
     }

     // Now we have all numbers
     m_mapDAC[std::make_pair( std::make_pair(det,lay), gain)] = DAC;
     m_gmask |= (0x1<<gain);
  }
}
