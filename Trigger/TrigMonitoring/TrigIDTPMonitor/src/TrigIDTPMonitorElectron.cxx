/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigMonitoring/TrigIDTPMonitor/TrigIDTPMonitorElectron.cxx
 **
 **   Description: Monitoring algorithm for ID T&P Z->ee candidates
 **
 **
 **   Author: Jonathon Langford <jonathon.langford@cern.ch>
 **           ATLAS Summer Student - private email: jonno_1995@hotmail.co.uk
 **                                  (for queries after my CERN account has been closed)
 **
 **   Created:   05.07.2016
 **   Modified:  25.08.2016
 **
 **
 ***************************************************************************/

#include "TrigIDTPMonitorElectron.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

#include "xAODEventInfo/EventInfo.h"

#include "AthLinks/ElementLink.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

TrigIDTPMonitorElectron::TrigIDTPMonitorElectron(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::FexAlgo(name, pSvcLocator) {

  //histogram containers
  declareMonitoredStdContainer("PTInvMass",  m_PTinv,      AutoClear);
  declareMonitoredStdContainer("FTFInvMass",  m_FTFinv,      AutoClear);
  declareMonitoredStdContainer("PTOptInvMass",   m_PTOptinv,   AutoClear);
  declareMonitoredStdContainer("FTFOptInvMass",   m_FTFOptinv,   AutoClear);
  declareMonitoredStdContainer("PTInvMassTrack",  m_PTinvTrack,      AutoClear);
  declareMonitoredStdContainer("FTFInvMassTrack",  m_FTFinvTrack,      AutoClear);
  declareMonitoredStdContainer("InvMassCluster",  m_invCluster,      AutoClear);
  declareMonitoredStdContainer("InvMassCluster_before_cuts", m_invCluster_before_cuts,    AutoClear);
  declareMonitoredStdContainer("InvMassClusterTagTrack",  m_invClusterTagTrack,      AutoClear);
  declareMonitoredStdContainer("InvMassClusterTagTrack_before_cuts", m_invClusterTagTrack_before_cuts,    AutoClear);
  declareMonitoredStdContainer("clusterPT",  m_clusterPT,      AutoClear);
  declareMonitoredStdContainer("clusterPhi",  m_clusterPhi,      AutoClear);
  declareMonitoredStdContainer("clusterEta",  m_clusterEta,      AutoClear);
  declareMonitoredStdContainer("clusterPT_before_cuts",  m_clusterPT_before_cuts,      AutoClear);
  declareMonitoredStdContainer("clusterPhi_before_cuts",  m_clusterPhi_before_cuts,      AutoClear);
  declareMonitoredStdContainer("clusterEta_before_cuts",  m_clusterEta_before_cuts,      AutoClear);
  declareMonitoredStdContainer("deltaPhi",  m_deltaPhi,      AutoClear);
  declareMonitoredStdContainer("deltaPhi_before_cuts",  m_deltaPhi_before_cuts,      AutoClear);
  declareMonitoredStdContainer("PTdeltaR",   m_PTdeltaR,    AutoClear);
  declareMonitoredStdContainer("PTOptdeltaR",   m_PTOptDeltaR,    AutoClear);
  declareMonitoredStdContainer("FTFdeltaR",   m_PTdeltaR,    AutoClear);
  declareMonitoredStdContainer("FTFOptdeltaR",  m_FTFOptDeltaR,   AutoClear);
  declareMonitoredVariable("PTClusterDeltaEta",   m_PTdeltaEta,   -999);
  declareMonitoredVariable("FTFClusterDeltaEta",  m_FTFdeltaEta,  -999);
  declareMonitoredVariable("TagDeltaEta",  m_tagDeltaEta,  -999);
  declareMonitoredVariable("PTClusterDeltaPhi",   m_PTdeltaPhi,   -999);
  declareMonitoredVariable("PTClusterDeltaPhiPositron",   m_PTdeltaPhiPositron,   -999);
  declareMonitoredVariable("FTFClusterDeltaPhi",  m_FTFdeltaPhi,  -999);
  declareMonitoredVariable("TagDeltaPhi",  m_tagDeltaPhi,  -999);
  declareMonitoredVariable("TagExpectedDeltaPhi",   m_tagExpectedDeltaPhi,   -999);
  declareMonitoredVariable("TagEta", m_tagEta,   -999);
  declareMonitoredVariable("tagpT",   m_tagpT,   -1);
  declareMonitoredVariable("tagZ0",   m_tagZ0,    10000);
  declareMonitoredVariable("tagTrackpT", m_tagTrackpT,   -1);
  declareMonitoredVariable("tagPoverE",   m_tagPoverE,   -1);
  declareMonitoredStdContainer("tagClusterEta_before_cuts",   m_tagClusterEta_before_cuts,   AutoClear);
  declareMonitoredStdContainer("tagClusterPhi_before_cuts",   m_tagClusterPhi_before_cuts,   AutoClear);
  declareMonitoredStdContainer("tagClusterEta",   m_tagClusterEta,   AutoClear);
  declareMonitoredStdContainer("tagClusterPhi",   m_tagClusterPhi,   AutoClear);
  declareMonitoredVariable("PTnumberOfPixelHits",   m_PTnumberOfPixelHits,    -10);
  declareMonitoredVariable("PTnumberOfSCTHits",   m_PTnumberOfSCTHits,    -10);
  declareMonitoredVariable("PTnumberOfTRTHits",   m_PTnumberOfTRTHits,    -10);
  declareMonitoredStdContainer("PTEfficientCombDeltaZ0",    m_dZ0,    AutoClear);

  //For profile histograms
  declareMonitoredVariable("probePtEfficiency",   m_ptEfficiencyProfile,   -1);
  declareMonitoredVariable("probePhiEfficiency",   m_phiEfficiencyProfile,   -999);
  declareMonitoredVariable("probeEtaEfficiency",   m_etaEfficiencyProfile,   -999);
  declareMonitoredVariable("probeInvMassEfficiency",  m_invMassEfficiencyProfile,   -1);  
  declareMonitoredVariable("PTfound",    m_PTfound,      -1);
  declareMonitoredVariable("FTFfound",    m_FTFfound,     -1);
  declareMonitoredVariable("PTfoundAlt", m_PTfoundAlt,     -1);
  declareMonitoredVariable("PTfoundCombined", m_PTfoundCombined,    -1);
  //for IBL Monitoring
  declareMonitoredVariable("PTeta", m_PTeta,   -999);
  declareMonitoredVariable("PTphi", m_PTphi,   -999);
  declareMonitoredVariable("PTpixelFound", m_PTpixelFound, -1);
  declareMonitoredVariable("PTpixelNextToFound", m_PTpixelNextToFound, -1);
}

TrigIDTPMonitorElectron::~TrigIDTPMonitorElectron(){ }

HLT::ErrorCode TrigIDTPMonitorElectron::hltInitialize(){
  ATH_MSG_DEBUG( "Running TrigIDTPMonitorElectron::hltInitialize" );
  return HLT::OK;
}

HLT::ErrorCode TrigIDTPMonitorElectron::hltFinalize(){
  ATH_MSG_DEBUG( "Running TrigIDTPMonitorElectron::hltFinalize" );
  return HLT::OK;
}

HLT::ErrorCode TrigIDTPMonitorElectron::hltBeginRun() {
  ATH_MSG_INFO( "beginning run in this " << name() );
  return HLT::OK;
}

HLT::ErrorCode TrigIDTPMonitorElectron::acceptInput(const HLT::TriggerElement* /*inputTE*/, bool& pass){
  ATH_MSG_DEBUG( "Running TrigIDTPMonitorElectron::acceptInputs" );
  pass = true;
  return HLT::OK;
}

HLT::ErrorCode TrigIDTPMonitorElectron::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE){

  ATH_MSG_DEBUG( "Running TrigIDTP::hltExecute - WE_ARE_GOOD!" );

  ATH_MSG_DEBUG( "Input TE: " << *inputTE );
  ATH_MSG_DEBUG( "Output TE: " << *outputTE );


  //Define constants
  const double Zmass=91187.6;//MeV

  //Accessing Trigger Elements
  //Store in vector tes
  std::vector<HLT::TriggerElement*> tes = config()->getNavigation()->getDirectPredecessors(inputTE);

  ATH_MSG_DEBUG( "Retrieved Trigger Elements = " << tes );

  //We require two trigger elements, therefore if tes has a size not equal to 2 then return OK!
  if(tes.size() != 2){
    ATH_MSG_DEBUG( "There are " << tes.size() << " Trigger Elements" );
    ATH_MSG_INFO( "Exiting IDTPMonitorElectron" );
    return HLT::OK;
  }

  //get Labels for T&P identification
  std::string teInLabel1;
  std::string teInLabel2;

  //Get label from front element of vector, will later see if this is tag or probe
  TrigConf::HLTTriggerElement::getLabel( tes.front()->getId(), teInLabel1 );
  ATH_MSG_DEBUG( "Name1: " << teInLabel1 );

  //Get label from back element of vector tes, will later see if this is tag or probe
  TrigConf::HLTTriggerElement::getLabel( tes.back()->getId(), teInLabel2 );
  ATH_MSG_DEBUG( "Name2: " << teInLabel2 );

  //declare region of interest descriptors -> Error code if Navigation problem/Region of interest empty
  const TrigRoiDescriptor *roi1(0);
  const TrigRoiDescriptor *roi2(0);
  //If there has been a problem in getting ID labels...
  if( getFeature(tes.front(), roi1, "") != HLT::OK){      //check name of the forID dscriptor!
    ATH_MSG_WARNING( "Navigation error whilst getting RoI descriptor 1" );
    return HLT::OK;
  }
  if( roi1==NULL ){
    ATH_MSG_DEBUG( "Navigation Error: RoI descriptor 1 retrieved null pointer" );
    return HLT::OK;
  }
  if( getFeature(tes.back(), roi2, "") != HLT::OK){  //check name of the forID dscriptor!
    ATH_MSG_WARNING( "Navigation error whilst getting RoI descriptor 2" );
    return HLT::OK;
  }
  if( roi2==NULL ){
    ATH_MSG_DEBUG( "Navigation Error: RoI descriptor 2 retrieved null pointer" );
    return HLT::OK;
  }

  //Retrieve tracking particle containers
  //For Tag
  const xAOD::ElectronContainer* ec1;
  const xAOD::CaloClusterContainer* tagClusterCont; 
  //For probe... (Precision tracking and FTF)
  const xAOD::TrackParticleContainer* PTTrkContainerElectron(NULL);
  const xAOD::TrackParticleContainer* FTFTrkContainerElectron(NULL);
  //Cluster Container
  const xAOD::CaloClusterContainer* clusterCont;


  //Define bool objects which will indicate which electron is tag and which is probe
  bool tag1probe2=false;
  bool tag2probe1=false;

  //Vectors of combined tracks
  //Define tag tracks
  std::vector<const xAOD::TrackParticle*> tagTracks;
  //Define 2 probe tracks one for PT and one for FTF 
  std::vector<const xAOD::TrackParticle*> PTprobeTracks;
  std::vector<const xAOD::TrackParticle*> FTFprobeTracks;

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //TRIGGER IS HARD CODED!!!!!! - Needs to be changed to allow monitoring on other triggers
  std::string tagname = "EF_e26_lhtight_nod0_L1_EM22VHI";
  std::string probename = "EF_e15_etcut_L1EM7_L1_EM7";
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  //Now must see which is tag and which is probe, therefore compare the ID label to tagname and probename, and change bool operator accordingly
  if(teInLabel1.compare(tagname) == 0 && teInLabel2.compare(probename) == 0){
    ATH_MSG_DEBUG( "The first electron is the tag, the second electron is the probe" );
    tag1probe2 = true;
  }
  else if(teInLabel1.compare(probename) == 0 && teInLabel2.compare(tagname) == 0){
    ATH_MSG_DEBUG( "The second electron is the tag, the first electron is the probe" );
    tag2probe1 = true;
  }

  //Get features from containers, depending on which is tag and which is probe will need front or back trigger element
  //Enter for first electron tag and second probe
  if(tag1probe2){
    //TAG1:Electron Container
    if(getFeature(tes.front(), ec1) != HLT::OK){
      ATH_MSG_WARNING( "Navigation error whilst getting Electron Container" );
      return HLT::OK;
    }
    //TAG1: Cluster container
    if(getFeature(tes.front(), tagClusterCont) != HLT::OK){
      ATH_MSG_WARNING( "Navigation error whilst getting Tag Cluster Container" );
      return HLT::OK;
    }
    //PROBE2:Cluster Container
    if(getFeature(tes.back(), clusterCont) != HLT::OK){
      ATH_MSG_WARNING( "Navigation error whilst getting Cluster Container" );
      return HLT::OK;
    }
    //PROBE2: PT Tracking Container
    if(getFeature(tes.back(), PTTrkContainerElectron, "InDetTrigTrackingxAODCnv_Electron_IDTrig") != HLT::OK){
      ATH_MSG_WARNING( "There was an error retrieving Probe's ID TrackParticleContainer PT" );
    } 
    //PROBE2: FTF Tracking Container
    if(getFeature(tes.back(), FTFTrkContainerElectron, "InDetTrigTrackingxAODCnv_Electron_FTF") != HLT::OK){
      ATH_MSG_WARNING( "There was an error retrieving Probe's ID TrackParticleContainer FTF" );
    }
  }
  //Enter for first probe and second electron tag
  else if(tag2probe1){
    //TAG2: Electron Container
    if(getFeature(tes.back(), ec1) != HLT::OK){
      ATH_MSG_WARNING( "Navigation error whilst getting Electron Container 1" );
      return HLT::OK;
    }
    //TAG2: Cluster Container
    if(getFeature(tes.back(), tagClusterCont) != HLT::OK){
      ATH_MSG_WARNING( "Navigation error whilst getting Tag Cluster Container" );
      return HLT::OK;
    }
    //PROBE1:Cluster Container
    if(getFeature(tes.front(), clusterCont) != HLT::OK){
      ATH_MSG_WARNING( "Navigation error whilst getting Cluster Container" );
      return HLT::OK;
    }
    //PROBE1: PT Tracking Container
    if(getFeature(tes.front(), PTTrkContainerElectron, "InDetTrigTrackingxAODCnv_Electron_IDTrig") != HLT::OK){
      ATH_MSG_WARNING( "There was an error retrieving Probe's ID TrackParticleContainer PT" );
    }
    //PROBE1: FTF Tracking Container                                                                                                                                                                                                                          
    if(getFeature(tes.front(), FTFTrkContainerElectron, "InDetTrigTrackingxAODCnv_Electron_FTF") != HLT::OK){
      ATH_MSG_WARNING( "There was an error retrieving Probe's ID TrackParticleContainer FTF" );
    }
  }
  //ELSE->Situation of no tag/probe pair then LEAVE
  else{
    ATH_MSG_DEBUG( "No tag/probe pair. Leaving!" );
    return HLT::OK;
  }

  //Check that the electron container has just one electron per roi
  if( ec1->size() != 1 ){
    if( ec1->size() == 0 )
      ATH_MSG_DEBUG( "No electron in RoI" );
    else
      ATH_MSG_DEBUG( "More than one electron RoI" );
    return HLT::OK;
  }
  ATH_MSG_DEBUG( "Retrieved electron container: " << ec1 );
  //Check cluster container is not empty
  if( clusterCont->size() == 0){
    ATH_MSG_DEBUG("No cluster in RoI");
    return HLT::OK;
  }



  //TAG - extract information from fully reconstructed electron. 
  
  //Declare variables to store topological info of the tag electron
  double tagpT=0, tagPhi=0, tagEta=0, tagPx=0, tagPy=0, tagPz=0, tagd0=0, tagCharge=100, tagZ0=-999;
  //Declare variables to store topological info of the tag track and tag cluster only
  double tagTrackEta=0, tagTrackPhi=0, tagTrackpT=0, tagTrackPz=0, tagClusterEta=-999, tagClusterPhi=-999;

  //Loop over tag electrons (should be just one fully reconstructed electron in RoI)
  for (auto electronIterator: *ec1){
    if (electronIterator == NULL){
      ATH_MSG_WARNING("Non initialised electron in first ROI... Leaving!");
      return HLT::OK;
    }
    //Extract tag electron topological info
    tagpT=electronIterator->pt();
    tagPhi=electronIterator->phi();
    tagEta=electronIterator->eta();
    tagPx=electronIterator->p4().Px();
    tagPy=electronIterator->p4().Py();
    tagPz=electronIterator->p4().Pz();

    //Looking at tag tracks 
    //Note this is not required by algorithm. Check for effects of bremstrahlung (i.e. the difference between the tag electron and the tag track)
    //Accessing tag track
    const xAOD::TrackParticle* electronTrack= electronIterator->trackParticle();
    //Check if electronTrack is NULL pointer
    if (electronTrack == NULL){
      ATH_MSG_DEBUG("Tag electron returns null TrackParticleLink");
      return HLT::OK;
    }
    //Check track corresponds to a particle with charge
    if(abs(electronTrack->charge())>0){
      //if so add to vector of tag tracks
      tagTracks.push_back(electronTrack);
      ATH_MSG_DEBUG("Tag track added to vector!");
    }
    //extract tag track topological info
    tagd0=electronTrack->d0();
    tagZ0=electronTrack->z0();
    tagCharge=electronTrack->charge();
    tagTrackpT=electronTrack->pt();
    tagTrackPz=electronTrack->p4().Pz();
    tagTrackEta=electronTrack->eta();
    tagTrackPhi=electronTrack->phi();
  }

  //Calculate E(tagElectron)/P(tagTrack) and store in monitored variable
  m_tagPoverE=(sqrt(tagTrackpT*tagTrackpT+tagTrackPz*tagTrackPz))/(sqrt(tagpT*tagpT+tagPz*tagPz+0.261121));

  //Looking at tag cluster
  //Note this is also not required by algorithm. Again used to check for effects of brem.
  for(auto clusterIterator: *tagClusterCont){
    if(clusterIterator == NULL){
      ATH_MSG_WARNING( "Not initialised in relevant roi, leaving!" );
      return HLT::OK;
    }
    //extract tag cluster topological info
    tagClusterEta=clusterIterator->eta();
    tagClusterPhi=clusterIterator->phi();
    //push back to store in monitored variables
    m_tagClusterEta_before_cuts.push_back(clusterIterator->eta());
    m_tagClusterPhi_before_cuts.push_back(clusterIterator->phi());
  }

  //IF fully reconstructed electron is triggered then the following checks should never be entered (as electron requires both cluster and track)
  //Therefore if either track or cluster not there then leave as must be a problem
  //Check if tagTracks is empty
  if(tagTracks.size() == 0){
    ATH_MSG_DEBUG( "Empty Tag vector, leaving" );
    return HLT::OK;
  }
  //Check if tagCluster is empty
  if(tagClusterEta==-999){
    ATH_MSG_DEBUG("Empty tag cluster, leaving");
   return HLT::OK;
  }

  //Checking the effect of Bremstrahlung by alignment of tagTrack and tagCluster
  //Determine deltaEta and deltaPhi
  m_tagDeltaEta=tagClusterEta-tagTrackEta;
  m_tagDeltaPhi=tagClusterPhi-tagTrackPhi;
  //Calculate the expected deltaPhi due to bending from magnets alone. Derivation in logbook
  m_tagExpectedDeltaPhi=-acos(((tagpT/600)-(588/tagpT))/(tagpT/600));
  //multiply by -1 for positrons
  if(tagCharge==1){
    m_tagExpectedDeltaPhi=-1*m_tagExpectedDeltaPhi;
  }

  //IMPLEMENT SELECTION CUTS ON TAGS
  //exclude electrons in crack region of calorimeter between barrel and end-cap (1.37-1.52)
  if((tagEta>1.37&&tagEta<1.52)||(tagEta>-1.52&&tagEta<-1.37)){
    //Leave
    ATH_MSG_DEBUG("TAG:::Eta outside of desired range: "<<tagEta);
    return HLT::OK;
  }

  //FILLING OF MONITORED VARIABLES FOR HISTOGRAMMING
  //declare monitored variables for histogram filling
  m_tagEta=tagEta;
  m_tagpT=tagpT;
  m_tagTrackpT=tagTrackpT;



  //PROBE CLUSTER - extract information from probe cluster. Make selection cuts to exclude candidates which are not Z->ee
  //CHECK - may get more than one cluster for same region of interest. Therefor loop over clusters in container and then choose optimal cluster for tag-electron probe-cluster Minv closest to Zmass

  //declare variables to store topological info of all clusters (temporary). These will be stored in monitored variables with names: before_cuts
  double tempPT=0, tempPHI=0, tempETA=0, tempPx=0, tempPy=0, tempPz=0, tempInvMass=0, tempClusterTagTrackInvMass=0;
  //declare variables to store the topological info for optimal cluster
  double CLUSTEROptInvMass=0, CLUSTEROptPT=0, CLUSTEROptPhi=0, CLUSTEROptEta=0, CLUSTEROptTagTrackInvMass=0;
  //declare variables to store the difference in phi between the tag and probe -> Expect to be ~pi since electrons produced back to back
  double tempDeltaPhi=0, CLUSTEROptDeltaPhi=0;
  
  //loop over clusters in container
  for(auto clusterIterator: *clusterCont){
    //If not initialised in ROI then leave
    if(clusterIterator == NULL){
      ATH_MSG_WARNING( "Not initialised in relevant roi, leaving!" );
      return HLT::OK;
    }
    //Extract cluster topological info
    tempPT=clusterIterator->pt();
    tempPHI=clusterIterator->phi();
    tempETA=clusterIterator->eta();
    tempPx=clusterIterator->p4().Px();
    tempPy=clusterIterator->p4().Py();
    tempPz=clusterIterator->p4().Pz();
    //Calculate invariant mass of tag-electron probe-cluster pair
    tempInvMass=ClusterInvMass(tagpT,tagPx,tagPy,tagPz,tempPT,tempPx,tempPy,tempPz);

    //pushback pT, phi, eta and Minv onto vectors: before_cuts
    m_clusterPT_before_cuts.push_back(tempPT);
    m_clusterPhi_before_cuts.push_back(tempPHI);
    m_clusterEta_before_cuts.push_back(tempETA);
    m_invCluster_before_cuts.push_back(tempInvMass);

    //Calculate invariant mass of tag-TRACK probe-cluster pair
    //NOTE: this is not required by algorithm, check for effects of bremstrahlung. Remove when algorithm implemented online
    for(auto tagTrack:tagTracks){//only one track for fully reconstructed tag
      tempClusterTagTrackInvMass=ClusterTagTrackInvMass(tagTrack,tempPT,tempPx,tempPy,tempPz);;
      //pushback onto vector
      m_invClusterTagTrack_before_cuts.push_back(tempClusterTagTrackInvMass);
    }

    //Calculate delta phi for cluster and tag track
    tempDeltaPhi=fabs(tempPHI-tagPhi);
    //push back onto vector before cuts
    m_deltaPhi_before_cuts.push_back(tempDeltaPhi);

    //Find optimal cluster in container by selecting cluster with Minv closest to Zmass
    if(fabs(Zmass-tempInvMass)<fabs(Zmass-CLUSTEROptInvMass)){
      //set optimal cluster topological info
      CLUSTEROptInvMass=tempInvMass;
      CLUSTEROptTagTrackInvMass=tempClusterTagTrackInvMass;
      CLUSTEROptPT=tempPT;
      CLUSTEROptPhi=tempPHI;
      CLUSTEROptEta=tempETA;
      CLUSTEROptDeltaPhi=tempDeltaPhi;
    }
  }

  //IMPLEMENT SELECTION CUTS ON TAG-PROBE CANDIDATES - isolate Z->ee events
  //On Minv (of tag-electron probe-cluster)-> If CLUSTEROptInvMass not inside window then leave
  //NOTE value of 6.8GeV from running on MC samples (2sigma for Minv distribution)
  if(fabs(CLUSTEROptInvMass-Zmass)>6800){
    ATH_MSG_DEBUG("CUT:::Invariant mass outside of Zmass window");
    return HLT::OK;
  }
  //Remove events in crack region of calorimeter
  if((CLUSTEROptEta>1.37&&CLUSTEROptEta<1.52)||(CLUSTEROptEta<-1.37&&CLUSTEROptEta>-1.52)){
    ATH_MSG_DEBUG("CUT:::Cluster in crack region of calorimeter");
    return HLT::OK;
  }
  //on deltaPhi
  if(CLUSTEROptDeltaPhi<2.5||CLUSTEROptDeltaPhi>3.7){
    ATH_MSG_DEBUG("CUT:::Tag-probe delta phi outside accepted range of 2.5-3.7");
    return HLT::OK;
  } 

  //FILLING OF MONITORED VARIABLES FOR HISTOGRAMS
  //Push back the values for the optimum cluster onto monitored containers                                                                                                                                                                            
  m_deltaPhi.push_back(CLUSTEROptDeltaPhi);
  m_invCluster.push_back(CLUSTEROptInvMass);
  m_clusterPT.push_back(CLUSTEROptPT);
  m_clusterPhi.push_back(CLUSTEROptPhi);
  m_clusterEta.push_back(CLUSTEROptEta);
  m_invClusterTagTrack.push_back(CLUSTEROptTagTrackInvMass);
  //Push back values for tagCluster:eta and phi after cuts
  m_tagClusterEta.push_back(tagClusterEta);
  m_tagClusterPhi.push_back(tagClusterPhi);



  //ACCESS PROBE TRACKS FROM CONTAINER AND STORE IN VECTORS
  //PRECISION TRACKER
  for(auto probeIterator: *PTTrkContainerElectron){
    //If not initialised in ROI, leave
    if(probeIterator == NULL){
      ATH_MSG_WARNING( "Not initialised in relavant roi, leaving!" );
      return HLT::OK;
    }
    //define tempProbeTrack from iterator
    const xAOD::TrackParticle* tempProbeTrack= probeIterator;//->trackParticle();    

    //Check track corresponds to a particle with charge
    if(abs(tempProbeTrack->charge())>0){
      //Add probe track to vector of combined tracks
      PTprobeTracks.push_back(tempProbeTrack);
      ATH_MSG_DEBUG("Probe track added to PT vector!");
    }
  }
  //FAST TRACK FINDER                                                                                                                                                                                               
  for(auto probeIterator: *FTFTrkContainerElectron){
    //If not initialised in ROI, leave                                                                                                                                                                                                                      
    if(probeIterator == NULL){
      ATH_MSG_WARNING( "Not initialised in relavant roi, leaving!" );
      return HLT::OK;
    }
    //define tempProbeTrack from iterator                                                                                                                                                                                                                   
    const xAOD::TrackParticle* tempProbeTrackFTF= probeIterator;                                                                                                                                                                                            
    //Check track corresponds to a particle with charge                                                                                                                                                                                                     
    if(abs(tempProbeTrackFTF->charge())>0){
      //Add probe track to vector of combined tracks                                                                                                                                                                                                        
      FTFprobeTracks.push_back(tempProbeTrackFTF);
      ATH_MSG_DEBUG("Probe track added to FTF vector!");
    }
  }



  //INVARIANT MASS CALCULATION - from tag "electron" - probe track pair
  //This section of code determines inv mass and deltaR for all tracks in probe region (PT and FTF). Then implements two independent methods for finding the optimum track...
  //1)Probe track with invariant mass of tag electron-probe track pair closest to the Zmass
  //2)Probe track with the smallest value of deltaR (gives quantitative measure of the cluster-track alignment)
  
  //declare variables to temporarily store invariant mass and deltaR of all tracks
  double m_InvMass, m_deltaR=9999;
  //declare variables to store optimal track Minv for both PT and FTF. Also define ints to store iterator value of the optimal tracks (used for DEBUG messages at end of code)
  double PTOptInvMass=0, FTFOptInvMass=0; int PTOptIterator=-1, PTOptIteratorAlt=-1, FTFOptIterator=-1, FTFOptIteratorAlt=-1;
  //declare variables to store the difference in Z0 between tag and probe tracks for both methods
  double InvMasscriteriadZ0=-999, dRcriteriadZ0=-999;
  //declare variable to store the charge of the optimum track
  double OptProbeCharge=999;
  //declare  variables to store optimal track deltaR (PT and FTF)
  double PTOptDeltaR=999; double FTFOptDeltaR=999;
  //declare unsigned 8 bit integers to access trackSummary information of the optimum track found by the Invariant mass condition
  uint8_t numberOfPixelHits, numberOfSCTHits, numberOfTRTHits;
  //declare unsigned 8 bit integers to access trackSummary information for innermost detector monitoring (and next to innermost)
  uint8_t expectInnermostPixelLayerHit, numberOfInnermostPixelLayerHits, expectNextToInnermostPixelLayerHit, numberOfNextToInnermostPixelLayerHits;
  int p=0;

  //PRECISION TRACKER
  //iterate over probe tracks
  for(auto probeTrack:PTprobeTracks){
    //Check probeTrack and tag have opposite charge, if not then exit iteration (track)
    if(tagCharge*probeTrack->charge()!=-1){
      p++;
      continue;
    }
    //Calculate invariant mass and delta R
    m_InvMass=CombinedInvMass(tagpT,tagPx,tagPy,tagPz,probeTrack);
    m_deltaR=sqrt((CLUSTEROptEta-probeTrack->eta())*(CLUSTEROptEta-probeTrack->eta())+(CLUSTEROptPhi-probeTrack->phi())*(CLUSTEROptPhi-probeTrack->phi()));
    //push back invariant mass onto vector (this stores Minv of all tracks in probe region (not just the optimal ones))
    m_PTinv.push_back(m_InvMass);
    m_PTdeltaR.push_back(m_deltaR);
    
    //Finding the optimal track
    //1)Probe track with Minv closest to Zmass
    if(fabs(m_InvMass-Zmass)<fabs(PTOptInvMass-Zmass)){
      //Set new optimal track
      PTOptInvMass=m_InvMass;
      PTOptIterator=p;

      //Determine delta eta and delta phi between cluster and track and store in monitored variable to be histogrammed
      m_PTdeltaEta=CLUSTEROptEta-probeTrack->eta();
      m_PTdeltaPhi=CLUSTEROptPhi-probeTrack->phi();
      //Define charge of optimum track
      OptProbeCharge=probeTrack->charge();
      //For IBL monitoring: store eta and phi of opimum probe track, and define hit expectations and measurements
      m_PTeta=probeTrack->eta();
      m_PTphi=probeTrack->phi();
      probeTrack->summaryValue(expectInnermostPixelLayerHit,xAOD::expectInnermostPixelLayerHit);
      probeTrack->summaryValue(numberOfInnermostPixelLayerHits,xAOD::numberOfInnermostPixelLayerHits);
      probeTrack->summaryValue(expectNextToInnermostPixelLayerHit,xAOD::expectNextToInnermostPixelLayerHit);
      probeTrack->summaryValue(numberOfNextToInnermostPixelLayerHits,xAOD::numberOfNextToInnermostPixelLayerHits);
      //m_PTetaNextToInnermost=probeTrack->eta();
      //m_PTphiNextToInnermost=probeTrack->phi();

      //Determine hits information for optimum track
      probeTrack->summaryValue(numberOfPixelHits,xAOD::numberOfPixelHits);
      probeTrack->summaryValue(numberOfSCTHits,xAOD::numberOfSCTHits);
      probeTrack->summaryValue(numberOfTRTHits,xAOD::numberOfTRTHits);

      //determine difference in z0 between tag and probe (PT) track for Minv method
      InvMasscriteriadZ0=tagZ0-probeTrack->z0();
    }
    //METHOD2: Minimum deltaR
    if(m_deltaR<PTOptDeltaR){
      //Set new optimum track deltaR
      PTOptDeltaR=m_deltaR;
      PTOptIteratorAlt=p;

      //determine difference in z0 between tag and probe (PT) track for deltaR method
      dRcriteriadZ0=tagZ0-probeTrack->z0();
      }
    p++;
  }  

  //FAST TRACK FINDER
  p=0;
  for(auto probeTrack:FTFprobeTracks){
    //Check probeTrack and tag have opposite charge, if not then exit iteration (track)                                                                                                                                                                     
    if(tagCharge*probeTrack->charge()!=-1){
      p++;
      continue;
    }
    //Calc inv mass
    m_InvMass=CombinedInvMass(tagpT,tagPx,tagPy,tagPz,probeTrack);
    m_deltaR=sqrt((CLUSTEROptEta-probeTrack->eta())*(CLUSTEROptEta-probeTrack->eta())+(CLUSTEROptPhi-probeTrack->phi())*(CLUSTEROptPhi-probeTrack->phi()));
    //push back onto vector
    m_FTFinv.push_back(m_InvMass);
    m_FTFdeltaR.push_back(m_deltaR);
    
    //Find optimal track -> 
    //METHOD!: Probe track corresponding to Minv closest to Zmass                                                                                                                                                                              
    if(fabs(m_InvMass-Zmass)<fabs(FTFOptInvMass-Zmass)){

      //Set new optimal track                                                                                                                                                                                                                               
      FTFOptInvMass=m_InvMass;
      FTFOptIterator=p;
    }
    //METHOD2: Minimum deltaR
    if(m_deltaR<FTFOptDeltaR){
      //Set new optimum track deltaR                                                                                                                                                                                                                        
      FTFOptDeltaR=m_deltaR;
      FTFOptIteratorAlt=p;
    }
    p++;
  }



  //FILLING OF MONITORED VARIABLES FOR HISTOGRAMS
  //push back optimum track invariant mass and deltaR onto monitored containers 
  if(PTOptInvMass != 0){
  m_PTOptinv.push_back(PTOptInvMass);
  }
  if(PTOptDeltaR != 999){
  m_PTOptDeltaR.push_back(PTOptDeltaR);
  }
  if(FTFOptInvMass != 0){
  m_FTFOptinv.push_back(FTFOptInvMass);
  }
  if(FTFOptDeltaR != 999){
    m_FTFOptDeltaR.push_back(FTFOptDeltaR);
  }
  //Declare monitored variable for the longitudinal impact parameter of tag
  m_tagZ0=tagZ0; 
  //monitored variables for number of hits in pixel, SCT and TRT by converting from unsigned 8 bit integer to type int
  if(m_PTOptinv.size()!=0){
    m_PTnumberOfPixelHits=static_cast<int>(numberOfPixelHits);
    m_PTnumberOfSCTHits=static_cast<int>(numberOfSCTHits);
    m_PTnumberOfTRTHits=static_cast<int>(numberOfTRTHits);
    //filling 2D histogram of PTinvMass vs deltaPhi for positron only 
    if(OptProbeCharge==1){
      m_PTdeltaPhiPositron=m_PTdeltaPhi;
    }
  }
  


  
  //INVARIANT MASS CALCULATION - from tag "track" - probe track pair 
  //Note this is NOT required by algorithm. Used as a check for effect of bremstrahlung on the track variables. Remove when running in the online.
  //declare monitored variable
  double m_trackInvMass;
  //iterate over tag and probe pairs
  for(auto tagTrack:tagTracks){
    for(auto probeTrack:PTprobeTracks){
      //Check if tag and probe track have opposite charge. If they do then calc invariant mass and pushback onto vector
      if(tagTrack->charge()*probeTrack->charge()!=-1){
	continue;
      }
      else{
	//Calc inv mass
	m_trackInvMass=TrackInvMass(tagTrack,probeTrack);
	//push back onto m_PTinvTrack to fill histogram...
	m_PTinvTrack.push_back(m_trackInvMass);	
      }
    }
    //FTF
    for(auto probeTrack:FTFprobeTracks){
      //Check if tag and probe track have opposite charge. If they do then calc inv mass and pushback onto vector
      if(tagTrack->charge()*probeTrack->charge()!=-1){
	continue;
      }
      else{
	//Calc inv mass                                   
	m_trackInvMass=TrackInvMass(tagTrack,probeTrack);
	//push back onto m_FTFinvTrack to fill histogram...                                                                                                                                                                                                 
	m_FTFinvTrack.push_back(m_trackInvMass);
      }
    }
  }




  //ELECTRON TRACKING EFFICICIENCY 

  //define monitored variables for efficiency profiles
  m_ptEfficiencyProfile=CLUSTEROptPT;  
  m_invMassEfficiencyProfile=CLUSTEROptInvMass;                                                                                                                                                                                                             
  m_phiEfficiencyProfile=CLUSTEROptPhi;
  m_etaEfficiencyProfile=CLUSTEROptEta;

  //PRECISION TRACKER - EFFICIENCY
  //Three sets of efficiency criteria defined in code
  //1) Invariant mass: 40 GeV < minv < 200 GeV
  //2) deltaR between probe cluster and track: deltaR < 0.1
  //3) A combination of the two (if just one of the conditions satisfied then efficient)

  //1)INVARIANT MASS CRITERIA
  if(m_PTOptinv.size()!=0){
    //If efficiency condition satisfied then set numerator equal to one
    if(PTOptInvMass>40000&&PTOptInvMass<200000){
      m_PTfound=1;
      ATH_MSG_DEBUG("PT:::efficient event with inv mass: "<<PTOptInvMass);
    }else{//If not then set numerator equal to zero
      m_PTfound=0;
      ATH_MSG_DEBUG("PT:::inefficient event with inv mass: "<<PTOptInvMass);
    }
  }
  else{
    //No track found -> Inefficient event
    m_PTfound=0;
    ATH_MSG_DEBUG("PT:::inefficient event - No track found");
    }
  
  //2)DETLAR CRITERIA
  if(m_PTOptDeltaR.size()!=0){
    //If efficiency criteria satisfied then set numerator equal to one
    if(PTOptDeltaR<0.1){
      m_PTfoundAlt=1;
      ATH_MSG_DEBUG("PTAlt:::efficient event with dR: "<<PTOptDeltaR);
    }
    else{//If not then set numerator equal to zero
      m_PTfoundAlt=0;
      ATH_MSG_DEBUG("PTAlt:::inefficient event with dR: "<<PTOptDeltaR);
    }
  }
  else{
    //No track found -> Inefficient event
    m_PTfoundAlt=0;
    ATH_MSG_DEBUG("PTAlt:::inefficent event - No track found");
    }

  //3)COMBINED CRITERIA
  if(m_PTOptinv.size()!=0 || m_PTOptDeltaR.size()!=0){
    //If efficiency criteria satisfied then set numerator equal to one
    if((PTOptInvMass>40000&&PTOptInvMass<200000)||PTOptDeltaR<0.1){
      m_PTfoundCombined=1;
      ATH_MSG_DEBUG("PTCombined:::efficient event");
      //z0 Monitoring of efficient tracks
      //fill Z0 monitored container for efficient events, depending on which criteria was satisfied
      //if both... then choose smallest dZ0 to fill histogram
      if((PTOptInvMass>40000&&PTOptInvMass<200000)&&PTOptDeltaR<0.1){
	if(InvMasscriteriadZ0<dRcriteriadZ0){
	  m_dZ0.push_back(InvMasscriteriadZ0);
	}else{
	  m_dZ0.push_back(dRcriteriadZ0);
	}
      }
      //if invMass criteria
      else if(PTOptDeltaR>=0.1){
	m_dZ0.push_back(InvMasscriteriadZ0);
      }
      //if dR criteria
      else{
	m_dZ0.push_back(dRcriteriadZ0);
      }
    }else{//If not then set numerator equal to zero
      m_PTfoundCombined=0;
      ATH_MSG_DEBUG("PTCombined:::inefficient event");
    }
  }
  else{
    //No track found -> Inefficient event
    m_PTfoundCombined=0;
    ATH_MSG_DEBUG("PTCombined:::inefficient event - No track found");
  }

  //FAST TRACK FINDER - EFFICIENCY
  //Only one efficiency criteria implemented in code
  //1) Combined criteria of: 40 GeV < Minv < 200 GeV OR deltaR < 0.1
  
  //1)COMBINED CRITERIA
  if(m_FTFOptinv.size()!=0 || m_FTFOptDeltaR.size()!=0){
    //If efficiency criteria satisfied then set numerator equal to one
    if((FTFOptInvMass>40000&&FTFOptInvMass<200000)||FTFOptDeltaR<0.1){
      m_FTFfound=1;
      ATH_MSG_DEBUG("FTFCombined:::efficient event");
    }else{//If not then set numerator equal to zero
      m_FTFfound=0;
      ATH_MSG_DEBUG("FTFCombined:::inefficient event");
    }
  }
  else{
    //No track found -> Inefficient event
    m_FTFfound=0;
    ATH_MSG_DEBUG("FTFCombined:::inefficient event - No track found");
  }




  //MONITORING EFFICIENCY OF INNERMOST AND NEXT TO INNERMOST LAYERS
  int expectInnermost, numberInnermost, expectNextToInnermost, numberNextToInnermost;
  if( m_PTOptinv.size()!=0 ){
    //Convert unsigned 8bit integers to type int
    expectInnermost = static_cast<int>(expectInnermostPixelLayerHit);
    numberInnermost = static_cast<int>(numberOfInnermostPixelLayerHits);
    expectNextToInnermost = static_cast<int>(expectNextToInnermostPixelLayerHit);
    numberNextToInnermost = static_cast<int>(numberOfNextToInnermostPixelLayerHits);
    //If we expect to see a hit in the innermost layer...
    if( expectInnermost == 1 ){
      //Check if we observe a number of hits in pixel detector
      if( numberInnermost>0 ){
        //Set profile variable: m_PTpixelFound equal to 1
        m_PTpixelFound=1;
      }else{
        //Set profile variable: m_PTpixelFound equal to 0
        m_PTpixelFound=0;
      }
    }
    //If we expect to see a hit in the next to innermost layer...
    if( expectNextToInnermost == 1 ){
      //Check if we observe a number of hits in pixel detector
      if( numberNextToInnermost>0 ){
        //Set profile variable: m_PTpixelFound equal to 1
        m_PTpixelNextToFound=1;
      }else{
        //Set profile variable: m_PTpixelFound equal to 0
        m_PTpixelNextToFound=0;
      }
    }
  }



  //DEBUG MESSAGES - These remain in code as checks. Will be removed before implementing in the online run.
 //DEBUG messages for IBL Monitoring
 ATH_MSG_DEBUG("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Innermost-Detector~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
 if( m_PTOptinv.size()!=0 ){ 
   ATH_MSG_DEBUG("PT-optimum-track filled...");
   if( expectInnermost == 1){
     ATH_MSG_DEBUG("Track is expected to have hits in the innermost layer of pixel detector -> expectInnermost = "<<expectInnermost);
   }else{
     ATH_MSG_DEBUG("Track is NOT expected to have hits in the innermost layer of pixel detector -> expectInnermost = "<<expectInnermost);
   }
   ATH_MSG_DEBUG("Track has "<<numberInnermost<<" in the innermost layer of detector -> m_PTpixelFound = "<<m_PTpixelFound);
   if(m_PTpixelFound == 1){
     ATH_MSG_DEBUG("Innermost detector working as expected");
   }else if(m_PTpixelFound == 0){
     ATH_MSG_DEBUG("PROBLEM with innermost layer");
   }
 }

 ATH_MSG_DEBUG("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Next-To-Innermost-Detector~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
 if( m_PTOptinv.size()!=0 ){ 
   ATH_MSG_DEBUG("PT-optimum-track filled...");
   if( expectNextToInnermost == 1){
     ATH_MSG_DEBUG("Track is expected to have hits in the next to innermost layer of pixel detector -> expectNextToInnermost = "<<expectNextToInnermost);
   }else{
     ATH_MSG_DEBUG("Track is NOT expected to have hits in the next to innermost layer of pixel detector -> expectNextToInnermost = "<<expectNextToInnermost);
   }
   ATH_MSG_DEBUG("Track has "<<numberNextToInnermost<<" in the next to innermost layer of detector -> m_PTpixelNextToFound = "<<m_PTpixelNextToFound);
   if(m_PTpixelNextToFound == 1){
     ATH_MSG_DEBUG("Innermost detector working as expected");
   }else if(m_PTpixelFound == 0){
     ATH_MSG_DEBUG("PROBLEM with the next to innermost layer");
   }
 }

  //DEBUG messages for tag and probe event
  ATH_MSG_DEBUG("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Tag-and-Probe-Method~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");  
  //Display topological info for cluster and probe track and tag combined
  int y=0; int z=0;
  ATH_MSG_DEBUG("CLUSTER:::Minv: "<<CLUSTEROptInvMass<<"pT: "<<CLUSTEROptPT<<", eta: "<<CLUSTEROptEta<<", phi: "<<CLUSTEROptPhi<<", dPhi: "<<CLUSTEROptDeltaPhi);
  
  if(PTOptIterator==PTOptIteratorAlt){
    ATH_MSG_DEBUG("PT:::Both methods have found the SAME OPTIMUM TRACK!");
  }else{
    ATH_MSG_DEBUG("PT:::Methods have found DIFFERENT OPTIMUM TRACK!");
  }
  if(FTFOptIterator==FTFOptIteratorAlt){
    ATH_MSG_DEBUG("FTF:::Both methods have found the SAME OPTIMUM TRACK!");
  }else{
    ATH_MSG_DEBUG("FTF:::Methods have found DIFFERENT OPTIMUM TRACK!");
  }

  for(auto probeTrack: PTprobeTracks){
    if(y==PTOptIterator){
      if(m_PTfound==0){
	ATH_MSG_DEBUG("PT:::1)INVARIANT MASS CRITERIA: This candidate was found to be INEFFICIENT");
      }
      if(m_PTfound==1){
        ATH_MSG_DEBUG("PT:::1)INVARIANT MASS CRITERIA: This candidate was found to be !EFFICIENT");
      }
      ATH_MSG_DEBUG("PT:::Minv: "<<PTOptInvMass<<", pT: "<<probeTrack->pt()<<", eta: "<<probeTrack->eta()<<", phi: "<<probeTrack->phi()<<", d0: "<<probeTrack->d0()<<", z0: "<<probeTrack->z0());
    }
    if(z==PTOptIteratorAlt){
      if(m_PTfoundAlt==0){
        ATH_MSG_DEBUG("PTAlt:::2)DELTAR CRITERIA: This candidate was found to be INEFFICIENT");
      }
      if(m_PTfoundAlt==1){
        ATH_MSG_DEBUG("PTAlt:::2)DELTRAR CRITERIA: This candidate was found to be !EFFICIENT");
      }
      ATH_MSG_DEBUG("PTAlt:::dR: "<<PTOptDeltaR<<", pT: "<<probeTrack->pt()<<", eta: "<<probeTrack->eta()<<", phi: "<<probeTrack->phi()<<", d0: "<<probeTrack->d0()<<", z0: "<<probeTrack->z0());
    }   
    y++; z++;
  }

  if(m_PTfoundCombined==0){
    ATH_MSG_DEBUG("PTCombined:::3)COMBINED CRITERIA: This candidate was found to be INEFFICIENT");
  }
  if(m_PTfoundCombined==1){
    ATH_MSG_DEBUG("PTCombined:::3)COMBINED CRITERIA: This candidate was found to be !EFFICIENT");
  }

  y=0; z=0;
  for(auto probeTrack: FTFprobeTracks){
    if(y==FTFOptIterator){
      ATH_MSG_DEBUG("FTF:::Minv: "<<FTFOptInvMass<<", pT: "<<probeTrack->pt()<<", eta: "<<probeTrack->eta()<<", phi: "<<probeTrack->phi()<<", d0: "<<probeTrack->d0()<<", z0: "<<probeTrack->z0());
    }
    if(z==FTFOptIterator){
      ATH_MSG_DEBUG("FTF:::dR: "<<FTFOptDeltaR<<", pT: "<<probeTrack->pt()<<", eta: "<<probeTrack->eta()<<", phi: "<<probeTrack->phi()<<", d0: "<<probeTrack->d0()<<", z0: "<<probeTrack->z0());
    }
    y++;
  }
  if(m_FTFfound==0){
    ATH_MSG_DEBUG("FTF:::1)COMBINED CRITERIA: This candidate was found to be INEFFICIENT");
  }
  if(m_FTFfound==1){
    ATH_MSG_DEBUG("FTF:::1)COMBINED CRITERIA: This candidate was found to be !EFFICIENT");
  }

 
  ATH_MSG_DEBUG("TAG:::pT: "<<tagpT<<", eta: "<<tagEta<<", phi: "<<tagPhi<<", d0: "<<tagd0<<", z0: "<<m_tagZ0);
  ATH_MSG_DEBUG("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`

  //END OF ALGORITHM
  ATH_MSG_DEBUG( "Finishing TrigIDTPMonitorElectron" );

  return HLT::OK;
}




//FUNCTIONS
//1)To calculate invariant mass from tag electron and probe track
double TrigIDTPMonitorElectron::CombinedInvMass(double ptTag, double pxTag, double pyTag, double pzTag, const xAOD::TrackParticle* probe){
  double ptProbe=probe->pt();
  double pxProbe=probe->p4().Px();
  double pyProbe=probe->p4().Py();
  double pzProbe=probe->p4().Pz();

  double pX=pxTag+pxProbe;
  double pY=pyTag+pyProbe;
  double pZ=pzTag+pzProbe;

  double electronmasssquared= 0.261121;

  double E = sqrt(ptTag*ptTag + pzTag*pzTag + electronmasssquared) + sqrt(ptProbe*ptProbe + pzProbe*pzProbe + electronmasssquared);
  double P2 = pX*pX + pY*pY + pZ*pZ;
  double invmass2 = E*E - P2;

  return sqrt(invmass2);
}
 
//2)Calculate invariant mass from tag electron and probe cluster
double TrigIDTPMonitorElectron::ClusterInvMass(double ptTag, double pxTag, double pyTag, double pzTag, double ptCluster, double pxCluster, double pyCluster, double pzCluster){

  double pX = pxTag+pxCluster;
  double pY = pyTag+pyCluster;
  double pZ = pzTag+pzCluster;

  double electronmasssquared = 0.261121;

  double E = sqrt(ptTag*ptTag + pzTag*pzTag + electronmasssquared) + sqrt(ptCluster*ptCluster + pzCluster*pzCluster + electronmasssquared);
  double P2 = pX*pX + pY*pY + pZ*pZ;
  double invmass2 = E*E - P2;

  return sqrt(invmass2);
}

//3)To calculate invariant mass of tag track and probe track                                                                                                                                                                                                
double TrigIDTPMonitorElectron::TrackInvMass(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2){

  double pt1 = track1->pt();
  double px1 = track1->p4().Px();
  double py1 = track1->p4().Py();
  double pz1 = track1->p4().Pz();

  double pt2 = track2->pt();
  double px2 = track2->p4().Px();
  double py2 = track2->p4().Py();
  double pz2 = track2->p4().Pz();

  double pX = px1+px2;
  double pY = py1+py2;
  double pZ = pz1+pz2;

  double electronmasssquared = 0.261121;

  double E = sqrt(pt1*pt1 + pz1*pz1 + electronmasssquared) + sqrt(pt2*pt2 + pz2*pz2 + electronmasssquared);
  double P2 = pX*pX + pY*pY + pZ*pZ;
  double invmass2 = E*E - P2;

  return sqrt(invmass2);
}

//4)To calculate invariant mass of tag track and probe cluster
double TrigIDTPMonitorElectron::ClusterTagTrackInvMass(const xAOD::TrackParticle* tag, double ptCluster, double pxCluster, double pyCluster, double pzCluster){

  double ptTag = tag->pt();
  double pxTag = tag->p4().Px();
  double pyTag = tag->p4().Py();
  double pzTag = tag->p4().Pz();

  double pX = pxTag+pxCluster;
  double pY = pyTag+pyCluster;
  double pZ = pzTag+pzCluster;

  double electronmasssquared = 0.261121;

  double E = sqrt(ptTag*ptTag + pzTag*pzTag + electronmasssquared) + sqrt(ptCluster*ptCluster + pzCluster*pzCluster + electronmasssquared);
  double P2 = pX*pX + pY*pY + pZ*pZ;
  double invmass2 = E*E - P2;

  return sqrt(invmass2);
}
