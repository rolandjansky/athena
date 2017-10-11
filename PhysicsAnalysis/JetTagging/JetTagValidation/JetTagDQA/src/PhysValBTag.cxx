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
#include "ParticleJetTools/JetFlavourInfo.h"

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
    m_isData(false),
    m_antiKt2PV0TrackJetPlots             (0, "BTag/AntiKt2PV0TrackJets/"          , "antiKt2PV0TrackJets"),
    m_antiKt4PV0TrackJetPlots             (0, "BTag/AntiKt4PV0TrackJets/"          , "antiKt4PV0TrackJets"),
    m_antiKt4EMTopoPlots                  (0, "BTag/AntiKt4EMTopo/"                , "antiKt4EMTopo"),
    m_antiKtVR30Rmax4Rmin02TrackJetsPlots (0, "BTag/AntiKtVR30Rmax4Rmin02TrackJets", "antiKtVR30Rmax4Rmin02TrackJets"),
    m_antiKt4EMPFlowJetsPlots             (0, "BTag/AntiKt4EMPFlowJets"            , "antiKtVR30Rmax4Rmin02TrackJets"),
    m_nevents(0)
  {

    declareProperty( "isData", m_isData );

    declareProperty( "JetContainerName1", m_jetName1 = "AntiKt2PV0TrackJets");
    declareProperty( "JetContainerName2", m_jetName2 = "AntiKt4PV0TrackJets");
    declareProperty( "JetContainerName3", m_jetName3 = "AntiKt4EMTopoJets" );
    declareProperty( "JetContainerName4", m_jetName4 = "AntiKtVR30Rmax4Rmin02TrackJets");
    declareProperty( "JetContainerName5", m_jetName5 = "AntiKt4EMPFlowJets");

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

	m_btagplots.insert(std::make_pair(m_jetName1, m_antiKt2PV0TrackJetPlots));
	m_btagplots.insert(std::make_pair(m_jetName2, m_antiKt4PV0TrackJetPlots));
	m_btagplots.insert(std::make_pair(m_jetName3, m_antiKt4EMTopoPlots));
	m_btagplots.insert(std::make_pair(m_jetName4, m_antiKtVR30Rmax4Rmin02TrackJetsPlots));
	m_btagplots.insert(std::make_pair(m_jetName5, m_antiKt4EMPFlowJetsPlots));
   
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
    
    ++m_nevents;
    //int njets = 0;
    //std::cout << "Number of proccessed events = " << m_nevents << std::endl;
    // primary vertex
    const xAOD::VertexContainer *vertices = 0;
    CHECK( evtStore()->retrieve(vertices, "PrimaryVertices") );
    int npv(0);
    size_t indexPV = 0;
    bool has_pv = false;
    xAOD::VertexContainer::const_iterator vtx_itr = vertices->begin();
    xAOD::VertexContainer::const_iterator vtx_end = vertices->end();
    int count = -1;
    for (; vtx_itr != vtx_end; ++vtx_itr) {
      count++;
      if ((*vtx_itr)->nTrackParticles() >= 2) {
        //v_PVz->push_back(  (*vtx_itr)->z() );
        npv++;
        if ((*vtx_itr)->vertexType() == 1) {
          if (PV_x != -999) ATH_MSG_WARNING( ".... second PV in the events ...!!!!!!");
          indexPV = count;
          has_pv = true;
          PV_x = (*vtx_itr)->x(); // ANDREA !!!!!!!!
          PV_y = (*vtx_itr)->y(); // ANDREA !!!!!!!!
          PV_z = (*vtx_itr)->z();
        }   //if ((*vtx_itr)->vertexType() == 1) {
      } //if ((*vtx_itr)->nTrackParticles() >= 2) {
    }//for (; vtx_itr != vtx_end; ++vtx_itr) {
    if (!has_pv) {
      //ATH_MSG_WARNING( ".... rejecting the event due to missing PV!!!!");
      return StatusCode::SUCCESS;
    }
    const xAOD::Vertex *myVertex = vertices->at(indexPV); // the (reco?) primary vertex
    //std::cout<<"z coordinate of PV: "<< myVertex->z() <<std::endl;

    for(std::map<std::string,JetTagDQA::BTaggingValidationPlots>::iterator plot_i = m_btagplots.begin(); plot_i != m_btagplots.end(); ++plot_i){
      
      //LOOP ON JETS
      const xAOD::JetContainer* jets(0);
      // ATH_CHECK(evtStore()->retrieve(jets, plot_i->first));
      StatusCode jetException = evtStore()->retrieve(jets, plot_i->first);
      // If StatusCode is not SUCCESS, no jet collection with this name, continue loop
      if (jetException != StatusCode::SUCCESS) continue;

      for (auto jet : *jets) {

        //			++njets;
        //     			m_jetPlots.fill(jet);
        //std::cout << "number of processed jet: " << njets << std::endl;

        //btagging
        const xAOD::BTagging* btag =  jet->btagging();

        int label(1000);
        //double dR(1000);

	//Arnaud: JVT cut to remove horns
	//if(jet->getAttribute<float>("Jvt") < 0.59 && jet->pt() < 60e3 && std::abs(jet->eta()) < 2.4) continue;

        if(jet->pt() > 20000 && std::abs(jet->eta()) < 2.5){
          (plot_i->second).fill(jet);

	  if(!m_isData) {
	    //if(jet->isAvailable<int>("HadronConeExclTruthLabelID")) label = jetFlavourLabel(jet, xAOD::JetFlavourLabelType::GAFinalHadron);
	    if(jet->isAvailable<int>("HadronConeExclTruthLabelID")) label = jetFlavourLabel(jet, xAOD::ConeFinalParton);
	    else jet->getAttribute("TruthLabelID",label);
	  }

          (plot_i->second).fill(label);
          if (btag){
            (plot_i->second).fill(btag);
            //(plot_i->second).fill(jet, btag);
       	    (plot_i->second).fill(jet, btag, myVertex); //ANDREA: added xAOD::Vertex * as input to BTaggingValidationPlots.cxx
          }
        }
      }//for (auto jet : *jets) {
    }//for(std::map<std::string,JetTagDQA::BTaggingValidationPlots>::iterator plot_i = m_btagplots.begin(); plot_i != m_btagplots.end(); ++plot_i){

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
