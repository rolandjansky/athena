//  ***************************************************************************
//  *                                                                         *
//  *   This program is free software; you can redistribute it and/or modify  *
//  *   it under the terms of the GNU General Public License as published by  *
//  *   the Free Software Foundation; either version 2 of the License, or     *
//  *   (at your option) any later version.                                   *
//  *                                                                         *
//  *   Author: John Morris (john.morris@cern.ch)                             *
//  *           Queen Mary University of London                               *
//  *                                                                         *
//  ***************************************************************************/

/**
   @class TriggerTowerThinningAlg
   @brief Thinning algorithm for the L1Calo Trigger Towers

   This algorithm can be used to keep only TriggerTowers which have either:
   1.) An ADC value > m_minADC  OR
   2.) A Calo Cell energy > m_minCaloCellEnergy

   For a random (TRandom3) m_minRandom % of events, all Trigger Towers are saved.
   This is very useful for pedestal and noise studies

**/

#include "TrigT1CaloCalibTools/TriggerTowerThinningAlg.h"


TriggerTowerThinningAlg::TriggerTowerThinningAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_thinningSvc( "ThinningSvc",  name ),
  m_tools("LVL1::L1CaloOfflineTriggerTowerTools/L1CaloOfflineTriggerTowerTools"),
  m_triggerTowerLocation("TriggerTowers"),
  m_caloCellsLocation("AllCalo"),
  m_larDigitLocation("FREE"),
  m_tileDigitsLocation("TileDigitsCnt"),
  m_tileTTL1ContainerName("TileCellTTL1Container"),
  m_minCaloCellEnergy(0.05),
  m_minADC(30),
  m_useRandom(false),
  m_minRandom(0.03),
  m_nEventsProcessed(0),
  m_nEventsAllTriggerTowersKeptByRandom(0),
  m_nTriggerTowersProcessed(0),
  m_nTriggerTowersKept(0),
  m_nTriggerTowersRejected(0),
  m_random(0)
{
  declareProperty("ThinService",m_thinningSvc);
  declareProperty("TriggerTowerLocation", m_triggerTowerLocation);
  declareProperty("CaloCellsLocation", m_caloCellsLocation);
  declareProperty("LArDigitLocatrion", m_larDigitLocation);
  declareProperty("TileDigitsLocation",m_tileDigitsLocation);
  declareProperty("TileTTL1ContainerName",m_tileTTL1ContainerName);
  declareProperty("MinCaloCellEnergy",m_minCaloCellEnergy);
  declareProperty("MinADC",m_minADC);
  declareProperty("UseRandom",m_useRandom);
  declareProperty("MinRandom",m_minRandom);
}

TriggerTowerThinningAlg::~TriggerTowerThinningAlg(){
  // Athena 16.0.0 will not let me delete m_random here
  // The package will compile but the libraries will not build correctly
  // moving delete m_random; to finalize()
  // Not really happy about this, but what works works.
}

StatusCode TriggerTowerThinningAlg::initialize(){
  StatusCode sc;

  sc = m_tools.retrieve();
  if(sc.isFailure()){ATH_MSG_INFO("Error loading L1CaloOfflineTriggerTowerTools");return sc;}

  // Random number generator
  if(m_useRandom == true){
    m_random = new TRandom3(0);
  }  

  // Get pointer to ThinningSvc and cache it :
  // m_thinningSvc is of type IThinningSvc
  if ( !m_thinningSvc.retrieve().isSuccess() )
    {
      if (msgLvl(MSG::ERROR))
	{
	  ATH_MSG_ERROR("Unable to retrieve pointer to ThinningSvc");
	}
      return sc;
    }
  
  return sc;
}

StatusCode TriggerTowerThinningAlg::execute(){
  StatusCode sc;

  // Create the mask to be used for thinning
  std::vector<bool> mask;

  // Counters
  unsigned long nKeep(0),nReject(0),nTotal(0);
  m_nEventsProcessed++;

  // Load everything needed from storeGate
  // Update the TriggerTowerTools m_tools
  const CaloCellContainer* cellContainer = 0;
  sc = evtStore()->retrieve(cellContainer, m_caloCellsLocation);
  if(sc.isFailure()){return sc;}
  m_tools->caloCells(cellContainer);
  
  if(evtStore()->contains<LArDigitContainer>(m_larDigitLocation)) {
    const LArDigitContainer* larDigitContainer = 0;
    sc = evtStore()->retrieve(larDigitContainer,m_larDigitLocation);
    if(sc.isFailure()){return sc;}
    m_tools->larDigits(larDigitContainer);
  }

  if(evtStore()->contains<TileDigitsContainer>(m_tileDigitsLocation)) {
    const TileDigitsContainer* tileDigitsContainer = 0;
    sc = evtStore()->retrieve(tileDigitsContainer,m_tileDigitsLocation);
    if(sc.isFailure()){return sc;}
    m_tools->tileDigits(tileDigitsContainer);
  }
  
  const TriggerTowerCollection* ttCollection = 0;
  sc = evtStore()->retrieve(ttCollection, m_triggerTowerLocation);
  if(sc.isFailure()){return sc;} 

  typedef TriggerTowerCollection::const_iterator Itr;
  
  // Should we save the trigger tower or not???
  // If this is calibration data, then yes - save the lot
  // Randomly save all trigger towers m_minRandom % of the time.
  //
  // If this is collisions data then only save if:
  // 1.) Any ADC value is greater than m_minADC  OR
  // 2.) The CaloCell energy is greater than m_minCaloCellEnergy

  bool globalSaveMe(false);
  // Random number save all
  if(m_useRandom == true){
    if(globalSaveMe == false){
      if(m_random->Rndm() < m_minRandom){
	globalSaveMe = true;
	m_nEventsAllTriggerTowersKeptByRandom++;
      }
    }
  }

  // Iterate over all trigger towers
  for(Itr i=ttCollection->begin();i!=ttCollection->end();++i){
    nTotal++;

    bool saveMe(false);
    if(globalSaveMe == true){saveMe = true;}
    
    // Test for EM Calo Energy
    if(saveMe == false){
      if(m_tools->emTTCellsEnergy((*i)) > m_minCaloCellEnergy){
	saveMe = true;
      }
    }
    
    // Test for Had Calo Energy
    if(saveMe == false){
      if(m_tools->hadTTCellsEnergy((*i)) > m_minCaloCellEnergy){
	saveMe = true;
      }
    } 

    // Test for EM ADC values
    typedef std::vector<int>::iterator Itr_tt;
    if(saveMe == false){
      std::vector<int> adc = (*i)->emADC();
      for(Itr_tt j=adc.begin();j!=adc.end();++j){
	if( (*j) > m_minADC){
	  saveMe = true;
	  break;
	}
      }
    } 
    
    // Test for Had ADC values
    if(saveMe == false){
      std::vector<int> adc = (*i)->hadADC();
      for(Itr_tt j=adc.begin();j!=adc.end();++j){
	if( (*j) > m_minADC){
	  saveMe = true;
	  break;
	}
      }
    }

    if(saveMe == true){
      mask.push_back(true);
      nKeep++;
    }
    else{
      mask.push_back(false);
      nReject++;
    }

  } // End loop over trigger towers


  sc = m_thinningSvc->filter(*ttCollection,mask,IThinningSvc::Operator::Or);
  if(!sc.isSuccess()){
    if (msgLvl(MSG::WARNING)){
      ATH_MSG_ERROR("Could not thin L1Calo Trigger Towers... problem with thinningSvc?? ");
    }
  }

  // Counters
  m_nTriggerTowersProcessed += nTotal;
  m_nTriggerTowersKept += nKeep;
  m_nTriggerTowersRejected += nReject;

  if (msgLvl(MSG::DEBUG))
    {
      ATH_MSG_DEBUG(" L1Calo Trigger Tower Thinning statistics: keeping " << nKeep << " cells"
	<< " and rejecting " << nReject << " cells");
    }

  return sc;
}

StatusCode TriggerTowerThinningAlg::finalize(){
  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);

  // Print info messages
  if (msgLvl(MSG::INFO))
    {
      ATH_MSG_INFO(
	"==> finalize " << name() << "...\n"
	<< "***************************************************************\n"
	<< "Results of " << name() << " thinning algorithm:\n"
	<< "-------------");
      ATH_MSG_INFO(" Number of processed events:  " << m_nEventsProcessed);

      if(m_nTriggerTowersProcessed > 0) { 
	ATH_MSG_INFO(
	  " Average percent of Trigger Towers kept = "
	  << 100.0 * m_nTriggerTowersKept / (double)m_nTriggerTowersProcessed << " %");
      }
      if(m_useRandom == true && m_nEventsProcessed > 0){
	ATH_MSG_INFO(
	  " Percentage of events where all Trigger Towers were kept (should be approx "<< 100.0*m_minRandom << ") = "
	  << 100.0 * m_nEventsAllTriggerTowersKeptByRandom / (double)m_nEventsProcessed);
      }
    }

  // This should really be deleted in the destructor
  // Seems like a dirty hack to do it here
  // Have to make it so that the package builds properly
  delete m_random;


  return sc;
}
