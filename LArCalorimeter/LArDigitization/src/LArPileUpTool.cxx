/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// +==========================================================================+
// +                                                                          +
// + Authors .......: G.Unal                                                  +
// +    migrate LAr digitization to PileUpTool framework                      +
// +==========================================================================+
//

#include "LArDigitization/LArPileUpTool.h"

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

  declareInterface<ILArPileUpTool>(this);

  // default properties
  m_LowGainThresh[EM]    = 3900;//ADC counts in MediumGain
  m_HighGainThresh[EM]   = 1300;//ADC counts in MediumGain
  m_LowGainThresh[HEC]   = 2500;//ADC counts in MediumGain
  m_HighGainThresh[HEC]  = 0;//-> high-gain never used for HEC
  m_LowGainThresh[FCAL]  = 2000.;//ADC counts in Medium Gain
  m_HighGainThresh[FCAL] = 1100.;//ADCcounts in MediumGain
  m_LowGainThresh[EMIW]    = 3900;//ADC counts in MediumGain
  m_HighGainThresh[EMIW]   = 1300;//ADC counts in MediumGain
  //
  // ........ declare the private data as properties
  //
  declareProperty("LowGainThreshEM",m_LowGainThresh[EM],"Medium/Low gain transition in EM");
  declareProperty("HighGainThreshEM",m_HighGainThresh[EM],"Medium/High gain transition in EM");
  declareProperty("LowGainThreshHEC",m_LowGainThresh[HEC],"Medium/Low gain transition in HEC");
  declareProperty("HighGainThreshHEC",m_HighGainThresh[HEC],"Medium/High gain transition in HEC");
  declareProperty("LowGainThreshFCAL",m_LowGainThresh[FCAL],"Medium/Low gain transition in FCAL");
  declareProperty("HighGainThreshFCAL",m_HighGainThresh[FCAL],"Medium/High gain transition in FCAL");
  declareProperty("LowGainThreshEMECIW",m_LowGainThresh[EMIW],"Medium/Low gain transition in EMEC IW");
  declareProperty("HighGainThreshEMECIW",m_HighGainThresh[EMIW],"Medium/High gain transition in EMEC IW");

  return;
}


LArPileUpTool::~LArPileUpTool()
{
  return;
}


StatusCode LArPileUpTool::initialize()
{
	
   ATH_MSG_INFO(" initialize LArPileUpTool : digit container name " << m_DigitContainerName);
  //
  // ........ print random event overlay flag
  //
  if (m_RndmEvtOverlay)
  {
    if (!m_isMcOverlay) m_NoiseOnOff = false ;
    else {
       ATH_MSG_INFO(" MC overlay case => switch back on noise only to emulate extra noise for cells with different gains");
       m_NoiseOnOff=true;
    }
    m_PileUp = true ;
    ATH_MSG_INFO(" pileup and/or noise added by overlaying digits of random events");
    if (m_isMcOverlay) ATH_MSG_INFO("   random events are from MC ");
    else               ATH_MSG_INFO("   random events are from data ");
  }
  else
  {
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

  ATH_CHECK(m_adc2mevKey.initialize());

  // retrieve tool to compute sqrt of time correlation matrix
  ATH_CHECK(m_autoCorrNoiseKey.initialize(!m_RndmEvtOverlay && m_NoiseOnOff));

  ATH_CHECK(m_bcContKey.initialize());
  ATH_CHECK(m_badFebKey.initialize());

  ATH_CHECK(m_bcMask.buildBitMask(m_problemsToMask,msg()));  

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

  ATH_CHECK(m_noiseKey.initialize((!m_RndmEvtOverlay || m_isMcOverlay) && !m_pedestalNoise && m_NoiseOnOff));

  ATH_CHECK(m_shapeKey.initialize());
  ATH_CHECK(m_fSamplKey.initialize());
  ATH_CHECK(m_pedestalKey.initialize());

  ATH_CHECK(m_xtalkKey.initialize() );

  ATH_CHECK(m_cablingKey.initialize());

  ATH_CHECK(m_hitMapKey.initialize());
  ATH_CHECK(m_hitMapKey_DigiHSTruth.initialize(m_doDigiTruth));

  ATH_CHECK(m_DigitContainerName.initialize());
  ATH_CHECK(m_DigitContainerName_DigiHSTruth.initialize(m_doDigiTruth));
  


  // decide sample to use for gain selection
  //   It is sample 2 (starting from 0) by default when we start from beginning of pulse shape
  //   Then shift by firstSample
  //    Note: this logic could be improved by looking at the true maximum of the full pulse shape instead, right now we
  //     implicitely assume that all MC 32-samples pulse shape have always the max at sample 2......
  m_sampleGainChoice = 2 - m_firstSample;
  if (m_sampleGainChoice<0) m_sampleGainChoice=0;

  //
  // ..... get OFC pointer for overlay case
  ATH_CHECK(m_OFCKey.initialize(m_RndmEvtOverlay  && !m_isMcOverlay));

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
  
  m_DigitContainer = SG::makeHandle(m_DigitContainerName, ctx);//new LArDigitContainer();
  ATH_CHECK(m_DigitContainer.record(std::make_unique<LArDigitContainer>()));

  if (m_doDigiTruth) {
    m_DigitContainer_DigiHSTruth = SG::makeHandle(m_DigitContainerName_DigiHSTruth, ctx);//new LArDigitContainer();
    ATH_CHECK(m_DigitContainer_DigiHSTruth.record(std::make_unique<LArDigitContainer>()));
  }

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

//
// ............ loop over the wanted hit containers
//
  SubEventIterator iEvt(bSubEvents);
  while (iEvt != eSubEvents) {

    // do we deal with the MC signal event ?
    bool isSignal = ( (iEvt->type()==xAOD::EventInfo_v1::PileUpType::Signal) || m_RndmEvtOverlay);

    // fill LArHits in map
    if (this->fillMapFromHit(iEvt, tbunch,isSignal).isFailure()) {

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

  const LArXTalkWeightGlobal* weights = pointerFromKey<LArXTalkWeightGlobal>(ctx,m_xtalkKey);

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
        if (this->AddHit(cellId,energy,time,isSignal,*weights).isFailure()) return StatusCode::FAILURE;
      } // End of loop over LArHitContainer
    } // End of loop over SG::ReadHandles
  }

  if (!m_PileUp) {
    float time=0.;
    StoreGateSvc* myEvtStore = &(*evtStore());
    if (this->fillMapFromHit(myEvtStore,time,true).isFailure()) {
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
            if (this->AddHit(cellId,energy,time,isSignal,*weights).isFailure()) return StatusCode::FAILURE;
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
            if (this->AddHit(cellId,energy,time,isSignal,*weights).isFailure()) return StatusCode::FAILURE;
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

  if (this->mergeEvent(ctx).isFailure()) {
    ATH_MSG_ERROR("error in mergeEvent");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

// ---------------------------------------------------------------------------------------------------------------------------------


StatusCode LArPileUpTool::mergeEvent(const EventContext& ctx)
{
   SG::ReadCondHandle<LArBadFebCont> badFebHdl(m_badFebKey, ctx);
   const LArBadFebCont* badFebs=*badFebHdl;

   int it,it_end;
   it =  0;
   it_end = m_hitmap->GetNbCells();

   const std::vector<std::pair<float,float> >* TimeE;
   const std::vector<std::pair<float,float> >* TimeE_DigiHSTruth = nullptr;

   ATHRNG::RNGWrapper* rngWrapper = m_rndmGenSvc->getEngine(this, m_randomStreamName);
   CLHEP::HepRandomEngine * engine = rngWrapper->getEngine(ctx);

   for( ; it!=it_end;++it) // now loop on cells
   {
      const LArHitList& hitlist = m_hitmap->GetCell(it);
      
      if (!m_Windows || hitlist.inWindows()) {
	TimeE = &(hitlist.getData());
	if(m_doDigiTruth) {
	  auto& hitlist_DigiHSTruth=m_hitmap_DigiHSTruth->GetCell(it);
	  TimeE_DigiHSTruth = &(hitlist_DigiHSTruth.getData());
	}

	if (TimeE->size() > 0 || m_NoiseOnOff || m_RndmEvtOverlay) {
	  const Identifier cellID=m_calocell_id->cell_id(IdentifierHash(it));
	  HWIdentifier ch_id = m_cabling->createSignalChannelIDFromHash(IdentifierHash(it));
	  HWIdentifier febId = m_laronline_id->feb_Id(ch_id);
	  bool missing=!(badFebs->status(febId).good());
	  if (!missing) {
	    const LArDigit * digit = 0 ;
	    if(m_RndmEvtOverlay) digit = m_hitmap->GetDigit(it);
	    // MakeDigit called if in no overlay mode or
	    // if in overlay mode and random digit exists
	    if( (!m_RndmEvtOverlay) || (m_RndmEvtOverlay && digit) ) {
              if ( this->MakeDigit(ctx,cellID, ch_id,TimeE, digit, engine, TimeE_DigiHSTruth)
		   == StatusCode::FAILURE ) return StatusCode::FAILURE;
	    }
	  }
	}
      }     // check window
   }        // end of loop over the cells


  ATH_MSG_DEBUG(" total number of hits found= " << m_nhit_tot);
  ATH_MSG_DEBUG(" number of created digits  = " << m_DigitContainer->size());

  return StatusCode::SUCCESS;

}

// ============================================================================================

StatusCode LArPileUpTool::fillMapFromHit(StoreGateSvc* myStore, float bunchTime, bool isSignal)
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  const LArXTalkWeightGlobal* weights = pointerFromKey<LArXTalkWeightGlobal>(ctx,m_xtalkKey);
  for (const std::string& containerName : m_hitContainerNames) {

  //
  // ..... Get the pointer to the Hit Container from StoreGate
  //
    ATH_MSG_DEBUG(" fillMapFromHit: asking for: " << containerName);

    if (m_useLArHitFloat) {
     const LArHitFloatContainer* hit_container = nullptr;
     if (myStore->contains<LArHitFloatContainer>(containerName)) {
       StatusCode sc = myStore->retrieve( hit_container,containerName ) ;
       if (sc.isFailure() || !hit_container) {
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

         if (this->AddHit(cellId,energy,time,isSignal,*weights).isFailure()) return StatusCode::FAILURE;
       }
     }
     else {
      if (isSignal)  {
         ATH_MSG_WARNING(" LAr HitFloat container not found for signal event key " << containerName);
      }
     }

    }
    else {
     const LArHitContainer* hit_container = nullptr;
     if (myStore->contains<LArHitContainer>(containerName)) {
       StatusCode sc = myStore->retrieve( hit_container,containerName ) ;
       if (sc.isFailure() || !hit_container) {
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

         if (this->AddHit(cellId,energy,time,isSignal,*weights).isFailure()) return StatusCode::FAILURE;
       }
     }
     else {
      if (isSignal)  {
         ATH_MSG_WARNING(" LAr Hit container not found for signal event key " << containerName);
      }
     }
    }
  }   // end loop over containers

  return StatusCode::SUCCESS;
}

// ============================================================================================
StatusCode LArPileUpTool::fillMapFromHit(SubEventIterator iEvt, float bunchTime, bool isSignal)
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  const LArXTalkWeightGlobal* weights = pointerFromKey<LArXTalkWeightGlobal>(ctx,m_xtalkKey);
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

         if (this->AddHit(cellId,energy,time,isSignal,*weights).isFailure()) return StatusCode::FAILURE;
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

         if (this->AddHit(cellId,energy,time,isSignal,*weights).isFailure()) return StatusCode::FAILURE;
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


StatusCode LArPileUpTool::MakeDigit(const EventContext& ctx, const Identifier & cellId,
                                    HWIdentifier & ch_id,
                                    const std::vector<std::pair<float,float> >* TimeE,
                                    const LArDigit * rndmEvtDigit, CLHEP::HepRandomEngine * engine,
				    const std::vector<std::pair<float,float> >* TimeE_DigiHSTruth)

{
  bool createDigit_DigiHSTruth = true;

  int i;
  short Adc;
  short Adc_DigiHSTruth;

  CaloGain::CaloGain igain;
  std::vector<short> AdcSample(m_NSamples);
  std::vector<short> AdcSample_DigiHSTruth(m_NSamples);

  float MeV2GeV=0.001;   // to convert hit from MeV to GeV before apply GeV->ADC

  float SF=1.;
  float SigmaNoise;
  std::vector<float> rndm_energy_samples(m_NSamples) ;


  SG::ReadCondHandle<LArADC2MeV> adc2mevHdl(m_adc2mevKey, ctx);
  const LArADC2MeV* adc2MeVs=*adc2mevHdl;

  SG::ReadCondHandle<ILArfSampl> fSamplHdl(m_fSamplKey, ctx);
  const ILArfSampl* fSampl=*fSamplHdl;

  SG::ReadCondHandle<ILArPedestal> pedHdl(m_pedestalKey, ctx);
  const ILArPedestal* pedestal=*pedHdl;

  const ILArNoise* noise=nullptr;  
  if ( (!m_RndmEvtOverlay || m_isMcOverlay)  && !m_pedestalNoise && m_NoiseOnOff ){
    SG::ReadCondHandle<ILArNoise> noiseHdl(m_noiseKey, ctx);
    noise=*noiseHdl;
  }

  const LArAutoCorrNoise* autoCorrNoise=nullptr;
  if ( !m_RndmEvtOverlay  &&  m_NoiseOnOff) {
    SG::ReadCondHandle<LArAutoCorrNoise>  autoCorrNoiseHdl(m_autoCorrNoiseKey, ctx);
    autoCorrNoise=*autoCorrNoiseHdl;
  }

  /** Retrieve BadChannels */
  SG::ReadCondHandle<LArBadChannelCont> bch{m_bcContKey,ctx};
  const LArBadChannelCont* bcCont{*bch}; 

  LArDigit *Digit;
  LArDigit *Digit_DigiHSTruth;



  int iCalo=0;
  if(m_larem_id->is_lar_em(cellId)) {
     if (m_larem_id->is_em_endcap_inner(cellId)) iCalo=EMIW;
     else iCalo=EM;
  }
  if(m_larem_id->is_lar_hec(cellId))  iCalo=HEC;
  if(m_larem_id->is_lar_fcal(cellId)) iCalo=FCAL;

  CaloGain::CaloGain  initialGain = CaloGain::LARHIGHGAIN;
  if (iCalo==HEC) initialGain = CaloGain::LARMEDIUMGAIN;

  CaloGain::CaloGain rndmGain = CaloGain::LARHIGHGAIN;

// ........ retrieve data (1/2) ................................
//
  SF=fSampl->FSAMPL(ch_id);

//
// ....... dump info ................................
//


#ifndef NDEBUG
  ATH_MSG_DEBUG("    Cellid " << m_larem_id->show_to_string(cellId));
  ATH_MSG_DEBUG("    SF: " << SF);
#endif

//
// ....... make the five samples
//
  for (i=0;i<m_NSamples;i++) {
   m_Samples[i]=0.;
   if(m_doDigiTruth) {
     m_Samples_DigiHSTruth[i]=0.;
   }
  }

#ifndef NDEBUG
  ATH_MSG_DEBUG(" number of hit for this cell " << TimeE->size());
#endif

//
// convert Hits into energy samples and add result to m_Samples assuming LARHIGHGAIN for pulse shape
//
  bool isDead = m_bcMask.cellShouldBeMasked(bcCont,ch_id);

  if (!isDead) {
    if( this->ConvertHits2Samples(ctx, cellId,ch_id,initialGain,TimeE, m_Samples).isFailure() ) return StatusCode::SUCCESS;
    if(m_doDigiTruth){
      if( this->ConvertHits2Samples(ctx, cellId,ch_id,initialGain,TimeE_DigiHSTruth, m_Samples_DigiHSTruth).isFailure() ) return StatusCode::SUCCESS;
    }
  }

//
// .... add random event digit if needed
//
 float energy2adc ;
 float rAdc ;
 if(m_RndmEvtOverlay && rndmEvtDigit ) // no overlay if missing random digit
 {
  rndmGain= rndmEvtDigit->gain();
  auto polynom_adc2mev =adc2MeVs->ADC2MEV(cellId,rndmEvtDigit->gain());
  if (polynom_adc2mev.size() > 1) {
     float adc2energy = SF * polynom_adc2mev[1];
     const std::vector<short> & rndm_digit_samples = rndmEvtDigit->samples() ;
     float Pedestal = pedestal->pedestal(ch_id,rndmEvtDigit->gain());
     if (Pedestal <= (1.0+LArElecCalib::ERRORCODE)) {
       ATH_MSG_WARNING("  Pedestal not found in database for this channel offID " << cellId << " Use sample 0 for random");
       Pedestal = rndm_digit_samples[0];
     }
     ATH_MSG_DEBUG(" Params for inverting LAr Digitization: pedestal " << Pedestal << "  adc2energy " << adc2energy);

// in case Medium or low gain, take into account ramp intercept in ADC->"energy" computation
//   this requires to take into account the sum of the optimal filter coefficients, as they don't compute with ADC shift
     float adc0=0.;
     if (!m_isMcOverlay && rndmEvtDigit->gain()>0) {
        SG::ReadCondHandle<ILArOFC> larOFC(m_OFCKey, ctx);
        if (larOFC.cptr() != nullptr) {
          ILArOFC::OFCRef_t ofc_a = larOFC->OFC_a(ch_id,rndmEvtDigit->gain(),0);
          float sumOfc=0.;
          if (ofc_a.size()>0) {
            for (unsigned int j=0;j<ofc_a.size();j++) sumOfc += ofc_a.at(j);
          }
          if (sumOfc>0) adc0 =  polynom_adc2mev[0] * SF /sumOfc;
        }
     }

     int nmax=m_NSamples;
     if ((int)(rndm_digit_samples.size()) < m_NSamples) {
         ATH_MSG_WARNING("Less digit Samples than requested in digitization for cell " << ch_id.get_compact() << " Digit has " << rndm_digit_samples.size() << " samples.  Digitization request " << m_NSamples); 
         nmax=rndm_digit_samples.size();
     }
     for(i=0 ; i<nmax ; i++)
     {
       rAdc = (rndm_digit_samples[i] - Pedestal ) * adc2energy + adc0;
       rndm_energy_samples[i] = rAdc ;
       m_Samples[i] += rAdc ;
     }
  }
  else {
    ATH_MSG_WARNING(" No ramp found for this random cell " << m_larem_id->show_to_string(cellId) << " for gain " << rndmEvtDigit->gain());
  }
 }
//...................................................................

//
//........ choice of the gain
//
//
// fix the shift +1 if HEC  and nSamples 4 and firstSample 0
  int ihecshift=0;
  if(iCalo == HEC && m_NSamples.value() == 4 && m_firstSample.value() == 0) ihecshift=1;
  float samp2=m_Samples[m_sampleGainChoice-ihecshift]*MeV2GeV;
  if ( samp2 <= m_EnergyThresh ) return(StatusCode::SUCCESS);

  if(m_doDigiTruth){
    float Samp2_DigiHSTruth=m_Samples_DigiHSTruth[m_sampleGainChoice-ihecshift]*MeV2GeV;
    if ( Samp2_DigiHSTruth <= m_EnergyThresh ) createDigit_DigiHSTruth = false;
  }
    //We choose the gain in applying thresholds on the 3rd Sample (index "2")
    //converted in ADC counts in MediumGain (index "1" of (ADC2MEV)).
    //Indeed, thresholds in ADC counts are defined with respect to the MediumGain.
    //
    //              1300              3900
    // ---------------|----------------|--------------> ADC counts in MediumGain
    //    HighGain  <---  MediumGain  --->  LowGain

  float pseudoADC3;
  float Pedestal = pedestal->pedestal(ch_id,CaloGain::LARMEDIUMGAIN);
  if (Pedestal <= (1.0+LArElecCalib::ERRORCODE)) {
   ATH_MSG_DEBUG(" Pedestal not found for medium gain ,cellID " << cellId <<  " assume 1000 ");
   Pedestal=1000.;
  }
  auto polynom_adc2mev = adc2MeVs->ADC2MEV(cellId,CaloGain::LARMEDIUMGAIN);
  if ( polynom_adc2mev.size() < 2) {
    ATH_MSG_WARNING(" No medium gain ramp found for cell " << m_larem_id->show_to_string(cellId) << " no digit produced...");
    return StatusCode::SUCCESS;
  }
  pseudoADC3 = m_Samples[m_sampleGainChoice-ihecshift]/(polynom_adc2mev[1])/SF + Pedestal ;

  //
  // ......... try a gain
  //
  if ( pseudoADC3 <= m_HighGainThresh[iCalo])
    igain = CaloGain::LARHIGHGAIN;
   else if ( pseudoADC3 <= m_LowGainThresh[iCalo])
    igain = CaloGain::LARMEDIUMGAIN;
  else
    igain = CaloGain::LARLOWGAIN;

 // check that select gain is never lower than random gain in case of overlay
  if (rndmGain==CaloGain::LARMEDIUMGAIN && igain==CaloGain::LARHIGHGAIN) igain=CaloGain::LARMEDIUMGAIN;
  if (rndmGain==CaloGain::LARLOWGAIN ) igain=CaloGain::LARLOWGAIN;

//
// recompute m_Samples if igain != HIGHGAIN
//
   if (igain != initialGain ){

     for (i=0;i<m_NSamples;i++) {
       if(m_doDigiTruth) m_Samples_DigiHSTruth[i] = 0.;
       if (m_RndmEvtOverlay) m_Samples[i]= rndm_energy_samples[i] ;
       else m_Samples[i] = 0.;
     }

     if (!isDead) {
       if( this->ConvertHits2Samples(ctx, cellId,ch_id,igain,TimeE, m_Samples) == StatusCode::FAILURE ) return StatusCode::SUCCESS;
       if(m_doDigiTruth){
         if( this->ConvertHits2Samples(ctx, cellId,ch_id,igain,TimeE_DigiHSTruth, m_Samples_DigiHSTruth) == StatusCode::FAILURE ) return StatusCode::SUCCESS;
       }
     }

   }

//
// ........ add the noise ................................
//

  int BvsEC=0;
  if(iCalo==EM || iCalo==EMIW) BvsEC=abs(m_larem_id->barrel_ec(cellId));

  bool addedNoise=false;
  if(    m_NoiseOnOff
      && (    (BvsEC==1 && m_NoiseInEMB)
           || (BvsEC>1  && m_NoiseInEMEC)
           || (iCalo==HEC && m_NoiseInHEC)
           || (iCalo==FCAL && m_NoiseInFCAL)
         )
    )
    //add the noise only in the wanted sub-detectors
  {
     if ( !m_RndmEvtOverlay ) {
        if (!m_pedestalNoise) {
          SigmaNoise =noise->noise(ch_id,igain );
        } else {
          float noise = pedestal->pedestalRMS(ch_id,igain);
          if (noise >= (1.0+LArElecCalib::ERRORCODE) ) SigmaNoise = noise;
          else SigmaNoise=0.;
        }
        // Sqrt of noise covariance matrix
	const std::vector<float>& CorGen=autoCorrNoise->autoCorrSqrt(cellId,igain);

        RandGaussZiggurat::shootArray(engine,m_NSamples,m_Rndm,0.,1.);

        int index;
        for (int i=0;i<m_NSamples;i++) {
           m_Noise[i]=0.;
           for(int j=0;j<=i;j++) {
             index = i*m_NSamples + j;
             m_Noise[i] += m_Rndm[j] * CorGen[index];
           }
           m_Noise[i]=m_Noise[i]*SigmaNoise;
        }
        addedNoise=true;
     } else  {
        // overlay case a priori don't add any noise
        for (int i=0;i<m_NSamples;i++) m_Noise[i]=0.;
        // if gain from zerobias events is < gain from mixed events => add extra noise to account for gain vs noise dependance
        //   done in a simple way without taking into account the time correlation of this extra noise properly
        if (rndmEvtDigit) {
            // if gain of cell is different from ZB event gain
            if (igain > rndmEvtDigit->gain() ) {
               double SigmaNoiseZB=0.;    // noise in ZB event for gain of ZB event
               double SigmaNoise=0.;      // noise expected for new gain value
               double SigmaExtraNoise=0.;  // quadratic difference of noise values
               if (!m_pedestalNoise) {
                  SigmaNoiseZB =  noise->noise(ch_id,rndmEvtDigit->gain());
                  SigmaNoise   =  noise->noise(ch_id,igain );
               }    
               else {
                  float noise = pedestal->pedestalRMS(ch_id,rndmEvtDigit->gain() );
                  if (noise >= (1.0+LArElecCalib::ERRORCODE) ) SigmaNoiseZB = noise;
                  else SigmaNoiseZB=0.;
                  noise = pedestal->pedestalRMS(ch_id,igain);
                  if (noise >= (1.0+LArElecCalib::ERRORCODE) ) SigmaNoise=noise;
                  else SigmaNoise=0.;
               }
               // Convert SigmaNoiseZB in noise in ADC counts for igain conversion
               auto polynom_adc2mevZB = adc2MeVs->ADC2MEV(cellId,rndmEvtDigit->gain() );
               auto polynom_adc2mev = adc2MeVs->ADC2MEV(cellId,igain);
               if ( polynom_adc2mevZB.size()>1 &&  polynom_adc2mev.size()>1) {
                  if (polynom_adc2mev[1] >0.) {
                   SigmaNoiseZB = SigmaNoiseZB * (polynom_adc2mevZB[1])/(polynom_adc2mev[1]);
                   if (SigmaNoise > SigmaNoiseZB) SigmaExtraNoise = sqrt(SigmaNoise*SigmaNoise - SigmaNoiseZB*SigmaNoiseZB);
                  }
               }    // check that AC2MeV factors are there
               RandGaussZiggurat::shootArray(engine,m_NSamples,m_Rndm,0.,1.);   // generate noise
               for (int i=0;i<m_NSamples;i++) m_Noise[i] = SigmaExtraNoise * m_Rndm[i];
               addedNoise=true;
            }  // different gains
        }      // rndm Digit is there
     }         // rndm Overlay test
  }            // add noise ?
//
// ......... convert into adc counts  ................................
//
  Pedestal = pedestal->pedestal(ch_id,igain);
  if (Pedestal <= (1.0+LArElecCalib::ERRORCODE)) {
     ATH_MSG_WARNING(" pedestal not found for cellId " << cellId << " assume 1000" );
     Pedestal=1000.;
  }
  polynom_adc2mev = adc2MeVs->ADC2MEV(cellId,igain);
  if (polynom_adc2mev.size() < 2) {
    ATH_MSG_WARNING(" No ramp found for requested gain " << igain << " for cell " << m_larem_id->show_to_string(cellId) << " no digit made...");
    return StatusCode::SUCCESS;
  }

  energy2adc=1./(polynom_adc2mev[1])/SF;

// in case Medium or low gain, take into account ramp intercept in energy->ADC computation
//   this requires to take into account the sum of the optimal filter coefficients, as they don't compute with ADC shift
  if(!m_isMcOverlay && m_RndmEvtOverlay  && igain>0)
  {
    SG::ReadCondHandle<ILArOFC> larOFC(m_OFCKey, ctx);
    if (larOFC.cptr() != nullptr) {
      float sumOfc=0.;
      ILArOFC::OFCRef_t ofc_a = larOFC->OFC_a(ch_id,igain,0);
      if (ofc_a.size()>0) {
        for (unsigned int j=0;j<ofc_a.size();j++) sumOfc+= ofc_a.at(j);
      }
      if ((polynom_adc2mev[1])>0 && sumOfc>0) Pedestal = Pedestal - (polynom_adc2mev[0])/(polynom_adc2mev[1])/sumOfc;
      ATH_MSG_DEBUG("  Params for final LAr Digitization  gain: " << igain << "    pedestal: " << Pedestal <<  "   energy2adc: " << energy2adc);
    }
  }
  for(i=0;i<m_NSamples;i++)
  {
    double xAdc;
    double xAdc_DigiHSTruth = 0;

    if ( addedNoise ){
      xAdc =  m_Samples[i]*energy2adc + m_Noise[i] + Pedestal + 0.5;
      if(m_doDigiTruth) {
        xAdc_DigiHSTruth =  m_Samples_DigiHSTruth[i]*energy2adc + m_Noise[i] + Pedestal + 0.5;
      }
    }

    else {
      if (m_roundingNoNoise) {
        float flatRndm = RandFlat::shoot(engine);
        xAdc =  m_Samples[i]*energy2adc + Pedestal + flatRndm;
        if(m_doDigiTruth) {
          xAdc_DigiHSTruth =  m_Samples_DigiHSTruth[i]*energy2adc + Pedestal + flatRndm;
        }

      }
      else{
         xAdc =  m_Samples[i]*energy2adc + Pedestal + 0.5;
         if(m_doDigiTruth) {
           xAdc_DigiHSTruth =  m_Samples_DigiHSTruth[i]*energy2adc + Pedestal + 0.5;
         }
      }

    }

//
// ........ truncate at maximum value + 1
//          add possibility to saturate at 0 for negative signals
//
    if (xAdc <0)  Adc=0;
    else if (xAdc >= MAXADC) Adc=MAXADC;
    else Adc = (short) xAdc;

    AdcSample[i]=Adc;

    if(m_doDigiTruth){
      if (xAdc_DigiHSTruth <0)  Adc_DigiHSTruth=0;
      else if (xAdc_DigiHSTruth >= MAXADC) Adc_DigiHSTruth=MAXADC;
      else Adc_DigiHSTruth = (short) xAdc_DigiHSTruth;
      AdcSample_DigiHSTruth[i] = Adc_DigiHSTruth;
    }

#ifndef NDEBUG
    ATH_MSG_DEBUG(" Sample " << i << "  Energy= " << m_Samples[i] << "  Adc=" << Adc);
#endif

  }

//
// ...... create the LArDigit and push it into the Digit container ..................
//
  Digit = new LArDigit(ch_id,igain,AdcSample);
  m_DigitContainer->push_back(Digit);


  if(m_doDigiTruth && createDigit_DigiHSTruth){
    createDigit_DigiHSTruth = false;

    for(int i=0; i<m_NSamples; i++) {
      if(m_Samples_DigiHSTruth[i] != 0) createDigit_DigiHSTruth = true;
    }

    Digit_DigiHSTruth = new LArDigit(ch_id,igain,AdcSample_DigiHSTruth);
    m_DigitContainer_DigiHSTruth->push_back(Digit_DigiHSTruth);
  }


  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------------------------------------------------------------------

StatusCode LArPileUpTool::ConvertHits2Samples(const EventContext& ctx,
                                              const Identifier & cellId, const HWIdentifier ch_id, CaloGain::CaloGain igain,
					      //const std::vector<std::pair<float,float> >  *TimeE)
					      const std::vector<std::pair<float,float> >  *TimeE, std::vector<double> &sampleList)

{
// Converts  hits of a particular LAr cell into energy samples
// declarations
   int nsamples ;
   int nsamples_der ;
   int i ;
   int j ;
   float energy ;
   float time ;

   SG::ReadCondHandle<ILArShape> shapeHdl(m_shapeKey, ctx);
   const ILArShape* shape=*shapeHdl;


// ........ retrieve data (1/2) ................................
//
   ILArShape::ShapeRef_t Shape = shape->Shape(ch_id,igain);
   ILArShape::ShapeRef_t ShapeDer = shape->ShapeDer(ch_id,igain);

  nsamples = Shape.size();
  nsamples_der = ShapeDer.size();

  if (nsamples==0) {
    ATH_MSG_INFO(" No samples for cell = " << cellId );
    return StatusCode::FAILURE;
  }

#ifndef NDEBUG
  ATH_MSG_DEBUG(" Cellid " << m_larem_id->show_to_string(cellId));
  for (i=0;i<nsamples;i++)
  {
       ATH_MSG_DEBUG(Shape[i] << " ");
  }
#endif

  std::vector<std::pair<float,float> >::const_iterator first = TimeE->begin();
  std::vector<std::pair<float,float> >::const_iterator last  = TimeE->end();

  while (first != last)
  {
   energy = (*first).first;
   time   = (*first).second;

#ifndef NDEBUG
  ATH_MSG_DEBUG("m_NSamples, m_usePhase " << m_NSamples << " " << m_usePhase);
#endif

   // fix the shift +1 if HEC  and nSamples 4 and firstSample 0
   // in case of data overlay this should NOT  be done as the pulse shape read from the database is already shifted
   //   but this should still be done in case of MC overlay
   int ihecshift=0;
   if((!m_RndmEvtOverlay || m_isMcOverlay) && m_larem_id->is_lar_hec(cellId) && m_NSamples.value() == 4 && m_firstSample.value() == 0) ihecshift=1;


   if (!m_usePhase) {

 // Atlas like mode where we use 25ns binned pulse shape and derivative to deal with time offsets

// shift between reference shape and this time
      int ishift=(int)(rint(time*(1./25.)));
      double dtime=time-25.*((double)(ishift));

      for (i=0;i<m_NSamples.value();i++)
      {
       j = i - ishift + m_firstSample + ihecshift;
#ifndef NDEBUG
       ATH_MSG_DEBUG(" time/i/j " << time << " "<< i << " " << j);
#endif
       if (j >=0 && j < nsamples ) {
         if (j<nsamples_der && std::fabs(ShapeDer[j])<10. )
              sampleList[i] += (Shape[j]- ShapeDer[j]*dtime)*energy ;
         else sampleList[i] += Shape[j]*energy ;
       }
      }
   }

// Mode to use phase (tbin) to get pulse shape ( pulse shape with fine time binning should be available)

   else {

     // FIXME hardcode 8phases3ns configuration (cannot access parameters from ILArShape interface now)
     int nTimeBins = 8;
     float timeBinWidth = 25./24.*3.;

//    -50<t<-25 phase=-t-25, shift by one peak time (for s2 uses shape(3) with tbin)
// for -25<t<0  phase = -t, no shift of peak time
// for 0<t<25   phase=25-t, shift by one peak time (for s2 uses shape(1) with tbin)
//    25<t<50   phase=50-t, shift by two
//  etc...

      int ishift = (int)(time*(1./25.));
      int tbin;
      if (time>0) {
         tbin = (int)(fmod(time,25)/timeBinWidth);
          if (tbin>0) {
             tbin=nTimeBins-tbin;
             ishift +=1;
          }
      } else {
         tbin = (int)(fmod(-time,25)/timeBinWidth);
      }

      double dtime = time - ( 25.*((float)(ishift)) - timeBinWidth*tbin);

      Shape = shape->Shape(ch_id,igain,tbin);
      ShapeDer = shape->ShapeDer(ch_id,igain,tbin);

      nsamples = Shape.size();
      nsamples_der = ShapeDer.size();


      for (i=0;i<m_NSamples.value();i++)
      {
       j = i - ishift+m_firstSample + ihecshift;
#ifndef NDEBUG
       ATH_MSG_DEBUG(" time/i/j " << time << " "<< i << " " << j);
#endif
       if (j >=0 && j < nsamples ) {
         if (j<nsamples_der && std::fabs(ShapeDer[j])<10. )
              sampleList[i] += (Shape[j]- ShapeDer[j]*dtime)*energy ;
         else sampleList[i] += Shape[j]*energy ;
       }
      }

   }     // else if of m_usePhase

   ++first;
  }         // loop over hits

   return StatusCode::SUCCESS;

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
