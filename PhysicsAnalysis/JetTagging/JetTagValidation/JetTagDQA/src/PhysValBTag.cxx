///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhysValBTag.cxx
// Implementation file for class PhysValBTag
// Author: E.Schopf<elisabeth.schopf@cern.ch>
/////////////////////////////////////////////////////////////////// 
// JetTagDQA includes
#include "PhysValBTag.h"

// STL includes
#include <vector>

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/TrackParticle.h" 
#include "xAODTracking/Vertex.h" 
#include "xAODBTagging/BTagging.h" 
//#include "../../Event/xAOD/xAODBTagging/xAODBTagging/BTagging.h"

#include "AthenaBaseComps/AthCheckMacros.h"

namespace JetTagDQA {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

  PhysValBTag::PhysValBTag( const std::string& type, 
				  const std::string& name, 
				  const IInterface* parent ) : 
    ManagedMonitorToolBase( type, name, parent ),
//   m_jetPlots(0, "Summary/Jet/", "Jet"),
//    m_trkvtxPlots(0, "Summary/TrackAndVertex/"),
    m_antiKt4EMTopoPlots(0, "BTag/AntiKt4EMTopo/", "antiKt4EMTopo"),
    m_antiKt4LCTopoPlots(0, "BTag/AntiKt4LCTopo/", "antiKt4LCTopo"),
//    m_antiKt4TruthPlots(0, "BTag/AntiKt4Truth/", "antiKt4Truth"),
    m_antiKt4TruthWZPlots(0, "BTag/AntiKt4TruthWZ/", "antiKt4TruthWZ"),
    m_antiKt10LCTopoPlots(0, "BTag/AntiKt10LCTopo/", "antiKt10LCTopo"),
//    m_antiKt10TruthPlots(0, "BTag/AntiKt10Truth/", "antiKt10Truth"),
    m_antiKt10TruthWZPlots(0, "BTag/AntiKt10TruthWZ/", "antiKt10TruthWZ"),
    m_antiKt3PV0TrackJetPlots(0, "BTag/AntiKt3PV0TrackJets/", "antiKt3PV0TrackJets"),
    m_antiKt4PV0TrackJetPlots(0, "BTag/AntiKt4PV0TrackJets/", "antiKt4PV0TrackJets")
  {
    
    declareProperty( "JetContainerName1", m_jetName1 = "AntiKt4EMTopoJets" );
    declareProperty( "JetContainerName2", m_jetName2 = "AntiKt4LCTopoJets"); 
//    declareProperty( "JetContainerName3", m_jetName3 = "AntiKt4TruthJets"); 
    declareProperty( "JetContainerName4", m_jetName4 = "AntiKt4TruthWZJets"); 
    declareProperty( "JetContainerName5", m_jetName5 = "AntiKt10LCTopoJets"); 
//    declareProperty( "JetContainerName6", m_jetName6 = "AntiKt10TruthJets"); 
    declareProperty( "JetContainerName7", m_jetName7 = "AntiKt10TruthWZJets"); 
    declareProperty( "JetContainerName8", m_jetName8 = "AntiKt3PV0TrackJets"); 
    declareProperty( "JetContainerName9", m_jetName9 = "AntiKt4PV0TrackJets"); 
    declareProperty( "TrackContainerName", m_trackName = "InDetTrackParticles" );  
    declareProperty( "VertexContainerName", m_vertexName = "PrimaryVertices" );  
  }
  
  // Destructor
  ///////////////
  PhysValBTag::~PhysValBTag()
  {}
  
  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode PhysValBTag::initialize()
  {
	ATH_MSG_INFO ("Initializing " << name() << "...");    
	ATH_CHECK(ManagedMonitorToolBase::initialize());

	m_btagplots.insert(std::make_pair(m_jetName1, m_antiKt4EMTopoPlots));
	m_btagplots.insert(std::make_pair(m_jetName2, m_antiKt4LCTopoPlots));
//	m_btagplots.insert(std::make_pair(m_jetName3, m_antiKt4TruthPlots));
	m_btagplots.insert(std::make_pair(m_jetName4, m_antiKt4TruthWZPlots));
	m_btagplots.insert(std::make_pair(m_jetName5, m_antiKt10LCTopoPlots));
//	m_btagplots.insert(std::make_pair(m_jetName6, m_antiKt10TruthPlots));
	m_btagplots.insert(std::make_pair(m_jetName7, m_antiKt10TruthWZPlots));
	m_btagplots.insert(std::make_pair(m_jetName8, m_antiKt3PV0TrackJetPlots));
	m_btagplots.insert(std::make_pair(m_jetName9, m_antiKt4PV0TrackJetPlots));
	m_nevents = 0;		
	//std::cout << "HALLO   " << evtStore()->contains<xAOD::JetContainer>("AntiKt3PV0TrackJets") << std::endl;
    return StatusCode::SUCCESS;
  }
  
  StatusCode PhysValBTag::book(PlotBase& plots)
  {
    plots.initialize();
    std::vector<HistData> hists = plots.retrieveBookedHistograms();
    
    for (auto& hist : hists){
      ATH_MSG_INFO ("Initializing " << hist.first << " " << hist.first->GetName() << " " << hist.second << "...");
      ATH_CHECK(regHist(hist.first,hist.second,all));
    }
    return StatusCode::SUCCESS;      
  }

  StatusCode PhysValBTag::bookHistograms()
 {
      ATH_MSG_INFO ("Booking hists " << name() << "...");      

      if (m_detailLevel >= 10) {

	for(std::map<std::string,JetTagDQA::BTaggingValidationPlots>::iterator plot_i = m_btagplots.begin(); plot_i != m_btagplots.end(); ++plot_i){
//		ATH_CHECK(book(m_jetPlots));		
		ATH_CHECK(book(plot_i->second));
// 		ATH_CHECK(book(m_trkvtxPlots));
	}
      }

      return StatusCode::SUCCESS;      
  }

  StatusCode PhysValBTag::fillHistograms()
  {
	ATH_MSG_INFO ("Filling hists " << name() << "...");
   
	if (m_detailLevel < 10) return StatusCode::SUCCESS;

	//++m_nevents;
	//int njets = 0;
	//std::cout << "Number of proccessed events = " << m_nevents << std::endl; 

	// Jets
	for(std::map<std::string,JetTagDQA::BTaggingValidationPlots>::iterator plot_i = m_btagplots.begin(); plot_i != m_btagplots.end(); ++plot_i){
		const xAOD::JetContainer* jets(0);
    		ATH_CHECK(evtStore()->retrieve(jets, plot_i->first));

    		for (auto jet : *jets) {

//			++njets;
//     			m_jetPlots.fill(jet);
			//std::cout << "number of processed jet: " << njets << std::endl; 

			//btagging
			const xAOD::BTagging* btag =  jet->btagging();

     			int label(1000);
			//double dR(1000);
 		 
   			try{
				jet->getAttribute<int>("TruthLabelID", label);
			}		
			catch(std::exception& exception){
				label=-1;		
			}
			//std::cout << "final hadron flavour label: " << xAOD::GAFinalHadronFlavourLabel(jet) << std::endl;
  			//jet->getAttribute("TruthLabelDeltaR_B", dR);
		
			if(jet->pt() > 20000 && abs(jet->eta()) < 2.5){ 	   
				(plot_i->second).fill(jet);  
				//std::cout << "Hallo info b-tag weight = " << btag->IP3D_loglikelihoodratio() << std::endl;	
				(plot_i->second).fill(label);
     				if (btag){
		 			(plot_i->second).fill(btag);
					(plot_i->second).fill(jet, btag);
				}
			}
     		}
	}

    // Tracks/Vertices
//    const xAOD::TrackParticleContainer* trks(0);
//    ATH_CHECK(evtStore()->retrieve(trks, m_trackName));
//    const xAOD::VertexContainer* vtxs(0);
//    ATH_CHECK(evtStore()->retrieve(vtxs, m_vertexName));
//    for (auto vtx : *vtxs) m_trkvtxPlots.fill(vtx);
//    m_trkvtxPlots.fill(trks->size(), vtxs->size() /*, event->averageInteractionsPerCrossing() */);

    return StatusCode::SUCCESS;
  }
  
  StatusCode PhysValBTag::procHistograms()
  {
    ATH_MSG_INFO ("Finalising hists " << name() << "...");
    for(std::map<std::string,JetTagDQA::BTaggingValidationPlots>::iterator plot_i = m_btagplots.begin(); plot_i != m_btagplots.end(); ++plot_i){	
	(plot_i->second).finalize();
    }
    return StatusCode::SUCCESS;
  }
  
/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 


}

//  LocalWords:  str 
