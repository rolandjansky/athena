/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrigTrkValidationNtupleWriter.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////
#include "TrigTrkValAlgs/TrigTrkValidationNtupleWriter.h"

#include "TTree.h"

// Gaudi includes
#include "GaudiKernel/ITHistSvc.h"

//Trigger tools
#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/FeatureContainer.h"

//included collections which needs to be stored in the Ntuple
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"

//inlcuded helper tools
#include "TrkValInterfaces/IValidationNtupleTool.h"
#include "TrkValInterfaces/IEventPropertyNtupleTool.h"
#include "TrigTrkValInterfaces/IRoIPropertyNtupleTool.h"
#include "TrigTrkValInterfaces/ITrigL2ValidationNtupleTool.h"
#include <iostream>

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

using namespace std;

Trk::TrigTrkValidationNtupleWriter::TrigTrkValidationNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator),
    m_ValTrkParticleNtupleTool("Trk::BasicValTrkParticleNtupleTool/BasicValTrkParticleNtupleTool"),
    m_TrigL2ValidationNtupleTool("TrigL2ValidationNtupleTool/TrigL2ValidationNtupleTool"),
    m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_eventPropertyNtupleHandles(),
    m_eventPropertyNtupleTools(0),
    m_roiPropertyNtupleHandles(),
    m_roiPropertyNtupleTools(0),
    m_inputTrackParticleCollection(0),
    m_inputEFTrackParticleCollection(0),  
    m_trees(0),
    m_eventLinkTree(0),
    m_roiLinkTree(0)
    
 {
    m_roiPropertyNtupleHandles.push_back("Trk::RoIPropertyNtupleTool/RoIPropertyNtupleTool");
    m_roiPropertyNtupleHandles.push_back("Trk::RoIToTrackLinkNtupleTool/RoIToTrackLinkNtupleTool");
    
     
    m_inputEFTrackParticleCollection.push_back("TrackParticleEF");

    m_eventPropertyNtupleHandles.push_back("Trk::EventToTrackLinkNtupleTool/EventToTrackParticleEFLinkNtupleTool");
    
    declareProperty("TrackParticleCollection",   m_inputTrackParticleCollection,   "Names of the TrackParticle collections");
    declareProperty("EFTrackParticleCollection", m_inputEFTrackParticleCollection, "Names of the TrackParticle collections");
    declareProperty("InDetTrackTrigCollection",  m_inputInDetTrackTrigCollection,  "Names of the InDetTrack collections");
    declareProperty("TrigDecisionTool", m_trigDec, "Tool to access the trigger decision");
    declareProperty("RoIPropertyNtupleTools", m_roiPropertyNtupleHandles,"List of event property ntuple tools");
    declareProperty("ValTrkParticleNtupleTool", m_ValTrkParticleNtupleTool, "Tool to write the validation ntuple for Track Particles");
    declareProperty("TrigL2ValidationNtupleTool", m_TrigL2ValidationNtupleTool, "Tool to write the validation ntuple for TrigInDetTrack");
    declareProperty("EventPropertyNtupleTools", m_eventPropertyNtupleHandles,"List of event property ntuple tools");
    declareProperty("ChainName", m_chainName = ".*");
    declareProperty("RoiDescrName", m_roiName= "initialRoI");
    declareProperty("doRoI", m_doRoI=true);
    declareProperty("NtupleFileName", m_ntupleFileName= "TRKVAL");
    declareProperty("NtupleDirName", m_ntupleDirName="Validation");
}

Trk::TrigTrkValidationNtupleWriter::~TrigTrkValidationNtupleWriter() {
                                                                     
                                                                     }

StatusCode Trk::TrigTrkValidationNtupleWriter::initialize() {
    msg(MSG::INFO) <<"TrackValidationNtupleWriter initialize()" << endreq;

    // Get TrackParticle Validation ntuple Tool
    if ( m_ValTrkParticleNtupleTool.retrieve().isFailure()) {
        msg(MSG::FATAL) << "Could not retrieve "<< m_ValTrkParticleNtupleTool <<" (to write TrackParticle validation ntuple) "<< endreq;
        return StatusCode::FAILURE;
     }

    // Get TrigInDetTrack Validation ntuple Tool
    if ( m_TrigL2ValidationNtupleTool.retrieve().isFailure()) {
        msg(MSG::FATAL) << "Could not retrieve "<< m_TrigL2ValidationNtupleTool <<" (to write TrigInDetTrack validation ntuple) "<< endreq;
        return StatusCode::FAILURE;
     }

    // -------------------------------
    // get event property tools
    ToolHandleArray< Trk::IEventPropertyNtupleTool >::iterator itTools;
    if ( m_eventPropertyNtupleHandles.retrieve().isFailure() ) {
      msg(MSG::ERROR) << "Failed to retreive " << m_eventPropertyNtupleHandles << endreq;
    } else {
      msg(MSG::INFO) << "Retrieved " << m_eventPropertyNtupleHandles << endreq;
    }

     
    //std::vector<std::string> classifierNames(0);
    itTools = m_eventPropertyNtupleHandles.begin();
    for (  ; itTools != m_eventPropertyNtupleHandles.end(); ++itTools ) {
      // add tool to list
      m_eventPropertyNtupleTools.push_back(&(*(*itTools)));
    }

    // ---------------------------
    // retrive pointer to THistSvc
    ITHistSvc *tHistSvc;
    if (service("THistSvc", tHistSvc).isFailure()) {
        msg(MSG::ERROR) << "Unable to retrieve pointer to THistSvc" << endreq;
        return StatusCode::FAILURE;
    }
    
    StatusCode sc = StatusCode::SUCCESS;
    // create tree for each given track particle collection and register it to THistSvc
    std::vector<std::string>::const_iterator trackParticleColNameIter = m_inputTrackParticleCollection.begin();
    
    for (; trackParticleColNameIter != m_inputTrackParticleCollection.end(); trackParticleColNameIter++ ) {
        TTree* tree = new TTree((*trackParticleColNameIter).c_str(), ((*trackParticleColNameIter)+" Validation").c_str());
        m_trees.push_back(tree); 
        std::string fullNtupleName =  "/"+m_ntupleFileName+"/"+m_ntupleDirName+"/"+(*trackParticleColNameIter);
        sc = tHistSvc->regTree(fullNtupleName, tree);
        if (sc.isFailure()) {
            msg(MSG::ERROR) << "Unable to register TTree : " << fullNtupleName << endreq;
            return sc;
        }
        // add the ntuple branches to this tree
        sc = m_ValTrkParticleNtupleTool->addNtupleItems(tree); 
        if (sc.isFailure()) {
            msg(MSG::ERROR) << "ValidationNtupleTool could not add its branches for tree " << fullNtupleName << endreq;
            return sc;
        }

    } //loop over m_inputTrackParticleCollection

      // create tree for each given track particle collection and register it to THistSvc
    std::vector<std::string>::const_iterator trackParticleEFColNameIter = m_inputEFTrackParticleCollection.begin();
    
    for (; trackParticleEFColNameIter != m_inputEFTrackParticleCollection.end(); trackParticleEFColNameIter++ ) {
        TTree* tree = new TTree((*trackParticleEFColNameIter).c_str(), ((*trackParticleEFColNameIter)+" Validation").c_str());
        m_trees.push_back(tree); 
        std::string fullNtupleName =  "/"+m_ntupleFileName+"/"+m_ntupleDirName+"/"+(*trackParticleEFColNameIter);
        sc = tHistSvc->regTree(fullNtupleName, tree);
        if (sc.isFailure()) {
            msg(MSG::ERROR) << "Unable to register TTree : " << fullNtupleName << endreq;
            return sc;
        }
        // add the ntuple branches to this tree
        sc = m_ValTrkParticleNtupleTool->addNtupleItems(tree); 
        if (sc.isFailure()) {
            msg(MSG::ERROR) << "ValidationNtupleTool could not add its branches for tree " << fullNtupleName << endreq;
            return sc;
        }

    } //loop over m_inputTrackParticleEFCollection

       // create tree for each given track particle collection and register it to THistSvc
    std::vector<std::string>::const_iterator trigInDetTrackColNameIter =  m_inputInDetTrackTrigCollection.begin();
    
    for (; trigInDetTrackColNameIter !=  m_inputInDetTrackTrigCollection.end(); trigInDetTrackColNameIter++ ) {
        TTree* tree = new TTree((*trigInDetTrackColNameIter).c_str(), ((*trigInDetTrackColNameIter)+" Validation").c_str());
        m_trees.push_back(tree); 
        std::string fullNtupleName =  "/"+m_ntupleFileName+"/"+m_ntupleDirName+"/"+(*trigInDetTrackColNameIter);
        sc = tHistSvc->regTree(fullNtupleName, tree);
        if (sc.isFailure()) {
            msg(MSG::ERROR) << "Unable to register TTree : " << fullNtupleName << endreq;
            return sc;
        }
        // add the ntuple branches to this tree
        sc = m_TrigL2ValidationNtupleTool->addNtupleItems(tree); 
        if (sc.isFailure()) {
            msg(MSG::ERROR) << "ValidationNtupleTool could not add its branches for tree " << fullNtupleName << endreq;
            return sc;
        }

    } //loop over  m_inputInDetTrackTrigCollection

    for (unsigned int toolIndex = 0 ; toolIndex < m_eventPropertyNtupleTools.size(); ++toolIndex ) {
      if( m_eventPropertyNtupleTools[toolIndex]->isTrkParticleTrigLinkTool( ) ) 
	           m_eventPropertyNtupleTools[toolIndex]->registerTrackCollections( m_inputEFTrackParticleCollection, false );
      else if(m_eventPropertyNtupleTools[toolIndex]->isTrkParticleLinkTool() ) // register just Trk::TrackParticleBase collection retrieved from SG
	          m_eventPropertyNtupleTools[toolIndex]->registerTrackCollections( m_inputTrackParticleCollection,  false  );
      else if(m_eventPropertyNtupleTools[toolIndex]->isInDetTrackTrigLinkTool() ) // register just Trk::TrackParticleBase collection retrieved from SG
	          m_eventPropertyNtupleTools[toolIndex]->registerTrackCollections( m_inputInDetTrackTrigCollection,  false  );
                                                                                       } // for over m_eventPropertyNtupleTools

    // ---------------------------
    // add event-wise track tree with link to the information in the track-wise tree
      m_eventLinkTree = new TTree("EventToTrkParticleLink", "Event to track entry link");
      std::string fullNtupleName =  "/"+m_ntupleFileName+"/"+m_ntupleDirName+"/EventToTrkParticleLink";
      sc = tHistSvc->regTree(fullNtupleName, m_eventLinkTree);
      if (sc.isFailure()) {
        msg(MSG::ERROR) << "Unable to register TTree : " << fullNtupleName << endreq;
        return sc;
      }

    for (unsigned int toolIndex=0 ; toolIndex < m_eventPropertyNtupleTools.size(); ++toolIndex ) {
      if( !m_eventPropertyNtupleTools[toolIndex]->isTrackLinkTool() ) {
      if( m_eventPropertyNtupleTools[toolIndex]->addNtupleItems( m_eventLinkTree ).isFailure() ) 
	                         {
	                           msg(MSG::ERROR) << "Unable to add items into the event tree: " << m_eventLinkTree->GetTitle() << endreq;
	                           return StatusCode::SUCCESS;
	                         } // if addNtupleItems isFailure
        } // if TrackParticleEF or TrackParticle
    } //loop over m_eventPropertyNtupleTools



  // Retrieve trigger decision tool
    if(m_trigDec.retrieve().isFailure()){
                     msg(MSG::ERROR)
		         <<"Trigger Decision Tool Trig::TriDecisionTool cannot be retrived"
		         <<endreq;
		     return StatusCode::FAILURE;
                      }
    
     
    
    if(m_doRoI) {
    cout<<"PAVEL: DOROI"<<endreq;
    // -------------------------------
    // get roi property tools 
    ToolHandleArray< Trk::IRoIPropertyNtupleTool >::iterator itRoiTools;
    if ( m_roiPropertyNtupleHandles.retrieve().isFailure() ) {
      msg(MSG::ERROR) << "Failed to retreive " << m_roiPropertyNtupleHandles << endreq;
    } else {
      msg(MSG::INFO) << "Retrieved " << m_roiPropertyNtupleHandles << endreq;
    }
    
    itRoiTools = m_roiPropertyNtupleHandles.begin();
    for (  ; itRoiTools != m_roiPropertyNtupleHandles.end(); ++itRoiTools ) {
      // add tool to list
      m_roiPropertyNtupleTools.push_back(&(*(*itRoiTools)));
    }
              } //if do RoI

    
 //--------------------- RoI property initization --------------------------------------------
  if(m_doRoI) { 
    // register collection Trk::TrackCollections and Rec::TrackParticleConatainer in roi link tool
     for (unsigned int toolIndex = 0 ; toolIndex < m_roiPropertyNtupleTools.size(); ++toolIndex ){
       if(m_roiPropertyNtupleTools[toolIndex]->isTrkParticleLinkRoI())
	       m_roiPropertyNtupleTools[toolIndex]->registerTrackCollections(m_inputEFTrackParticleCollection);
       else if(m_roiPropertyNtupleTools[toolIndex]->isTrigInDetTrackLinkRoI())
	       m_roiPropertyNtupleTools[toolIndex]->registerTrackCollections(m_inputInDetTrackTrigCollection);
        } 
    
      // add roi-wise track tree with link to the information in the track-wise tree
        m_roiLinkTree = new TTree("RoiToTrackLink", "Roi to track entry link");
        std::string fullNtupleName =  "/"+m_ntupleFileName+"/"+m_ntupleDirName+"/RoiToTrackLink";
        sc = tHistSvc->regTree(fullNtupleName, m_roiLinkTree);
        if (sc.isFailure()) {
         msg(MSG::ERROR) << "Unable to register TTree : " << fullNtupleName << endreq;
         return sc;
        }
        for (unsigned int toolIndex=0 ; toolIndex < m_roiPropertyNtupleTools.size(); ++toolIndex ) {
          sc = m_roiPropertyNtupleTools[toolIndex]->addNtupleItems( m_roiLinkTree );
          if (sc.isFailure()) return sc;
        }
             } //if do RoI     

 msg(MSG::DEBUG) << "TrigTrkValidationNtupleWriter::initialize() finished " << endreq;
 return StatusCode::SUCCESS;
}


// execute() most of the code had to be copied from TrackValidationNtuple writer because of filling event property trees 
StatusCode Trk::TrigTrkValidationNtupleWriter::execute() {
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)    << "TrackValidationNtupleWriter execute() start" << endreq;

  for (unsigned int toolIndex = 0 ; toolIndex < m_eventPropertyNtupleTools.size(); ++toolIndex ) {
      if(m_eventPropertyNtupleTools[toolIndex]->resetVariables( ).isFailure()) {
	msg(MSG::ERROR) <<"Failure when reset variables in eventPropertyNtupleTool" << endreq;
	return StatusCode::FAILURE;
      } // if failure
  } // loop over event property tools


     // fill track particle data 
    for (unsigned int trackParticleColIndex = 0; trackParticleColIndex<m_inputTrackParticleCollection.size(); trackParticleColIndex++) 
      {
        if (writeTrackParticleData(trackParticleColIndex).isFailure() ) {
            msg(MSG::ERROR) <<"Failure when writing TrackParticle data for collection " << m_inputTrackParticleCollection[trackParticleColIndex] << endreq;
            return StatusCode::FAILURE;
        }
      }    
     

    // fill track particle trigger data  
    for (unsigned int trackParticleEFColIndex = 0; trackParticleEFColIndex<m_inputEFTrackParticleCollection.size(); trackParticleEFColIndex++) 
      {
        if (writeTrackParticleEFData(trackParticleEFColIndex).isFailure()) {
            msg(MSG::ERROR) <<"Failure when writing TrackParticleEF data for collection " << m_inputEFTrackParticleCollection[trackParticleEFColIndex] << endreq;
            return StatusCode::FAILURE;
        }
      } // loop over m_inputEFTrackParticleCollection  

// fill track particle trigger data  
    for (unsigned int inDetTrackTrigColIndex = 0; inDetTrackTrigColIndex<m_inputInDetTrackTrigCollection.size(); inDetTrackTrigColIndex++) 
      {
        if (writeInDetTrackTrigData(inDetTrackTrigColIndex).isFailure()) {
            msg(MSG::ERROR) <<"Failure when writing TrigInDetTrack data for collection " << m_inputEFTrackParticleCollection[inDetTrackTrigColIndex] << endreq;
            return StatusCode::FAILURE;
        }
      } // loop over m_inputInDetTrackTrigCollection  

    
    // fill info about whole event
    
    for (unsigned int toolIndex = 0 ; toolIndex < m_eventPropertyNtupleTools.size(); ++toolIndex ) {
      if (m_eventPropertyNtupleTools[toolIndex]->fillEventData( ).isFailure()) {
        msg(MSG::ERROR) <<"Failure when filling event data." << endreq;
        return StatusCode::FAILURE;
      }
    }
        
    
    m_eventLinkTree->Fill();
    msg(MSG::DEBUG) <<"Event properties were successfully  written"<<endreq;
    
 return StatusCode::SUCCESS;
}

StatusCode Trk::TrigTrkValidationNtupleWriter::writeTrackParticleData(unsigned int trackParticleColIndex ) {
  msg(MSG::DEBUG) << "writeTrackParticleData method started" << endreq;
 
  // retrieve Trk::TrackParticleBaseCollection from the SG
 const Trk::TrackParticleBaseCollection* trackParticles = 0;

 if (m_inputTrackParticleCollection[trackParticleColIndex] != "" && 
     evtStore()->contains<Trk::TrackParticleBaseCollection>(m_inputTrackParticleCollection[trackParticleColIndex])) {
            if (evtStore()->retrieve(trackParticles, m_inputTrackParticleCollection[trackParticleColIndex]).isFailure()) {
                                    msg(MSG::WARNING) <<"Trk::TrackParticleBasesContainer not found:" << m_inputTrackParticleCollection[trackParticleColIndex] << endreq;
                                   return StatusCode::SUCCESS;
               } else {
                       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Trk::TrackParticleBasesContainer found: "<< m_inputTrackParticleCollection[trackParticleColIndex] <<endreq;
                      }
    } else {
        msg(MSG::WARNING) <<"Trk::TrackParticleBasesContainer " << m_inputTrackParticleCollection[trackParticleColIndex] << " not found in StoreGate." << endreq;
        return StatusCode::SUCCESS;
           }

 // indexes for eventPropertyNtupleTool
    int trackTreeIndexBegin = m_trees[trackParticleColIndex]->GetEntries();
    int nTrkParticlesPerEvent = 0;

 // fill coresponding Rec:TrackParticle tree using BasicTrkParticleValidationNtuple 
 Trk::TrackParticleBaseCollection::const_iterator trackParticleIterator = (*trackParticles).begin();
    for ( ; trackParticleIterator < (*trackParticles).end(); ++trackParticleIterator) {
        if (!((*trackParticleIterator))) {
            msg(MSG::WARNING) <<"TrackParticleCollection " << m_inputTrackParticleCollection[trackParticleColIndex] << "contains empty entries" << endreq;
            continue;
	                                 } // if empty entry
 
        if ( m_ValTrkParticleNtupleTool->fillTrackParticleData( *(*trackParticleIterator) ).isFailure() ){
                                        msg(MSG::ERROR) << "Validation Ntuple Tool could not fill track data." << endreq;
                                        return StatusCode::FAILURE;
	                                } // if StatusCode is FAILURE
     nTrkParticlesPerEvent += 1;
     
     if ( m_ValTrkParticleNtupleTool->writeRecord(m_trees[trackParticleColIndex]).isFailure() ){
    	                                msg(MSG::ERROR) << "Validation Ntuple Tool could not write track data." << endreq;
                                        return StatusCode::FAILURE;
        } 
    } // loop over Rec::TrackPartcielContainer
    
    // Record link between Event Property and Trk::TrackParticleBase information
    for (unsigned int toolIndex = 0 ; toolIndex < m_eventPropertyNtupleTools.size(); ++toolIndex ){ 
      if( m_eventPropertyNtupleTools[toolIndex]->isTrkParticleLinkTool() )
                     m_eventPropertyNtupleTools[toolIndex]->setTrackTreeIndices(trackParticleColIndex, trackTreeIndexBegin, nTrkParticlesPerEvent );
    }

    msg(MSG::DEBUG) <<"writeTrackParticleData successfully finished " << endreq;
    return StatusCode::SUCCESS;
}

StatusCode Trk::TrigTrkValidationNtupleWriter::writeTrackParticleEFData(unsigned int trackParticleColIndex) {
  msg(MSG::DEBUG) << "writeTrackParticleEFData method started" << endreq;
  // indexes for EventProperty Tools and RoI tools
  int trackTreeIndexBegin = m_trees[trackParticleColIndex + m_inputTrackParticleCollection.size() ]->GetEntries();
  int nTrkParticlesPerEvent  =  0;
  int trackRoITreeIndexBegin = -1;
  
  bool isChainPassed = false;

  if(m_doRoI) {
    for (unsigned int toolIndex = 0 ; toolIndex < m_roiPropertyNtupleTools.size(); ++toolIndex ){ 
      if (m_roiPropertyNtupleTools[toolIndex]->resetVariables().isFailure()){
	msg(MSG::ERROR) << "roiPropertyNtupleTools could not reset." << endreq;;
	return StatusCode::FAILURE;
      }
    } //loop over RoI Tools
  } // end of if block doRoI
  
  // retrieve feature conateiner for given chain and loop over combination
  if(m_trigDec->isPassed(m_chainName)) {
    msg(MSG::VERBOSE)<<"Trigger chain " <<m_chainName <<" passed."<<endreq;
    isChainPassed = true;
  }
  else msg(MSG::DEBUG)<<" Trigger chain "<< m_chainName <<" not passed."<<endreq;

  Trig::FeatureContainer f = m_trigDec->features(m_chainName);
  Trig::FeatureContainer::combination_const_iterator cIt;
  msg(MSG::DEBUG) << " checking RoIs " << f.getCombinations().size() << endreq;
  
  for ( cIt = f.getCombinations().begin(); cIt != f.getCombinations().end(); ++cIt ) {
    msg(MSG::VERBOSE) << *cIt << endreq; // print combination
    
    
    std::vector<Trig::Feature<Rec::TrackParticleContainer> > trkParticleFeature;
    
    if (m_inputEFTrackParticleCollection[trackParticleColIndex] == "TrackParticleEF") { 
      msg(MSG::DEBUG)   << "All Rec::TrackParticleContainers are retrived as features" <<endreq;
      trkParticleFeature = cIt->get<Rec::TrackParticleContainer>();
      msg(MSG::DEBUG)   << "Received number of features containers: " << trkParticleFeature.size() <<endreq;
    } else {
      trkParticleFeature = cIt->get<Rec::TrackParticleContainer>(m_inputEFTrackParticleCollection[trackParticleColIndex]);
      msg(MSG::DEBUG)   << "Received number of features containers: "  << trkParticleFeature.size() <<endreq;
    } // else
    
    
    std::vector<Trig::Feature<Rec::TrackParticleContainer> >::const_iterator trkFeatureI;
    for(trkFeatureI = trkParticleFeature.begin(); trkFeatureI != trkParticleFeature.end(); trkFeatureI ++) {
      if(m_doRoI) {
	trackRoITreeIndexBegin=m_trees[trackParticleColIndex + m_inputTrackParticleCollection.size()]->GetEntries();
	
	Trig::Feature<TrigRoiDescriptor> roiDescriptor =  m_trigDec->ancestor<TrigRoiDescriptor>((*trkFeatureI), m_roiName);
	if ( roiDescriptor.empty() )
	  msg(MSG::DEBUG) << "Received TrigRoiDescriptor "<< m_roiName  <<" feature is empty" << endreq;
	else {
	  for (unsigned int toolIndex = 0 ; toolIndex < m_roiPropertyNtupleTools.size(); ++toolIndex ){ 
	    if ( m_roiPropertyNtupleTools[toolIndex]->fillRoIData( roiDescriptor.cptr(), isChainPassed ).isFailure())
	      {
		msg(MSG::ERROR) << "Validation Ntuple Tool could not fill RoI info." << endreq;
		return StatusCode::FAILURE;
	      }
	  }
	} // else roiDescriptor is not empty
      }// if do RoI
      
      msg(MSG::DEBUG) << "Number of  Rec::TrackParticles in a given feature container: "<< trkFeatureI->cptr()->size() <<endreq;
      for(Rec::TrackParticleContainer::const_iterator trkParticleI = trkFeatureI->cptr()->begin(); trkParticleI != trkFeatureI->cptr()->end(); trkParticleI++) {
	
	if ( m_ValTrkParticleNtupleTool->fillTrackParticleData( (*(*trkParticleI))).isFailure()){
	  msg(MSG::ERROR) << "Validation Ntuple Tool could not fill track data." << endreq;
	  return StatusCode::FAILURE;
	}
	
	if ( m_ValTrkParticleNtupleTool->writeRecord(m_trees[trackParticleColIndex + m_inputTrackParticleCollection.size()]).isFailure() ){
	  msg(MSG::ERROR) << "Validation Ntuple Tool could not write track data into " 
			  << m_trees[trackParticleColIndex + + m_inputTrackParticleCollection.size() ]->GetTitle() 
			  << endreq;
	  return StatusCode::FAILURE;
	};  
	
	
	nTrkParticlesPerEvent++;
      } // loop over track particle container
      if(m_doRoI) {
	for (unsigned int toolIndex = 0 ; toolIndex < m_roiPropertyNtupleTools.size(); ++toolIndex ){ 
	  if(m_roiPropertyNtupleTools[toolIndex]->isTrkParticleLinkRoI())
	    m_roiPropertyNtupleTools[toolIndex]->setTrackTreeIndices(trackParticleColIndex, trackRoITreeIndexBegin, trkFeatureI->cptr()->size() );
	} //loop over RoI Tools
	m_roiLinkTree->Fill();
      }// if do RoI
    } // loop over track particle features
    
 }// loop over feature combinations
 
 // Record link between Event Property and Rec::TrackParticle information
 for (unsigned int toolIndex = 0 ; toolIndex < m_eventPropertyNtupleTools.size(); ++toolIndex ){ 
   if( m_eventPropertyNtupleTools[toolIndex]->isTrkParticleTrigLinkTool() )
     m_eventPropertyNtupleTools[toolIndex]->setTrackTreeIndices(trackParticleColIndex, trackTreeIndexBegin, nTrkParticlesPerEvent );
 }
 
 msg(MSG::DEBUG) <<"writeTrackParticleEFData successfully finished " << endreq;
 return StatusCode::SUCCESS;
}


StatusCode Trk::TrigTrkValidationNtupleWriter::writeInDetTrackTrigData(unsigned int inDetTrackColIndex) {
 msg(MSG::DEBUG) << "writeInetTrackTrigData method started" << endreq;

if(m_doRoI) {
 for (unsigned int toolIndex = 0 ; toolIndex < m_roiPropertyNtupleTools.size(); ++toolIndex ){ 
   if (m_roiPropertyNtupleTools[toolIndex]->resetVariables().isFailure()){
     msg(MSG::ERROR) << "roiPropertyNtupleTools could not reset." << endreq;;
     return StatusCode::FAILURE;
   }
	                                                                                     } //loop over RoI Tools
 } // end of if block doRoI

// indexes for EventProperty Tools and RoI tools
 int trackTreeIndexBegin = m_trees[inDetTrackColIndex + m_inputTrackParticleCollection.size() + m_inputEFTrackParticleCollection.size()]->GetEntries();
 int nTrkParticlesPerEvent  =  0;
 int trackRoITreeIndexBegin = -1;

  bool isChainPassed = false;

 // retrieve feature conateiner for given chain and loop over combination
 Trig::FeatureContainer f = m_trigDec->features(m_chainName);
 Trig::FeatureContainer::combination_const_iterator cIt;
  msg(MSG::VERBOSE) << " checking RoIs " << f.getCombinations().size() << endreq;

  if(m_trigDec->isPassed(m_chainName)) {
      msg(MSG::DEBUG)<< m_chainName <<" PASSED"<<endreq;
      isChainPassed = true;
  }
  else msg(MSG::DEBUG)<< m_chainName <<" NOT PASSED"<<endreq;

 for ( cIt = f.getCombinations().begin(); cIt != f.getCombinations().end(); ++cIt ) {
    msg(MSG::VERBOSE) << *cIt << endreq; // print combination


    std::vector<Trig::Feature<TrigInDetTrackCollection> > inDetTrackFeature;

     if (m_inputInDetTrackTrigCollection[inDetTrackColIndex] == "InDetTrack") { 
                msg(MSG::DEBUG)   << "All TrigInDetTrackCollection are retrived as features" <<endreq;
                inDetTrackFeature = cIt->get<TrigInDetTrackCollection>();
                msg(MSG::DEBUG)   << "Received number of features containers: " << inDetTrackFeature.size() <<endreq;
     } else {
       inDetTrackFeature = cIt->get<TrigInDetTrackCollection>(m_inputInDetTrackTrigCollection[inDetTrackColIndex]);
                msg(MSG::DEBUG)   << "Received number of features containers: "  << inDetTrackFeature.size() <<endreq;
     } // else
     

    std::vector<Trig::Feature<TrigInDetTrackCollection> >::const_iterator trkFeatureI;
	    for(trkFeatureI = inDetTrackFeature.begin(); trkFeatureI != inDetTrackFeature.end(); trkFeatureI ++) {
	      if(m_doRoI) {
	      trackRoITreeIndexBegin=m_trees[inDetTrackColIndex + m_inputTrackParticleCollection.size() +m_inputEFTrackParticleCollection.size()]->GetEntries();

	      Trig::Feature<TrigRoiDescriptor> roiDescriptor =  m_trigDec->ancestor<TrigRoiDescriptor>((*trkFeatureI), m_roiName);
	      if ( roiDescriptor.empty() ) msg(MSG::DEBUG) << "Received TrigRoiDescriptor "<< m_roiName  <<" feature is empty" << endreq;
	      else {
	      for (unsigned int toolIndex = 0 ; toolIndex < m_roiPropertyNtupleTools.size(); ++toolIndex ){ 
		if ( m_roiPropertyNtupleTools[toolIndex]->fillRoIData( roiDescriptor.cptr(),  isChainPassed ).isFailure())
											                             {
										       msg(MSG::ERROR) << "Validation Ntuple Tool could not fill RoI info." << endreq;
                                                                                       return StatusCode::FAILURE;
											                             }
	                                                                                                     }
	           } // else roiDescriptor is not empty
	                  }// if do RoI
	      
	      msg(MSG::DEBUG) << "Number of  TrigInDetTrackCollection in a given feature container: "<< trkFeatureI->cptr()->size() <<endreq;
	      for(TrigInDetTrackCollection::const_iterator inDetTrack = trkFeatureI->cptr()->begin(); inDetTrack != trkFeatureI->cptr()->end(); inDetTrack++) {
               
		if (m_TrigL2ValidationNtupleTool->fillTrackData( (*(*inDetTrack)) ).isFailure() ){
	                                                                               msg(MSG::ERROR) << "Validation Ntuple Tool could not fill track data." << endreq;
                                                                                       return StatusCode::FAILURE;
	                                                                                          }
 
             if ( m_TrigL2ValidationNtupleTool->writeRecord(m_trees[inDetTrackColIndex + m_inputTrackParticleCollection.size() + m_inputEFTrackParticleCollection.size()]).isFailure() ){
	                                                                msg(MSG::ERROR) << "Validation Ntuple Tool could not write track data into " 
			                                                << m_trees[inDetTrackColIndex + + m_inputTrackParticleCollection.size() ]->GetTitle() 
			                                                << endreq;
                                                                        return StatusCode::FAILURE;
	                                                                                             };  

	     
	      nTrkParticlesPerEvent++;
	      } // loop over track particle container
	      if(m_doRoI) {
              	    for (unsigned int toolIndex = 0 ; toolIndex < m_roiPropertyNtupleTools.size(); ++toolIndex ){ 
		      if(m_roiPropertyNtupleTools[toolIndex]->isTrigInDetTrackLinkRoI())
			m_roiPropertyNtupleTools[toolIndex]->setTrackTreeIndices(inDetTrackColIndex, trackRoITreeIndexBegin, trkFeatureI->cptr()->size() );
		    } //loop over RoI Tools
		    m_roiLinkTree->Fill();
		  }// if do RoI
	    } // loop over track particle features
    
 }// loop over feature combinations

    // Record link between Event Property and Rec::TrackParticle information
    for (unsigned int toolIndex = 0 ; toolIndex < m_eventPropertyNtupleTools.size(); ++toolIndex ){ 
      if( m_eventPropertyNtupleTools[toolIndex]->isInDetTrackTrigLinkTool() )
                     m_eventPropertyNtupleTools[toolIndex]->setTrackTreeIndices(inDetTrackColIndex, trackTreeIndexBegin, nTrkParticlesPerEvent );
    }

  msg(MSG::DEBUG) <<"writeInDetTrackTrigData successfully finished" << endreq;
  return StatusCode::SUCCESS;
}



StatusCode Trk::TrigTrkValidationNtupleWriter::finalize() {

  msg(MSG::INFO)  << "TrackValidationNtupleWriter finalize()" << endreq;

    for (unsigned int toolIndex = 0 ; toolIndex < m_eventPropertyNtupleTools.size(); ++toolIndex )
      if (m_eventPropertyNtupleTools[toolIndex]->resetVariables( ).isFailure()){
	msg(MSG::ERROR) << "eventPropertyNtupleTools could not reset." << endreq;;
	return StatusCode::FAILURE;
      }
   
    return StatusCode::SUCCESS;
}
