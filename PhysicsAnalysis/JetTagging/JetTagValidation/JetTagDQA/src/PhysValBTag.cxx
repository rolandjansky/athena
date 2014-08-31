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
    m_btagPlots(0, "BTag/", "BTag")
  {
    
    declareProperty( "JetContainerName", m_jetName = "AntiKt4EMTopoJets" );
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

	m_nevents = 0;		

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
//	ATH_CHECK(book(m_jetPlots));
	ATH_CHECK(book(m_btagPlots));
// 	ATH_CHECK(book(m_trkvtxPlots));
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
	const xAOD::JetContainer* jets(0);
    	ATH_CHECK(evtStore()->retrieve(jets, m_jetName));

    	for (auto jet : *jets) {

//		++njets;
//     		m_jetPlots.fill(jet);
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

  		//jet->getAttribute("TruthLabelDeltaR_B", dR);
		
		if(jet->pt() > 20000 && abs(jet->eta()) < 2.5){ 	   
			m_btagPlots.fill(jet);  	
			m_btagPlots.fill(label);
     			if (btag) m_btagPlots.fill(btag);
			m_btagPlots.fill(jet, btag);
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
    m_btagPlots.finalize();
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
