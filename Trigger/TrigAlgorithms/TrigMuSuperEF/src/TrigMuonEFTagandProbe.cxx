
/*
  Skeleton for in progress Event Filter Z_mu+mu- Online Tag and Probe algorithm
  Possible names: TrigMuonEFTagandProbe
  Authors: JJamieson, University of Glasgow, 09/10/2017, Edit: 07/11/2017





  Accesses collection of xaod::muons within event and picks only good muons, i.e. muons formed from combined (NOT SA) tracks including possible duplicates

  Preceding Mu22_mu8noL1 trigger can pass more than 2 muon events provided that there are at least one with Pt>22GeV and another with Pt>8GeV, Thus data is cleaned by first applying a 10 GeV cut on pt to remove events with pt below that expected in z_mu+mu- events (10 GeV picked to not be too high as to bias against low momentum Z events and to stay consistent with previous work

  >2 muon events are then trimmed by pairing up all the avaliable opposite charge muons and picking the best fit to Z invariant mass pair, applying a +-20 GeV threshold cut around the Z mass. 



  Is there any reason to try and collect all good muon pairs first in one vector or can we do all calculations live then scrap each pair as they go? 
  Otherwise Would need to declare good muons as a monitored container, remove the clear step from execute and work out how to send the container on as an output of the algorithm

  Need to acces Lvl1 ROI information in order to link lvl1 trigger selection to muon pairs
  Can access lvl1 ROIs in the same way as Muons/CBtracks and get out phi and eta
  Use this to get deltaR^2 = deltaphi^2 + deltaeta^2 for each pairing of lvl 1 ROI to good muon?
  Plot delta R but I assume the idea is to pick the smaller of the two values for each muon pair so maybe cut on massive values before plotting




  TODO:
  Check Python Monitoring and config files for uneeded imports as well


  athenaHLT:

  Run on multiple files using: athenaHLT.py -M -f "['file1','file2',...]" -o HLT_MCV7  -c 'testMCV7=True;HLTOutputLevel=INFO;doValidation=True;fpeAuditor=True' -C "topSequence.TrigSteer_HLT.TrigMuonEFTagandProbe.OutputLevel=DEBUG" TriggerRelease/runHLT_standalone.py >& log 

  Output of online histogramming will be in r000...HLT-Histogramming.root

  athena:

  multiple files in different directories: athena.py -c 'testMCV7=True;HLTOutputLevel=INFO;doValidation=True;fpeAuditor=True;BSRDOInput=[“/path/to/file1.root”,”/path/to/file2.root”]'  TriggerRelease/runHLT_standalone.py ../setupDebugForMuons.py >& log_file

  multiple files in 1 directory: athena.py -c 'testMCV7=True;HLTOutputLevel=INFO;doValidation=True;fpeAuditor=True;from glob import glob;BSRDOInput=glob("/path/to/dir/“)'  TriggerRelease/runHLT_standalone.py setupDebugForMuons.py >& log_file

  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration 
 */

#include <iomanip>
#include <cmath>
#include "TrigMuonEFTagandProbe.h"

#include "TrigConfHLTData/HLTTriggerElement.h"
//#include "TrigT1Interfaces/RecMuonRoI.h"


//Constructor for new algorithm class inheriting from base FexAlgo class
//ISvcLocator is service locator for shared global services between algorithms (e.g. magnetic field information etc)
TrigMuonEFTagandProbe::TrigMuonEFTagandProbe(const std::string &name, ISvcLocator *pSvcLocator) : 
  FexAlgo(name, pSvcLocator),
  m_debug(false),
  m_verbose(false)

{
  //Use constructor to declare properties and containers



  declareMonitoredStdContainer("EF_FS_InvMass",             m_dimuon_invmass,            IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("LVL1_CB_DeltaR",            m_delta_r,                   IMonitoredAlgo::AutoClear);


}

TrigMuonEFTagandProbe::~TrigMuonEFTagandProbe(){} //Destructor


HLT::ErrorCode TrigMuonEFTagandProbe::hltInitialize()
{

  ATH_MSG_DEBUG("testphrase TrigMuonEFTagandProbe::hltInitialize()"); 

  //Might need to use setup to apply pre cuts to reduce data for tag and probe
  //Probably depends where on chain tag and probe is applied 
  //Leave empty for now

  ATH_MSG_INFO("testphrase TrigMuonEFTagandProbe Initialization completed successfully");
 
  return HLT::OK;
}




HLT::ErrorCode TrigMuonEFTagandProbe::hltFinalize()
{
  ATH_MSG_DEBUG("testphrase TrigMuonEFTagandProbe::hltFinalize()"); 

  return HLT::OK;
}



//inputTE and TEout are passed to algorithm from Trigger menu
HLT::ErrorCode TrigMuonEFTagandProbe::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* TEout) 
{
  // Conditional debug flags for when debug output requires extra compute
  m_debug = msgLvl() <= MSG::DEBUG; // MSG levels are 1-7, VERBOSE,DEBUG,INFO,WARNINGS,ERROR,FATAL,ALWAYS
  m_verbose = msgLvl() <= MSG::VERBOSE;

  HLT::ErrorCode hltStatus = HLT::OK; //set modifiable errorcode status as function return for when we want subsequent code to run after an error

  //Reset temporary containers

  m_good_muons.clear();


  ATH_MSG_DEBUG("testphrase TrigMuonEFTagandProbe::hltExecute()"); 

  //DEBUG to check if Trigger events are passing through algorithm or not
  if (m_debug){
    std::string teInLabel;
    std::string teOutLabel;
    TrigConf::HLTTriggerElement::getLabel( inputTE->getId(), teInLabel );
    TrigConf::HLTTriggerElement::getLabel( TEout->getId(), teOutLabel );

    ATH_MSG_DEBUG("testphrase TrigMuonEFTagandProbeExecute() chain Trigger Event input ID=" << inputTE->getId() << " Trigger Event output ID=" << TEout->getId() << " Input Label=" << teInLabel << " Output Label=" << teOutLabel);
  }
 


  // Get muonContainer linked to inputTE--------------------------------------------------------------------

  const xAOD::MuonContainer*  muonContainer=0; 

  // getFeature takes a trigger element (inputTE) and a target feature (MuonContainer) and fills the target with the feature found in the trigger element, Output is a HLT::code, but OK does NOT mean a matching, non zero feature was found just that the feature exists within the trigger element
 
  if(getFeature(inputTE, muonContainer)!=HLT::OK) {  //If getfeature fails
    ATH_MSG_DEBUG("testphrase no xAOD::MuonContainer Feature found");
    return HLT::MISSING_FEATURE;
  } 
  else { //If get feature succeeds 
    if (!muonContainer) { // if muonContainer entry is null
      ATH_MSG_DEBUG("testphrase null xAOD::MuonContainer Feature found");
      return HLT::MISSING_FEATURE;
    }
    
    ATH_MSG_DEBUG("testphrase xAOD::MuonContainer found with size " << muonContainer->size());
  }
  

  // Initialise muon counter, temporary values and other useful stuff here
  
  unsigned int mu_count=0;                                                                                                
  float invMass_dimuon=0.0;
  float deltaR=0.0;

  
  // loop on the muons within the combined RoIs----------------------------------------------------------

  //C++11 new range based for loop style, loops through muonContainer but we have to dereference it first with *
  //auto intelligently gets type from muoncontainer, muon is the name of the variable we write to on every loop
  for(auto muon : *muonContainer) {
                              
    if (!muon) { // Check if muon object is null
      ATH_MSG_DEBUG("testphrase No xAOD::Muon found.");
      continue; //move to next muon
    } 
    else {

      if (muon->primaryTrackParticle()) { //was there a muon in this RoI? (check muon hasn't been stripped of information) 
                                                                                
        const xAOD::TrackParticle* tr = muon->trackParticle(xAOD::Muon::CombinedTrackParticle); //Checking muon is good (i.e. Combined muon NOT stand alone muon)

        if (!tr || tr->pt()/CLHEP::GeV < 10.0) { // if this muon has NOT been formed from a combined track > 10GeV

          ATH_MSG_DEBUG("testphrase Bad muon is of type: " << muon->muonType() << " With 0=combined, 1=MuonSA, 2=SegmentTagged, 3=CaloTagged, 4=SiliconAssociatedForwardMuon, Abs pt = " << muon->pt()/CLHEP::GeV << " GeV. Charge=" << muon->charge() << " Psuedorapidity = " << muon->eta() << " Phi = " << muon->phi());

          continue;
        } 

	//Collect good muons------------------------------------------------------------------------------

	else { //Current muon is formed from combined track
	  ATH_MSG_DEBUG("testphrase Retrieved muon of type " << muon->muonType() << ": Combined track with abs pt " << tr->pt()/CLHEP::GeV << " GeV. Charge=" << tr->charge() << " match chi2 = " << tr->chiSquared() << " Psuedorapidity = " << tr->eta() << " Phi = " << tr->phi());
	  
	  m_good_muons.push_back(muon);
	  mu_count++;
	}
      }
    }
  }
  


  //Might be able to implement this more sensibly 
  
  if (m_good_muons.size()<=1){
    ATH_MSG_WARNING("testphrase Less than 2 combined muons before trimming so event will be ignored, moving to next event");
    return HLT::OK;
  }

  if (m_good_muons.size()>2 || (m_good_muons)[0]->charge() == (m_good_muons)[1]->charge())
    {
      ATH_MSG_DEBUG("testphrase >2 muons found, no. of \"good\" muons = " << m_good_muons.size());
      trim_container(m_good_muons);
      
      if (m_good_muons.size()<=1){
	ATH_MSG_WARNING("testphrase Less than 2 good muons after trimming so further event analysis will be ignored, moving to next event");
	return HLT::OK;
      }
      
      ATH_MSG_DEBUG("testphrase container trimmmed, no. of good muons = " << m_good_muons.size());
    }
  
  
  //Calculate dimuon invariant mass from 4 momenta
  
  invMass_dimuon = (((m_good_muons)[0]->p4() + (m_good_muons)[1]->p4()).M())/CLHEP::GeV;
  
  //  if (invMass_dimuon>20){ //Cut to remove invariant mass calculated from duplicate muons (shouldn't need anymore)
  
  m_dimuon_invmass.push_back(invMass_dimuon);
  //}
  //else{
  //  ATH_MSG_INFO("testphrase DUPLICATES still passing!");
  //}
  
  ATH_MSG_INFO("testphrase total number of combined muon tracks found = " << mu_count);
  ATH_MSG_INFO("testphrase final size of good muon container = " << m_good_muons.size());
  ATH_MSG_INFO("testphrase Dimuon Invariant Mass from 4mom  = " << invMass_dimuon);
  
  //Access LVL1 trigger ROI information-----------------------------------------------------------------------
  //Have to access directly from Storegate as AllTE combiner algorithms remove regular access back to seeding RoIs
  
  if(evtStore()->retrieve(m_l1_muon_RoIs).isFailure()){
    ATH_MSG_ERROR("testphrase Problem retrieving L1 muon ROI");
    return HLT::MISSING_FEATURE;
  }
  
  if(m_l1_muon_RoIs->empty()){ //Save compute by killing if there is no L1 information (MOVE ABOVE)
    ATH_MSG_ERROR("testphrase L1 RoI size = 0, moving to next event");
    return HLT::OK; 
  }
  
  ATH_MSG_INFO("testphrase Number of L1 RoIs found = "<<m_l1_muon_RoIs->size());    
  
  
  for (auto RoI : *m_l1_muon_RoIs){ //Have to read storegate data into a pointer to DataVector of pointers
    for(auto goodmuon : m_good_muons) {
      deltaR=sqrt(pow(RoI->phi()-goodmuon->phi(),2) + pow(RoI->eta()-goodmuon->eta(),2));
      m_delta_r.push_back(deltaR);
      ATH_MSG_INFO("testphrase Delta R value = " << deltaR);
    }
  }
  
  
  return hltStatus;
  
}


//Function to resolve cases of >2 muons within muon container
void TrigMuonEFTagandProbe::trim_container (std::vector<const xAOD::Muon*>& good_muons){

  //Is this dumb and over engineered?? 

  //Splits good_muon vector into two vectors by charge of muons
  //Loop through both vectors comparing Invariant Mass of element pairs
  //if abs(91.1876-invariant mass) is below a given threshold (20 GeV) then write this as a key into a map with the second loop muon as the element
  //at last element of second loop push back first map element and first loop muon pair into good_muons vector then remove muon from second loop to avoid producing two pairs with a shared muon (Still doesn't account for sharing first loop muon but multiple z-mumu per event is so unlikely this is null

  //QQ should I define these globally (in h file) and just clear at every call of function( is there a difference, how exactly are the variables working here?)
  std::vector<const xAOD::Muon*> posvec;
  std::vector<const xAOD::Muon*> negvec;

  std::map<const double,std::pair <const xAOD::Muon*,unsigned int> > comparisonmap;

  for(auto muon : good_muons) {
    if (muon->charge()>0){
      posvec.push_back(muon);
    }    
    else{
      negvec.push_back(muon);
    }
  }
  
  good_muons.clear();

  if (posvec.empty() || negvec.empty()){
    ATH_MSG_INFO("testphrase (trim_container) only found muons of one sign, clearing container and moving on"); 
    return;
  }

  for(auto muon1 : posvec) {
    comparisonmap.clear();
    for (unsigned int j=0;j<negvec.size();j++){
      double tempinvmass = abs(91.1876-((muon1->p4() + negvec[j]->p4()).M())/CLHEP::GeV);
      if (tempinvmass < 20){
	comparisonmap[tempinvmass] = std::make_pair(negvec[j],j);
      }
      if (j == (negvec.size()-1)){
	if (comparisonmap.size()>0){ //if condition stops cbegin() being called on an empty map returning NAN as a value
	  good_muons.push_back(((*(comparisonmap.cbegin())).second).first); //Push back muon 2 (cbegin() always gives pointer)
	  good_muons.push_back(muon1); //Push back muon 1
	  ATH_MSG_DEBUG("testphrase (trim_container) best match found, current negvec size = "<<negvec.size()); 
	  negvec.erase(negvec.begin()+((*(comparisonmap.cbegin())).second).second);
	  ATH_MSG_DEBUG("testphrase (trim_container) removing best match, new negvec size = "<<negvec.size()); 
	}	
      }
    }
  }
}
