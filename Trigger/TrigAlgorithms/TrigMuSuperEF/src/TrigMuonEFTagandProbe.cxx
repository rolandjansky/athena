
/*
  Skeleton for in progress Event Filter Z_mu+mu- Online Tag and Probe algorithm
  Possible names: TrigMuonEFTagandProbe
  Authors: JJamieson, University of Glasgow, 09/10/2017, Edit: 07/11/2017





  Accesses collection of xaod::muons within event and picks only good muons, i.e. muons formed from combined (NOT SA) tracks including possible duplicates

  Preceding Mu22_mu8noL1 trigger can pass more than 2 muon events provided that there are at least one with Pt>22GeV and another with Pt>8GeV, Thus data is cleaned by first applying a 10 GeV cut on pt to remove events with pt below that expected in z_mu+mu- events (10 GeV picked to not be too high as to bias against low momentum Z events and to stay consistent with previous work

  >2 muon events are then trimmed by pairing up all the avaliable opposite charge muons and picking the best fit to Z invariant mass pair, applying a +-10 GeV threshold cut around the Z mass to match the eventual pair requirement. 

 LVL 1 trigger information is then accessed for the event and Delta R values for each RoI and muon pair are collected 

 For the Tag and probe method a tag muon is defined as any good muon with pt > 10GeV that can be associated with a LVL 1 RoI (DeltaR <0.1) and a candidate probe muon is any other muon with pt > 10GeV with the additional requirement on tag and probe pairs that their invariant mass be within +-10GeV from the Z mass 

 Trigger efficiency plots for kinemetic variables (eta,phi and pt) are produced by dividing histograms of kinematic to number passing probes by kinemetic to number of valid tags   


 Should we impose requirement for origin at same vertex/isolation etc??

  Is there any reason to try and collect all good muon pairs first in one vector or can we do all calculations live then scrap each pair as they go? 
  Otherwise Would need to declare good muons as a monitored container, remove the clear step from execute and work out how to send the container on as an output of the algorithm




  TODO:
  Check Python Monitoring and config files for uneeded imports as well


  athenaHLT:

  Run on multiple files using: athenaHLT.py -M -f "['file1','file2',...]" -o HLT_MCV7  -c 'testMCV7=True;HLTOutputLevel=INFO;ServiceMgr.MessageSvc.defaultLimit=9999999;doValidation=True;fpeAuditor=True' -C "topSequence.TrigSteer_HLT.TrigMuonEFTagandProbe.OutputLevel=DEBUG" TriggerRelease/runHLT_standalone.py >& log 

  Output of online histogramming will be in r000...HLT-Histogramming.root

  athena:

  multiple files in different directories: athena.py -c 'testMCV7=True;HLTOutputLevel=INFO;ServiceMgr.MessageSvc.defaultLimit=9999999;doValidation=True;fpeAuditor=True;BSRDOInput=[“/path/to/file1.root”,”/path/to/file2.root”]'  TriggerRelease/runHLT_standalone.py ../setupDebugForMuons.py >& log_file

  multiple files in 1 directory: athena.py -c 'testMCV7=True;HLTOutputLevel=INFO;ServiceMgr.MessageSvc.defaultLimit=9999999;doValidation=True;fpeAuditor=True;from glob import glob;BSRDOInput=glob("/path/to/dir/“)'  TriggerRelease/runHLT_standalone.py setupDebugForMuons.py >& log_file

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
  declareMonitoredStdContainer("EF_FS_DeltaZ",              m_delta_z,                   IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("LVL1_CB_DeltaR",            m_delta_r,                   IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Tag",                m_eta_tag,                   IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Eta_Probe",              m_eta_probe,                 IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Tag",                m_phi_tag,                   IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Phi_Probe",              m_phi_probe,                 IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Tag",                 m_pt_tag,                    IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_Pt_Probe",               m_pt_probe,                  IMonitoredAlgo::AutoClear);


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

  //Idea is nice but this is not really being used
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


  //Access LVL1 trigger ROI information-----------------------------------------------------------------------
  
  //Have to access directly from Storegate as AllTE combiner algorithms remove regular access back to seeding RoIs
  //Check for LVL1 muon RoI information in current event first, if there is no info then we shouldn't bother calculating anything else and should move to next event 

  if(evtStore()->retrieve(m_l1_muon_RoIs).isFailure()){
    ATH_MSG_ERROR("testphrase Problem retrieving L1 muon ROI");
    return HLT::MISSING_FEATURE;
  }
  
  if(m_l1_muon_RoIs->empty()){ //Save compute by killing if there is no L1 information (MOVE ABOVE)
    ATH_MSG_ERROR("testphrase L1 RoI size = 0, moving to next event");
    return HLT::OK; 
  }
  
  ATH_MSG_INFO("testphrase Number of L1 RoIs found = "<<m_l1_muon_RoIs->size());    



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
  unsigned int hit=0; // Tracks total number of LVL2 muons with matching level 1 triggers
  unsigned int total=0; // Total number of LVL2 muons
  float invMass_dimuon=0.0; 
  float deltaR=0.0; 
  float deltaZ=0.0; 

  
  // loop on the muons within the combined RoIs----------------------------------------------------------

  // CHECK Might want to remove call to trackparticle because we can use muon->muonType()==0 to check for combined muon
  // Would not be able to output chi^2 value associated to muon tracking in this case though

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
      trim_container(m_good_muons); //Performs trimming of muon container reducing to size = 2 (or a multiple of 2)
      
      if (m_good_muons.size()<=1){
	ATH_MSG_WARNING("testphrase Less than 2 good muons after trimming so further event analysis will be ignored, moving to next event");
	return HLT::OK;
      }
      
      ATH_MSG_DEBUG("testphrase container trimmed, no. of good muons = " << m_good_muons.size());
    }
  
  
  //Calculate dimuon invariant mass from 4 momenta
  
  invMass_dimuon = (((m_good_muons)[0]->p4() + (m_good_muons)[1]->p4()).M())/CLHEP::GeV;
  
    
  m_dimuon_invmass.push_back(invMass_dimuon);
  
  const xAOD::TrackParticle* track1 = m_good_muons[0]->trackParticle(xAOD::Muon::CombinedTrackParticle); 
  const xAOD::TrackParticle* track2 = m_good_muons[1]->trackParticle(xAOD::Muon::CombinedTrackParticle); 
  deltaZ=abs(track1->z0()-track2->z0());
  m_delta_z.push_back(deltaZ);

  
  ATH_MSG_INFO("testphrase total number of combined muon tracks found = " << mu_count);
  ATH_MSG_INFO("testphrase final size of good muon container = " << m_good_muons.size());
  ATH_MSG_INFO("testphrase Dimuon Invariant Mass from 4mom  = " << invMass_dimuon);
  ATH_MSG_INFO("testphrase Dimuon delta Z = " << deltaZ);
  


  //Calculate LVL1 ROI DeltaR information-----------------------------------------------------------------------
  
  /* 
  for (auto RoI : *m_l1_muon_RoIs){ //Have to read storegate data into a pointer to DataVector of pointers
    for(auto goodmuon : m_good_muons) {
      deltaR=sqrt(pow(RoI->phi()-goodmuon->phi(),2) + pow(RoI->eta()-goodmuon->eta(),2));
      m_delta_r.push_back(deltaR);
      ATH_MSG_INFO("testphrase Delta R value = " << deltaR);
      
      if(abs(91.1876-invMass_dimuon)<10.0 && deltaR<0.1){

      }

    }
  }
  */

  //Define a vector of muon pairs 
  std::vector<std::pair <const xAOD::Muon*,const xAOD::Muon*> > TaP; // QQ Better in .h or here??
  int sharedRoItest = -1; //test to make sure both muons in pair aren't mapped to same LvL1 RoI

  for (unsigned int i=0;i<m_good_muons.size();i++){ //loop over good muons
    bool uniqueLVL1 = false; //test to make sure each muon is only matched to one lvl 1 RoI
    for (int j=0;j<m_l1_muon_RoIs->size();j++){ //loop over LVL1 RoIs

      
      //For each muon calculate all Delta R values in scope
      deltaR=sqrt(pow((*m_l1_muon_RoIs)[j]->phi()-m_good_muons[i]->phi(),2) + pow((*m_l1_muon_RoIs)[j]->eta()-m_good_muons[i]->eta(),2));
      m_delta_r.push_back(deltaR);
      ATH_MSG_INFO("testphrase Delta R value = " << deltaR); 
      
      if(abs(91.1876-invMass_dimuon)<10.0 && deltaR<0.1 && !uniqueLVL1 && j!=sharedRoItest){
	//Fill vector with muon pair if one muon passes LVL1 Delta R criteria (Pt criteria is automatic for other muon)
	TaP.push_back(std::make_pair(m_good_muons[i],m_good_muons[1-i])); //Assumes m_good_muons is size 2
	uniqueLVL1=true;
	sharedRoItest=j;
	ATH_MSG_DEBUG("testphrase Tag found with j value = " << j); 
      }
    }
  }

  for(unsigned int n=0;n<TaP.size();n++){ //Loop over each probe candidate
    for(unsigned int m=0;m<TaP.size();m++){ //Loop over each tag candidate 
      // Loop over all agreed tags for each probe candidate and if probe matches a tag then count as hit
      if ((*TaP[n].second).pt()==((*TaP[m].first).pt())){ //Also assumes m_good_muons is size 2 else we could double count
	m_eta_probe.push_back((*TaP[n].second).eta());
	m_phi_probe.push_back((*TaP[n].second).phi());
	m_pt_probe.push_back((*TaP[n].second).pt()/CLHEP::GeV);
	ATH_MSG_DEBUG("testphrase Probe found with LVL1; eta = " << (*TaP[n].second).eta() << " phi = " << (*TaP[n].second).phi() << " pt = " << (*TaP[n].second).pt()/CLHEP::GeV); 
	hit++;
      }
    }
    m_eta_tag.push_back((*TaP[n].second).eta());
    m_phi_tag.push_back((*TaP[n].second).phi());
    m_pt_tag.push_back((*TaP[n].second).pt()/CLHEP::GeV);
    total++;
  }
  ATH_MSG_INFO("testphrase TOTAL = " << total << " HITS = " << hit); 

  return hltStatus;
  
}


//Function to resolve cases of >2 muons within muon container
void TrigMuonEFTagandProbe::trim_container (std::vector<const xAOD::Muon*>& good_muons){

  //Is this dumb and over engineered?? 

  //Splits good_muon vector into two vectors by charge of muons
  //Loop through both vectors comparing Invariant Mass of element pairs
  //if abs(91.1876-invariant mass) is below a given threshold (10 GeV) then write this as a key into a map with the second loop muon as the element
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
      if (tempinvmass < 10){ // Cut to match condition for valid tag and probe pair
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
