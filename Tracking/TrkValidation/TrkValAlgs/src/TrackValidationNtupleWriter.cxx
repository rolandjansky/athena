/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackValidationNtupleWriter.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

#include "TrkValAlgs/TrackValidationNtupleWriter.h"
// Gaudi
#include "GaudiKernel/ITHistSvc.h"

#include "TTree.h"

// Trk
#include "TrkTrack/Track.h"

#include "AtlasHepMC/GenParticle.h"
#include "TrkValEvent/GenParticleJet.h"
#include "TrkToolInterfaces/ITruthToTrack.h"

//#include "TrkParameters/Perigee.h"

#include "GeneratorObjects/McEventCollection.h"

#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "VxVertex/VxContainer.h"

// inventory of tools
#include "TrkValInterfaces/ITrackValidationNtupleTool.h"
#include "TrkValInterfaces/IValidationNtupleTool.h"
#include "TrkValInterfaces/IGenParticleSelector.h"
#include "TrkValInterfaces/IJetTruthNtupleTool.h"
#include "TrkValInterfaces/IGenParticleJetFinder.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "TrkValInterfaces/ITrackTruthClassifier.h"
#include "TrkValInterfaces/ITruthNtupleTool.h"
#include "TrkValInterfaces/IEventPropertyNtupleTool.h"

#include "CLHEP/Vector/LorentzVector.h"
using CLHEP::HepLorentzVector;

Trk::TrackValidationNtupleWriter::TrackValidationNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
    m_ValidationNtupleTools(),
    m_ValTrkParticleNtupleTool("Trk::BasicValTrkParticleNtupleTool/BasicValTrkParticleNtupleTool"),
    m_truthNtupleTool("Trk::TruthNtupleTool/TruthNtupleTool"),
    m_jetTruthNtupleTool(""),
    m_trackTruthClassifierHandles(),
    m_trackTruthClassifiers(0),
    m_eventPropertyNtupleHandles(),
    m_eventPropertyNtupleTools(0),
    m_inputTrackCollection(0),
    m_trackTruthCollectionName(0),
    m_McEventCollectionName("TruthEvent"),
    m_inputPrimaryVertexCollection(""),
    m_ntupleFileName("TRKVAL"),
    m_ntupleDirName("Validation"),
    m_truthToTrack("Trk::TruthToTrack/TruthToTrack"),
    m_trackSelector("InDet::InDetTrackSelectorTool/InDetTrackSelectorTool"),
    m_genPartSelector("Trk::InDetReconstructableSelector/InDetReconstructableSelector"),
    m_genJetFinder("Trk::GenParticleJetFinder/GenParticleJetFinder"),
    m_useExternalEventLinkTree(false),
    m_doTruth(true),
    m_doTrackParticle(false),
    m_visibleParticleWithoutTruth(0),
    m_nTrackTreeRecords(0),
    m_trees(0),
    m_eventLinkTree(0)
    
{
    m_ValidationNtupleTools.push_back("Trk::TrackInformationNtupleTool/TrackInformationNtupleTool");
    m_ValidationNtupleTools.push_back("Trk::PerigeeParametersNtupleTool/PerigeeParametersNtupleTool");
     m_ValidationNtupleTools.push_back("Trk::MeasurementVectorNtupleTool/MeasurementVectorNtupleTool");
    m_trackTruthClassifierHandles.push_back("Trk::PrimaryTruthClassifier/PrimaryTruthClassifier");
    
    m_eventPropertyNtupleHandles.push_back("Trk::EventPropertyNtupleTool/EventPropertyNtupleTool");
    m_eventPropertyNtupleHandles.push_back("Trk::EventToTrackLinkNtupleTool/EventToTrackLinkNtupleTool");

    m_inputTrackCollection.push_back("Tracks");
    m_trackTruthCollectionName.push_back("TrackTruthCollection");

    declareProperty("TrackCollection",      m_inputTrackCollection,     "Names of the track collections");
    declareProperty("TrackParticleCollection", m_inputTrackParticleCollection, "Names of the track particle collections");
    declareProperty("TrackTruthCollection", m_trackTruthCollectionName, "Names of the truth track collections (if DoTruth=True)");
    declareProperty("TruthCollection",      m_McEventCollectionName,    "Name of the truth (gen event) collection (if DoTruth=True)");
    declareProperty("PrimaryVertexCollection",      m_inputPrimaryVertexCollection,             "SG key of input PrimaryVertex collection (needed for TrackSelector, if no vertex collection is given, the selector will not use the vertex position)");
    declareProperty("NtupleFileName",       m_ntupleFileName,           "Ntuple file handle");
    declareProperty("NtupleDirectoryName",  m_ntupleDirName,            "directory name for ntuple tree");
    //declareProperty("NtupleTreeName",       m_ntupleTreeNames,          "Name of the ntuple trees (list corresponding to the list of input track collections)");
    declareProperty("UseExternalEventLinkTree", m_useExternalEventLinkTree, "flag if event link tree is supplied externally, eg as part of CBNT");
    declareProperty("DoTruth",              m_doTruth,                  "Write truth data?");
    declareProperty("ValidationNtupleTools", m_ValidationNtupleTools,   "Tool to write the validation ntuple");
    declareProperty("ValTrkParticleNtupleTool", m_ValTrkParticleNtupleTool, "Tool to write the validation ntuple for Track Particles");
    declareProperty("TruthNtupleTool",      m_truthNtupleTool,          "Tool to write the truth ntuple");
    declareProperty("JetNtupleTool",        m_jetTruthNtupleTool,       "Tool to add jet information to ntuple");
    declareProperty("TruthToTrackTool",     m_truthToTrack,             "Tool for truth to track");
    declareProperty("TrackSelectorTool",    m_trackSelector,            "Tool for the selection of tracks");
    declareProperty("GenParticleSelectorTool",  m_genPartSelector,      "Tool for the selection of truth particles");
    declareProperty("GenParticleJetFinder", m_genJetFinder,             "Tool for jet clustering (MC)");
    declareProperty("TruthClassifierTools", m_trackTruthClassifierHandles,  "List of truth classifier tools");
    declareProperty("EventPropertyNtupleTools", m_eventPropertyNtupleHandles,"List of event property ntuple tools");
    declareProperty("doTrackParticle", m_doTrackParticle, "Swith to record Rec:TrackParticle Trees");
}

Trk::TrackValidationNtupleWriter::~TrackValidationNtupleWriter() {}

StatusCode Trk::TrackValidationNtupleWriter::initialize() {

    msg(MSG::INFO) <<"TrackValidationNtupleWriter initialize()" << endmsg;

    //check that m_inputTrackCollection and m_trackTruthCollectionName have the same size
    if (m_doTruth && (m_inputTrackCollection.size() != m_trackTruthCollectionName.size())) {
        msg(MSG::FATAL) << "joboptions TrackCollection and TrackTruthCollection have different sizes!" << endmsg;
        msg(MSG::FATAL) << "If you like to get truth data (DoTruth=True)," << endmsg;
        msg(MSG::FATAL) << "  please make sure to set for each TrackCollection the corresponding TrackTruthCollection" << endmsg;
        return StatusCode::FAILURE;
    }

    // Get Validation ntuple Tools
    StatusCode sc = m_ValidationNtupleTools.retrieve();
    if (sc.isFailure()) {
        msg(MSG::FATAL) << "Could not retrieve "<< m_ValidationNtupleTools <<" (to write validation ntuple) "<< endmsg;
        return sc;
    }
    if(m_doTrackParticle){
    // Get TrackParticle Validation ntuple Tool
    sc = m_ValTrkParticleNtupleTool.retrieve();
    if (sc.isFailure()) {
        msg(MSG::FATAL) << "Could not retrieve "<< m_ValTrkParticleNtupleTool <<" (to write TrackParticle validation ntuple) "<< endmsg;
        return sc;
     }
    }

    // Get the Track Selector Tool
    if ( !m_trackSelector.empty() ) {
      sc = m_trackSelector.retrieve();
      if (sc.isFailure()) {
        msg(MSG::FATAL) << "Could not retrieve "<< m_trackSelector <<" (to select the tracks which are written to the ntuple) "<< endmsg;
        msg(MSG::INFO) << "Set the ToolHandle to None if track selection is supposed to be disabled" << endmsg;
        return sc;
      }
    }

    msg(MSG::INFO) <<"Track Selector retrieved" << endmsg;
    // -------------------------------
    // get event property tools
    ToolHandleArray< Trk::IEventPropertyNtupleTool >::iterator itTools;
    if ( m_eventPropertyNtupleHandles.retrieve().isFailure() ) {
      msg(MSG::ERROR) << "Failed to retreive " << m_eventPropertyNtupleHandles << endmsg;
    } else {
      msg(MSG::INFO) << "Retrieved " << m_eventPropertyNtupleHandles << endmsg;
    }

     
    //std::vector<std::string> classifierNames(0);
    itTools = m_eventPropertyNtupleHandles.begin();
    for (  ; itTools != m_eventPropertyNtupleHandles.end(); ++itTools ) {
      // add tool to list
      m_eventPropertyNtupleTools.push_back(&(*(*itTools)));
    }

    // Retrieve the TruthToTrack tool if truth is required
    if ( m_doTruth ) {
        sc = m_truthToTrack.retrieve();
        if (sc.isFailure()) {
          ATH_MSG_FATAL ("Could not retrieve "<< m_truthToTrack);
          return sc;
        }

        // retrieve truth selector for efficiency denominator
        sc = m_genPartSelector.retrieve();
        if (sc.isFailure()) {
          ATH_MSG_FATAL ("Could not retrieve "<< m_genPartSelector);
          return sc;
        }

        // -------------------------------
        // get given classifier tools
        ToolHandleArray< ITrackTruthClassifier >::iterator itTools;
        if ( m_trackTruthClassifierHandles.retrieve().isFailure() ) {
            msg(MSG::ERROR) << "Failed to retreive " << m_trackTruthClassifierHandles << endmsg;
        } else {
            msg(MSG::INFO) << "Retrieved " << m_trackTruthClassifierHandles << endmsg;
        }
        //std::vector<std::string> classifierNames(0);
        itTools = m_trackTruthClassifierHandles.begin();
        for (  ; itTools != m_trackTruthClassifierHandles.end(); ++itTools ) {
            // add tool to list
            m_trackTruthClassifiers.push_back(&(*(*itTools)));
            //classifierNames.push_back((*itTools)->nameOfClassifier());
        }

        // Get the Jet-Truth Ntuple Tool
        if (! m_jetTruthNtupleTool.empty()) {
          sc = m_jetTruthNtupleTool.retrieve();
          if (sc.isFailure()) {
            msg(MSG::ERROR) << "Could not retrieve "<< m_jetTruthNtupleTool
                  << " (to write jet data)."<< endmsg << "--> Instead configure "
                  << "to empty string if jet data not desired." << endmsg;
            return sc;
          }
          sc = m_genJetFinder.retrieve();
          if (sc.isFailure()) {
            msg(MSG::ERROR) << "Could not retrieve "<< m_genJetFinder
                  << " (to find jets at truth level)."<< endmsg;
            return sc;
          }
        }

        // Get the Truth Ntuple Tool
        sc = m_truthNtupleTool.retrieve();
        if (sc.isFailure()) {
            msg(MSG::FATAL) << "Could not retrieve "<< m_truthNtupleTool <<" (to write truth data) "<< endmsg;
            return sc;
        }
        bool include_jets = (! m_jetTruthNtupleTool.empty());
        sc = m_truthNtupleTool->initBranches(m_trackTruthClassifiers, include_jets, m_inputTrackCollection);
        if (sc.isFailure()) return sc;

        m_visibleParticleWithoutTruth = new HepMC::GenParticle(HepLorentzVector(), 0);

    } // if truth is activated

    // ---------------------------
    // retrive pointer to THistSvc
    ITHistSvc *tHistSvc;
    sc =  service("THistSvc", tHistSvc);
    if (sc.isFailure()) {
        msg(MSG::ERROR) << "Unable to retrieve pointer to THistSvc" << endmsg;
        return sc;
    }
    // ---------------------------
    // create tree for each given track collection and register it to THistSvc
    std::vector<std::string>::const_iterator trackColNameIter = m_inputTrackCollection.begin();
    //for (unsigned int trackColIndex = 0; trackColIndex<m_inputTrackCollection.size(); trackColIndex++) {
    for (; trackColNameIter != m_inputTrackCollection.end(); trackColNameIter++ ) {
        TTree* tree = new TTree((*trackColNameIter).c_str(), ((*trackColNameIter)+" Validation").c_str());
        m_trees.push_back(tree);
        std::string fullNtupleName =  "/"+m_ntupleFileName+"/"+m_ntupleDirName+"/"+(*trackColNameIter);
        sc = tHistSvc->regTree(fullNtupleName, tree);
        if (sc.isFailure()) {
            msg(MSG::ERROR) << "Unable to register TTree : " << fullNtupleName << endmsg;
            return sc;
        }
        // add the ntuple branches to this tree
        ToolHandleArray< Trk::ITrackValidationNtupleTool >::iterator itTools;
        itTools = m_ValidationNtupleTools.begin();
        for (  ; itTools != m_ValidationNtupleTools.end(); ++itTools ) {
          if (((*itTools)->addNtupleItems(tree)).isFailure()) {
            msg(MSG::ERROR) << "ValidationNtupleTool could not add its branches"
                            << " for tree " << fullNtupleName << endmsg;
            return StatusCode::FAILURE;
          }
        }
	// initialize tree offsets to 0:
	m_nTrackTreeRecords.push_back( 0 );
    }


    // create tree for each given track particle collection and register it to THistSvc
    std::vector<std::string>::const_iterator trackParticleColNameIter = m_inputTrackParticleCollection.begin();
    
    for (; trackParticleColNameIter != m_inputTrackParticleCollection.end(); trackParticleColNameIter++ ) {
        TTree* tree = new TTree((*trackParticleColNameIter).c_str(), ((*trackParticleColNameIter)+" Validation").c_str());
        m_trees.push_back(tree); 
        std::string fullNtupleName =  "/"+m_ntupleFileName+"/"+m_ntupleDirName+"/"+(*trackParticleColNameIter);
        sc = tHistSvc->regTree(fullNtupleName, tree);
        if (sc.isFailure()) {
            msg(MSG::ERROR) << "Unable to register TTree : " << fullNtupleName << endmsg;
            return sc;
        }
        // add the ntuple branches to this tree
        sc = m_ValTrkParticleNtupleTool->addNtupleItems(tree); 
        if (sc.isFailure()) {
            msg(MSG::ERROR) << "ValidationNtupleTool could not add its branches for tree " << fullNtupleName << endmsg;
            return sc;
        }

    } //loop over m_inputTrackParticleCollection

    for (unsigned int toolIndex = 0 ; toolIndex < m_eventPropertyNtupleTools.size(); ++toolIndex ) {
      if( m_eventPropertyNtupleTools[toolIndex]->isTrackLinkTool( ) ) {
	           m_eventPropertyNtupleTools[toolIndex]->registerTrackCollections( m_inputTrackCollection,
										    m_doTruth );
		                                                                      } //if
      else if(m_eventPropertyNtupleTools[toolIndex]->isTrkParticleLinkTool() ) // register just Trk::TrackParticleBase collection retrieved from SG
	          m_eventPropertyNtupleTools[toolIndex]->registerTrackCollections( m_inputTrackParticleCollection,  m_doTruth  );
                                                                                       } // for over m_eventPropertyNtupleTools

    // ---------------------------
    // add event-wise track tree with link to the information in the track-wise tree
    if (!m_useExternalEventLinkTree) {
      m_eventLinkTree = new TTree("EventToTrackLink", "Event to track entry link");
      std::string fullNtupleName =  "/"+m_ntupleFileName+"/"+m_ntupleDirName+"/EventToTrackLink";
      sc = tHistSvc->regTree(fullNtupleName, m_eventLinkTree);
      if (sc.isFailure()) {
        msg(MSG::ERROR) << "Unable to register TTree : " << fullNtupleName << endmsg;
        return sc;
      }

    for (unsigned int toolIndex=0 ; toolIndex < m_eventPropertyNtupleTools.size(); ++toolIndex ) {
      if( m_eventPropertyNtupleTools[toolIndex]->isTrkParticleLinkTool() ||
    	  m_eventPropertyNtupleTools[toolIndex]->isTrackLinkTool( ) || 
    	  m_eventPropertyNtupleTools[toolIndex]->isEvtPropertyTool( ) ) {
      if( m_eventPropertyNtupleTools[toolIndex]->addNtupleItems( m_eventLinkTree ).isFailure() ) 
	                         {
	                           msg(MSG::ERROR) << "Unable to add items into the event tree: " << m_eventLinkTree->GetTitle() << endmsg;
	                           return StatusCode::SUCCESS;
	                         } // if addNtupleItems isFailure
        } // if Track or TrackParticle
     } //loop over m_eventPropertyNtupleTools
    } // if not use external link tree
    return StatusCode::SUCCESS;
}



StatusCode Trk::TrackValidationNtupleWriter::execute() {

//    std::cout<< "TrackValidationNtupleWriter execute() start" <<std::endl;
    
    ATH_MSG_DEBUG ("TrackValidationNtupleWriter execute() start");

    StatusCode sc = StatusCode::SUCCESS; if (sc.isFailure()) { /* mute sc*/ }
    for (unsigned int toolIndex = 0 ; toolIndex < m_eventPropertyNtupleTools.size(); ++toolIndex ) {
      if (m_eventPropertyNtupleTools[toolIndex]->resetVariables( ).isFailure()){};
    }

    const McEventCollection*                 mcEventColl = 0;

    unsigned int nTruthTreeRecordsAtCurrentEvent = 0;
    std::vector<Trk::ValidationTrackTruthData>  truthData;
    std::vector<const HepMC::GenParticle*>*  selecParticles = 0;
    //std::vector<const Trk::TrackParameters*> extrapolatedTruthPerigees;
    //std::vector<std::vector<unsigned int> >  classifications;
    std::vector< Trk::GenParticleJet >*      genParticleJets = 0;
    //std::map<HepMC::GenParticle*, unsigned int>  particleToIndexMap;
    if (m_doTruth) 
    {
        // retrieve gen event collection
        if (evtStore()->retrieve(mcEventColl, m_McEventCollectionName).isFailure())
	{
            std::string key = "G4Truth";
            if (evtStore()->retrieve(mcEventColl, key).isFailure())
	    {
                msg(MSG::WARNING) << "Could not find the McEventCollection" << endmsg;
                return StatusCode::SUCCESS;
            }
        }
        
//        std::cout<<"collecton retrieved "<<std::endl;
    
        // select stable, charged particles
        selecParticles = m_genPartSelector->selectGenSignal(mcEventColl);
        if (selecParticles) 
	{
//	    std::cout<<"there are selected particles "<<std::endl;
	    	    
            // init the classification:
            //classifications.resize(selecParticles->size());
            //truthData.classifications.resize(selecParticles->size());
            //truthData.truthPerigee.resize(selecParticles->size());
            for (unsigned int toolIndex = 0 ; toolIndex < m_trackTruthClassifiers.size(); ++toolIndex ) 
	    {
              m_trackTruthClassifiers[toolIndex]->initClassification(*mcEventColl, selecParticles);
            }
	    
//	    std::cout<<"First loop done "<<std::endl;
	    
            //unsigned int partIndex = 0;
            std::vector< const HepMC::GenParticle* >::const_iterator partIter = selecParticles->begin();
            for ( ; partIter != selecParticles->end(); ++partIter) 
              {
                const HepMC::GenParticle* genParticle = *partIter;
                //truthData.genParticle = (*selecParticles);
                Trk::ValidationTrackTruthData partData;
                partData.genParticle = genParticle;
                // Perform extrapolation to generate perigee parameters
                const Trk::TrackParameters* generatedTrackPerigee(0);
                if ( genParticle->production_vertex() )
                  {
                    generatedTrackPerigee = m_truthToTrack->makePerigeeParameters( genParticle );
                    if (generatedTrackPerigee == NULL && genParticle->barcode() > 1000000 ) {
                      ATH_MSG_DEBUG ("No perigee available for interacting truth particle."
                                     <<" -> This is OK for particles from the TrackRecord, but probably"
                                     <<" a bug for production vertex particles.");
                    }

                }
                partData.truthPerigee = generatedTrackPerigee;
                // get classification
                partData.classifications.reserve(m_trackTruthClassifiers.size());
                for (unsigned int toolIndex = 0 ; toolIndex < m_trackTruthClassifiers.size(); ++toolIndex ) 
                  {
                    partData.classifications.push_back(m_trackTruthClassifiers[toolIndex]->classify(*genParticle));
                  }
                // resize the truth to track vectors to the number of input track collections:
                partData.truthToTrackIndices.resize(m_inputTrackCollection.size());
                partData.truthToTrackMatchingProbabilities.resize(m_inputTrackCollection.size());
                truthData.push_back(partData);
                //partIndex++;
              }
   
//            std::cout<<"Second loop done "<<std::endl;
            
            if (! m_jetTruthNtupleTool.empty()) {

//  	      std::cout<<"Second loop done "<<std::endl;
              genParticleJets = m_genJetFinder->jetMCFinder(*selecParticles);
              if (!genParticleJets) ATH_MSG_DEBUG ("no jets found!");
              else ATH_MSG_DEBUG ("jets found: " << genParticleJets->size());
            }
//	    std::cout<<" end of jet truth tool check"<<std::endl;
            
        } // end if (selecParticles)
        nTruthTreeRecordsAtCurrentEvent = m_truthNtupleTool->getNumberOfTreeRecords();
    } // end if(m_doTruth)

//    std::cout<<"Finished working with truth part"<<std::endl;
   
    // get vertex collection so that the track selector is able to make a correct d0, z0 cut
    const VxContainer* primaryVertexContainer = 0;
    if (!m_trackSelector.empty() && m_inputPrimaryVertexCollection != "") {
        sc = evtStore()->retrieve(primaryVertexContainer, m_inputPrimaryVertexCollection);
        if ( !primaryVertexContainer || sc.isFailure() ) {
            ATH_MSG_ERROR( " Primary Vertex container (" << m_inputPrimaryVertexCollection << ") not found in StoreGate" );
            delete genParticleJets;
            return StatusCode::FAILURE;
        }
    }
    const Trk::Vertex* vertex = 0;
    if (primaryVertexContainer) {
        if (primaryVertexContainer->size() > 0) vertex = &((*primaryVertexContainer)[0]->recVertex());
    }
    
    // fill the track trees for each track collection
    for (unsigned int trackColIndex = 0; trackColIndex<m_inputTrackCollection.size(); trackColIndex++) 
    {
        sc = writeTrackData(trackColIndex, truthData, vertex);
        if (sc.isFailure()) {
            ATH_MSG_ERROR ("Failure when writing track data for collection " << m_inputTrackCollection[trackColIndex]);
            delete genParticleJets;
            return sc;
        }
    }

     // fill track particle data 
    if(m_doTrackParticle){
    for (unsigned int trackParticleColIndex = 0; trackParticleColIndex<m_inputTrackParticleCollection.size(); trackParticleColIndex++) 
      {
        sc = writeTrackParticleData(trackParticleColIndex);
        if (sc.isFailure()) {
            msg(MSG::ERROR) <<"Failure when writing TrackParticle data for collection " << m_inputTrackParticleCollection[trackParticleColIndex] << endmsg;
            delete genParticleJets;
            return sc;
        }
    }    
}
    if (m_doTruth) {
      for (unsigned int toolIndex = 0 ; toolIndex < m_eventPropertyNtupleTools.size(); ++toolIndex )
        if( m_eventPropertyNtupleTools[toolIndex]->isTrackLinkTool() ) {
          m_eventPropertyNtupleTools[toolIndex]->setGenParticleTreeIndices
            (nTruthTreeRecordsAtCurrentEvent,
             selecParticles ? (int)selecParticles->size() : 0);
        }
    }

    // fill info about whole event
    for (unsigned int toolIndex = 0 ; toolIndex < m_eventPropertyNtupleTools.size(); ++toolIndex ) {
      sc = m_eventPropertyNtupleTools[toolIndex]->fillEventData( );
      if (sc.isFailure()) {
        msg(MSG::ERROR) <<"Failure when filling event data." << endmsg;
        delete genParticleJets;
        return sc;
      }
    }
    
    if (!m_useExternalEventLinkTree) m_eventLinkTree->Fill();

    // fill info about jets
    std::vector<unsigned int> truthToJetIndices(selecParticles ? selecParticles->size() : 0);
    if (m_doTruth && selecParticles && genParticleJets && !m_jetTruthNtupleTool.empty() ) {

      const unsigned int nJetTruthTreeRecordsAtCurrentEvent 
        = m_jetTruthNtupleTool->getNumberOfTreeRecords();

      for( std::vector<Trk::GenParticleJet>::iterator itrMcJet = genParticleJets->begin();
           itrMcJet < genParticleJets->end() ; ++itrMcJet) {

        std::vector<int> indices = (*itrMcJet).getIndicesInEvent();
        if (indices.size()!=0)
		  for (std::vector<int>::const_iterator k =indices.begin(); k != indices.end(); ++k) {
            if (*k >= 0 && *k <= (int)selecParticles->size() ) {
              truthData[*k].truthToJetIndex = nJetTruthTreeRecordsAtCurrentEvent
                + int(itrMcJet - genParticleJets->begin()) + 1;
            } else {
              msg(MSG::WARNING) <<" mistake with jet::indexInEvent !! " << *k << endmsg;
            }
          }
      }
      sc = m_jetTruthNtupleTool->writeJetTruthData(*genParticleJets,
                                                   nTruthTreeRecordsAtCurrentEvent);
      if ( sc.isFailure() ){
        msg(MSG::ERROR) << "Jet Truth Ntuple Tool could not fill data" << endmsg;
        delete genParticleJets;
        return StatusCode::FAILURE;
      }
    }


    if (m_doTruth && selecParticles){
      sc = m_truthNtupleTool->writeTruthData( truthData );
      if ( sc.isFailure() ){
        msg(MSG::ERROR) << "Truth Ntuple Tool could not fill data" << endmsg;
        delete genParticleJets;
        return StatusCode::FAILURE;
      }
    } // end if (m_doTruth && selecParticles)

    delete selecParticles;
    delete genParticleJets;
    std::vector<Trk::ValidationTrackTruthData>::iterator truthDataIter = truthData.begin();
    for (; truthDataIter != truthData.end(); truthDataIter++) {
        delete (*truthDataIter).truthPerigee;
        (*truthDataIter).truthPerigee = 0;
    }
    return StatusCode::SUCCESS;
}

StatusCode Trk::TrackValidationNtupleWriter::writeTrackData(unsigned int trackColIndex,
                                                            std::vector<Trk::ValidationTrackTruthData>&  truthData,
                                                            const Trk::Vertex* primaryVertex ) {
    StatusCode sc = StatusCode::SUCCESS; if (sc.isFailure()) { /* mute sc*/ }
    // retrieve reconstructed tracks
    const TrackCollection* tracks = 0;
    if (m_inputTrackCollection[trackColIndex] != "" && evtStore()->contains<TrackCollection>(m_inputTrackCollection[trackColIndex])) {
        sc = evtStore()->retrieve(tracks, m_inputTrackCollection[trackColIndex]);
        if (sc.isFailure()) {
            msg(MSG::WARNING) <<"Tracks not found:  " << m_inputTrackCollection[trackColIndex] << endmsg;
            return StatusCode::SUCCESS;
        } else {
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Tracks found: " << m_inputTrackCollection[trackColIndex] <<endmsg;
        }
    } else {
        msg(MSG::WARNING) <<"TrackCollection " << m_inputTrackCollection[trackColIndex] << " not found in StoreGate." << endmsg;
        return StatusCode::SUCCESS;
    }

    const TrackTruthCollection* trackTruthCollection = 0;
    if (m_doTruth) {
        // retrieve track truth collection
        if (m_trackTruthCollectionName[trackColIndex] != "" && evtStore()->contains<TrackTruthCollection>(m_trackTruthCollectionName[trackColIndex])) {
            sc = evtStore()->retrieve(trackTruthCollection, m_trackTruthCollectionName[trackColIndex]);
            if (sc.isFailure()) {
                msg(MSG::WARNING) <<"TrackTruthCollection not found:  " << m_trackTruthCollectionName[trackColIndex] << endmsg;
                // FIXME: return is not good here...
                return StatusCode::SUCCESS;
            } else {
                ATH_MSG_DEBUG ("TrackTruthColl found: " << m_trackTruthCollectionName[trackColIndex]);
                // TODO: use trackTruthCollection.trackCollectionLink() to check whether consistent track collection was used
            }
        } else {
            msg(MSG::WARNING) <<"TrackTruthCollection " << m_trackTruthCollectionName[trackColIndex] << " not found in StoreGate." << endmsg;
            // FIXME: return is not good here...
            return StatusCode::SUCCESS;
        }
    } // end if m_doTruth

    const unsigned int nTruthTreeRecordsAtCurrentEvent = 
      (m_doTruth ? m_truthNtupleTool->getNumberOfTreeRecords() : 0 );
    //truthData.truthToTrackIndices
    //std::vector< std::vector<unsigned int> >    truthToTrackIndices(selecParticles ? selecParticles->size() : 0);
    //std::vector< std::vector<float> >           truthToTrackMatchingProbabilities(selecParticles ? selecParticles->size() : 0);

    int trackTreeIndexBegin = m_trees[trackColIndex]->GetEntries();
    int nTracksPerEvent = 0;

    // loop over tracks
    TrackCollection::const_iterator trackIterator = (*tracks).begin();
    for ( ; trackIterator < (*tracks).end(); ++trackIterator) {
        if (!((*trackIterator))) {
            msg(MSG::WARNING) <<"TrackCollection " << m_inputTrackCollection[trackColIndex] << "contains empty entries" << endmsg;
            continue;
        }
	if (m_trackSelector.empty() || m_trackSelector->decision(*(*trackIterator), primaryVertex)) {
            ATH_MSG_VERBOSE ("track selected!");
            ToolHandleArray< Trk::ITrackValidationNtupleTool >::iterator itTools;
            itTools = m_ValidationNtupleTools.begin();
            for (  ; itTools != m_ValidationNtupleTools.end(); ++itTools ) {
              if (((*itTools)->fillTrackData( *(*trackIterator), 0 )).isFailure()) {
                ATH_MSG_ERROR ("Validation Ntuple Tool could not fill track data.");
                return StatusCode::FAILURE;
              }
            }
            nTracksPerEvent += 1;

            if (m_doTruth){
                // find matching truth particle
                const TrackTruth* trackTruth = 0;
                const HepMC::GenParticle* genParticle = 0;
                TrackTruthCollection::const_iterator truthIterator = trackTruthCollection->find( trackIterator - (*tracks).begin() );
                if ( truthIterator == trackTruthCollection->end() ){
                  ATH_MSG_DEBUG ("No matching truth particle found for track");
                } else {
                  trackTruth = &((*truthIterator).second);
                  if ( !(trackTruth->particleLink().isValid()) ) {
                    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Link to generated particle information is not there - assuming a lost G4 particle ('fake fake')." << endmsg;
                    genParticle = m_visibleParticleWithoutTruth; // with pdg_id 0
                  } else {
                    genParticle = trackTruth->particleLink().cptr();
                    if ( genParticle!=NULL && genParticle->pdg_id() == 0 ) {
                      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Associated Particle ID " << genParticle->pdg_id()
                            << " does not conform to PDG requirements... ignore it!" << endmsg;
                      genParticle = 0;
                    } 
                  }
                }
                if (genParticle) {
                  ATH_MSG_VERBOSE("Associated Particle ID: " << genParticle->pdg_id());
                  // Perform extrapolation to generate perigee parameters
                  const Trk::TrackParameters* generatedTrackPerigee(0);
                  const Trk::TrackParameters* newTrackPerigee(0);
                  // fill the truth data in the track tree
                  //unsigned int truthIndex = particleToIndexMap[genParticle];
                  int truthIndex = -1;
                  //std::vector<const HepMC::GenParticle*>::const_iterator matchedPartIter = find(selecParticles->begin(), selecParticles->end(), genParticle);
                  // TODO: do the search somehow better:
                  std::vector<Trk::ValidationTrackTruthData>::iterator matchedPartIter = truthData.begin();
                  for (; matchedPartIter != truthData.end(); matchedPartIter++) {
                    truthIndex++;
                    if ((*matchedPartIter).genParticle == genParticle) break;
                  }
                  if (matchedPartIter == truthData.end()) {
                    // did not find particle in list of selected particles
                    truthIndex = -1;
                    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Matched particle with barcode " << genParticle->barcode() << " is not in list of selected particles" << endmsg;
                    if ( genParticle->production_vertex() ) {
                      newTrackPerigee = m_truthToTrack->makePerigeeParameters( genParticle );
                      generatedTrackPerigee = newTrackPerigee;
                    }
                  } else {
                    //truthIndex = int(matchedPartIter - selecParticles->begin());
                    // store the index in the track tree of the current track (establish link from truth to track)
                    (*matchedPartIter).truthToTrackIndices[trackColIndex].push_back(m_nTrackTreeRecords[trackColIndex]);
                    (*matchedPartIter).truthToTrackMatchingProbabilities[trackColIndex].push_back(trackTruth->probability());
                    generatedTrackPerigee = (*matchedPartIter).truthPerigee;
                    // calculate entry index in tree:
                    truthIndex += nTruthTreeRecordsAtCurrentEvent;
                  }
                  ToolHandleArray< Trk::ITrackValidationNtupleTool >::iterator itTools;
                  itTools = m_ValidationNtupleTools.begin();
                  for (  ; itTools != m_ValidationNtupleTools.end(); ++itTools ) {
                    if (((*itTools)->fillTrackTruthData( generatedTrackPerigee, *trackTruth, truthIndex )).isFailure()) {
                      ATH_MSG_ERROR ("Validation Ntuple Tool could not fill track truth data.");
                      delete newTrackPerigee;
                      return StatusCode::FAILURE;
                    }
                  }
                  // New memory allocated for the true perigee - must be deleted
                  delete newTrackPerigee;
                  
                } // end if (genParticle)
            } // end if (m_doTruth)

            m_trees[trackColIndex]->Fill();
            itTools = m_ValidationNtupleTools.begin();
            for (  ; itTools != m_ValidationNtupleTools.end(); ++itTools )
              (*itTools)->resetVariables();
            // call getEntries - 1, push_back in index vektor

            m_nTrackTreeRecords[trackColIndex]++;
        } else {
            if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) <<"track not selected!" <<endmsg;
        }
    } // end for (trackIterator)

    for (unsigned int toolIndex = 0 ; toolIndex < m_eventPropertyNtupleTools.size(); ++toolIndex )
      if( m_eventPropertyNtupleTools[toolIndex]->isTrackLinkTool() ) m_eventPropertyNtupleTools[toolIndex]->setTrackTreeIndices
        (trackColIndex, trackTreeIndexBegin, nTracksPerEvent );

    return StatusCode::SUCCESS;
}

StatusCode Trk::TrackValidationNtupleWriter::writeTrackParticleData(unsigned int trackParticleColIndex ) {

 msg(MSG::DEBUG) << "writeTrackParticleData method started" << endmsg;
 
  // retrieve Trk::TrackParticleBaseCollection from the SG
 const Trk::TrackParticleBaseCollection* trackParticles = 0;

 if (m_inputTrackParticleCollection[trackParticleColIndex] != "" && 
     evtStore()->contains<Trk::TrackParticleBaseCollection>(m_inputTrackParticleCollection[trackParticleColIndex])) {
            if (evtStore()->retrieve(trackParticles, m_inputTrackParticleCollection[trackParticleColIndex]).isFailure()) {
                                    msg(MSG::WARNING) <<"Trk::TrackParticleBasesContainer not found:" << m_inputTrackParticleCollection[trackParticleColIndex] << endmsg;
                                   return StatusCode::SUCCESS;
               } else {
                       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Trk::TrackParticleBasesContainer found: "<< m_inputTrackParticleCollection[trackParticleColIndex] <<endmsg;
                      }
    } else {
        msg(MSG::WARNING) <<"Trk::TrackParticleBasesContainer " << m_inputTrackParticleCollection[trackParticleColIndex] << " not found in StoreGate." << endmsg;
        return StatusCode::SUCCESS;
           }

 // indexes for eventPropertyNtupleTool
    int trackTreeIndexBegin = m_trees[trackParticleColIndex + m_inputTrackCollection.size()]->GetEntries();
    int nTrkParticlesPerEvent = 0;

 // fill coresponding Rec:TrackParticle tree using BasicTrkParticleValidationNtuple 
 Trk::TrackParticleBaseCollection::const_iterator trackParticleIterator = (*trackParticles).begin();
    for ( ; trackParticleIterator < (*trackParticles).end(); ++trackParticleIterator) {
        if (!((*trackParticleIterator))) {
            msg(MSG::WARNING) <<"TrackParticleCollection " << m_inputTrackParticleCollection[trackParticleColIndex] << "contains empty entries" << endmsg;
            continue;
	                                 } // if emty entry
 
        if ( m_ValTrkParticleNtupleTool->fillTrackParticleData( *(*trackParticleIterator) ).isFailure() ){
                                        msg(MSG::ERROR) << "Validation Ntuple Tool could not fill track data." << endmsg;
                                        return StatusCode::FAILURE;
	                                } // if StatusCode is FAILURE
     nTrkParticlesPerEvent += 1;
     
     if ( m_ValTrkParticleNtupleTool->writeRecord(m_trees[trackParticleColIndex + m_inputTrackCollection.size()]).isFailure() ){
    	                                msg(MSG::ERROR) << "Validation Ntuple Tool could not write track data." << endmsg;
                                        return StatusCode::FAILURE;
        } 
    } // loop over Rec::TrackPartcielContainer
    
    // Record link between Event Property and Trk::TrackParticleBase information
    for (unsigned int toolIndex = 0 ; toolIndex < m_eventPropertyNtupleTools.size(); ++toolIndex ){ 
      if( m_eventPropertyNtupleTools[toolIndex]->isTrkParticleLinkTool() )
                     m_eventPropertyNtupleTools[toolIndex]->setTrackTreeIndices(trackParticleColIndex, trackTreeIndexBegin, nTrkParticlesPerEvent );
    }

    msg(MSG::DEBUG) <<"writeTrackParticleData successfully finished " << endmsg;
    return StatusCode::SUCCESS;
}


StatusCode Trk::TrackValidationNtupleWriter::finalize() {

    msg(MSG::INFO)  << "TrackValidationNtupleWriter finalize()" << endmsg;

    delete m_visibleParticleWithoutTruth;
    for (unsigned int toolIndex = 0 ; toolIndex < m_eventPropertyNtupleTools.size(); ++toolIndex ){
      if (m_eventPropertyNtupleTools[toolIndex]->resetVariables( ).isFailure()){};
    }

    return StatusCode::SUCCESS;
}

// const Trk::TrackParameters* Trk::TrackValidationNtupleWriter::associateTruthToTrack( const Trk::Track* track, 
// 										     const TrackCollection* trackCollection, 
// 										     const TrackTruth*& trackTruth )
// {
//     
//    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Starting InDet::FitterValidationTool::determineGeneratedPerigee()..." << endmsg;
// 
//   StatusCode sc;
// 
//   const TrackTruthCollection* trackTruthCollection = 0;
// 
//   sc = evtStore()->retrieve( trackTruthCollection, m_trackTruthCollectionName );
// 
//   if ( sc.isFailure() ){
//     msg(MSG::ERROR) << "Attempt to retrieve track truth using StoreGate collection name " << m_trackTruthCollectionName << " failed... Exiting" << endmsg;
//     return 0;
//   }
// 
//   ElementLink<TrackCollection> trackLink;
// 
//   trackLink.setElement( const_cast<Trk::Track*>( track ) );
//   trackLink.setStorableObject( *trackCollection );
//  
//   typedef TrackTruthCollection::const_iterator TruthIterator;
//   // typedef std::map<ElementLink< DataVector<Trk::Track> >, TrackTruth>::const_iterator TruthIterator; 
//  
//   TruthIterator truthIterator = trackTruthCollection->find( trackLink );
// 
//   if ( truthIterator == trackTruthCollection->end() ){
//     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Truth could not be found... exiting" << endmsg;
//     return 0;
//   }
// 
//   trackTruth = new TrackTruth( ( *truthIterator ).second );
// 
//   const HepMcParticleLink& particleLink = trackTruth->particleLink();
//   
//   const HepMC::GenParticle* genParticle = particleLink.cptr();
//   
//   if ( !genParticle ){
//     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Link to generated particle information cannot be found... No truth association possible" << endmsg;
//     return 0;
//   }
//  
//   int particleID = genParticle->pdg_id();
//   
//   if ( particleID == 0 ){
//     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Associated Particle ID " << particleID << " does not conform to PDG requirements... exiting" << endmsg;
//     return 0;
//   }
//  
//   else
//     if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Associated Particle ID: " << particleID << endmsg;
// 
//   const Trk::TrackParameters* generatedTrackPerigee(0);
// 
//   // Perform extrapolation to generate perigee parameters
//   if ( genParticle->production_vertex() )
//     generatedTrackPerigee = m_truthToTrack->makePerigeeParameters( genParticle );
// 
//   return generatedTrackPerigee;
// 
// }


