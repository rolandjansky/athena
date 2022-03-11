/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// +==========================================================================+
// +                                                                          +
// + Authors .......: G.Unal                                                  +
// +    migrate LAr digitization to PileUpTool framework                      +
// +==========================================================================+
//

#include "LArPileUpTool.h"

#include "LArDigitization/LArHitEMap.h"

#include "AthenaKernel/ITriggerTime.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CaloIdentifier/LArID.h"
#include "CaloIdentifier/LArID_Exception.h"
#include "CaloIdentifier/LArNeighbours.h"
#include "Identifier/IdentifierHash.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArSimEvent/LArHitFloat.h"
#include "LArSimEvent/LArHit.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "EventInfoUtils/EventIDFromStore.h"
#include "GeneratorObjects/McEventCollection.h"

#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandomEngine.h"

#include <CLHEP/Random/Randomize.h>

using CLHEP::RandFlat;
using CLHEP::RandGaussZiggurat;

LArPileUpTool::LArPileUpTool(const std::string& type, const std::string& name, const IInterface* parent) :
  PileUpToolBase(type, name, parent) {

}


LArPileUpTool::~LArPileUpTool()
{
  return;
}


StatusCode LArPileUpTool::initialize()
{

  //
  // ........ Check for inconsistent configuration
  //
  if (m_RndmEvtOverlay && !m_isMcOverlay && m_NoiseOnOff) {
    ATH_MSG_FATAL("Adding noise to hard-scatter Hits is not supported for Data Overlay! Fix your configuration. Bailing out.");
    return StatusCode::FAILURE;
  }
  if (m_RndmEvtOverlay && m_isMcOverlay && !m_NoiseOnOff) {
    ATH_MSG_FATAL("MC overlay. Need to switch back on noise only to emulate extra noise for cells with different gains! Fix your configuration. Bailing out.");
    return StatusCode::FAILURE;
  }
  if (m_RndmEvtOverlay && !m_PileUp) {
    ATH_MSG_FATAL("If RndmEvtOverlay==True then PileUp must also be True. Fix your configuration. Bailing out.");
    return StatusCode::FAILURE;
  }
  if (!m_PileUp && m_onlyUseContainerName) {
     ATH_MSG_FATAL("If PileUp==False then OnlyUserContainerName must also be False. Fix your configuration. Bailing out.");
    return StatusCode::FAILURE;
  }
  //
  // ........ print random event overlay flag
  //
  if (m_RndmEvtOverlay) {
    ATH_MSG_INFO(" pileup and/or noise added by overlaying digits of random events");
    if (m_isMcOverlay) { ATH_MSG_INFO("   random events are from MC "); }
    else { ATH_MSG_INFO("   random events are from data "); }
  }
  else {
    ATH_MSG_INFO(" No overlay of random events");
  }

  if (m_onlyUseContainerName) {
    ATH_CHECK(m_mergeSvc.retrieve());
    ATH_MSG_INFO( "PileUpMergeSvc successfully initialized");
  }

  //
  // ......... print the noise flag
  //
  if ( m_NoiseOnOff )
  {
    ATH_MSG_INFO(" Electronic noise will be added in each cell " );
  }
  else
  {
    ATH_MSG_INFO(" No electronic noise added.");

    //not useful (see MakeDigit), but in case of...
    m_NoiseInEMB= false;
    m_NoiseInEMEC=false;
    m_NoiseInHEC= false;
    m_NoiseInFCAL=false;
  }

  //
  // ............ print cross-talk configuration
  //
  if (m_CrossTalk )
  {
    ATH_MSG_INFO(" Cross-talk in EM barrel will be taken into account : ");
    ATH_MSG_INFO("     Cross talk strip strip included ");
    if (m_CrossTalk2Strip) ATH_MSG_INFO("     Cross talk strip-2nd strip included ");
    ATH_MSG_INFO("     Cross talk strip middle included ");
    if (m_CrossTalkStripMiddle) ATH_MSG_INFO("     Cross talk strip middle included ");
    if (m_CrossTalkMiddle)      ATH_MSG_INFO("     Cross talk middle middle included");
  }
  else
  {
    ATH_MSG_INFO(" no Cross-Talk simulated");
  }

  if (m_onlyUseContainerName) {
    if (m_useLArHitFloat) {
      for (const auto& RHkey : m_hitFloatContainerKeys.keys()) {
        m_hitContainerNames.push_back(RHkey->key());
      }
    }
    else {
      for (const auto& RHkey : m_hitContainerKeys.keys()) {
        m_hitContainerNames.push_back(RHkey->key());
      }
    }
  }
  ATH_MSG_DEBUG("Input objects in these containers : '" << m_hitContainerNames << "'");

  // Initialize ReadHandleKey
  ATH_CHECK(m_hitContainerKeys.initialize(!m_onlyUseContainerName && !m_hitContainerKeys.empty() ));
  ATH_CHECK(m_hitFloatContainerKeys.initialize(!m_onlyUseContainerName && !m_hitFloatContainerKeys.empty() ));
  ATH_CHECK(m_inputDigitContainerKey.initialize(!m_onlyUseContainerName && !m_inputDigitContainerKey.empty() ));

  ATH_CHECK(m_caloMgrKey.initialize());
  

  //retrieve ID helpers
  ATH_CHECK(detStore()->retrieve(m_calocell_id,"CaloCell_ID"));


  const CaloIdManager* caloIdMgr = nullptr;
  StatusCode sc = detStore()->retrieve(caloIdMgr);
  if (sc.isFailure()) {
    ATH_MSG_ERROR(" Unable to retrieve CaloIdManager from DetectoreStore");
    return StatusCode::FAILURE;
  }
  m_larem_id   = caloIdMgr->getEM_ID();
  m_larhec_id  = caloIdMgr->getHEC_ID();
  m_larfcal_id = caloIdMgr->getFCAL_ID();

  sc = detStore()->retrieve(m_laronline_id);
  if (sc.isFailure()) {
    ATH_MSG_ERROR(" Unable to retrieve LArOnlineId from DetectoreStore");
    return StatusCode::FAILURE;
  }

  if (m_useTriggerTime) {
     if (m_triggerTimeTool.retrieve().isFailure()) {
        ATH_MSG_ERROR(" Unable to find Trigger Time Tool");
        return StatusCode::FAILURE;
     }
  }
  else {
    m_triggerTimeTool.disable();
  }

  ATH_CHECK(m_rndmGenSvc.retrieve());

  if (m_NSamples>32) {
     ATH_MSG_WARNING(" Cannot do more than 32 samples:  NSamples reset to 32 ");
     m_NSamples=32;
  }
// working arrays to store rndm numbers,samples
  m_Samples.resize(m_NSamples);
  if(m_doDigiTruth) {
    m_Samples_DigiHSTruth.resize(m_NSamples);
  }

  m_Noise.resize(m_NSamples);


// register data handle for conditions data

  ATH_CHECK(m_xtalkKey.initialize() );

  ATH_CHECK(m_cablingKey.initialize());

  ATH_CHECK(m_hitMapKey.initialize());
  ATH_CHECK(m_hitMapKey_DigiHSTruth.initialize(m_doDigiTruth));

  // decide sample to use for gain selection
  //   It is sample 2 (starting from 0) by default when we start from beginning of pulse shape
  //   Then shift by firstSample
  //    Note: this logic could be improved by looking at the true maximum of the full pulse shape instead, right now we
  //     implicitely assume that all MC 32-samples pulse shape have always the max at sample 2......
  m_sampleGainChoice = 2 - m_firstSample;
  if (m_sampleGainChoice<0) m_sampleGainChoice=0;

  //
  // ..... get OFC pointer for overlay case

  ATH_MSG_DEBUG("Initialization completed successfully");

  return StatusCode::SUCCESS;

}

// ----------------------------------------------------------------------------------------------------------------------------------

StatusCode LArPileUpTool::prepareEvent(const EventContext& ctx, unsigned int /*nInputEvents */)
{

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey, ctx};
  m_cabling=*cablingHdl;
  if(!m_cabling) {
     ATH_MSG_ERROR("Failed to retrieve LAr Cabling map with key " << m_cablingKey.key() );
     return StatusCode::FAILURE;
  }

  SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey,ctx};
  const CaloDetDescrManager* caloDDMgr = *caloMgrHandle;


  m_hitmap=SG::makeHandle(m_hitMapKey, ctx);
  auto hitMapPtr=std::make_unique<LArHitEMap>(m_cabling,m_calocell_id,caloDDMgr,m_RndmEvtOverlay);
  ATH_CHECK(m_hitmap.record(std::move(hitMapPtr)));
  ATH_MSG_DEBUG(" Number of created  cells in Map " << m_hitmap->GetNbCells());

  if (!m_useMBTime) m_energySum.assign(m_hitmap->GetNbCells(),0.);

  
  if (m_doDigiTruth) {
    m_hitmap_DigiHSTruth=SG::makeHandle(m_hitMapKey_DigiHSTruth, ctx);
    auto hitMapPtr=std::make_unique<LArHitEMap>(m_cabling,m_calocell_id,caloDDMgr,m_RndmEvtOverlay);
    ATH_CHECK(m_hitmap_DigiHSTruth.record(std::move(hitMapPtr)));
    if (!m_useMBTime) m_energySum_DigiHSTruth.assign(m_hitmap_DigiHSTruth->GetNbCells(),0.);
  }

  // get the trigger time if requested

  m_trigtime=0;
  if (m_useTriggerTime && m_triggerTimeTool) {
     m_trigtime = m_triggerTimeTool->time();
     ATH_MSG_DEBUG(" Trigger time used : " << m_trigtime);
  }

  ATHRNG::RNGWrapper* rngWrapper = m_rndmGenSvc->getEngine(this, m_randomStreamName);
  ATHRNG::RNGWrapper::SeedingOptionType seedingmode=m_useLegacyRandomSeeds ? ATHRNG::RNGWrapper::MC16Seeding : ATHRNG::RNGWrapper::SeedingDefault;
  rngWrapper->setSeedLegacy( m_randomStreamName, ctx, m_randomSeedOffset, seedingmode );

  // add random phase (i.e subtract it from trigtime)
  if (m_addPhase) {
    m_trigtime = m_trigtime - (m_phaseMin + (m_phaseMax-m_phaseMin)*RandFlat::shoot(rngWrapper->getEngine(ctx))  );
  }

  if (m_Windows) {
    ATH_MSG_DEBUG(" redefine windows list ");

    // FIXME: Use a ReadHandle.
    const McEventCollection* mcCollptr = nullptr;
    if ( evtStore()->retrieve(mcCollptr).isFailure() ) {
      ATH_MSG_WARNING ("LArHitEMap:cannot retrieve McEventCollection  (keyless)");
    }
    
    m_hitmap->BuildWindows(mcCollptr,
                           m_WindowsEtaSize,m_WindowsPhiSize,
                           m_WindowsPtCut);
    if(m_doDigiTruth) {
      m_hitmap_DigiHSTruth->BuildWindows(mcCollptr,
                                         m_WindowsEtaSize,m_WindowsPhiSize, m_WindowsPtCut);
    }

  }

  //
  // ....... create the LAr Digit Container
  //
  

  m_nhit_tot = 0;
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------------------------------------------------------

StatusCode LArPileUpTool::processBunchXing(int bunchXing,
                                           SubEventIterator bSubEvents,
                                           SubEventIterator eSubEvents)
{
  ATH_MSG_VERBOSE ( "processBunchXing()" );
  float tbunch = (float)(bunchXing);
  const EventContext& ctx = Gaudi::Hive::currentContext();
  SG::ReadCondHandle<LArXTalkWeightGlobal> weightHdl(m_xtalkKey, ctx);
  const LArXTalkWeightGlobal& weights = **weightHdl;

//
// ............ loop over the wanted hit containers
//
  SubEventIterator iEvt(bSubEvents);
  while (iEvt != eSubEvents) {

    // do we deal with the MC signal event ?
    bool isSignal = ( (iEvt->type()==xAOD::EventInfo_v1::PileUpType::Signal) || m_RndmEvtOverlay);

    // fill LArHits in map
    if (this->fillMapFromHit(iEvt, tbunch,isSignal, weights).isFailure()) {

      ATH_MSG_ERROR(" cannot fill map from hits ");
      return StatusCode::FAILURE;
    }

    // store digits from randoms for overlay
    if (m_RndmEvtOverlay) {
      const LArDigitContainer* rndm_digit_container;
      if (m_mergeSvc->retrieveSingleSubEvtData(m_inputDigitContainerKey.key(), rndm_digit_container, bunchXing, iEvt).isSuccess()) {
	int ndigit=0;
	for (const LArDigit* digit : *rndm_digit_container) {
	  if (m_hitmap->AddDigit(digit)) ndigit++;
	}
	ATH_MSG_INFO(" Number of digits stored for RndmEvt Overlay " << ndigit);
      }
    }

    ++iEvt;

  }

  if (!m_useMBTime) {
      if (!this->fillMapfromSum(tbunch)) {
         ATH_MSG_ERROR(" error in FillMapFromSum ");
         return StatusCode::FAILURE;
      }
  }


  return StatusCode::SUCCESS;

}

// ---------------------------------------------------------------------------------------------------------------------------------

StatusCode LArPileUpTool::processAllSubEvents(const EventContext& ctx)
{

  if (this->prepareEvent(ctx, 0).isFailure()) {
     ATH_MSG_ERROR("error in prepareEvent");
     return StatusCode::FAILURE;
  }

  SG::ReadCondHandle<LArXTalkWeightGlobal> weightHdl(m_xtalkKey, ctx);
  const LArXTalkWeightGlobal& weights = **weightHdl;

  if(!m_onlyUseContainerName && m_RndmEvtOverlay) {
    auto hitVectorHandles = m_hitContainerKeys.makeHandles(ctx);
    for (auto & inputHits : hitVectorHandles) {
      if (!inputHits.isValid()) {
        ATH_MSG_ERROR("BAD HANDLE"); //FIXME improve error here
        return StatusCode::FAILURE;
      }
      bool isSignal(true);
      double SubEvtTimOffset(0.0);
      double timeCurrBunch=-9999999.;
      for (const LArHit* hit : *inputHits) {
        m_nhit_tot++;
        float energy = (float) (hit->energy());
        float time;
        if (m_ignoreTime && isSignal) time=0.;
        else time = (float) (SubEvtTimOffset+ hit->time() -m_trigtime);
        Identifier cellId = hit->cellID();
        if (!m_useMBTime) {
          if (std::fabs(SubEvtTimOffset-timeCurrBunch)>1.) {
            if (timeCurrBunch>-9999.) {
              if (!this->fillMapfromSum(timeCurrBunch)) {
                ATH_MSG_ERROR(" error in FillMapFromSum ");
                return(StatusCode::FAILURE);
              }
            }
            timeCurrBunch = SubEvtTimOffset;
          }
        }
        if (this->AddHit(cellId,energy,time,isSignal,weights).isFailure()) return StatusCode::FAILURE;
      } // End of loop over LArHitContainer
    } // End of loop over SG::ReadHandles
  }

  if (!m_PileUp) {
    float time=0.;
    if (this->fillMapFromHit(ctx, time,true,weights).isFailure()) {
      ATH_MSG_ERROR("error in fillMapFromHit");
      return StatusCode::FAILURE;
    }
  }

  else {

    for (const std::string& containerName : m_hitContainerNames) {

      ATH_MSG_DEBUG(" pileUpOld asking for: " << containerName);

      double timeCurrBunch=-9999999.;

      if (!m_useLArHitFloat) {
        typedef PileUpMergeSvc::TimedList<LArHitContainer>::type TimedHitContList;
        TimedHitContList hitContList;
        //
        // retrieve list of pairs (time,container) from PileUp service

        if (!(m_mergeSvc->retrieveSubEvtsData(containerName
            ,hitContList).isSuccess()) && hitContList.size()==0) {
         ATH_MSG_ERROR("Could not fill TimedHitContList");
         return StatusCode::FAILURE;
        }

        // loop over this list
        TimedHitContList::iterator iFirstCont(hitContList.begin());
        TimedHitContList::iterator iEndCont(hitContList.end());
        if(m_RndmEvtOverlay) {
          iEndCont = iFirstCont ;
          ATH_MSG_DEBUG(" random event overlay mode : only time 0 read ");
          ++iEndCont ;
        }
        double SubEvtTimOffset;
        while (iFirstCont != iEndCont) {
        // get time for this subevent
        // new coding of time information (January 05)
          bool isSignal = ( iFirstCont == hitContList.begin());
          const PileUpTimeEventIndex* time_evt = &(iFirstCont->first);
          SubEvtTimOffset = time_evt->time();
          // get LArHitContainer for this subevent
          const LArHitContainer& firstCont = *(iFirstCont->second);
          // Loop over cells in this LArHitContainer
          for (const LArHit* hit : firstCont) {
            m_nhit_tot++;
            float energy = (float) (hit->energy());
            float time;
            if (m_ignoreTime && isSignal) time=0.;
            else time = (float) (SubEvtTimOffset+ hit->time() -m_trigtime);
            Identifier cellId = hit->cellID();

            if (!m_useMBTime) {
              if (std::fabs(SubEvtTimOffset-timeCurrBunch)>1.) {
                 if (timeCurrBunch>-9999.) {
                    if (!this->fillMapfromSum(timeCurrBunch)) {
                        ATH_MSG_ERROR(" error in FillMapFromSum ");
                        return(StatusCode::FAILURE);
                    }
                  }
                  timeCurrBunch = SubEvtTimOffset;
              }
            }
            if (this->AddHit(cellId,energy,time,isSignal,weights).isFailure()) return StatusCode::FAILURE;
          }              //  loop over  hits
          ++iFirstCont;
        }                 // loop over subevent list
      }

      else {

        typedef PileUpMergeSvc::TimedList<LArHitFloatContainer>::type TimedHitContList;
        TimedHitContList hitContList;
        //
        // retrieve list of pairs (time,container) from PileUp service

        if (!(m_mergeSvc->retrieveSubEvtsData(containerName
              ,hitContList).isSuccess()) && hitContList.size()==0) {
         ATH_MSG_ERROR("Could not fill TimedHitContList");
         return StatusCode::FAILURE;
        }

        // loop over this list
        TimedHitContList::iterator iFirstCont(hitContList.begin());
        TimedHitContList::iterator iEndCont(hitContList.end());
        if(m_RndmEvtOverlay) {
          iEndCont = iFirstCont ;
          ATH_MSG_DEBUG("random event overlay mode : only time 0 read ");
          ++iEndCont ;
        }
        double SubEvtTimOffset;
        while (iFirstCont != iEndCont) {
          bool isSignal = ( iFirstCont == hitContList.begin());
          // get time for this subevent
          // new coding of time information (January 05)
          const PileUpTimeEventIndex* time_evt = &(iFirstCont->first);
          SubEvtTimOffset = time_evt->time();
          // get LArHitContainer for this subevent
          const LArHitFloatContainer& firstCont = *(iFirstCont->second);
          // Loop over cells in this LArHitContainer
          for (const LArHitFloat& hit : firstCont) {
            m_nhit_tot++;
            float energy = (float)( hit.energy());
            float time;
            if (m_ignoreTime && isSignal) time=0.;
            else time = (float) (SubEvtTimOffset+ hit.time() - m_trigtime);
            Identifier cellId = hit.cellID();

            if (!m_useMBTime) {
              if (std::fabs(SubEvtTimOffset-timeCurrBunch)>1.) {
                 if (timeCurrBunch>-9999.) {
                    if (!this->fillMapfromSum(timeCurrBunch)) {
                        ATH_MSG_ERROR(" error in FillMapFromSum ");
                        return(StatusCode::FAILURE);
                    }
                  }
                  timeCurrBunch = SubEvtTimOffset;
              }
            }
            if (this->AddHit(cellId,energy,time,isSignal,weights).isFailure()) return StatusCode::FAILURE;
          }              //  loop over  hits
          ++iFirstCont;
        }                 // loop over subevent list

      }  // LArHitFloat vs LArHit useage

      if (!m_useMBTime) {
        if (!this->fillMapfromSum(timeCurrBunch)) {
             ATH_MSG_ERROR(" error in FillMapFromSum ");
             return(StatusCode::FAILURE);
        }
      }

    }    // loop over containers

    // get digits for random overlay
    if(m_RndmEvtOverlay)
    {
      if (!m_onlyUseContainerName)
      {
        SG::ReadHandle<LArDigitContainer> digitCollection(m_inputDigitContainerKey, ctx);
        if (!digitCollection.isValid()) {
          ATH_MSG_ERROR("Could not get LArDigitContainer container " << digitCollection.name() << " from store " << digitCollection.store());
          return StatusCode::FAILURE;
        }

        ATH_MSG_DEBUG("LArDigitContainer found with " << digitCollection->size() << " digits");

        size_t ndigit{};
        for (const LArDigit* digit : *digitCollection) {
          if (m_hitmap->AddDigit(digit)) ndigit++;
        }
        ATH_MSG_DEBUG(" Number of digits stored for RndmEvt Overlay " << ndigit);
      }
      else
      {
        typedef PileUpMergeSvc::TimedList<LArDigitContainer>::type TimedDigitContList ;
        LArDigitContainer::const_iterator rndm_digititer_begin ;
        LArDigitContainer::const_iterator rndm_digititer_end ;
        LArDigitContainer::const_iterator rndm_digititer ;


        TimedDigitContList digitContList;
        if (!(m_mergeSvc->retrieveSubEvtsData(m_inputDigitContainerKey.key(),
            digitContList).isSuccess()) || digitContList.size()==0)
        {
          ATH_MSG_ERROR("Cannot retrieve LArDigitContainer for random event overlay or empty Container");
          ATH_MSG_ERROR("Random Digit Key= " << m_inputDigitContainerKey.key() << ",size=" << digitContList.size());
          return StatusCode::FAILURE ;
        }
        TimedDigitContList::iterator iTzeroDigitCont(digitContList.begin()) ;
        double SubEvtTimOffset;
        // get time for this subevent
        const PileUpTimeEventIndex* time_evt = &(iTzeroDigitCont->first);
        SubEvtTimOffset = time_evt->time();
        ATH_MSG_DEBUG(" Subevt time : " << SubEvtTimOffset);
        const LArDigitContainer& rndm_digit_container =  *(iTzeroDigitCont->second);
        int ndigit=0;
        for (const LArDigit* digit : rndm_digit_container) {
        if (m_hitmap->AddDigit(digit)) ndigit++;
        }
        ATH_MSG_INFO(" Number of digits stored for RndmEvt Overlay " << ndigit);
      }
    }

  }  // if pileup

  return StatusCode::SUCCESS;

}

// ============================================================================================

StatusCode LArPileUpTool::fillMapFromHit(const EventContext& ctx, float bunchTime, bool isSignal, const LArXTalkWeightGlobal& weights)
{
  if (m_useLArHitFloat) {
    auto hitVectorHandles = m_hitFloatContainerKeys.makeHandles(ctx);
    for (auto & hit_container : hitVectorHandles) {
      if (hit_container.isValid()) {
        for (const LArHitFloat& hit : *hit_container)
          {
            m_nhit_tot++;
            Identifier cellId = hit.cellID();
            float energy = (float) hit.energy();
            float time;
            if (m_ignoreTime) time=0.;
            else time   = (float) (hit.time() - m_trigtime);
            time = time + bunchTime;
            if (this->AddHit(cellId,energy,time,isSignal,weights).isFailure()) return StatusCode::FAILURE;
          }
      }
      else {
        if (isSignal)  {
          ATH_MSG_WARNING(" LAr HitFloat container not found for signal event key " << hit_container.key());
        }
      }
    }
  }
  else {
    auto hitVectorHandles = m_hitContainerKeys.makeHandles(ctx);
    for (auto & hit_container : hitVectorHandles) {
      if (hit_container.isValid()) {
        for (const LArHit* hit : *hit_container)
          {
            m_nhit_tot++;
            Identifier cellId = hit->cellID();
            float energy = (float) hit->energy();
            float time;
            if (m_ignoreTime) time=0.;
            else time   = (float) (hit->time() - m_trigtime);
            time = time + bunchTime;
            if (this->AddHit(cellId,energy,time,isSignal,weights).isFailure()) return StatusCode::FAILURE;
          }
      }
      else {
        if (isSignal)  {
          ATH_MSG_WARNING(" LAr Hit container not found for signal event key " << hit_container.key());
        }
      }
    }
  }   // end loop over containers

  return StatusCode::SUCCESS;
}

// ============================================================================================
StatusCode LArPileUpTool::fillMapFromHit(SubEventIterator iEvt, float bunchTime, bool isSignal, const LArXTalkWeightGlobal& weights)
{
  for (const std::string& containerName : m_hitContainerNames) {

  //
  // ..... Get the pointer to the Hit Container from StoreGate through the merge service
  //

    ATH_MSG_DEBUG(" fillMapFromHit: asking for: " << containerName);

    if (m_useLArHitFloat) {

      const LArHitFloatContainer * hit_container;

      if (!(m_mergeSvc->retrieveSingleSubEvtData(containerName, hit_container, bunchTime,
						 iEvt).isSuccess())){
	ATH_MSG_ERROR(" LAr Hit container not found for event key " << containerName);
	return StatusCode::FAILURE;
      }

      for (const LArHitFloat& hit : *hit_container)
	{
	  m_nhit_tot++;
	  Identifier cellId = hit.cellID();
	  float energy = (float) hit.energy();
	  float time;
	  if (m_ignoreTime) time=0.;
	  else time   = (float) (hit.time() - m_trigtime);
	  time = time + bunchTime;

         if (this->AddHit(cellId,energy,time,isSignal,weights).isFailure()) return StatusCode::FAILURE;
	}
    }
    else {

      const LArHitContainer * hit_container;

      if (!(m_mergeSvc->retrieveSingleSubEvtData(containerName, hit_container, bunchTime,
                                             iEvt).isSuccess())){
	ATH_MSG_ERROR(" LAr Hit container not found for event key " << containerName);
	return StatusCode::FAILURE;
      }

      LArHitContainer::const_iterator hititer;
      for(hititer=hit_container->begin();
	  hititer != hit_container->end();++hititer)
	{
	  m_nhit_tot++;
	  Identifier cellId = (*hititer)->cellID();
	  float energy = (float) (*hititer)->energy();
	  float time;
	  if (m_ignoreTime) time=0.;
	  else time   = (float) ((*hititer)->time() - m_trigtime);
	  time = time + bunchTime;

         if (this->AddHit(cellId,energy,time,isSignal,weights).isFailure()) return StatusCode::FAILURE;
	}
    }
  }   // end loop over containers

  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------------------------------------------------------

StatusCode LArPileUpTool::AddHit(const Identifier cellId, const float energy, const float time, const bool isSignal, const LArXTalkWeightGlobal& weights)
{

  // remove pathological energies...
  if (std::fabs(energy)>1e+9) {
      ATH_MSG_WARNING(" Pathological energy ignored Id= "<< m_larem_id->show_to_string(cellId) << "  energy= " << energy );
      return StatusCode::SUCCESS;
  }

#ifndef NDEBUG
  ATH_MSG_DEBUG(" Found hit  Id= "<< m_larem_id->show_to_string(cellId)<< "  energy= " << energy << "(MeV)  time= " << time << "(ns)");
#endif

  IdentifierHash idHash=m_calocell_id->calo_cell_hash(cellId);

// simulation of cross talk if requested (EM barrel + EndCap)
  if (m_CrossTalk && isSignal && m_calocell_id->is_em(cellId)) 
  {
       std::vector<IdentifierHash> neighbourList;
       std::vector<float> energyList;
       //bool dump=false;
       //if (energy>200. || m_larem_id->sampling(cellId)==3) dump=true;
       //if(dump) std::cout << " Input cell energy " << m_larem_id->show_to_string(cellId) << " " << energy  << std::endl;
       this->cross_talk(idHash,cellId,energy,  //FIXME -> Needs to work with full hash! 
                        neighbourList,energyList, weights);
       //if(dump) std::cout <<" After cross-talk " ;
       for (unsigned int icell=0;icell<neighbourList.size();icell++)
       {
         //unsigned int index=neighbourList[icell];
	 //Turn sub-calo hash in neighbour list into gloabl calo-cell hash:
	 const IdentifierHash index=m_calocell_id->calo_cell_hash(m_calocell_id->sub_calo(cellId),neighbourList[icell]);
         float e = energyList[icell];
         //Identifier id2=m_larem_id->channel_id(neighbourList[icell]);
         //if(dump) std::cout << "Cell/E " <<  m_larem_id->show_to_string(id2) << " " << e << " ";
         if ( !m_hitmap->AddEnergy(index,e,time) )
         {
             ATH_MSG_ERROR("  Cell " << m_larem_id->show_to_string(cellId) << " could not add the energy= " << energy  << " (GeV)");
             return(StatusCode::FAILURE);
         }
         if ( m_doDigiTruth){ 
           if(!m_hitmap_DigiHSTruth->AddEnergy(index,e,time) ) {
             ATH_MSG_ERROR("  Cell " << m_larem_id->show_to_string(cellId) << " could not add the energy= " << energy  << " (GeV)");
             return(StatusCode::FAILURE);
           }
        }
       }
       //if (dump) std::cout << std::endl;
  }
  else    // no cross-talk simulated
  {
      if (isSignal || m_useMBTime)
      {
        if ( !m_hitmap->AddEnergy(idHash,energy,time) )
        {
          ATH_MSG_ERROR("  Cell " << m_larem_id->show_to_string(cellId) << " could not add the energy= " << energy  << " (GeV)");
          return(StatusCode::FAILURE);
         }
         if ( m_doDigiTruth){ 
          if(!m_hitmap_DigiHSTruth->AddEnergy(idHash,energy,time) ) {
             ATH_MSG_ERROR("  Cell " << m_larem_id->show_to_string(cellId) << " could not add the energy= " << energy  << " (GeV)");
             return(StatusCode::FAILURE);
          }
        }
      }
      else
      {
        if (idHash<m_energySum.size()) m_energySum[idHash] += energy;
      }
  }     // end if cross-talk
  return StatusCode::SUCCESS;
}

// -------------------------------------------------------------------------------------------------------------------------------------

void LArPileUpTool::cross_talk(const IdentifierHash& hashId,
                                const Identifier& cellId,
                                const float& energy,
                                std::vector<IdentifierHash>& neighbourList,
                                std::vector<float>& energyList,
				const LArXTalkWeightGlobal& weights)
{
  neighbourList.clear();
  energyList.clear();
  int result=0;
  neighbourList.reserve(8);
  energyList.reserve(8);


  int ibec     = abs(m_larem_id->barrel_ec(cellId));   // 1 barrel 2 EC OW  3 EC IW
  int sampling = m_larem_id->sampling(cellId);
  int eta      = m_larem_id->eta(cellId);
  int region   = m_larem_id->region(cellId);
  float fcr=0,e,er;
  float fcr2=0.;
  std::vector<IdentifierHash> tmpList;

  er=energy;    // total energy of hit to be spread among channels

// cross-talk in strips
  if ((ibec==1 && sampling == 1 && region == 0)
      || (ibec==2 && sampling ==1) )
  {

     if (ibec==1) fcr2 = weights.get_xtalk(LArXTalkWeightGlobal::TWOSTRIP,eta);
     if (ibec==2) fcr2 = weights.get_xtalk(LArXTalkWeightGlobal::TWOSTRIP_EC,region,eta);

// next in eta
     if ( (ibec==1 && eta !=447) || (ibec==2 && (eta!=3 || region !=5)) )
     {
       result=m_larem_id->get_neighbours(hashId,
                  LArNeighbours::nextInEta,tmpList);
       if(ibec==1) fcr = weights.get_xtalk(LArXTalkWeightGlobal::STRIP,eta+1)*m_scaleStripXtalk; 
       if(ibec==2) fcr = weights.get_xtalk(LArXTalkWeightGlobal::STRIP_EC,region,eta+1)*m_scaleStripXtalk; 

       if (result==0) {
         if (tmpList.size() == 1) {
            e=energy*fcr;
            er=er-e;
            neighbourList.push_back(tmpList[0]);
            energyList.push_back(e);

// second neighbor cross-talk
            if ((   (ibec==1 && eta !=446)
                  ||(ibec==2 && (eta!=2 || region !=5)) )  && m_CrossTalk2Strip) {
                std::vector<IdentifierHash> tmpList2;
                result = m_larem_id->get_neighbours(tmpList[0],LArNeighbours::nextInEta,tmpList2);
                if (result==0) {
                   if (tmpList2.size()==1) {
                     e=energy*fcr2;
                     er=er-e;
                     neighbourList.push_back(tmpList2[0]);
                     energyList.push_back(e);
                   }
                }
            }

         }
       }
     }
// prev in eta (if possible)
     if ( (ibec==1 && eta >1) || (ibec==2 && (eta !=0 || region !=0)) )
     {
       result=m_larem_id->get_neighbours(hashId,
                  LArNeighbours::prevInEta,tmpList);
       if(ibec==1) fcr =  weights.get_xtalk(LArXTalkWeightGlobal::STRIP,eta)*m_scaleStripXtalk;
       if(ibec==2) fcr =  weights.get_xtalk(LArXTalkWeightGlobal::STRIP_EC,region,eta)*m_scaleStripXtalk; 
       if (result==0 ) {
         if (tmpList.size() == 1) {
            e=energy*fcr;
            er=er-e;
            neighbourList.push_back(tmpList[0]);
            energyList.push_back(e);

// second neighbor cross-talk
            if ((   (ibec==1 && eta !=2)
                  ||(ibec==2 && (eta!=1 || region !=0)) ) && m_CrossTalk2Strip) {
                std::vector<IdentifierHash> tmpList2;
                result = m_larem_id->get_neighbours(tmpList[0],LArNeighbours::prevInEta,tmpList2);
                if (result==0) {
                   if (tmpList2.size()==1) {
                     e=energy*fcr2;
                     er=er-e;
                     neighbourList.push_back(tmpList2[0]);
                     energyList.push_back(e);
                   }
                }
            }

         }
       }
     }
  }

// cross-talk strip to middle
  if (m_CrossTalkStripMiddle) {
    if ((ibec==1 && sampling==1 && region==0)
       || (ibec==2 && sampling==1) )
    {
      if (ibec==1) fcr = weights.get_xtalk(LArXTalkWeightGlobal::STRIPMIDDLE,eta)*m_scaleStripMiddle;
      if (ibec==2) fcr = weights.get_xtalk(LArXTalkWeightGlobal::STRIPMIDDLE_EC,region,eta)*m_scaleStripMiddle; 

      if (ibec==1) fcr = fcr*2.;  // 8 strips for 4 middle cells
      if (ibec==2) {
         if (region==0) fcr=fcr/4.;   // 1 strip for 4 middle cells
         if (region==1) fcr=fcr/4.;   // 1 strip for 4 middle cells
         if (region==2) fcr=fcr*2.;   // 8 strips for 4 middle cells
         if (region==3) fcr=fcr*1.5;  // 6 strips for 4 middle cells
                                      // (region 4:  4strips for 4 middle cells)
         if (region==5) fcr=fcr/4.;   // 1 strip for 4 middle cells
      }

      // next sampling  (should have only one cell)
      result = m_larem_id->get_neighbours(hashId, LArNeighbours::nextInSamp,tmpList);
      if (result==0) {
        e=energy*fcr;
        for (unsigned int ii=0;ii<tmpList.size();ii++) {
          er = er-e;
          neighbourList.push_back(tmpList[ii]);
          energyList.push_back(e);
        }
      }
    }

// cross-talk middle to strip
    if ((ibec==1 && sampling==2 && region==0)
        || (ibec==2 && sampling==2) )
    {
      // previous sampling, expect 8 channels in middle for barrel, varing number in end-cap
      result = m_larem_id->get_neighbours(hashId,
       LArNeighbours::prevInSamp,tmpList);
      if (result==0) {
        for (unsigned int ii=0;ii<tmpList.size();ii++) {
          Identifier stripId = m_larem_id->channel_id(tmpList[ii]);
          if (m_larem_id->sampling(stripId)==1) {
            neighbourList.push_back(tmpList[ii]);
            int eta2 = m_larem_id->eta(stripId);
            int region2 = m_larem_id->region(stripId);
            if (ibec==1) fcr=weights.get_xtalk(LArXTalkWeightGlobal::STRIPMIDDLE,eta2)*m_scaleStripMiddle; 
            if (ibec==2) fcr=weights.get_xtalk(LArXTalkWeightGlobal::STRIPMIDDLE_EC,region2,eta2)*m_scaleStripMiddle;
            e=energy*fcr;
            er=er-e;
            energyList.push_back(e);
          }
        }
      }
    }
  }  // strip middle crosstalk

// cross-talk middle to middle
  if (m_CrossTalkMiddle) {
    if ((ibec==1 && sampling==2 && region==0 ) ||
        (ibec==2 && sampling==2 && region==1)) {

     // fmiddle1 crosstalk to eta-1
     // fmiddle2 crosstalk to eta+1
     float fmiddle1=0.;
     float fmiddle2=0.;
     if (ibec==1) {
        fmiddle1 = weights.get_xtalk(LArXTalkWeightGlobal::MIDDLE1,eta)*m_scaleMiddleXtalk;
        fmiddle2 = weights.get_xtalk(LArXTalkWeightGlobal::MIDDLE2,eta)*m_scaleMiddleXtalk;
     }
     if (ibec==2) {
         fmiddle1 = weights.get_xtalk(LArXTalkWeightGlobal::MIDDLE1_EC,eta)*m_scaleMiddleXtalk;
         fmiddle2 = weights.get_xtalk(LArXTalkWeightGlobal::MIDDLE2_EC,eta)*m_scaleMiddleXtalk;
     }

     // next in eta
     if ( (ibec==1 && eta<55) || (ibec==2 && eta <42) ) {
       result=m_larem_id->get_neighbours(hashId,
                  LArNeighbours::nextInEta,tmpList);
       if (result==0) {
         if (tmpList.size() == 1) {
            e=energy*fmiddle2;
            er=er-e;
            neighbourList.push_back(tmpList[0]);
            energyList.push_back(e);
          }
       }
     }
     // previous in eta
     if ( (ibec==1 && eta>0) || (ibec==2 && eta >0) ) {
       result=m_larem_id->get_neighbours(hashId,
                  LArNeighbours::prevInEta,tmpList);
       if (result==0) {
         if (tmpList.size() == 1) {
            e=energy*fmiddle1;
            er=er-e;
            neighbourList.push_back(tmpList[0]);
            energyList.push_back(e);
          }
       }
     }
    }
  }

// cross-talk in middle to back
  if ( (ibec==1 && sampling ==2 && region == 0 )
      || (ibec==2 && sampling ==2 && region ==1 && eta > 2)  // no sampling 3 before 1.5
      || (ibec==3 && sampling ==1) )                         // inner wheel
  {
     if (ibec==1) {
      fcr = weights.get_xtalk(LArXTalkWeightGlobal::MIDDLEBACK,eta);
     } else if(ibec==2) {
      fcr = weights.get_xtalk(LArXTalkWeightGlobal::MIDDLEBACK_ECOW,eta);
     } else if(ibec==3) {
      fcr = weights.get_xtalk(LArXTalkWeightGlobal::MIDDLEBACK_ECIW,eta);  // same size of middle and back in IW
     }
// next sampling
     result=m_larem_id->get_neighbours(hashId,
      LArNeighbours::nextInSamp,tmpList);
     if (result==0) {
        if (tmpList.size() == 1) {
            e=energy*fcr;
            er=er-e;
            neighbourList.push_back(tmpList[0]);
            energyList.push_back(e);
        }
     }
  }

// cross-talk back to middle
  if ( (ibec==1 && sampling == 3 && region == 0 )
     ||(ibec==2 && sampling ==3)
     ||(ibec==3 && sampling ==2) )
  {
    if (ibec==1) {
     // eta2 = eta for middle layer cells
     int eta2=2*eta;
     fcr=0.5*(weights.get_xtalk(LArXTalkWeightGlobal::MIDDLEBACK,eta2)+weights.get_xtalk(LArXTalkWeightGlobal::MIDDLEBACK,eta2+1));
    } else if(ibec==2) {
     int eta2=3+2*eta;
     fcr=0.5*(weights.get_xtalk(LArXTalkWeightGlobal::MIDDLEBACK_ECOW,eta)+weights.get_xtalk(LArXTalkWeightGlobal::MIDDLEBACK,eta2+1));
    } else if(ibec==3) {
     fcr=weights.get_xtalk(LArXTalkWeightGlobal::MIDDLEBACK_ECIW,eta);
    }
// previous sampling, expect two channels in middle for barrel + OW, one for IW
    result = m_larem_id->get_neighbours(hashId,
       LArNeighbours::prevInSamp,tmpList);
    if (result==0) {
       if ((ibec==1 || ibec==2) && tmpList.size() == 2) {
          fcr=fcr/2.;
          e=energy*fcr;
          er=er-2.*e;
          neighbourList.push_back(tmpList[0]);
          neighbourList.push_back(tmpList[1]);
          energyList.push_back(e);
          energyList.push_back(e);
       }
       if (ibec==1 && tmpList.size()==1) {
          e=energy*fcr;
          er=er-e;
          neighbourList.push_back(tmpList[0]);
          energyList.push_back(e);
       }
       if (ibec==3 && tmpList.size() ==1) {
          e=energy*fcr;
          er=er-e;
          neighbourList.push_back(tmpList[0]);
          energyList.push_back(e);
       }
    }
  }

// remaining energy in original cell
  neighbourList.push_back(hashId);
  energyList.push_back(er);

}

// ----------------------------------------------------------------------------------------------------------------------------------


//
// take accumulated energy in cell vector for a given bunch time and push that in the hit map

bool LArPileUpTool::fillMapfromSum(float bunchTime)  {

  for (unsigned int i=0;i<m_energySum.size();i++) {
     float e = m_energySum[i];
     if (e>1e-6) {
        if (!m_hitmap->AddEnergy(i,e,bunchTime)) return false;
     }
     m_energySum[i]=0.;
  }
  if(m_doDigiTruth){
    for (unsigned int i=0;i<m_energySum_DigiHSTruth.size();i++) {
       float e = m_energySum_DigiHSTruth[i];
       if (e>1e-6) {
         if (!m_hitmap_DigiHSTruth->AddEnergy(i,e,bunchTime)) return false;
       }
       m_energySum_DigiHSTruth[i]=0.;
    }
  }

  return true;
}
