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
 **
 **   Created:   05.07.2016
 **   Modified:  12.07.2016
 **
 **
 ***************************************************************************/

#include "TrigIDTPMonitorElectron.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

#include "xAODEventInfo/EventInfo.h"

#include "AthLinks/ElementLink.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackingPrimitives.h"

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
  //declareMonitoredStdContainer("PTInvMassTrack_before_cuts",  m_PTinvTrack_before_cuts,      AutoClear);
  //declareMonitoredStdContainer("FTFInvMassTrack_before_cuts",  m_FTFinvTrack_before_cuts,      AutoClear);
  declareMonitoredStdContainer("PTInvMassTrack",  m_PTinvTrack,      AutoClear);
  declareMonitoredStdContainer("FTFInvMassTrack",  m_FTFinvTrack,      AutoClear);
  declareMonitoredStdContainer("InvMassCluster",  m_invCluster,      AutoClear);
  declareMonitoredStdContainer("InvMassCluster_before_cuts", m_invCluster_before_cuts,    AutoClear);
  declareMonitoredStdContainer("InvMassClusterTagTrack",  m_invClusterTagTrack,      AutoClear);
  declareMonitoredStdContainer("InvMassClusterTagTrack_before_cuts", m_invClusterTagTrack_before_cuts,    AutoClear);
  declareMonitoredStdContainer("clusterPT",  m_clusterPT,      AutoClear);
  declareMonitoredStdContainer("clusterPhi",  m_clusterPhi,      AutoClear);
  declareMonitoredStdContainer("clusterEta",  m_clusterEta,      AutoClear);
  declareMonitoredStdContainer("deltaPhi",  m_deltaPhi,      AutoClear);
  declareMonitoredStdContainer("clusterPT_before_cuts",  m_clusterPT_before_cuts,      AutoClear);
  declareMonitoredStdContainer("clusterPhi_before_cuts",  m_clusterPhi_before_cuts,      AutoClear);
  declareMonitoredStdContainer("clusterEta_before_cuts",  m_clusterEta_before_cuts,      AutoClear);
  declareMonitoredStdContainer("deltaPhi_before_cuts",  m_deltaPhi_before_cuts,      AutoClear);
  declareMonitoredStdContainer("PTnumberTracks",  m_PTnumber,      AutoClear);
  declareMonitoredStdContainer("FTFnumberTracks",  m_FTFnumber,      AutoClear);
  declareMonitoredStdContainer("PTdeltaR",   m_PTdeltaR,    AutoClear);
  declareMonitoredStdContainer("PTOptdeltaR",   m_PTOptDeltaR,    AutoClear);
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
  declareMonitoredVariable("PTnumberOfPixelHits",   m_PTnumberOfPixelHits,    -10);
  declareMonitoredVariable("PTnumberOfSCTHits",   m_PTnumberOfSCTHits,    -10);
  declareMonitoredVariable("PTnumberOfTRTHits",   m_PTnumberOfTRTHits,    -10);
  declareMonitoredVariable("PTEfficientDeltaZ0",    m_dZ0,    -999);

  //For profile histograms
  declareMonitoredVariable("probePtEfficiency",   m_ptEfficiencyProfile,   -1);
  declareMonitoredVariable("probePhiEfficiency",   m_phiEfficiencyProfile,   -999);
  declareMonitoredVariable("probeEtaEfficiency",   m_etaEfficiencyProfile,   -999);
  declareMonitoredVariable("probeInvMassEfficiency",  m_invMassEfficiencyProfile,   -1);  
  declareMonitoredVariable("PTfound",    m_PTfound,      -1);
  declareMonitoredVariable("FTFfound",    m_FTFfound,     -1);
  declareMonitoredVariable("PTfoundAlt", m_PTfoundAlt,     -1);
  declareMonitoredVariable("PTfoundCombined", m_PTfoundCombined,    -1);

  //Set number of events and cuts to zero in constructor
  m_PTnumberTracks=0;
  m_FTFnumberTracks=0;
  m_PTnumberCuts=0;
  m_FTFnumberCuts=0;
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

  //CHANGE TO TEST SVN STUFF

  //Retrieve event info ->See notepad.cxx

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
  std::vector<const xAOD::TrackParticle*> tagTracks;
  //Define 2 probe tracks one for PT and one for FTF 
  std::vector<const xAOD::TrackParticle*> PTprobeTracks;
  std::vector<const xAOD::TrackParticle*> FTFprobeTracks;


  //CHECK THE CHAIN NAME!! IS IT e26
  std::string tagname = "EF_e26_lhtight_nod0_L1_EM22VHI";
  std::string probename = "EF_e15_etcut_L1EM7_L1_EM7";

  //Now must see which is tag and which is probe, therefore compare the ID label to tagname and probename, and change bool operator accordingly
  if(teInLabel1.compare(tagname) == 0 && teInLabel2.compare(probename) == 0){
    ATH_MSG_DEBUG( "The first electron is the tag, the second electron is the probe" );
    tag1probe2 = true;
  }
  else if(teInLabel1.compare(probename) == 0 && teInLabel2.compare(tagname) == 0){
    ATH_MSG_DEBUG( "The second electron is the tag, the first electron is the probe" );
    tag2probe1 = true;
  }

  //Get features from electron containers, depending on which is tag and which is probe will need front or back trigger element
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


  //TAG
  //Declare variables to store tag info
  double tagpT=0, tagPhi=0, tagEta=0, tagPx=0, tagPy=0, tagPz=0, tagd0=0, tagCharge=100, tagZ0=-999;
  double tagTrackEta=0, tagTrackPhi=0, tagClusterEta=-999, tagClusterPhi=-999;
  double tagTrackpT=0, tagTrackPz=0;
  //Check TAG has combined tracks and if this is so -> Add it to vector of combined tracks (tagTracks)
  //Loop over tag electrons
  for (auto electronIterator: *ec1){
    if (electronIterator == NULL){
      ATH_MSG_WARNING("Non initialised electron in first ROI... Leaving!");
      return HLT::OK;
    }
    //For combined (track and cluster) information (straight from electronIterator
    tagpT=electronIterator->pt();
    tagPhi=electronIterator->phi();
    tagEta=electronIterator->eta();
    tagPx=electronIterator->p4().Px();
    tagPy=electronIterator->p4().Py();
    tagPz=electronIterator->p4().Pz();

    //For tracking information...
    const xAOD::TrackParticle* electronTrack= electronIterator->trackParticle();
    //Check if electronTrack is NULL pointer
    if (electronTrack == NULL){
      ATH_MSG_DEBUG("Tag electron returns null TrackParticleLink");
      return HLT::OK;
    }
    //Otherwise check the charge is NOT equal to 0 and if so add tag track to the vector of combined tagTracks
    if (abs(electronTrack->charge())>0){//possibly needs || true ->ASK JIRI WHAT THIS MEANS
      tagTracks.push_back(electronTrack);
      ATH_MSG_DEBUG("Tag track added to vector!");
    }
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

  //For tag cluster information...
  for(auto clusterIterator: *tagClusterCont){
    if(clusterIterator == NULL){
      ATH_MSG_WARNING( "Not initialised in relevant roi, leaving!" );
      return HLT::OK;
    }
    tagClusterEta=clusterIterator->eta();
    tagClusterPhi=clusterIterator->phi();
  }

  //Check if tagTracks is empty
  if(tagTracks.size() == 0){
    ATH_MSG_DEBUG( "Empty Tag vector, nothing to analyse, leaving" );
    return HLT::OK;
  }
  //Check if tagCluster is empty
  if(tagClusterEta==-999){
    ATH_MSG_DEBUG("Empty tag cluster, leaving");
   return HLT::OK;
  }

  //Determine deltaEta and deltaPhi (to be histogrammed)
  m_tagDeltaEta=tagClusterEta-tagTrackEta;
  m_tagDeltaPhi=tagClusterPhi-tagTrackPhi;
  m_tagExpectedDeltaPhi=-acos(((tagTrackpT/600)-(588/tagTrackpT))/(tagTrackpT/600));
  //multiply by -1 for positrons
  if(tagCharge==1){
    m_tagExpectedDeltaPhi=-1*m_tagExpectedDeltaPhi;
  }

  //TAG SELECTION CUTS
  //eta: exclude electrons in crack region between barrel and end-cap (1.37-1.52)
  if((tagEta>1.37&&tagEta<1.52)||(tagEta>-1.52&&tagEta<-1.37)){
    //Leave
    ATH_MSG_DEBUG("TAG:::Eta outside of desired range: "<<tagEta);
    return HLT::OK;
  }

  //declare monitored variables for histogram filling
  m_tagEta=tagEta;
  m_tagpT=tagpT;
  m_tagTrackpT=tagTrackpT;

  //LOOP OVER PROBE CLUSTER CANDIDATES - extract variables and store in a vector
  //Define temporary floats to hold variables
  double tempPT=0, tempPHI=0, tempETA=0, tempPx=0, tempPy=0, tempPz=0, tempInvMass=0, tempClusterTagTrackInvMass=0;
  double CLUSTEROptInvMass=0, CLUSTEROptPT=0, CLUSTEROptPhi=0, CLUSTEROptEta=0, CLUSTEROptTagTrackInvMass=0;;
  double tempDeltaPhi=0, CLUSTEROptDeltaPhi=0;
  for(auto clusterIterator: *clusterCont){
    //If not initialised in ROI then leave
    if(clusterIterator == NULL){
      ATH_MSG_WARNING( "Not initialised in relevant roi, leaving!" );
      return HLT::OK;
    }
    //Extract cluster tranverse momentum/phi/eta/4-momentum and store in temporary floats
    tempPT=clusterIterator->pt();
    tempPHI=clusterIterator->phi();
    tempETA=clusterIterator->eta();
    tempPx=clusterIterator->p4().Px();
    tempPy=clusterIterator->p4().Py();
    tempPz=clusterIterator->p4().Pz();

    //pushback pT, phi and eta onto vectors before cuts
    m_clusterPT_before_cuts.push_back(tempPT);
    m_clusterPhi_before_cuts.push_back(tempPHI);
    m_clusterEta_before_cuts.push_back(tempETA);

    //Calculate invariant mass of cluster
    tempInvMass=ClusterInvMass(tagpT,tagPx,tagPy,tagPz,tempPT,tempPx,tempPy,tempPz);
    //pushback onto invariant mass vector before cuts
    m_invCluster_before_cuts.push_back(tempInvMass);

    //Calculate invariant mass of cluster from TAG TRACK only
    //loop over tag Tracks->SHOULD ONLY BE ONE, if there is more than one will have to adapt code as takes last tagTrack invariant mass
    for(auto tagTrack:tagTracks){
    tempClusterTagTrackInvMass=ClusterTagTrackInvMass(tagTrack,tempPT,tempPx,tempPy,tempPz);;
    //pushback onto vector
    m_invClusterTagTrack_before_cuts.push_back(tempClusterTagTrackInvMass);
    }

    //Calculate delta phi for cluster and tag track
    tempDeltaPhi=fabs(tempPHI-tagPhi);
    //push back onto vector before cuts
    m_deltaPhi_before_cuts.push_back(tempDeltaPhi);

    //ONLY SELECT CLUSTER WITH InvMASS CLOSEST TO ZMASS
    if(fabs(Zmass-tempInvMass)<fabs(Zmass-CLUSTEROptInvMass)){
      CLUSTEROptInvMass=tempInvMass;
      CLUSTEROptTagTrackInvMass=tempClusterTagTrackInvMass;
      CLUSTEROptPT=tempPT;
      CLUSTEROptPhi=tempPHI;
      CLUSTEROptEta=tempETA;
      CLUSTEROptDeltaPhi=tempDeltaPhi;
    }
  }

  //Selection Cuts to isolate Z->ee candidates
  //On cluster mass -> If CLUSTEROptInvMass not inside window then leave event
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
  /*if(CLUSTERtopDeltaPhi<3||CLUSTERtopDeltaPhi>3.25){
    ATH_MSG_DEBUG("Cluster delta phi outside accepted range");
    return HLT::OK;
    }*/
  //On transverse momentum 

  //Push back the values for the optimum cluster onto the vectors                                                                                                                                                                             
  m_deltaPhi.push_back(CLUSTEROptDeltaPhi);
  m_invCluster.push_back(CLUSTEROptInvMass);
  m_clusterPT.push_back(CLUSTEROptPT);
  m_clusterPhi.push_back(CLUSTEROptPhi);
  m_clusterEta.push_back(CLUSTEROptEta);
  m_invClusterTagTrack.push_back(CLUSTEROptTagTrackInvMass);



  //LOOP OVER PROBE Tracking CANDIDATES for precision tracker
  for(auto probeIterator: *PTTrkContainerElectron){
    //If not initialised in ROI, leave
    if(probeIterator == NULL){
      ATH_MSG_WARNING( "Not initialised in relavant roi, leaving!" );
      return HLT::OK;
    }
    //define tempProbeTrack from iterator
    const xAOD::TrackParticle* tempProbeTrack= probeIterator;//->trackParticle();    
    //auto tempProbeTrack=*probeIterator;

    //Check track corresponds to a particle with charge
    if(abs(tempProbeTrack->charge())>0){
      //And probe track to vector of combined tracks
      PTprobeTracks.push_back(tempProbeTrack);
      ATH_MSG_DEBUG("Probe track added to PT vector!");
    }
  }

  //LOOP OVER PROBE Tracking CANDIDATES for FTF tracker                                                                                                                                                                                               
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



  //INVARIANT MASS CALCULATION FROM TAG COMBINED and probe track
  //declare variable
  double m_InvMass, m_deltaR, OptProbeCharge=999;//deltaR defined as the quadrature sum of deltaEta and deltaPhi
  //declare variables to store optimal InvMass
  double PTOptInvMass=0; double FTFOptInvMass=0; int PTOptIterator=-1; /*int FTFOptIterator=-1*/; int PTOptIteratorAlt=-1;
  double dZ0=-999;
  double PTOptDeltaR=999; //double FTFOptDeltaR=999
  uint8_t numberOfPixelHits, numberOfSCTHits, numberOfTRTHits;
  int p=0;

  //iterate over probe tracks
  //PT
  for(auto probeTrack:PTprobeTracks){
    //Check probeTrack and tag have opposite charge, if not then exit iteration (track)
    if(tagCharge*probeTrack->charge()!=-1){
      continue;
    }
    //Calculate invariant mass and delta R
    m_InvMass=CombinedInvMass(tagpT,tagPx,tagPy,tagPz,probeTrack);
    m_deltaR=sqrt((CLUSTEROptEta-probeTrack->eta())*(CLUSTEROptEta-probeTrack->eta())+(CLUSTEROptPhi-probeTrack->phi())*(CLUSTEROptPhi-probeTrack->phi()));
    //push back invariant mass onto vector
    m_PTinv.push_back(m_InvMass);

    //Find optimal track ->
    //METHOD1: Probe track corresponding to Minv closest to Zmass
    if(fabs(m_InvMass-Zmass)<fabs(PTOptInvMass-Zmass)){
      //Set new optimal track
      PTOptInvMass=m_InvMass;
      PTOptIterator=p;
      //Determine delta eta and delta phi between cluster and track and store in monitored variable
      m_PTdeltaEta=CLUSTEROptEta-probeTrack->eta();
      m_PTdeltaPhi=CLUSTEROptPhi-probeTrack->phi();

      OptProbeCharge=probeTrack->charge();

      probeTrack->summaryValue(numberOfPixelHits,xAOD::numberOfPixelHits);
      probeTrack->summaryValue(numberOfSCTHits,xAOD::numberOfSCTHits);
      probeTrack->summaryValue(numberOfTRTHits,xAOD::numberOfTRTHits);

      //determine difference in z0 between tag and probe (PT) track
      dZ0=tagZ0-probeTrack->z0();
    }
    //METHOD2: Minimum deltaR
    if((m_deltaR<PTOptDeltaR)&&(fabs(CLUSTEROptPhi-probeTrack->phi())>0.005)){
      //Set new optimum track deltaR
      PTOptDeltaR=m_deltaR;
      PTOptIteratorAlt=p;
      //Set invariant mass
      //PTOptInvMass=m_InvMass;
      //PTOptIterator=p;
      //Determine delta eta and delta phi between cluster and track and store in monitored variable 
      //m_PTdeltaEta=CLUSTEROptEta-probeTrack->eta();
      //m_PTdeltaPhi=CLUSTEROptPhi-probeTrack->phi();
      }
    p++;
  }  

  p=0;
  //FTF
  for(auto probeTrack:FTFprobeTracks){
    //Check probeTrack and tag have opposite charge, if not then exit iteration (track)                                                                                                                                                                     
    if(tagCharge*probeTrack->charge()!=-1){
      continue;
    }
    //Calc inv mass
    m_InvMass=CombinedInvMass(tagpT,tagPx,tagPy,tagPz,probeTrack);
    m_deltaR=sqrt((CLUSTEROptEta-probeTrack->eta())*(CLUSTEROptEta-probeTrack->eta())+(CLUSTEROptPhi-probeTrack->phi())*(CLUSTEROptPhi-probeTrack->phi()));
    //push back onto vector
    m_FTFinv.push_back(m_InvMass);
    m_PTdeltaR.push_back(m_deltaR);
    //Selection Cuts
    //...
    //...
    //Find optimal track -> 
    //METHOD!: Probe track corresponding to Minv closest to Zmass                                                                                                                                                                              
    if(fabs(m_InvMass-Zmass)<fabs(FTFOptInvMass-Zmass)){
      //Set new optimal track                                                                                                                                                                                                                               
      FTFOptInvMass=m_InvMass;
      //FTFOptIterator=p;
      //Determine delta eta and delta phi between cluster and track and store in monitored variable                                                                                                                                                         
      m_FTFdeltaEta=CLUSTEROptEta-probeTrack->eta();
      m_FTFdeltaPhi=CLUSTEROptPhi-probeTrack->phi();
    }
    /*//METHOD2: Minimum deltaR
    if(m_deltaR<FTFOptDeltaR){
      //Set new optimum track deltaR                                                                                                                                                                                                                        
      FTFOptDeltaR=m_deltaR;
      //Set invariant mass                                                                                                                                                                                                                                  
      FTFOptInvMass=m_InvMass;
      FTFOptIterator=p;
      //Determine delta eta and delta phi between cluster and track and store in monitored variable
      m_FTFdeltaEta=CLUSTEROptEta-probeTrack->eta();
      m_FTFdeltaPhi=CLUSTEROptPhi-probeTrack->phi();
    }*/
    p++;
  }

  //fill histograms by declaring monitored variables 
  m_PTOptinv.push_back(PTOptInvMass);
  m_PTOptDeltaR.push_back(PTOptDeltaR);
  m_FTFOptinv.push_back(FTFOptInvMass);
  m_tagZ0=tagZ0;

  //fill monitored variables for number of hits in pixel, SCT and TRT
  if(m_PTinv.size()!=0){
    m_PTnumberOfPixelHits=static_cast<int>(numberOfPixelHits);
    m_PTnumberOfSCTHits=static_cast<int>(numberOfSCTHits);
    m_PTnumberOfTRTHits=static_cast<int>(numberOfTRTHits);
    ATH_MSG_DEBUG("PIXEL: "<<m_PTnumberOfPixelHits<<", SCT: "<<m_PTnumberOfSCTHits<<", TRT: "<<m_PTnumberOfTRTHits);
  }
  //filling 2D histogram of PTinvMass vs deltaPhi for positron only 
  if(OptProbeCharge==1){
    m_PTdeltaPhiPositron=m_PTdeltaPhi;
  }

  //INVARIANT MASS CALCULATION FROM TAG TRACK and probe track
  //declare monitored variable
  double m_trackInvMass;
  //iterate over tag and probe pairs
  for(auto tagTrack:tagTracks){
    ATH_MSG_DEBUG("TAG TRACK:::pT: "<<tagTrack->pt()<<", phi: "<<tagTrack->phi()<<", eta: "<<tagTrack->eta());
    for(auto probeTrack:PTprobeTracks){
      //Display the transverse momentum, phi and eta values of the tracks                                                                                                                                                                                  
      ATH_MSG_DEBUG("PT:Probe pT: "<<probeTrack->pt()<<"   Phi: "<<probeTrack->phi()<<"   Eta: "<<probeTrack->eta()<<"    D0: "<<probeTrack->d0()<<"    Z0: "<<probeTrack->z0());
      //if(probeTrack->summaryValue(numberOfPixelHits,xAOD::numberOfPixelHits)){
      //	ATH_MSG_DEBUG("Successfully recieved number of pixel hits="<<static_cast<int>(numberOfPixelHits));
      //      } 

      //Add one to track iterator
      m_PTnumberTracks++;
      //Check if tag and probe track have opposite charge. If they do then calc invariant mass and pushback onto vector
      if(tagTrack->charge()*probeTrack->charge()!=-1){
        //ATH_MSG_DEBUG("PT Tracks have same charge");
	//Add one to number of cut
	m_PTnumberCuts++;
      }
      //Check if tag and probe tracks have high pT
      else if(tagTrack->pt()<10000||probeTrack->pt()<10000){
	//ATH_MSG_DEBUG("PT Too small transverse momentum");
	m_PTnumberCuts++;
      }
      else{
	//Calc inv mass
	m_trackInvMass=TrackInvMass(tagTrack,probeTrack);
	//push back onto m_PTinvTrack to fill histogram...
	m_PTinvTrack.push_back(m_trackInvMass);	
	//Display the transverse momentum, phi and eta values of the tracks
        //ATH_MSG_DEBUG("PT:Tag PT: "<<tagTrack->pt()<<"   Tag Phi: "<<tagTrack->phi()<<"   Tag Eta: "<<tagTrack->eta());
        //ATH_MSG_DEBUG("PT:Probe PT: "<<probeTrack->pt()<<"   Probe Phi: "<<probeTrack->phi()<<"   Probe Eta: "<<probeTrack->eta());
      }
    }
    //FTF
    for(auto probeTrack:FTFprobeTracks){
      //Display the transverse momentum, phi and eta values of the tracks 
      ATH_MSG_DEBUG("FTF:Probe pT: "<<probeTrack->pt()<<"   Phi: "<<probeTrack->phi()<<"   Eta: "<<probeTrack->eta()<<"    D0: "<<probeTrack->d0()<<"    Z0: "<<probeTrack->z0());
      //Add one to track iterator
      m_FTFnumberTracks++;
      //Check if tag and probe track have opposite charge. If they do then calc inv mass and pushback onto vector
      if(tagTrack->charge()*probeTrack->charge()!=-1){
        //ATH_MSG_DEBUG("FTF Tracks have same charge");
        //Add one to number of cut                                                                                                                                                                                                                          
        m_FTFnumberCuts++;
      }
      //Check if tag and probe tracks have high pT                                                                                                                                                                                                          
      else if(tagTrack->pt()<10000||probeTrack->pt()<10000){
        //ATH_MSG_DEBUG("FTF Too small transverse momentum");
        m_FTFnumberCuts++;
      }
      else{
	//Calc inv mass                                   
	m_trackInvMass=TrackInvMass(tagTrack,probeTrack);
	//push back onto m_FTFinvTrack to fill histogram...                                                                                                                                                                                                 
	m_FTFinvTrack.push_back(m_trackInvMass);
	//Display the transverse momentum, phi and eta values of the tracks                                   
	//ATH_MSG_DEBUG("FTF:Tag PT: "<<tagTrack->pt()<<"   Tag Phi: "<<tagTrack->phi()<<"   Tag Eta: "<<tagTrack->eta());                                                                                                                                  
	//ATH_MSG_DEBUG("FTF:Probe PT: "<<probeTrack->pt()<<"   Probe Phi: "<<probeTrack->phi()<<"   Probe Eta: "<<probeTrack->eta());
      }
    }
  }



  //NEW CODE FOR EFFICIENCY PROFILES
  m_ptEfficiencyProfile=CLUSTEROptPT;                                                                                                                                                                                                                
  m_invMassEfficiencyProfile=CLUSTEROptInvMass;                                                                                                                                                                                                             
  m_phiEfficiencyProfile=CLUSTEROptPhi;
  m_etaEfficiencyProfile=CLUSTEROptEta;

  if(m_PTOptinv.size()!=0){
    //Efficiency condition
    if(PTOptInvMass>40000&&PTOptInvMass<200000){//InvMass
    //if(m_PTdeltaEta<0.02){//deltaEta between cluster and track
    //if((fabs(PTOptInvMass-Zmass)<20000)&&(m_PTdeltaEta<0.02)){//COMBINED: InvMass and deltaEta
      //set numerator of profile histogram to one
      m_PTfound=1;
      ATH_MSG_DEBUG("PT:::efficient event with inv mass: "<<PTOptInvMass);
      //event is efficient so store dZ0 in monitored variable to be histogrammed
      m_dZ0=dZ0;
    }else{
      m_PTfound=0;
      ATH_MSG_DEBUG("PT:::inefficient event with inv mass: "<<PTOptInvMass);
    }
  }
  else{
    //No track found -> Inefficient event
    m_PTfound=0;
    ATH_MSG_DEBUG("PT:::inefficient event - No track found");
  }

  if(m_FTFOptinv.size()!=0){
    //Efficinecy condition
    if(FTFOptInvMass>40000&&FTFOptInvMass<200000){//InvMass
    //if(m_FTFdeltaEta<0.02){//deltaEta between cluster and track
    //if((fabs(FTFOptInvMass-Zmass)<20000)&&(m_FTFdeltaEta<0.02)){//COMBINED: InvMass and deltaEta
      //set numerator of profile histogram to one
      m_FTFfound=1;
      ATH_MSG_DEBUG("FTF:::efficient event with inv mass: "<<FTFOptInvMass);
    }else{
      m_FTFfound=0;
      ATH_MSG_DEBUG("FTF:::inefficient event with inv mass: "<<FTFOptInvMass);
    }
  }
  else{
    //No track found -> Inefficient event                                                                                                                                                                                                                   
    m_FTFfound=0;
    ATH_MSG_DEBUG("FTF:::inefficient event - No track found");
  }

  //Alternative criteria...
  if(m_PTOptinv.size()!=0){
    //Efficiency criteria
    if(PTOptDeltaR<0.1){
      m_PTfoundAlt=1;
      //track matched to cluster
      ATH_MSG_DEBUG("PTAlt:::efficient event with dR: "<<PTOptDeltaR);
    }
    else{
      m_PTfoundAlt=0;
      ATH_MSG_DEBUG("PTAlt:::inefficient event with dEta: "<<PTOptDeltaR);
    }
  }
  else{
    //No track found -> Inefficient event
    m_PTfoundAlt=0;
    ATH_MSG_DEBUG("PTAlt:::inefficent event - No track found");
  }

  //Combined Criteria of Minv and deltaR
  if(m_PTOptinv.size()!=0){
    if((PTOptInvMass>40000&&PTOptInvMass<200000)||PTOptDeltaR<0.1){
      //event is efficient
      m_PTfoundCombined=1;
      //track matched to cluster
      ATH_MSG_DEBUG("PTCombined:::efficient event");
    }else{
      m_PTfoundCombined=0;
      ATH_MSG_DEBUG("PTCombined:::inefficient event");
    }
  }
  else{
    //No track found 
    m_PTfoundCombined=0;
    ATH_MSG_DEBUG("PTCombined:::inefficient event - No track found");
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //DEBUG MESSAGES AT THE END
  ATH_MSG_DEBUG("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DEBUG~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");  

  //int numberPTTracks=PTprobeTracks.size();
  //int numberFTFTracks=FTFprobeTracks.size();
  //m_PTnumber.push_back(numberPTTracks);
  //m_FTFnumber.push_back(numberFTFTracks);
  //Display the delta phi of cluster and the corresponding number of tracks -> Looking for correlation-maybe deltaPhi~2 are jets with large number of tracks                                                                                                
  //ATH_MSG_DEBUG("DELTA_PHI: "<<CLUSTEROptDeltaPhi<<", Number of PT Tracks: "<<numberPTTracks<<", Number of FTF Tracks: "<<numberFTFTracks);

  //Display topological info for cluster and probe track and tag combined
  int y=0; int z=0;
  ATH_MSG_DEBUG("CLUSTER:::Minv: "<<CLUSTEROptInvMass<<"pT: "<<CLUSTEROptPT<<", eta: "<<CLUSTEROptEta<<", phi: "<<CLUSTEROptPhi<<", dPhi: "<<CLUSTEROptDeltaPhi);
  if(PTOptIterator==PTOptIteratorAlt){
    ATH_MSG_DEBUG("Both methods have found the SAME OPTIMUM TRACK!");
  }else{
    ATH_MSG_DEBUG("Methods have found DIFFERENT OPTIMUM TRACK!");
  }
  for(auto probeTrack: PTprobeTracks){
    if(y==PTOptIterator){
      if(m_PTfound==0){
	ATH_MSG_DEBUG("Minv Criteria: This event was found to be INEFFICIENT");
      }
      if(m_PTfound==1){
        ATH_MSG_DEBUG("Minv Criteria: This event was found to be !EFFICIENT");
      }
      ATH_MSG_DEBUG("PT:::Minv: "<<PTOptInvMass<<", pT: "<<probeTrack->pt()<<", eta: "<<probeTrack->eta()<<", phi: "<<probeTrack->phi()<<", d0: "<<probeTrack->d0()<<", z0: "<<probeTrack->z0());
    }
    if(z==PTOptIteratorAlt){
      if(m_PTfoundAlt==0){
        ATH_MSG_DEBUG("dR Criteria: This event was found to be INEFFICIENT");
      }
      if(m_PTfoundAlt==1){
        ATH_MSG_DEBUG("dR Criteria: This event was found to be !EFFICIENT");
      }
      ATH_MSG_DEBUG("PTAlt:::dR: "<<PTOptDeltaR<<", pT: "<<probeTrack->pt()<<", eta: "<<probeTrack->eta()<<", phi: "<<probeTrack->phi()<<", d0: "<<probeTrack->d0()<<", z0: "<<probeTrack->z0());
    }   
    y++; z++;
  }
  //y=0;
  /*for(auto probeTrack: FTFprobeTracks){
    if(y==FTFOptIterator){
      ATH_MSG_DEBUG("FTF:::Minv: "<<FTFOptInvMass<<", pT: "<<probeTrack->pt()<<", eta: "<<probeTrack->eta()<<", phi: "<<probeTrack->phi()<<", d0: "<<probeTrack->d0()<<", z0: "<<probeTrack->z0());
    }
    y++;
    }*/
  ATH_MSG_DEBUG("TAG:::pT: "<<tagpT<<", eta: "<<tagEta<<", phi: "<<tagPhi<<", d0: "<<tagd0<<", z0: "<<m_tagZ0);
  ATH_MSG_DEBUG("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`

  //END OF ALGORITHM
  ATH_MSG_INFO( "Finishing TrigIDTPMonitorElectron" );

  return HLT::OK;
}



//FUNCTIONS
//1)To calculate invariant mass from combined tag adn probe track
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
 
//2)Calculate invariant mass from tag track and probe cluster...
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

//3)To calculate invariant mass of two tracks...                                                                                                                                                                                                            
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

//4)To calculate invariant mass of Tag track and probe cluster
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
