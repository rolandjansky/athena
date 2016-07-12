/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// +==========================================================================+
// +                                                                          +
// + Authors .......: G.Unal                                                  +
// +    migrate LAr digitization to PileUpTool framework                      +
// +==========================================================================+
// 

#include <CLHEP/Random/Randomize.h>
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"
#include "LArDigitization/HGTDPileUpTool.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/HGTD_ID.h"
#include "Identifier/Identifier.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/PileUpTimeEventIndex.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IService.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"
#include "PileUpTools/PileUpMergeSvc.h"


using CLHEP::RandFlat;
using CLHEP::RandGaussZiggurat;


HGTDPileUpTool::HGTDPileUpTool(const std::string& type, const std::string& name, const IInterface* parent) :
  PileUpToolBase(type, name, parent), 
  m_mergeSvc(0)
{

  declareInterface<IPileUpTool>(this);
  declareInterface<IHGTDPileUpTool>(this);
  
  // default properties

  m_DigitContainerName    = "HGTDDigitContainer_MC";
  m_HgtdHitContainerName  = "LArHitHGTD";
  m_NoiseOnOff        = true;
  m_CrossTalk         = true;
  m_EnergyThresh      = -99.;
  m_PileUp            = false;
  m_rndmSvc           = "AtRndmGenSvc";
  m_rndmEvtRun        = false;
  m_RndmEvtOverlay    = false;
  m_useBad            = true;
  m_timeBinType       = "Uniform";
  m_timeBinWidth      = 0.005;
  //
  // ........ declare the private data as properties
  //
  declareProperty("DigitContainer",m_DigitContainerName,"Name of output digit container");
  declareProperty("HgtdHitContainerName",m_HgtdHitContainerName,"Hit container name for HGTD");
  declareProperty("NoiseOnOff",m_NoiseOnOff,"put electronic noise (default=true)");
  declareProperty("CrossTalk",m_CrossTalk,"Simulate cross-talk (default=true)");
  declareProperty("EnergyThresh",m_EnergyThresh,"Hit energy threshold (default=-99)");
  declareProperty("PileUp",m_PileUp,"Pileup mode (default=false)");
  declareProperty("RndmSvc",m_rndmSvc,"Random number service for HGTD digitization");
  declareProperty("UseRndmEvtRun",m_rndmEvtRun,"Use Run and Event number to seed rndm number (default=false)");
  declareProperty("RndmEvtOverlay",m_RndmEvtOverlay,"Pileup and/or noise added by overlaying random events (default=false)");
  declareProperty("TimeBinType",m_timeBinType,"type of quantization of time (default=Uniform)");
  declareProperty("TimeBinWidth",m_timeBinWidth,"width of time bin (default=5ps)");

  return;
}


HGTDPileUpTool::~HGTDPileUpTool()
{  
  return;
}


StatusCode HGTDPileUpTool::initialize()
{
   ATH_MSG_INFO(" initialize HGTDPileUpTool : digit container name " << m_DigitContainerName);
  //
  // ........ print random event overlay flag
  //
  if (m_RndmEvtOverlay)
  {
    m_NoiseOnOff = false ; 
    m_PileUp = true ;
    ATH_MSG_INFO(" pileup and/or noise added by overlaying digits of random events");
  } 
  else 
  {
     ATH_MSG_INFO(" No overlay of random events");
  }

  if (m_PileUp) {
       if (service("PileUpMergeSvc", m_mergeSvc).isFailure()) {
            ATH_MSG_ERROR( "Can not retrieve PileUpMergeSvc" );
            ATH_MSG_ERROR( "Setting PileUp and RndmOverlay flags to FALSE ");
            m_PileUp = m_RndmEvtOverlay = false;
        } else {
            ATH_MSG_INFO( "PileUpMergeSvc successfully initialized");
        }
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
  }

  //
  // ............ print cross-talk configuration
  //
  if (m_CrossTalk )
  {
    ATH_MSG_INFO("     Cross talk requested but not yet implemented");
  }
  else
  {
    ATH_MSG_INFO(" no Cross-Talk simulated");
  }

  //retrieve ID helpers
  const DataHandle<CaloIdManager> caloIdMgr;
  StatusCode sc = detStore()->retrieve(caloIdMgr);
  if (sc.isFailure()) {
    ATH_MSG_ERROR(" Unable to retrieve CaloIdManager from DetectoreStore");
    return StatusCode::FAILURE;
  }   
  m_hgtd_id   = caloIdMgr->getHGTD_ID();

  // initialize random number service
  if ( m_NoiseOnOff ) {
    static const bool CREATEIFNOTTHERE(true);
    sc = service(m_rndmSvc, m_AtRndmGenSvc, CREATEIFNOTTHERE);
    if (!sc.isSuccess() || 0 == m_AtRndmGenSvc) {
      ATH_MSG_ERROR(" Unable to initialize AtRndmGenSvc");
      return StatusCode::FAILURE;
    }
    else {
      ATH_MSG_INFO(" Found random number service " << m_rndmSvc);
    }
    if (!m_rndmEvtRun) {
      m_engine = m_AtRndmGenSvc->GetEngine("LArDigitization");
      if (m_engine==0) {
        ATH_MSG_ERROR(" Could find find random engine LArDigitization ");
        return StatusCode::FAILURE;
      }
    }
  }

  ATH_MSG_DEBUG("Initialization completed successfully");

  return StatusCode::SUCCESS;

}

// ----------------------------------------------------------------------------------------------------------------------------------

StatusCode HGTDPileUpTool::prepareEvent(unsigned int /*nInputEvents */)
{

  // if noise on, get random number engine and initialize with well defined seeds
  if (m_NoiseOnOff ) {
    //if use run-event number for seed
    if (m_rndmEvtRun) {
      const EventInfo* eventInfo;
      StatusCode sc =  evtStore()->retrieve(eventInfo);
      int iSeedNumber=1;
      if (sc.isFailure()) {
	ATH_MSG_ERROR(" can not retrieve event info. Dummy rndm initialization...");
	m_engine = m_AtRndmGenSvc->setOnDefinedSeeds(iSeedNumber,this->name());
      } 
      else {  
	const EventID* myEventID=eventInfo->event_ID();
	const int iSeedNumber = myEventID->event_number()+1;
	const int iRunNumber = myEventID->run_number();
	ATH_MSG_DEBUG(" Run/Event " <<  myEventID->run_number() << " " << myEventID->event_number());
	ATH_MSG_DEBUG(" iSeedNumber,iRunNumber " << iSeedNumber << " " << iRunNumber);
	m_engine = m_AtRndmGenSvc->setOnDefinedSeeds(iSeedNumber,iRunNumber,this->name());
      }
    }
  }
  
  //
  // ....... create the LAr HIT Container for the output
  //
  m_DigitContainer = new LArHitContainer();
  if ( m_DigitContainer == 0 )
    {
      ATH_MSG_ERROR("Could not allocate a new LArHitContainer");
      return StatusCode::FAILURE;	  
    }
  
  //
  // ...... register the digit container into the TDS and check if succeeded
  //
  StatusCode sc = evtStore()->record(m_DigitContainer ,  m_DigitContainerName) ;
  if( sc.isFailure() )
    {
      ATH_MSG_ERROR("Could not record new LArDigitContainer in TDS : " << m_DigitContainerName);
      return StatusCode::FAILURE;	  
    }
  
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------------------------------------------------------

StatusCode HGTDPileUpTool::processBunchXing(int bunchXing,
                                           PileUpEventInfo::SubEvent::const_iterator bSubEvents,
                                           PileUpEventInfo::SubEvent::const_iterator eSubEvents) 
{

  float tbunch = (float)(bunchXing);

//
// ............ loop over the wanted hit containers
//
  PileUpEventInfo::SubEvent::const_iterator iEvt(bSubEvents);
  while (iEvt != eSubEvents) {
    
    // event store for this sub event
    StoreGateSvc &seStore(*iEvt->pSubEvtSG);

    // do we deal with the MC signal event ?
    bool isSignal = ( (iEvt->type()==PileUpTimeEventIndex::Signal) || m_RndmEvtOverlay);

    // fill LArHits in map
    if (this->fillMapFromHit(&seStore,tbunch,isSignal).isFailure()) {
      ATH_MSG_ERROR(" cannot fill map from hits ");
      return StatusCode::FAILURE;
    }
    ++iEvt;
  }
  
  return StatusCode::SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------------------------

StatusCode HGTDPileUpTool::processAllSubEvents()
{
  if (this->prepareEvent(0).isFailure()) {
     ATH_MSG_ERROR("error in prepareEvent");
     return StatusCode::FAILURE;
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

    typedef PileUpMergeSvc::TimedList<LArHitContainer>::type TimedHitContList;
    TimedHitContList hitContList; 
    // 
    // retrieve list of pairs (time,container) from PileUp service
      
    if (!(m_mergeSvc->retrieveSubEvtsData(m_HgtdHitContainerName, hitContList).isSuccess()) 
	&& hitContList.size()==0) {
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
      LArHitContainer::const_iterator hititer;
      for(hititer=firstCont.begin();
	  hititer != firstCont.end();hititer++) {
	double time;
	if (isSignal) time=0.;
	else time = (double) (SubEvtTimOffset);
	if (this->AddHit((*hititer),time).isFailure()) return StatusCode::FAILURE;
      }              //  loop over  hits
      ++iFirstCont;
    }                 // loop over subevent list
  }

  if (this->mergeEvent().isFailure()) {
    ATH_MSG_ERROR("error in mergeEvent");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

// ---------------------------------------------------------------------------------------------------------------------------------


StatusCode HGTDPileUpTool::mergeEvent() 
{

  // should not be called per cell, but per event
  if ( this->MakeDigit() == StatusCode::FAILURE ) return StatusCode::FAILURE;

  // lock Digit container in StoreGate
  StatusCode sc = evtStore()->setConst(m_DigitContainer);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( " Cannot lock DigitContainer ");
    return(StatusCode::FAILURE);
  }
 
  ATH_MSG_DEBUG(" number of created digits  = " << m_DigitContainer->size());

  return StatusCode::SUCCESS;

}

// ----------------------------------------------------------------------------------------------------------------------------------
StatusCode HGTDPileUpTool::MakeDigit()
{
  //
  // ...... create the LArHit and push it into the Digit container ..................
  //
  for(const auto i : m_timeBins) {
    
    const hits_t* hitSet = i.second;
      
    // For each hit in the set...
    for(auto hit : *hitSet){
      // Because of the design, we are sure this is going into the right hit container
      hit->finalize();
      m_DigitContainer->push_back(hit);
    } // End of loop over hits in the set
      
  } // End of loop over time bins
    
  for(auto i : m_timeBins) delete i.second;
  m_timeBins.clear();

  return StatusCode::SUCCESS;
}

StatusCode HGTDPileUpTool::fillMapFromHit(StoreGateSvc* myStore, float bunchTime, bool isSignal)
{
  if ( isSignal ) {
    // hookup for a special treatment of the signal
  }
  // retrieve the hitcontainer and loop over hits:
  const DataHandle<LArHitContainer> hit_container ;
  if (myStore->contains<LArHitContainer>(m_HgtdHitContainerName)) {
    StatusCode sc = myStore->retrieve( hit_container, m_HgtdHitContainerName);
    if (sc.isFailure() || !hit_container) {
      return StatusCode::FAILURE;
    }       
    LArHitContainer::const_iterator hititer;
    for(hititer=hit_container->begin();
	hititer != hit_container->end();hititer++) {
      if (this->AddHit((*hititer),bunchTime).isFailure()) return StatusCode::FAILURE;
    } // End while hits
  }
  return StatusCode::SUCCESS;
}

StatusCode HGTDPileUpTool::AddHit(const LArHit *theHit, double timeOffset){

  // fill LArHits in map 
  int timeBin = getTimeBin(theHit->time()+timeOffset);
      
  hits_t* hitCollection = 0;
  auto setForThisBin = m_timeBins.find( timeBin );
  
  if (setForThisBin == m_timeBins.end()) {
    // New time bin
    hitCollection = new hits_t;
    m_timeBins[ timeBin ] = hitCollection;
  } else {
    // Get the existing set of hits for this time bin.
    // Reminders:
    // setForThisBin = iterator (pointer) into the m_timeBins map
    // (*setForThisBin) = pair< G4int, m_hits_t* >
    // (*setForThisBin).second = m_hits_t*, the pointer to the set of hits
    
    hitCollection = (*setForThisBin).second;
  }
  
  LArHit* hit = new LArHit(theHit->cellID(),theHit->energy(),theHit->time()+timeOffset);
  
  // If we haven't had a hit in this cell before, create one and add
  // it to the hit collection.
  
  // If we've had a hit in this cell before, then add the energy to
  // the existing hit.
  
  // Look for the key in the hitCollection (this is a binary search).
  auto bookmark = hitCollection->lower_bound(hit);
  
  // The lower_bound method of a map finds the first element
  // whose key is not less than the identifier.  If this element
  // == our hit, we've found a match.
  
  // Reminders:
  // bookmark = iterator (pointer) into the hitCollection set.
  // (*bookmark) = a member of the set, which is a LArG4Hit*.
  
  // Equals() is a function defined in LArG4Hit.h; it has the value of
  // "true" when a LArG4Hit* points to the same identifier.
  
  if (bookmark == hitCollection->end() ||
      !(*bookmark)->Equals(hit)) {
    // We haven't had a hit in this readout cell before.  Add it
    // to our set.
    if (hitCollection->empty() ||
	bookmark == hitCollection->begin()) {
      // Insert the hit before the first entry in the map.
      hitCollection->insert(hit);
    } else {
      // We'just done a binary search of hitCollection, so we should use
      // the results of that search to speed up the insertion of a new
      // hit into the map.  The "insert" method is faster if the new
      // entry is right _after_ the bookmark.  If we left bookmark
      // unchanged, the new entry would go right _before_ the
      // bookmark.  We therefore want to decrement the bookmark from
      // the lower_bound search.
      
      hitCollection->insert(--bookmark, hit);
    }
  } else {
    // Update the existing hit.
    (*bookmark)->Add(hit);
    
    // We don't need our previously-created hit anymore.
    delete hit;
  }
  return StatusCode::SUCCESS;
}      
int HGTDPileUpTool::getTimeBin(double time) const {
 
  int timeBin = INT_MAX;
    
  if(m_timeBinType != "Uniform" )
    {
      if(time<0.)
	time=0.;
      if(time<10.)
	timeBin = int(time/2.5);
      else if(time<50.)
	timeBin = 4 + int((time-10.)/5.);
      else if(time<100.)
	timeBin = 12 + int((time-50.)/25.);
      else
	timeBin = 14;
    }
  else {
    // Uniform binning by 2.5 ns
    double timeBinf = time/m_timeBinWidth;
    
    if (timeBinf < double(INT_MAX))
      timeBin = int(timeBinf);
  }
  
  return timeBin;
} 

bool HGTDLessHit::operator() ( LArHit* const& p, LArHit* const& q ) const
{
  return p->Less(q);
}
