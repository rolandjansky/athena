/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaMonitoring/HLTEgammaNavSigTEMonTool.h"

//check whether these might be inherited
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/Algorithm.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigCaloEvent/TrigEMCluster.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"

#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

//for extrapolation
#include "CaloDetDescr/CaloDepthTool.h"
#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"

#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/EMTrackMatch.h"

#include "EventInfo/EventInfo.h"
#include <EventInfo/EventID.h>

//for LAr Cleaning
#include "LArRecEvent/LArEventBitInfo.h"

#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/Electron.h"

#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TMath.h"
#include "TTree.h"

//OVERLAPTEST
#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"
//OVERLAPTEST

/*  CONSTRUCTOR
 *  - reads in properties from job options
 *  - initialises some members
 */
HLTEgammaNavSigTEMonTool::HLTEgammaNavSigTEMonTool(const string & type, const string & name, const IInterface* parent)
  : HLTEgammaFEXNavSigTEBaseTool(type,name,parent)
{

  //read in declarations from job options
  declareProperty("signatures",m_signatures);  
  declareProperty("UsePreConditionalPassThroughChain", m_usePreConditionalPassThroughChain = false);
  declareProperty("PassThroughChainEF", m_passThroughChainEF = "EF_e10_NoCut");
  declareProperty("PassThroughChainL2", m_passThroughChainL2 = "L2_e10_NoCut");
  declareProperty("egammaContainer", m_electronContainerName="ElectronCollection");
  declareProperty("egammaContainer", m_photonContainerName="PhotonCollection");
  declareProperty("DoOffline", m_doOfflineComparisons=true);
  declareProperty("OfflineEleMinPTCut", m_offEle_minptcut= 25.);//GeV
  declareProperty("OfflinePhoMinPTCut", m_offPho_minptcut= 25.);//GeV
  declareProperty("DRmatchToOffline", m_dR_off= 0.15);//GeV
  declareProperty("DoLumiCalc", m_doLumiCalc = true);//Lumi  
  declareProperty("PayloadName",m_payload = "LBAvInstLumi");//Lumi
  declareProperty("DatabaseName", m_database =  "COOLOFL_TRIGGER/COMP200");
  /* "COOLONL_TRIGGER/COMP200" //Online for data commissioning
     "COOLOFL_TRIGGER/COMP200" //Offline for data commissioning
     "COOLONL_TRIGGER/OFLP200"// for MC */
     declareProperty("CoolFolderName",m_coolfolder = "/TRIGGER/LUMI/LBLESTONL");
  /*"/TRIGGER/LUMI/LBLEST" //???
    "/TRIGGER/LUMI/LBLESTONL" online DB: single-version folder on the ATLAS_COOLONL_TRIGGER 
    "/TRIGGER/OFLLUMI/LBLESTOFL" offline DB: multi-version folder on the ATLAS_COOLOFL_TRIGGER schema (fake for TopMix?) */
  declareProperty("TagName",m_tagname = /* "OflLumi-7TeV-001" */ "OflLumi-Dec2009-001");//Lumi
  declareProperty("ChannelId",m_chanid = 0);//Lumi: ChannelId= 0 is the ATLAS_PREFFERED
  declareProperty("LumiTrigger",m_triggerForLumi = "L1_MBTS_2");//Lumi  
  declareProperty("LumiHist_LBBins",m_nLB = 140);//Lumi Hists  
  declareProperty("LumiHist_LBRangeLo",m_nLBLo = 0);//Lumi Hists
  declareProperty("LumiHist_LBRangeUp",m_nLBUp = 700);//Lumi Hists
  declareProperty("LumiHist_LumiNBins",m_nLumi = 80);//Lumi  Hists
  declareProperty("LumiHist_LumiRangeUp",m_nLumiUp = 35);//Lumi  Hists
  declareProperty("LumiHist_LumiRangeLo",m_nLumiLo = 27);//Lumi  Hists  

  //initialise pointers to egamma collections
  m_elecTES = NULL;
  m_photTES = NULL;

  //initialise pointers to extrapolation tool and interface
  m_toCalo = NULL;
  m_calodepth = NULL;

  //reset timestamps
  m_firstTimeStamp = m_lastTimeStamp = 0;
}




//default destructor
// - TODO: is there really nothing to clean up?
HLTEgammaNavSigTEMonTool::~HLTEgammaNavSigTEMonTool()
{ }




/*  INITIALISATION
 *  - loads track->calo extrapolation tool
 *  - initialises fill and rate conditions
 */
StatusCode HLTEgammaNavSigTEMonTool::init()
{
  ATH_MSG_DEBUG("Starting initialisation.");

  //declare status code to be used to check on success of initialisations
  StatusCode sc = StatusCode::FAILURE;

  //check on ToolSvc
  sc = service("ToolSvc", m_toolSvc);
  if(sc.isFailure()){
    ATH_MSG_ERROR("Unable to get ToolSvc!");
    return(StatusCode::FAILURE);
  }

  //fetch extrapolation tool if requested
  if(m_doExtrapol){

    //set up generic interface pointer
    IAlgTool* algtool = NULL;

    //try to fetch tool
    sc = m_toolSvc->retrieveTool("ExtrapolTrackToCaloTool/extrapolTrackToCaloTool", algtool, this);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Cannot get ExtrapolTrackToCaloTool");
      return(StatusCode::FAILURE);
    }

    //cast to correct interface pointer species
    m_toCalo=dynamic_cast<IExtrapolateToCaloTool*>(algtool);

    //retrieve calo depth tool through interface
    m_calodepth = &(*m_toCalo->getCaloDepth());
    if (!m_calodepth) {
      ATH_MSG_ERROR("Cannot get CaloDepthTool");
      return(StatusCode::FAILURE);
    }

  }//done fetching extrapolation tool

  //setup fill/rate condiftions
  //TODO might not be needed anymore
  m_fill_condition=TrigDefs::alsoDeactivateTEs;
  ATH_MSG_DEBUG("Using Fill Condition "<<m_fill_condition);
  m_rate_condition=TrigDefs::Physics;
  ATH_MSG_DEBUG("Using Rate Condition "<<m_rate_condition);

  //check whether the requested pass-states make sense
  vector<string> passStates;
  fetchPassStates(passStates);
  vector<string>::iterator passIt = passStates.begin();
  for(; passIt!=passStates.end(); ++passIt){
    ATH_MSG_INFO("Booking and filling histograms for pass-state: "<<*passIt);
  }
  if(passStates.size()==0){
    ATH_MSG_WARNING("Not filling any trigger histograms. Are you sure this is what you want?");
  }

  //all done
  ATH_MSG_DEBUG("Finished initialisation.");
  return(StatusCode::SUCCESS);
}



/*  HISTOGRAM BOOKING
 *
 *  - implementation of purely virtual function defined in IHLTMonTool
 *  - actual booking performed in base class
 */
StatusCode HLTEgammaNavSigTEMonTool::book()
{
  ATH_MSG_DEBUG("Starting to book histograms.");

  //book for new run (or start of file)
    ATH_MSG_DEBUG("Booking histograms for new run.");

    //fetch running triggers from trig decision tool
    vector<string> runningSignatures = getTDT()->getListOfTriggers(".*");

    //loop through monitored signatures to remove the ones not running
    // - note: we will remove items not found, so only advance the iterator if something is
    //         found. otherwise this will be handled by vector::erase()
    vector<string>::iterator monSigIt = m_signatures.begin();
    for(; monSigIt!=m_signatures.end();){

      //form EF-item name of chain
      string efItemName = "EF_" + *monSigIt; 
      bool signatureFound = false;

      //loop through running signatures
      vector<string>::const_iterator runSigIt = runningSignatures.begin();
      vector<string>::const_iterator runSigItE = runningSignatures.end();
      for(; runSigIt != runSigItE; ++runSigIt){

        //check whether we have found a match
        if(efItemName == *runSigIt){
          signatureFound = true;
          break;
        }
      }//done looping through running signatures

      //remove monitored signatures that are not running
      if (!signatureFound){
        ATH_MSG_INFO("Monitored chain not found: "<<*monSigIt);
        monSigIt = m_signatures.erase(monSigIt);
      }
      //otherwise advance iterator to next signature
      else{
        ATH_MSG_INFO("Monitored chain found: "<<*monSigIt);
        ++monSigIt;
      }
    }//done looping through monitored signatures

    //loop through monitored signatures to book histograms
    // - note: reusing iterator from above
    for(monSigIt = m_signatures.begin(); monSigIt != m_signatures.end(); ++monSigIt){

      //call histogram booking on this signature and bail out if it fails
      if(book_per_signature(*monSigIt).isFailure()){ return(StatusCode::FAILURE); }

      //---Prepare new table with counters
      for(size_t ii=0;ii<9;++ii) {
        m_counters[(*monSigIt)].push_back(0);	
      }
    
    }//done looping through monitored signatures


    //book histograms for offline comparison if requested
    if(m_doOfflineComparisons){
      ATH_MSG_DEBUG("Booking histograms for offline comparison.");

      //book electron (isPhoton=false)
      if(book_offline(false).isFailure()){ return(StatusCode::FAILURE); }
      //book photon (isPhoton=true)
      if(book_offline(true).isFailure()){ return(StatusCode::FAILURE); }

    }else{ ATH_MSG_DEBUG("Not booking histograms for offline comparison."); }

    //book lumi-dependent histograms
    if(m_doLumiCalc){
      ATH_MSG_DEBUG("Booking histograms for lumi.");
      if(book_lumi().isFailure()){ return(StatusCode::FAILURE); }
    }else{ ATH_MSG_DEBUG("Not booking histograms for lumi."); }


  //booking histograms for new lumi block
    ATH_MSG_VERBOSE("Booking for new lumiblock");

    //clear trigger counters 
    m_counters_perLB.clear();
    m_counters_perLB_activeTE.clear();

    //loop through signatures
    vector<string>::iterator sigIt = m_signatures.begin();
    for(; sigIt != m_signatures.end(); ++sigIt){
      for(size_t i=0;i<9;++i){
        ATH_MSG_VERBOSE("Resetting per-LB counters for lumiblock "<<m_lumiblocknbr<<" and signature "<<*sigIt);
        m_counters_perLB[*sigIt].push_back(0);
        m_counters_perLB_activeTE[*sigIt].push_back(0);
      }
    }


  //all done
  ATH_MSG_DEBUG("Done booking histograms.");
  return(StatusCode::SUCCESS);
}
//END OF HISTOGRAM BOOKING



/*  FILL HISTOGRAMS
 */
StatusCode HLTEgammaNavSigTEMonTool::fill()
{
  ATH_MSG_DEBUG("Starting to fill histograms.");

  //fetch event info
  // - note: maybe we should bail out if we cannot retrieve event info.
  //         i don't know whether a situation can arise where this fails
  //         but the rest is working fine.
  // - note: we might also try and use the run/LB/event numbers already
  //         supplied by IHLTMonTool, but would still need to read timestamps.
  const EventInfo *eventInfo = NULL;
  if((m_storeGate->retrieve(eventInfo)).isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve EventInfo container for this event.");
  }else if(!eventInfo){
    ATH_MSG_ERROR("Retrieved NULL pointer for EventInfo container.");
  }else{

    //fetch run/LB/event numbers and timestamp
    m_runnbr = eventInfo->event_ID()->run_number() ;
    m_lumiblocknbr  = eventInfo->event_ID()->lumi_block();
    m_eventnbr  = eventInfo->event_ID()->event_number();
    unsigned int timeStamp = eventInfo->event_ID()->time_stamp();
    ATH_MSG_DEBUG("EventInfo: Run "<<m_runnbr<<", LB "<<m_lumiblocknbr<<", event "<<m_eventnbr
		  <<", timestamp "<<timeStamp);

    //set first timestamp if this has not already been done
    if(!m_firstTimeStamp){ m_firstTimeStamp = timeStamp; }

    //update last timestamp
    m_lastTimeStamp = eventInfo->event_ID()->time_stamp();

    //add LB to list of processed LBs
    // - first need to check whether we've seen it already
    // - if not we'll add it to the vector
    bool newLB = true;
    vector<unsigned long int>::const_iterator lbIt = m_lumiblocknbr_vect.begin();
    for(; lbIt != m_lumiblocknbr_vect.end(); ++lbIt){
      if(*lbIt == m_lumiblocknbr){ newLB = false; }
    }
    if(newLB){ m_lumiblocknbr_vect.push_back(m_lumiblocknbr); }

  }//done fetching event info

  //apply LAr cleaning
  if ( eventInfo->isEventFlagBitSet(EventInfo::LAr,LArEventBitInfo::BADFEBS) ){
    ATH_MSG_DEBUG("Event rejected by LAr Cleaning");
    return(StatusCode::SUCCESS);
  }

  //////////////////////////////
  // fill offline egamma info //
  //////////////////////////////

  //Retrieve Offline Egamma Containers if needed
  if(!m_doOfflineComparisons){
    ATH_MSG_DEBUG("Offline comparisons not requested. Not retrieving containers.");
  }else{
    ATH_MSG_DEBUG("Retrieving offline egamma containers.");

    //first check that Electron and Photon Container exist in Storegate, if not quit
    if(!m_storeGate->contains<egammaContainer>(m_electronContainerName) ||
       !m_storeGate->contains<egammaContainer>(m_photonContainerName)){
      ATH_MSG_WARNING("Electron or photon containers not found in TDS.");
      return(StatusCode::SUCCESS);
    }

    //retrieve electron container
    if (m_storeGate->retrieve( m_elecTES, m_electronContainerName).isSuccess()  &&  m_elecTES ) {
      ATH_MSG_DEBUG("Successfully retrieved offline electron container."); 
    }else{
      ATH_MSG_WARNING("Couldn't retrieve offline electron container.");
    }     

    //retrieve offline photon container
    if (m_storeGate->retrieve( m_photTES, m_photonContainerName).isSuccess()  &&  m_photTES ) {
      ATH_MSG_DEBUG("Successfully retrieved offline photon container."); 
    }else{
      ATH_MSG_WARNING("Couldn't retrieve offline photon container.");
    }

    //fill from containers
    fillOfflineEgammas(m_elecTES);
    fillOfflineEgammas(m_photTES);

  }//done filling offline egamma info

  //OVERLAPTEST
  // - collect chains that sent this event to express stream
  tmp_chainsToEs.clear();
  const TrigOperationalInfoCollection* opiCol = NULL;
  if(m_storeGate->retrieve(opiCol, "HLT_EXPRESS_OPI_EF").isSuccess() && opiCol){
    ATH_MSG_DEBUG("Successfully retrieved trigger operational information container.");
  }else{
    ATH_MSG_DEBUG("Couldn't retrieve trigger operational information container.");
  }

  if(opiCol){
    ATH_MSG_DEBUG(" - OPI container holds "<<opiCol->size()<<" item(s).");

    //loop through container
    for(TrigOperationalInfoCollection::const_iterator it=opiCol->begin(); it!=opiCol->end(); ++it){

      //fetch item
      std::pair<std::vector<std::string>, std::vector<float> > opiInfo = (*it)->infos();
      ATH_MSG_DEBUG(" -- OPI item contains "<<opiInfo.first.size()<<" express stream chain(s).");

      //loop through chains
      for(unsigned int i=0; i<opiInfo.first.size(); i++){
        ATH_MSG_DEBUG(" --- ("<<i<<") "<<opiInfo.first.at(i));
        tmp_chainsToEs.push_back(opiInfo.first.at(i));
      }//done looping through chains
    
    }//done looping through container 
  
  }
  //OVERLAPTEST

  ////////////////////////
  // Fill Per Signature //
  ////////////////////////

  //loop over signatures
  vector<string>::const_iterator sigIt = m_signatures.begin();
  for(; sigIt != m_signatures.end(); ++sigIt){

    //reset counters
    m_counters[*sigIt][0]++; // m_nTotal    
    m_counters_perLB[*sigIt][0]++; // Total N. of Events per LB
    m_counters_perLB_activeTE[*sigIt][0]++; // Total N. of Events per LB for ActiveTEs

    //call filling function
    if(fill_per_signature(*sigIt).isFailure()){
      ATH_MSG_ERROR("Filling for signature "<<*sigIt<<" failed.");
      return(StatusCode::FAILURE);
    }

  }//done looping over signature

  //all done
  ATH_MSG_DEBUG("Finished filling histograms.");
  return(StatusCode::SUCCESS);
}
//END OF MAIN FILLING



/*  FILL PER SIGNATURE
 */
StatusCode HLTEgammaNavSigTEMonTool::fill_per_signature(const string signature)
{
  ATH_MSG_DEBUG("Starting to fill histograms for signature "<<signature);

  //print out Information on Chains (list of triggers, passes, prescale
  if (print_info_chains(signature).isFailure() ) {
    ATH_MSG_WARNING("Printing of chain info failed for signature "<<signature);
  }

  //start fetching trigger combinations and call combination filler
  // - note: we need to do this separately for our pass-states

  //Possible entries are
  // - 0: ActiveChain -> condition = TrigDefs::Physics, onlyActiveTe = true
  // - 1: ActiveTe -> condition = TrigDefs::alsoDeactivateTE, onlyActiveTe = true
  // - 2: AllTe -> condition = TrigDefs::alsoDeactivateTE, onlyActiveTe = false
  vector<unsigned int> conditions;
  vector<bool> activeTeSwitches;
  if(m_doActiveChain){
    conditions.push_back(TrigDefs::Physics);
    activeTeSwitches.push_back(true);
  }
  if(m_doActiveTe){
    conditions.push_back(TrigDefs::alsoDeactivateTEs);
    activeTeSwitches.push_back(true);
  }
  if(m_doAllTe){
    conditions.push_back(TrigDefs::alsoDeactivateTEs);
    activeTeSwitches.push_back(false);
  }

  //loop through them
  unsigned int n_passStates = conditions.size();
  for(unsigned int i_passStates = 0; i_passStates<n_passStates; i_passStates++){

    //recover values
    unsigned int condition = conditions.at(i_passStates);
    bool onlyActiveTe = activeTeSwitches.at(i_passStates);

    //get L2 chains
    // - note: this can be skipped for ActiveChain filling, because by definition
    //         the chain must have made it through to EF and we can pick up the L2 part
    //         from the EF combination.
    if(condition!=TrigDefs::Physics){

      //get chain groups for signatures
      const Trig::ChainGroup* L2_Chains  = getTDT()->getChainGroup("L2_"+signature);

      //get features for this chain group
      Trig::FeatureContainer L2_features = getTDT()->features(L2_Chains,condition);

      if(msg().level() <= MSG::VERBOSE) msg() << MSG::VERBOSE << " L2 Chains -> Combination Size = " << L2_features.getCombinations().size() << endreq; 

      //loop over l2 combinations
      Trig::FeatureContainer::combination_const_iterator L2_cIt = L2_features.getCombinations().begin();
      for ( ; L2_cIt != L2_features.getCombinations().end(); ++L2_cIt){

        //--- Print out Combination and TE in Combination
        ATH_MSG_VERBOSE(" !!! Combinations = "<<*L2_cIt);

        //skip if this combination is fully active
        // - it will have gotten through to EF and the same argument as for ActiveChain mode above applies
        if(L2_cIt->active()){
          ATH_MSG_VERBOSE("L2 combination is active and will be picked up from EF combination. Skipping.");
          continue;
        }

        //call combination filler
        if(fillCombination((*L2_cIt), signature, condition, onlyActiveTe).isFailure()){
          ATH_MSG_INFO("fillCombination(...) failed for L2_"<<signature);
        } 

      }//done looping over l2 combinations

    }//finished with L2 chains


    const Trig::ChainGroup* EF_Chains  = getTDT()->getChainGroup("EF_"+signature);
    const Trig::ChainGroup* SelChains = EF_Chains;  


    Trig::FeatureContainer EF_features = getTDT()->features(SelChains, condition);


    Trig::FeatureContainer::combination_const_iterator EF_cIt = EF_features.getCombinations().begin();
    for ( ; EF_cIt != EF_features.getCombinations().end(); ++EF_cIt){    
      ATH_MSG_VERBOSE(" !!! Combinations = "<<*EF_cIt);

      if(fillCombination((*EF_cIt), signature, condition, onlyActiveTe).isFailure()){
        ATH_MSG_INFO("fillCombination(...) failed for EF_"<<signature);
      }

    } // End of loop over combinations RoIs


  }//done looping through pass-states

  //all done
  ATH_MSG_VERBOSE("Finished filling for signature "<<signature);
  return(StatusCode::SUCCESS);

} // End of fill_per_signature
//END OF PER-SIGNATURE FILLING




/*  POSTPROCESSING
 *  
 *  - performs end-of-run processing
 *  - fetches lumi for lumiblocks we ran over from DB and fills lumi-related histograms
 *  - calls per-signature prost-processing defined in base
 */
StatusCode HLTEgammaNavSigTEMonTool::proc() {

  //print timestamps of events run over
  ATH_MSG_DEBUG("First timestamp "<<m_firstTimeStamp);
  ATH_MSG_DEBUG("Last  timestamp "<<m_lastTimeStamp);

  //bail out if this is not the end of a run (or input file)
  if(!endOfRun){ return(StatusCode::SUCCESS); }
  ATH_MSG_VERBOSE("Starting end-of-run processing.");

  //luminosity based processing
  if(m_doLumiCalc) {
    ATH_MSG_DEBUG("Trying to access lumi info");

    //create CoolQuery object
    ATH_MSG_DEBUG("Starting CoolQuery for database "<<m_database<< " and trigger "<<m_triggerForLumi);
    CoolQuery * cq_lumi = NULL;
    cq_lumi = new CoolQuery(m_database, m_triggerForLumi);       

    //open database connection (get's closed when CoolQuery is destructed)
    if(!(cq_lumi->openDbConn())) {
      ATH_MSG_INFO("No DB Connection: is this MC? or a problem?");
    }else{

      //loop over lumiblocks
      vector<unsigned long int>::const_iterator lbIt = m_lumiblocknbr_vect.begin();
      for(; lbIt!=m_lumiblocknbr_vect.end(); ++lbIt){ 

        //Create IOVRange
        ATH_MSG_DEBUG("Setting IOVRange for run "<<m_runnbr<<", LB "<<(*lbIt));
        IOVRange * iovr = new IOVRange(IOVTime(m_runnbr, (*lbIt)),IOVTime(m_runnbr, (*lbIt)));      

        // set IOVRange
        cq_lumi->setIOV(iovr->start().re_time(), iovr->stop().re_time());      

        // get payload
        ATH_MSG_DEBUG("Lumi calculation: payload "<<m_payload<<", COOLfolder "<<m_coolfolder<<
		      ", tagname "<<m_tagname<<", channel id "<<m_chanid);
        std::map<cool::ValidityKey, float> m_map = 
          cq_lumi->getLumiIterator<float>(m_payload, m_coolfolder, m_tagname, m_chanid);      

        //get luminosity, scale to /cm^2/s and also get log
        float InstLumi = (m_map.begin()->second) * 1E+30; 
        float InstLumi_log10 =  InstLumi>0. ? log10(InstLumi) : -1;
        ATH_MSG_DEBUG("LBAvInstLumi "<<InstLumi<<" /cm^2/s (log10 "<<InstLumi_log10<<")");
        ATH_MSG_DEBUG("Done accessing lumi information.");

        //fill lumi per LB
        FillLumi(InstLumi_log10, m_histoBasePath+"/"+m_lumiPath);
        FillLumiProfile((*lbIt),InstLumi, m_histoBasePath+"/"+m_lumiPath);

        //loop over signature
        vector<string>::const_iterator sigIt = m_signatures.begin();
        for(; sigIt != m_signatures.end(); ++sigIt){ 

          //TODO we could probably pass the vectors on to the functions directly
          //     or do something clever augmenting the dumb "stage"-getter used
          //     by the base class with a numbering scheme. then the vector indices
          //     would correspond to the correct strings and the rate lumi fillers
          //     would knowwhich histograms to fill. it would be much cleaner overall

          //read in counters
          float nTotal_perLB = m_counters_perLB[(*sigIt)][0];
          float nL1_perLB = m_counters_perLB[(*sigIt)][1];
          float nL2Calo_perLB = m_counters_perLB[(*sigIt)][2];
          float nL2ID_perLB = m_counters_perLB[(*sigIt)][3];
          float nEFCalo_perLB = m_counters_perLB[(*sigIt)][4];
          float nEFeg_perLB = m_counters_perLB[(*sigIt)][5];
          float nEFID_perLB = m_counters_perLB[(*sigIt)][6];

          //read in counters active TE
          float nTotal_perLB_activeTE = m_counters_perLB_activeTE[(*sigIt)][0];
          float nL1_perLB_activeTE = m_counters_perLB_activeTE[(*sigIt)][1];
          float nL2Calo_perLB_activeTE = m_counters_perLB_activeTE[(*sigIt)][2];
          float nL2ID_perLB_activeTE = m_counters_perLB_activeTE[(*sigIt)][3];
          float nEFCalo_perLB_activeTE = m_counters_perLB_activeTE[(*sigIt)][4];
          float nEFeg_perLB_activeTE = m_counters_perLB_activeTE[(*sigIt)][5];
          float nEFID_perLB_activeTE = m_counters_perLB_activeTE[(*sigIt)][6];


          ATH_MSG_VERBOSE("Rates per LB for signature "<<*sigIt<< "and LB "<<*lbIt
			  <<": total "<<nTotal_perLB<<", L1 "<<nL1_perLB<<", L2Calo "<<nL2Calo_perLB
			  <<", L2ID "<<nL2ID_perLB<<", EFCalo "<<nEFCalo_perLB<<", EFeg "<<nEFeg_perLB
			  <<", EFID "<<nEFID_perLB);
          ATH_MSG_VERBOSE("Active TE rates per LB for signature "<<*sigIt<< "and LB "<<*lbIt
			  <<": total "<<nTotal_perLB_activeTE<<", L1 "<<nL1_perLB_activeTE
			  <<", L2Calo "<<nL2Calo_perLB_activeTE<<", L2ID "<<nL2ID_perLB_activeTE
			  <<", EFCalo "<<nEFCalo_perLB_activeTE<<", EFeg "<<nEFeg_perLB_activeTE
			  <<", EFID "<<nEFID_perLB_activeTE);


          //--- Prepare Path
          string BasicPath;
          BasicPath = m_BasicPath;
          BasicPath+=(*sigIt);

          //---Fill NEvents/ Lumi vs LB for Active Chains
          bool isPhoton =  isPhotonSignature((*sigIt));
          string BasicPathRates_ActiveChain= m_BasicPath+(*sigIt)+m_BasicPathActiveChain;     

          //fill lumi histograms
          FillRateLumi((*lbIt),nTotal_perLB,"NTot", BasicPathRates_ActiveChain+"/Lumi");
          FillRateLumi((*lbIt),nL1_perLB,"NL1", BasicPathRates_ActiveChain+"/Lumi");	
          FillRateLumi((*lbIt),nL2Calo_perLB,"NL2Calo", BasicPathRates_ActiveChain+"/Lumi");
          FillRateLumi((*lbIt),nEFCalo_perLB,"NEFCalo", BasicPathRates_ActiveChain+"/Lumi");
          FillRateLumi((*lbIt),nEFeg_perLB,"NEFeg", BasicPathRates_ActiveChain+"/Lumi");
          if(!isPhoton) {
            FillRateLumi((*lbIt),nL2ID_perLB,"NL2ID", BasicPathRates_ActiveChain+"/Lumi");		
            FillRateLumi((*lbIt),nEFID_perLB,"NEFTrack", BasicPathRates_ActiveChain+"/Lumi");
          }

          //fill lumi profiles
          FillRateLumiProfile((*lbIt),InstLumi, nTotal_perLB,"NTot", BasicPathRates_ActiveChain+"/Lumi");
          FillRateLumiProfile((*lbIt),InstLumi, nL1_perLB,"NL1", BasicPathRates_ActiveChain+"/Lumi");
          FillRateLumiProfile((*lbIt),InstLumi, nL2Calo_perLB,"NL2Calo", BasicPathRates_ActiveChain+"/Lumi");
          FillRateLumiProfile((*lbIt),InstLumi, nEFCalo_perLB,"NEFCalo", BasicPathRates_ActiveChain+"/Lumi");
          FillRateLumiProfile((*lbIt),InstLumi, nEFeg_perLB,"NEFeg", BasicPathRates_ActiveChain+"/Lumi");
          if(!isPhoton) {
            FillRateLumiProfile((*lbIt),InstLumi, nL2ID_perLB,"NL2ID", BasicPathRates_ActiveChain+"/Lumi");
            FillRateLumiProfile((*lbIt),InstLumi, nEFID_perLB,"NEFTrack", BasicPathRates_ActiveChain+"/Lumi");
          }

          //---Fill NEvents/ Lumi vs LB for ActiveTEs (non necessarily active chains)
          string BasicPathRates_ActiveTEs= m_BasicPath+(*sigIt)+m_BasicPathActiveTEs;     

          FillRateLumi((*lbIt),nTotal_perLB_activeTE,"NTot", BasicPathRates_ActiveTEs+"/Lumi");
          FillRateLumi((*lbIt),nL1_perLB_activeTE,"NL1", BasicPathRates_ActiveTEs+"/Lumi");	
          FillRateLumi((*lbIt),nL2Calo_perLB_activeTE,"NL2Calo", BasicPathRates_ActiveTEs+"/Lumi");
          FillRateLumi((*lbIt),nEFCalo_perLB_activeTE,"NEFCalo", BasicPathRates_ActiveTEs+"/Lumi");
          FillRateLumi((*lbIt),nEFeg_perLB_activeTE,"NEFeg", BasicPathRates_ActiveTEs+"/Lumi");	
          if(!isPhoton){
            FillRateLumi((*lbIt),nL2ID_perLB_activeTE,"NL2ID", BasicPathRates_ActiveTEs+"/Lumi");		
            FillRateLumi((*lbIt),nEFID_perLB_activeTE,"NEFTrack", BasicPathRates_ActiveTEs+"/Lumi");
          }

          //
          FillRateLumiProfile((*lbIt),InstLumi, nTotal_perLB_activeTE,"NTot", BasicPathRates_ActiveTEs+"/Lumi");
          FillRateLumiProfile((*lbIt),InstLumi, nL1_perLB_activeTE,"NL1", BasicPathRates_ActiveTEs+"/Lumi");
          FillRateLumiProfile((*lbIt),InstLumi, nL2Calo_perLB_activeTE,"NL2Calo", BasicPathRates_ActiveTEs+"/Lumi");
          FillRateLumiProfile((*lbIt),InstLumi, nEFCalo_perLB_activeTE,"NEFCalo", BasicPathRates_ActiveTEs+"/Lumi");
          FillRateLumiProfile((*lbIt),InstLumi, nEFeg_perLB_activeTE,"NEFeg", BasicPathRates_ActiveTEs+"/Lumi");
          if(!isPhoton) {
            FillRateLumiProfile((*lbIt),InstLumi, nL2ID_perLB_activeTE,"NL2ID", BasicPathRates_ActiveTEs+"/Lumi");
            FillRateLumiProfile((*lbIt),InstLumi, nEFID_perLB_activeTE,"NEFTrack", BasicPathRates_ActiveTEs+"/Lumi");
          }


        }//done looping over signatures

      }//done looping over lumiblocks

    }//end of DB interaction

    //clean up (closes connection)
    delete cq_lumi;

  }//end of luminosity based processing


  //loop through signatures for per-signature post-processing
  vector<string>::const_iterator sigIt = m_signatures.begin();
  for(; sigIt != m_signatures.end(); ++sigIt){

    //call per-signature processing
    if(proc_per_signature(*sigIt).isFailure()){ return(StatusCode::FAILURE); }

    //read counters for Physics Conditions per Signatures     
    //
    m_nTotal = m_counters[(*sigIt)][0];
    m_nL1 = m_counters[(*sigIt)][1];
    m_nL2Calo = m_counters[(*sigIt)][2];
    m_nL2ID = m_counters[(*sigIt)][3];
    m_nEFCalo = m_counters[(*sigIt)][4];
    m_nEFeg = m_counters[(*sigIt)][5];
    m_nEFID = m_counters[(*sigIt)][6];		

    //print count table
    ATH_MSG_INFO("Table for signature "<<*sigIt);
    PrintTable();

  }//done looping over signatures

  //all done
  ATH_MSG_VERBOSE("Finished end-of-run processing.");
  return(StatusCode::SUCCESS);
}
//END OF POST PROCESSING





/*  PRINT INFO ABOUT CHAINS
 *  TODO still needs to be tidied
 */
StatusCode HLTEgammaNavSigTEMonTool::print_info_chains(const string signature)
{
  const Trig::ChainGroup* L2_passThrough_Chains  = getTDT()->getChainGroup("L2_e10_loose_passL2");
  const Trig::ChainGroup* EF_passThrough_Chains  = getTDT()->getChainGroup("EF_e10_loose_passEF");
  const Trig::ChainGroup* L1_Chains  = getTDT()->getChainGroup("L1_EM.*");
  const Trig::ChainGroup* L2_Chains  = getTDT()->getChainGroup("L2_"+signature);
  const Trig::ChainGroup* EF_Chains  = getTDT()->getChainGroup("EF_"+signature);
  //const Trig::ChainGroup* HLT_Chains = getTDT()->getChainGroup("L2_"+signature+", EF_"+signature);
  const Trig::ChainGroup* All_Chains = getTDT()->getChainGroup("L1_EM.*, L2_"+signature+", EF_"+signature);
  const Trig::ChainGroup* L2_ele_Chains  = getTDT()->getChainGroup("L2_e.*");
  const Trig::ChainGroup* EF_ele_Chains  = getTDT()->getChainGroup("EF_e.*");
  if(msg().level() <= MSG::DEBUG) msg() << MSG::DEBUG << " List Of All L2 Electron Triggers = " << L2_ele_Chains->getListOfTriggers()<< endreq;
  if(msg().level() <= MSG::DEBUG) msg() << MSG::DEBUG << " List Of All EF Electron Triggers = " << EF_ele_Chains->getListOfTriggers()<< endreq;

  //
  const Trig::ChainGroup* SelChains = All_Chains;
  if(msg().level() <= MSG::DEBUG) msg() << MSG::DEBUG << " List Of Selected Triggers = " << SelChains->getListOfTriggers()<< endreq;

  // Now print a list of the trigger chain groups and print whether they are passing or not 
  // Passed = any one of the trigger chains in the group passed 
  if (SelChains->isPassed()) { if(msg().level() <= MSG::DEBUG) msg() << MSG::DEBUG << "Selected Chain group passed!"<< endreq;  }
  if (L2_passThrough_Chains->isPassed()) { if(msg().level() <= MSG::DEBUG) msg() << MSG::DEBUG << "L2_passThrough_Chains group passed!"<< endreq;  }
  if (EF_passThrough_Chains->isPassed()) { if(msg().level() <= MSG::DEBUG) msg() << MSG::DEBUG << "EF_passThrough_Chains group passed!"<< endreq;  }
  if (L1_Chains->isPassed()) { if(msg().level() <= MSG::DEBUG) msg() << MSG::DEBUG << "L1_Chains group passed!"<< endreq;  }
  if (L2_Chains->isPassed()) { if(msg().level() <= MSG::DEBUG) msg() << MSG::DEBUG << "L2_Chains group passed!"<< endreq;  }
  if (EF_Chains->isPassed()) { if(msg().level() <= MSG::DEBUG) msg() << MSG::DEBUG << "EF_Chains group passed!"<< endreq;  }

  // List each of the PassThrough trigger chains and print individually whether it passed or not  
  // and its prescalei (at debug level since this will produce a lot of print)  
  vector<string> L2_passThrough_ChainsList = L2_passThrough_Chains->getListOfTriggers();
  vector<string>::iterator strItr_L2_passThrough;
  for (strItr_L2_passThrough=L2_passThrough_ChainsList.begin(); strItr_L2_passThrough!=L2_passThrough_ChainsList.end(); ++strItr_L2_passThrough) {

    if(msg().level() <= MSG::DEBUG) 
      msg() << MSG::DEBUG 
	    << "L2_passThrough trigger :" << *strItr_L2_passThrough 
	    << " status " << getTDT()->isPassed(*strItr_L2_passThrough) 
	    << " and has prescale " << getTDT()->getPrescale(*strItr_L2_passThrough)
	    << endreq; 
  }


  // and its prescalei (at debug level since this will produce a lot of print)  
  vector<string> EF_passThrough_ChainsList = EF_passThrough_Chains->getListOfTriggers();
  vector<string>::iterator strItr_EF_passThrough;
  for (strItr_EF_passThrough=EF_passThrough_ChainsList.begin(); strItr_EF_passThrough!=EF_passThrough_ChainsList.end(); ++strItr_EF_passThrough) {    
    if(msg().level() <= MSG::DEBUG) 
      msg() << MSG::DEBUG 
	    << "EF_passThrough trigger :" << *strItr_EF_passThrough 
	    << " status " << getTDT()->isPassed(*strItr_EF_passThrough) 
	    << " and has prescale " << getTDT()->getPrescale(*strItr_EF_passThrough)
	    << endreq; 
  }

  // List each of the trigger chains in the L1 chain group and print individually whether it passed or not  
  // and its prescalei (at debug level since this will produce a lot of print)  
  vector<string> L1_ChainsList = L1_Chains->getListOfTriggers();
  vector<string>::iterator strItr_L1;
  for (strItr_L1=L1_ChainsList.begin(); strItr_L1!=L1_ChainsList.end(); ++strItr_L1) {
    if(msg().level() <= MSG::DEBUG) 
      msg() << MSG::DEBUG 
	    << "L1 trigger :" << *strItr_L1 
	    << " status " << getTDT()->isPassed(*strItr_L1) 
	    << " and has prescale " << getTDT()->getPrescale(*strItr_L1)
	    << endreq; 
  }

  // List each of the trigger chains in the L2 chain group and print individually whether it passed or not
  // and its prescalei (at debug level since this will produce a lot of print)  
  vector<string> L2_ChainsList = L2_Chains->getListOfTriggers();
  vector<string>::iterator strItr_L2;
  for (strItr_L2=L2_ChainsList.begin(); strItr_L2!=L2_ChainsList.end(); ++strItr_L2) {
    if(msg().level() <= MSG::DEBUG) 
      msg() << MSG::DEBUG 
	    << "L2 trigger :" << *strItr_L2 
	    << " status " << getTDT()->isPassed(*strItr_L2) 
	    << " and has prescale " << getTDT()->getPrescale(*strItr_L2)
	    << endreq; 
  }

  // List each of the trigger chains in the EF chain group and print individually whether it passed or not
  // and its prescalei (at debug level since this will produce a lot of print)  
  vector<string> EF_ChainsList = EF_Chains->getListOfTriggers();
  vector<string>::iterator strItr_EF;
  for (strItr_EF=EF_ChainsList.begin(); strItr_EF!=EF_ChainsList.end(); ++strItr_EF) {
    if(msg().level() <= MSG::DEBUG) 
      msg() << MSG::DEBUG 
	    << "EF trigger :" << *strItr_EF 
	    << " status " << getTDT()->isPassed(*strItr_EF) 
	    << " and has prescale " << getTDT()->getPrescale(*strItr_EF)
	    << endreq; 
  }

  return StatusCode::SUCCESS;
}
//END OF CHAIN INFO PRINTING




/*  FILL OFFLINE EGAMMA HISTOGRAMS FROM CONTAINER
 */
void HLTEgammaNavSigTEMonTool::fillOfflineEgammas(const egammaContainer* egCont)
{
  ATH_MSG_DEBUG("Filling offline egamma histograms.");

  //loop over container
  ATH_MSG_VERBOSE("Looping over offline egamma container.");
  egammaContainer::const_iterator egIt = egCont->begin();
  for (; egIt != egCont->end() ; ++egIt) {

    //decide on electron/photon based on permitted authors & ID
    // - note: it's not either/or, because egamma can fail our author selection
    //         that's also why we can't just check the first object in the container
    bool isElectron = (*egIt)->author(egammaParameters::AuthorElectron);
    bool isPhoton = (*egIt)->author(egammaParameters::AuthorPhoton) ||
      (*egIt)->author(egammaParameters::AuthorRConv);

    bool isMediumPPElectron = (*egIt)->passID(egammaPID::ElectronIDMediumPP);
    bool isTightPhoton = (*egIt)->passID(egammaPID::PhotonIDTight);

    //now apply appropriate pt cut and call egamma filling function
    if(isElectron && isMediumPPElectron){
      fillOfflineEgamma((*egIt), isPhoton, "", m_offEle_minptcut, m_offPho_minptcut);
    }else if(isPhoton && isTightPhoton){
      fillOfflineEgamma((*egIt), isPhoton, "", m_offEle_minptcut, m_offPho_minptcut);
    }
  }//done looping over container

  //all done
  ATH_MSG_DEBUG("Finished filling offline egamma histograms.");
}




/*  FILL COMBINATIONS
 */
StatusCode HLTEgammaNavSigTEMonTool::fillCombination(Trig::Combination comb, const string signature,
						     unsigned int condition, bool onlyActiveTe) {


  //set path based on pass-state
  string path = m_histoBasePath + "/" + signature + "/";
  bool skipActiveTE = false;
  if(onlyActiveTe && condition==TrigDefs::alsoDeactivateTEs){ path += m_activeTePath; skipActiveTE = m_skipActiveTe;}
  else if(!onlyActiveTe && condition==TrigDefs::alsoDeactivateTEs){ path += m_allTePath; }
  else if(onlyActiveTe && condition==TrigDefs::Physics){ path += m_activeChainPath; }
  else {
    ATH_MSG_WARNING("Wrong arguments in fillCombination(...). QUITTING.");
    return(StatusCode::SUCCESS);
  }
  ATH_MSG_VERBOSE("Path set to "<<path);
  ATH_MSG_VERBOSE("Filling combination "<<comb);

  //find out whether this is a photon signature
  bool isPhoton = isPhotonSignature(signature);

  //abort if we require a passthrough chain to have passed and it didn't
  //TODO find out what effect an active passThroughChain would have
  if(m_usePreConditionalPassThroughChain &&
     (condition == TrigDefs::alsoDeactivateTEs) &&
     !getTDT()->isPassed(m_passThroughChainL2)){
    ATH_MSG_INFO("Required L2 passthrough chain "<<m_passThroughChainL2<< " not passed. Skipping L2 filling.");
    return(StatusCode::SUCCESS);
  }

  //fetch trigger elements from combination and print out info
  //TODO move all this into the VERBOSE block
  const vector<const HLT::TriggerElement*>& tes = comb.tes();
  ATH_MSG_VERBOSE("Checking combination: N_TE = "<<tes.size());
  for (unsigned int i=0; i<tes.size(); ++i) {
    ATH_MSG_VERBOSE("TE["<<i<<"] = "<<Trig::getTEName(*tes[i]));
  }

  //get initial ROIs and skip if none were found
  vector< Trig::Feature<TrigRoiDescriptor> > initRois = comb.get<TrigRoiDescriptor>("initialRoI",condition);
  ATH_MSG_VERBOSE("Number of initial ROIs = "<<initRois.size());
  if(initRois.size() < 1){
    ATH_MSG_DEBUG("No initial ROIs. Skipping this combination");
    return(StatusCode::SUCCESS);
  }

  //clear vectors to remember L1 basic kinematic info
  // - note: this may not have been done at the end of the previous call,
  //         because we don't always reach the end
  m_roi_et.clear();
  m_roi_eta.clear();
  m_roi_phi.clear();

  //OVERLAPTEST
  // - check whether this chain sent to ES
  bool chainToEs = false;
  if(condition==TrigDefs::Physics){
    for(unsigned int iChainsToEs=0; iChainsToEs<tmp_chainsToEs.size(); iChainsToEs++){
      if(tmp_chainsToEs.at(iChainsToEs)=="EF_"+signature){
        chainToEs = true;
        break;
      }
    }
  }
  //OVERLAPTEST


  //loop through ROIs
  vector< Trig::Feature<TrigRoiDescriptor> >::const_iterator initRoiIt = initRois.begin();
  for(; initRoiIt!= initRois.end(); ++initRoiIt){

    //print out ROI info
    const Trig::Feature<TrigRoiDescriptor> feature = *initRoiIt;
    const TrigRoiDescriptor* roi = feature.cptr();
    const HLT::TriggerElement* te = feature.te();
    ATH_MSG_DEBUG("ROI info: feature: "<<feature<<", roi word: "<<roi->roiWord()<<", TE name(s): "
		  <<Trig::getTEName(*te));

    //get EMTAU ROI associated to this initial ROI
    Trig::Feature<EmTau_ROI> emTauRoi = getTDT()->ancestor<EmTau_ROI>(*initRoiIt);
    if (emTauRoi.empty()){
      ATH_MSG_DEBUG("No EmTau_ROI for this TrigRoiDescriptor. Aborting L2 filling.");
      return(StatusCode::SUCCESS);
    }

    //store kinematic info
    m_roi_et.push_back((emTauRoi.cptr())->getEMClus());
    m_roi_eta.push_back((emTauRoi.cptr())->getEta());
    m_roi_phi.push_back((emTauRoi.cptr())->getPhi());

    
    //OVERLAPTEST
    // - fill L1 histograms depending on whether this chains sent to ES
    if(condition==TrigDefs::Physics){
      string overlapTestPath = m_histoBasePath+"/"+signature+"/OverlapTest";
      ATH_MSG_VERBOSE("Filling OVERLAPTEST histograms at path "<<overlapTestPath);
      if(chainToEs){
	hist("l1pt_this",overlapTestPath)->Fill((emTauRoi.cptr())->getEMClus()/CLHEP::GeV);
      }else{
	hist("l1pt_other",overlapTestPath)->Fill((emTauRoi.cptr())->getEMClus()/CLHEP::GeV);
      }
    }
    //OVERLAPTEST

    //fill level 1 plots
    if(!skipActiveTE) fillL1((*emTauRoi.cptr()),path+"/L1");

    //if we are only filling for active trigger elements (enforce a pass decision later on)
    //fill PreRejection histograms using this ROI
    if(onlyActiveTe){
      if(!skipActiveTE) FillRates((emTauRoi.cptr())->getEMClus(),(emTauRoi.cptr())->getEta(),
		(emTauRoi.cptr())->getPhi(), "L1" ,path+"/PreRejection");

      //if we are not requiring a passed chain, also fill L1 counter
      //this is so we won't loose L1 counts by only looking at chains
      //that are fully passed
      if(condition==TrigDefs::alsoDeactivateTEs){ m_counters_perLB_activeTE[signature][1]++; }

      //if we require a passed chain, also increment those counters
      if(condition==TrigDefs::Physics){
	//TODO can the total counter not perhaps be filled from the LB counter
	//     during end of lumiblock processing?
	m_counters[signature][1]++;
	m_counters_perLB[signature][1]++;
      }
    }

    //print kinematic info
    ATH_MSG_VERBOSE("L1 ROI info: thresholdname "<<(emTauRoi.cptr())->getThresholdNames()
		    <<", et "<<(emTauRoi.cptr())->getEMClus() / CLHEP::GeV<<", eta "<< (emTauRoi.cptr())->getEta()
		    <<", phi "<<(emTauRoi.cptr())->getPhi());

    //match to offline using calo information
    ATH_MSG_VERBOSE("- Trying to match L1 to offline");
    const egamma* matchedEgamma = matchOffline((emTauRoi.cptr())->getEta(),(emTauRoi.cptr())->getPhi(),
					       true, isPhoton);
    if(matchedEgamma){

      //fill resolution and efficiency
      if(!skipActiveTE) fillL1OffRes((*emTauRoi.cptr()),matchedEgamma, path+"/L1/Resolution");
      if(!skipActiveTE) fillOfflineMatches(matchedEgamma, isPhoton, path+"/L1", m_offEle_minptcut, m_offPho_minptcut);

    }//done matching to offline      

  }//done looping through initial ROIs


  //get L2 clusters
  vector< Trig::Feature<TrigEMCluster> > L2Calo = comb.get<TrigEMCluster>("",condition);
  ATH_MSG_VERBOSE("- Number of L2Calo Clusters "<<L2Calo.size());
  if(L2Calo.size() < 1){
    ATH_MSG_DEBUG("- No TrigEMClusters found. Aborting L2 combination filling.");
    return(StatusCode::SUCCESS);
  }

  //declare pointer to TrigEMCluster
  // - need to do this here, because we will use it outside of the cluster loop as well
  // - reused during L2ID filling (dEta/dPhi etc.)
  // - reused during EFCalo filling
  // TODO: find out whether picking the last cluster in the loop is appropriate
  const TrigEMCluster* l2CaloCluster = NULL;

  //create set of offline matches;
  set<const egamma*> L2CalomatchedEgamma;

  //loop over clusters
  vector< Trig::Feature<TrigEMCluster> >::const_iterator clusIt = L2Calo.begin();
  for(; clusIt!= L2Calo.end(); ++clusIt){

    //fetch pointer to cluster and print kinematic info
    // - this looks cumbersome. Is it a quirk of the FeatureContainer?
    l2CaloCluster = clusIt->cptr();
    ATH_MSG_VERBOSE("- L2Calo cluster info: et "<<calcEt(l2CaloCluster->energy(), l2CaloCluster->eta()) / CLHEP::GeV
		    <<", eta "<<l2CaloCluster->eta()<< ", phi "<< l2CaloCluster->phi());

    //check if CaloCluster is active and skip it if we're only looking at active TEs and it's not
    if (!(clusIt->te()->getActiveState()) && onlyActiveTe) {
      ATH_MSG_DEBUG("- L2Calo cluster not active. Skipping.");
      continue;
    }

    //fill L2Calo histograms
    if(!skipActiveTE) fillL2Calo(l2CaloCluster, path+"/L2Calo");

    //if we have required this cluster to be active, fill rates/counters/rejections
    if(onlyActiveTe){

      //match to L1 ROI and fill rejection histograms
      bool l1Matched = matchL1(l2CaloCluster->eta(), l2CaloCluster->phi());
      if(l1Matched && !skipActiveTE){ FillRates(m_roiMatch_et, m_roiMatch_eta, m_roiMatch_phi, "L2Calo" ,path+"/PreRejection"); }

      //increment counters
      if(condition== TrigDefs::alsoDeactivateTEs){ m_counters_perLB_activeTE[signature][2]++; }
      if(condition== TrigDefs::Physics){
        m_counters[signature][2]++;
        m_counters_perLB[signature][2]++;
      }

    }

    //match to Offline using calorimeter info
    ATH_MSG_VERBOSE(" - Trying to match L2Calo to offline");
    const egamma* matchedEgamma = matchOffline(l2CaloCluster->eta(),l2CaloCluster->phi(), true, isPhoton);

    //fill L2Calo resolutions if a match was found, insert pointer in set
    if(matchedEgamma){
      if(!skipActiveTE) fillL2CaloOffRes(l2CaloCluster,matchedEgamma, path+"/L2Calo/Resolution");
      L2CalomatchedEgamma.insert(matchedEgamma);
    }

  }//done looping over l2 calo clusters
  
  //fill matches for efficiencies L2Calo
  if(L2CalomatchedEgamma.size() > 0){
    set<const egamma*>::const_iterator L2CalomatchIt = L2CalomatchedEgamma.begin();
    for(; L2CalomatchIt != L2CalomatchedEgamma.end(); ++L2CalomatchIt){
      if(!skipActiveTE) fillOfflineMatches(*L2CalomatchIt, isPhoton, path+"/L2Calo", m_offEle_minptcut, m_offPho_minptcut);
    }
  }


  //get TrigInDetTrackCollections and check their number
  // - we will bail out if processing an electron chain but no track collections have been found
  vector< Trig::Feature<TrigInDetTrackCollection> > L2ID = comb.get<TrigInDetTrackCollection>("",condition);
  ATH_MSG_VERBOSE(" - Number of L2ID Collections = "<<L2ID.size());
  if (!isPhoton && !L2ID.size()){
    ATH_MSG_DEBUG(" - No L2ID Collections. Skipping L2 combination.");
    return(StatusCode::SUCCESS);
  }

  //create set of offline matches for L2Track
  set<const egamma*> L2TrackmatchedEgamma;

  //loop over track collections
  // - skipped in its entirety for photon chains, regardless of whether we do have track collections
  //   in this combination or not (we probably shouldn't anyway, but let's be on the safe side)
  // -- this is done in the "continue if"-part of the foor loop
  vector< Trig::Feature<TrigInDetTrackCollection> >::const_iterator trackCollIt =  L2ID.begin();
  for(; !isPhoton && trackCollIt!= L2ID.end(); ++ trackCollIt ){

    //skip collection if it's not active and we only look at active ones
    if (!(trackCollIt->te()->getActiveState()) && onlyActiveTe){
      ATH_MSG_VERBOSE(" - L2ID TE not active. Skipping it.");
      continue;
    }

    //get number of tracks in collection and check it
    //TODO why do we bail out rather than just going on to the next track collection?
    //     this happens multiple times
    ATH_MSG_VERBOSE(" - Number of tracks in L2ID Collection = "<<trackCollIt->cptr()->size());
    if (!trackCollIt->cptr()->size()){
      ATH_MSG_DEBUG(" - No tracks in L2ID Collection. Skipping it.");
      return(StatusCode::SUCCESS);
    }

    //loop over tracks in collection
    TrigInDetTrackCollection::const_iterator trackIt = trackCollIt->cptr()->begin();
    for(; trackIt != trackCollIt->cptr()->end(); ++trackIt ){


      //fill L2 track histograms
      if(!skipActiveTE) fillL2ID(*trackIt, l2CaloCluster, path+"/L2ID");

      //if only considering active track collections, fill rates/rejection
      //TODO is it just the express stream menu (without alternative tracking algos)
      //     that keeps us from doing this several times?
      if(onlyActiveTe){

        //extrapolate (again, after the L2ID filler already did it) to find eta/phi for L1 match
        float etac(-999), phic(-999);
        if(extrapolateTrackToCalo(*trackIt)){
          etac = m_extrapolatedEta;
          phic = m_extrapolatedPhi;
        }

        //match to L1 using calo position and fill rejection histograms
        bool l1Matched = matchL1(etac, phic);
        if(l1Matched && !skipActiveTE){ FillRates(m_roiMatch_et, m_roiMatch_eta, m_roiMatch_phi, "L2ID" ,path+"/PreRejection"); }

        //increment counters
        if(condition== TrigDefs::alsoDeactivateTEs){ m_counters_perLB_activeTE[signature][3]++; }
        if(condition== TrigDefs::Physics){
          m_counters[signature][3]++;
          m_counters_perLB[signature][3]++;
        }

      }

      //match to offline using track eta/phi for both l2 track and offline
      const egamma* matchedEgamma = matchOffline((*trackIt)->param()->eta(), (*trackIt)->param()->phi0(),
						 false, isPhoton);

      //fill L2 Track resolution histos if a match was found, insert pointer in L2 Track set
      if(matchedEgamma){      
        if(!skipActiveTE) fillL2IDOffRes(*trackIt, matchedEgamma, path+"/L2ID/Resolution");
  	L2TrackmatchedEgamma.insert(matchedEgamma);
      }      
    }//done looping through tracks

  }//done looping over L2 track collections


  //fill L2Track matched objects in efficiency histos
  if(!L2TrackmatchedEgamma.empty()){
    set<const egamma*>::iterator L2TrackmatchIt;
     for(L2TrackmatchIt = L2TrackmatchedEgamma.begin(); L2TrackmatchIt!=L2TrackmatchedEgamma.end(); ++L2TrackmatchIt){
       if(!skipActiveTE) fillOfflineMatches(*L2TrackmatchIt, isPhoton, path+"/L2ID", m_offEle_minptcut, m_offPho_minptcut);
    }
  }

  //TODO this is where we start diverging from the old fillL2Combination. Figure out whether we need
  //to do anything

  //////////////////
  //---Starting EF
  //////////////////

  //abort if we require a passthrough chain to have passed and it didn't
  //TODO find out what effect an active passThroughChain would have
  if(m_usePreConditionalPassThroughChain &&
     (condition == TrigDefs::alsoDeactivateTEs) &&
     !getTDT()->isPassed(m_passThroughChainL2)){
    ATH_MSG_INFO("Required EF passthrough chain "<<m_passThroughChainEF<< " not passed. Skipping EF filling.");
    return(StatusCode::SUCCESS);
  }

  //fetch EFCalo containers
  vector< Trig::Feature<CaloClusterContainer> > EFCalo = comb.get<CaloClusterContainer>("",condition); 
  ATH_MSG_VERBOSE("- Number of EFCalo ClusterContainers "<<EFCalo.size());
  if(!EFCalo.size()){
    ATH_MSG_DEBUG("- No CaloClusterContainers found. Aborting combination filling.");
    return(StatusCode::SUCCESS);
  }

  //loop over EFCalo containers
  vector< Trig::Feature<CaloClusterContainer> >::const_iterator clusContIt =  EFCalo.begin();
  for(; clusContIt!= EFCalo.end(); ++ clusContIt){

    //check if CaloCluster is active and skip it if we're only looking at active TEs and it's not
    if(onlyActiveTe && !clusContIt->te()->getActiveState()) { 
      ATH_MSG_VERBOSE(" - EFCalo ClusterContainer not active. Skipping.");
      continue;
    } 

    //report on number of clusters in container
    ATH_MSG_VERBOSE(" - Number of CaloClusters in EFCalo container = "<<clusContIt->cptr()->size());

    set<const egamma*> EFCalomatchedEgamma;

    //loop over clusters
    CaloClusterContainer::const_iterator clusIt = clusContIt->cptr()->begin();
    for(; clusIt != clusContIt->cptr()->end(); ++clusIt ){

      //---Fill EFCalo Plots
      
      if(!skipActiveTE) fillEFCalo(*clusIt, l2CaloCluster, path+"/EFCalo"); 
      
      //if we're only looking at active clusters, fill rejection/counters
      if(onlyActiveTe){

        bool l1Matched = matchL1((*clusIt)->eta(), (*clusIt)->phi());
        if(l1Matched && !skipActiveTE){ FillRates(m_roiMatch_et,m_roiMatch_eta,m_roiMatch_phi, "EFCalo" ,path+"/PreRejection"); }

        if(condition== TrigDefs::alsoDeactivateTEs){ m_counters_perLB_activeTE[signature][4]++; }
        if(condition== TrigDefs::Physics){
          m_counters[signature][4]++;
          m_counters_perLB[signature][4]++;
        }

      }

      //match to offline using calorimeter info
      ATH_MSG_VERBOSE("Trying to match EFCalo to offline");
      const egamma* matchedEgamma = matchOffline((*clusIt)->eta(),(*clusIt)->phi(), true, isPhoton);

      //fill EFCalo resolutions and offline efficiency if a match was found
      if(matchedEgamma){
        if(!skipActiveTE) fillEFCaloOffRes(*clusIt, matchedEgamma, path+"/EFCalo/Resolution");
	EFCalomatchedEgamma.insert(matchedEgamma);
      } 

    }//done looping over CaloClusters

    //fill EFCalo matched objects in efficiency histos 
    if(!EFCalomatchedEgamma.empty()){ 
      set<const egamma*>::iterator EFCalomatchIt; 
      for(EFCalomatchIt = EFCalomatchedEgamma.begin(); EFCalomatchIt!=EFCalomatchedEgamma.end(); ++EFCalomatchIt){ 
	if(!skipActiveTE) fillOfflineMatches(*EFCalomatchIt, isPhoton, path+"/EFCalo", m_offEle_minptcut, m_offPho_minptcut); 
      } 
    } 

  }//done looping over ClusterContainers


  //get EF egamma containers
  vector< Trig::Feature<egammaContainer> > EFeg = comb.get<egammaContainer>("",condition);
  ATH_MSG_VERBOSE(" - Number of EFeg Collections = "<<EFeg.size());
  if (!EFeg.size()){
    ATH_MSG_DEBUG(" - No EFeg Collections. Aborting combination filling.");
    return(StatusCode::SUCCESS);
  }

  //loop over EF egammaContainers
  vector< Trig::Feature<egammaContainer> >::const_iterator egContIt = EFeg.begin();
  for(; egContIt != EFeg.end() ; ++egContIt){

    //skip container if it's not active and we only look at active ones
    //TODO figure out whether the check for Electron/Photon containers is appropriate
    //     it looks like we might allow conversions without asking them to be active
    //     unless this is checked later on somewhere, which raises the question why we
    //     aren't checking it here
    if(onlyActiveTe && !egContIt->te()->getActiveState() &&
       ((egContIt->label().find("Electron") != string::npos ||
	 (egContIt->label().find("Photon") != string::npos)))) { 
      ATH_MSG_VERBOSE("EFeg not active. Skipping it.");
      continue;
    } 

    //loop over EFegamma objects
    ATH_MSG_VERBOSE(" - Number of objects in EFeg container = "<<egContIt->cptr()->size());
    egammaContainer::const_iterator egIt = egContIt->cptr()->begin();

    set<const egamma*> EFegmatchedEgamma;

    for(; egIt != egContIt->cptr()->end(); ++egIt){

      //
      if(!skipActiveTE) fillEFeg((*egIt), path+"/EFeg", 0);
      
      //fill EMShowers for calorimetry from EF egamma containers
      if(!skipActiveTE) fillEFCaloShower((*egIt), path+"/EFCalo");
      
      //if we are only looking at active TEs increment counters
      if(onlyActiveTe){

        if(condition== TrigDefs::alsoDeactivateTEs){ m_counters_perLB_activeTE[signature][5]++; }
        if(condition== TrigDefs::Physics){
          m_counters[signature][5]++; 
          m_counters_perLB[signature][5]++;
        }

      }

      //match to offline using track directions and fill EFeg resolution and efficiency
      ATH_MSG_VERBOSE("Trying to match EFeg to offline.");
      const egamma* matchedEgamma = matchOffline((*egIt)->eta(),(*egIt)->phi(), false, isPhoton);
      if(matchedEgamma){

        if(!skipActiveTE) fillEFegOffRes(*egIt,matchedEgamma, path+"/EFeg/Resolution" );
	EFegmatchedEgamma.insert(matchedEgamma);

      } 

      set<const egamma*> EFIDmatchedEgamma;

      //try getting associated track
      // - note: this relies on the compiler skipping the "if"'ed instructions as
      //         soon as no trackParticle pointer is returned. is this always safe?
      if((*egIt)->trackParticle() && (*egIt)->trackParticle()->measuredPerigee() ) {

        //fetch track directions for matching 
        float trk_eta = calcEta((*egIt)->trackParticle()->measuredPerigee()->parameters()[Trk::theta]);
        float trk_phi = (*egIt)->trackParticle()->measuredPerigee()->parameters()[Trk::phi];

        //fill EFID histograms
        if(!skipActiveTE) fillEFID(*egIt, path+"/EFID");

        //if we're only looking at active TEs fill rejection histograms and update pointers
        // - note: are we not active by definition?
        if(onlyActiveTe){
          bool l1Matched = matchL1(trk_eta, trk_phi);
          if(l1Matched && !skipActiveTE){ FillRates(m_roiMatch_et,m_roiMatch_eta,m_roiMatch_phi, "EFID" ,path+"/PreRejection"); }
          if(condition==TrigDefs::alsoDeactivateTEs){ m_counters_perLB_activeTE[signature][6]++; }
          if(onlyActiveTe && condition== TrigDefs::Physics){
            m_counters[signature][6]++;
            m_counters_perLB[signature][6]++;
          }
        }

        //---Match to Offline and fill resolutions/efficiencies
        ATH_MSG_VERBOSE("Trying to match EFID to offline using track directions.");
        const egamma* matchedEgamma = matchOffline(trk_eta,trk_phi, false, isPhoton);
        if(matchedEgamma){
          if(!skipActiveTE) fillEFIDOffRes(*egIt, matchedEgamma, path+"/EFID/Resolution");
	  EFIDmatchedEgamma.insert(matchedEgamma);
        } 

      }//done processing track

      //fill EFCalo matched objects in efficiency histos  
      if(!EFIDmatchedEgamma.empty()){  
	set<const egamma*>::iterator EFIDmatchIt;  
	for(EFIDmatchIt = EFIDmatchedEgamma.begin(); EFIDmatchIt!=EFIDmatchedEgamma.end(); ++EFIDmatchIt){  
	  if(!skipActiveTE) fillOfflineMatches(*EFIDmatchIt, isPhoton, path+"/EFID", m_offEle_minptcut, m_offPho_minptcut);  
	}  
      }
      
    }//done looping over egamma objects

    //fill EFeg matched objects in efficiency histos  
    if(!EFegmatchedEgamma.empty()){  
      set<const egamma*>::iterator EFegmatchIt;  
      for(EFegmatchIt = EFegmatchedEgamma.begin(); EFegmatchIt!=EFegmatchedEgamma.end(); ++EFegmatchIt){  
        if(!skipActiveTE) fillOfflineMatches(*EFegmatchIt, isPhoton, path+"/EFeg", m_offEle_minptcut, m_offPho_minptcut);  
      }  
    }  

  }//done looping over egamma containers

  //all done. clean up and say goodbye.
  m_roi_et.clear();
  m_roi_eta.clear();
  m_roi_phi.clear();
  ATH_MSG_DEBUG("Finished filling combination.");
  return(StatusCode::SUCCESS);
}
//END OF COMBINATION FILLING




/*************************************
 *
 *              HELPERS
 *
 ************************************/

/*  MATCHING TO L1 ROI BY ETA/PHI
 */
bool HLTEgammaNavSigTEMonTool::matchL1(const float eta, const float phi)
{
  //check roi kinematics vectors are filled consistently
  unsigned int n_roi = m_roi_et.size();
  if(m_roi_eta.size() != n_roi || m_roi_phi.size() != n_roi){
    ATH_MSG_WARNING("L1 ROI kinematics vectors not consistently filled.");
    return(false);
  }

  if(!n_roi){
    ATH_MSG_WARNING("L1 ROI kinematics vectors empty. Cannot match.");
    return(false);
  }

  //loop through vectors and find index of closest match
  //TODO currently matching to all radii, should we employ a maximum (i.e. 0.15)?
  float drMin = 999;
  unsigned int matchIndex = 9999;
  for(unsigned int i=0; i<n_roi; i++){
    float dr = calcDeltaR(eta, m_roi_eta.at(i), phi, m_roi_phi.at(i));
    if(dr < drMin){
      drMin = dr;
      matchIndex = i;
    }
  }//done looping through vectors

  //return if no match was found
  // - this should not happen, as we usually only try to match to L1 if there is something at L1
  // - if a strict maximum distance is introduced, it might happen
  // - the following relies on there never beein close to 10^4 ROIs
  if(matchIndex == 9999){ return(false); }

  //otherwise update match variables and return match confirmation
  m_roiMatch_et = m_roi_et.at(matchIndex);
  m_roiMatch_eta = m_roi_eta.at(matchIndex);
  m_roiMatch_phi = m_roi_phi.at(matchIndex);
  return(true);
}
//END OF MATCHING TO L1



/*  MATCHING TO OFFLINE EGAMMA OBJECT BY ETA/PHI
 *
 *  - uses electron/photon containers loaded during init()
 *  - checks author, applies etcut
 *  - returns pointer to closest object within dR < 0.15 (configurable in JOs)
 *            or NULL in case no suitable match is found
 */
const egamma* HLTEgammaNavSigTEMonTool::matchOffline(const float eta, const float phi, bool useCalo, bool isPhoton)
{
  ATH_MSG_DEBUG("Starting to match eta/phi to offline egamma object.");
  //initialise pointer to return object
  const egamma* match = NULL;
  //initialise distance of current match to maximum allowed (in job options)
  float match_dr = m_dR_off;

  //set egammaContainer based on isPhoton
  egammaContainer const* cont = NULL;
  if(isPhoton){ cont = m_photTES; }
  else{ cont = m_elecTES; }

  //set up container iterators
  egammaContainer::const_iterator egIt = cont->begin();
  egammaContainer::const_iterator egItE = cont->end();

  //loop over container
  for(; egIt != egItE ; ++egIt){

    //initialise variables to hold egamma info
    float eg_pt = -999;
    float eg_eta = -999;
    float eg_phi = -999;

    //try getting handle on cluster if we are going to use it
    const CaloCluster* clus = NULL;
    if(useCalo){
      clus = (*egIt)->cluster();
      if(!clus){ ATH_MSG_DEBUG(" - cluster use requested but no cluster found. falling back to egamma info"); }
    }

    //fetch egamma info (either from cluster if we have obtained it, or from combined object)
    if(clus){
      eg_eta = clus->eta();
      eg_phi = clus->phi();
      //TODO replace with calcEt
      eg_pt = calcEt( clus->et(), eg_eta);
    }else{
      eg_eta = (*egIt)->eta();
      eg_phi = (*egIt)->phi();
      eg_pt = (*egIt)->pt();
    }

    //dump current egamma info
    ATH_MSG_VERBOSE(" - at egamma object (eta="<<eg_eta<<", phi="<<eg_phi<<", pt="<<eg_pt<<")");

    //apply author / minimum pt cuts
    bool pass_author(false), pass_ID(false);
    if(isPhoton){
      pass_author = ((*egIt)->author(egammaParameters::AuthorPhoton) ||
		     (*egIt)->author(egammaParameters::AuthorRConv));
      pass_ID = (*egIt)->passID(egammaPID::PhotonIDTight);
    }else{
      pass_author = (*egIt)->author(egammaParameters::AuthorElectron);
      pass_ID = (*egIt)->passID(egammaPID::ElectronIDMediumPP); 
    }
    if(!pass_author || !pass_ID){ continue; }
    ATH_MSG_VERBOSE(" -- egamma object passes author and ID cuts.");

    //compute distance to trigger object and update match pointer if this is closer than the previous best
    float dr = calcDeltaR(eta,eg_eta,phi,eg_phi);
    ATH_MSG_VERBOSE(" -- egamma object at distance dr="<<dr);
    if(dr<match_dr){
      match_dr = dr;
      match = *egIt;
      ATH_MSG_VERBOSE(" -- egamma object is closest match so far.");
    }

  }//done looping over container

  //return match
  if(!match){ ATH_MSG_VERBOSE("No good match found. Returning NULL."); }
  ATH_MSG_DEBUG("Finished matching to offline egamma object.");
  return(match);

}

