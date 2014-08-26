/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///==================================================
///
/// PileupMCEventInfo.h, (c) ATLAS Detector software
///
/// Validation for Pileup Production
///
/// Authors : David W. Miller
/// In Collaboration with  : Sven Vahsen
///
/// Date : 16 April 2007
///
///==================================================

#include "PileupRTT/PileupMCEventInfo.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "GeneratorObjects/McEventCollection.h"

#include "PileupRTT/IPileupValidationTools.h"

#include "GaudiKernel/ITHistSvc.h"

///=================================
#include <iostream>
#include <sstream>
#include <fstream>
#include "TTree.h"
///=================================

namespace PileupRTT
{

  ///============
  /// CONSTRUCTOR
  ///=============
  PileupRTT::PileupMCEventInfo::PileupMCEventInfo(const std::string& name,ISvcLocator* pSvcLocator) : 
    AthAlgorithm(name, pSvcLocator),

    m_NHardPrimVertices(0),
    m_hardPrimVtxX(0),
    m_hardPrimVtxY(0),
    m_hardPrimVtxZ(0),

    m_NPileupVertices(0),
    m_NPrimPileupVertices(0),
    m_NGenEventWithoutVertex(0),
    m_pileupVtxX(0),
    m_pileupVtxY(0),
    m_pileupVtxZ(0),

    m_theTree(0),
    m_thistSvc(0)

  {
    //+++ Container Names
    declareProperty("McEventCollection", m_mcTruthInfo = "TruthEvent");

    //+++ Declare the output file for the MC GenTree info
    declareProperty("McOutFile",         m_outfile     = "MCEventInfo.txt");
    declareProperty("dumpMcInfo",        m_dumpMCInfo  = false);
  }

  ///============
  /// DESTRUCTOR
  ///============
  PileupRTT::PileupMCEventInfo::~PileupMCEventInfo() { }


  ///=============================================
  /// Initialize
  /// --> initialize StoreGate
  /// --> get a handle on the analysis tools
  /// --> book histograms
  ///=============================================
  StatusCode PileupRTT::PileupMCEventInfo::initialize()
  {

    ATH_MSG_INFO("In PileupRTT::PileupMCEventInfo::initialize()"); 

    //+++ Get handle to StoreGate (Persistency) Service
    StatusCode sc = service("StoreGateSvc", m_storeGate);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Unable to retrieve pointer to StoreGateSvc");
      return StatusCode::FAILURE;
    }
    
    ///+++ Get handle on Tools Service
    IToolSvc* toolSvc;
    sc = service("ToolSvc",toolSvc);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Can't get ToolSvc");
      return StatusCode::FAILURE;
    }
    
    ///+++ Grab the JetTrackVertexHelperTool
    if ( toolSvc->retrieveTool("PileupRTT::PileupValidationTools", m_pileupRTTTool).isFailure() ) {
      ATH_MSG_ERROR("Creation of algTool PileupValidationTools FAILED !!");
      return StatusCode::RECOVERABLE;
    }

    m_hardPrimVtxX = new std::vector<double>;
    m_hardPrimVtxY = new std::vector<double>;
    m_hardPrimVtxZ = new std::vector<double>;

    m_pileupVtxX = new std::vector<double>;
    m_pileupVtxY = new std::vector<double>;
    m_pileupVtxZ = new std::vector<double>;

    ///+++ Book ROOT histograms
    sc = this->bookHistograms();
    if (sc.isFailure()) {
      ATH_MSG_ERROR("bookHistograms failed!");
      return StatusCode::FAILURE;
    }

    ///+++ Add branches and leaves to the tree
    sc = this->addBranches();
    if (sc.isFailure()) {
      ATH_MSG_ERROR("addBranches failed!");
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("PileupRTT::PileupMCEventInfo::initialize() successful");
    return StatusCode::SUCCESS;
  }

  ///=============================================
  /// Finalize:
  /// delete any memory allocation from the heap
  ///=============================================
  StatusCode PileupRTT::PileupMCEventInfo::finalize() {
    ATH_MSG_INFO("In PileupRTT::PileupMCEventInfo::finalize()");


  ///=============================================
  /// Clear:
  /// clear members
  ///=============================================

    ATH_MSG_DEBUG("clear members");

    ///+++ For Athena-Aware NTuple
    m_NHardPrimVertices = 0;
    m_hardPrimVtxX->clear();
    m_hardPrimVtxY->clear();
    m_hardPrimVtxZ->clear();

    m_NPileupVertices        = 0;
    m_NPrimPileupVertices    = 0;
    m_NGenEventWithoutVertex = 0;
    m_pileupVtxX->clear();
    m_pileupVtxY->clear();
    m_pileupVtxZ->clear();

    m_theTree = 0;
    m_thistSvc = 0;

    return StatusCode::SUCCESS;
  }

  ///=============================================
  /// Execute:
  /// on event by event
  ///=============================================
  StatusCode PileupRTT::PileupMCEventInfo::execute() {
    
    ATH_MSG_DEBUG("in PileupRTT::PileupMCEventInfo::execute()");

    ///+++ Read the Pileup RDO
    StatusCode sc = readMCTruthEventInfo();
    if (sc.isFailure()) {
      ATH_MSG_ERROR("The method readMCTruthEventInfo() failed");
      return StatusCode::RECOVERABLE;
    }
    else ATH_MSG_INFO("readMCTruthEventInfo() successful!");

    m_theTree->Fill();

    return StatusCode::SUCCESS;
  }

  ///=============================================
  /// Pileup RDO Validation
  ///     "readPileupRDO"
  /// --> Read MC Truth Info
  ///=============================================
  StatusCode PileupRTT::PileupMCEventInfo::readMCTruthEventInfo() {

    ATH_MSG_INFO("in readPileupRDO()");
    StatusCode sc = StatusCode::SUCCESS;

    ///+++++++++++++++++++++++++++
    /// MC Truth Info
    ///+++++++++++++++++++++++++++
    //+++ Get True Vertices from Storegate
    const McEventCollection* mcTruth(0);
    sc = m_storeGate->retrieve(mcTruth, m_mcTruthInfo);
    if( sc.isFailure() ) {
      ATH_MSG_WARNING("MC Event " << m_mcTruthInfo << " not found.");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_INFO("McEventCollection successfully retrieved");
    ATH_MSG_INFO("Number of truth particles: " << mcTruth->size());

    //+++ Outfile for MC GenTree info
    const char* out = m_outfile.c_str();
    std::ofstream outfile(out);
    if (mcTruth) {
      McEventCollection::const_iterator MCeventItr    = mcTruth->begin();
      McEventCollection::const_iterator MCeventItrEnd = mcTruth->end();
      for(; MCeventItr != MCeventItrEnd; ++MCeventItr) {
        const HepMC::GenEvent* genEvent = (*MCeventItr);
        if(genEvent) {
          if (m_dumpMCInfo) genEvent->print(outfile);
          // Print info about all PARTICLES for this GenEvent
          if (m_dumpMCInfo) {
            HepMC::GenEvent::particle_const_iterator mcItr    = genEvent->particles_begin();
            HepMC::GenEvent::particle_const_iterator mcItrEnd = genEvent->particles_end();
            for (; mcItr != mcItrEnd; ++mcItr) {
                (*mcItr)->print(outfile);
            }
          }

          // Get the signal vertex (first Truth Event)
          if (genEvent == *(mcTruth->begin())) {
              HepMC::GenEvent::vertex_const_iterator vxp = genEvent->vertices_begin();
              if (genEvent->vertices_size() == 0) continue;
              m_NHardPrimVertices++;
              double xpvxh=(*vxp)->position().x();
              double ypvxh=(*vxp)->position().y();
              double zpvxh=(*vxp)->position().z();
              m_hardPrimVtxX->push_back(xpvxh);
              m_hardPrimVtxY->push_back(ypvxh);
              m_hardPrimVtxZ->push_back(zpvxh);
              if (m_dumpMCInfo) (*vxp)->print(outfile);
          }
          // Get the pileup vertices (all other Truth Events)
          else {
              HepMC::GenEvent::vertex_const_iterator vxp = genEvent->vertices_begin();
              if (genEvent->vertices_size() == 0) {
                m_NGenEventWithoutVertex++;
                continue;
              }
              m_NPileupVertices++;
              double xpvxp=(*vxp)->position().x();
              double ypvxp=(*vxp)->position().y();
              double zpvxp=(*vxp)->position().z();
              m_pileupVtxX->push_back(xpvxp);
              m_pileupVtxY->push_back(ypvxp);
              m_pileupVtxZ->push_back(zpvxp);
              if ( (zpvxp <= 100.) && (ypvxp <= 0.1) && (xpvxp <= 0.1) ) {
                m_NPrimPileupVertices++;
              }
              if (m_dumpMCInfo) (*vxp)->print(outfile);
          }
        }
      }
    }

    return StatusCode::SUCCESS;

  }

  ///=================
  /// Book Histograms
  ///=================
  StatusCode PileupRTT::PileupMCEventInfo::bookHistograms() {

    StatusCode sc = StatusCode::SUCCESS;
    ATH_MSG_DEBUG("Booking histograms");
   
    if (sc.isFailure()) {
      ATH_MSG_ERROR("ROOT Hist registration failed!");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  ///======================
  /// Add Branches to AANT
  ///======================
  StatusCode PileupRTT::PileupMCEventInfo::addBranches() {

    ///=======================================
    ///+++ Add branches and leaves to the tree

    StatusCode sc = service("THistSvc",m_thistSvc); 
    if (sc.isFailure()) { 
      ATH_MSG_ERROR( "Unable to retrieve pointer to THistSvc" ); 
      return sc;
    }

    ATH_MSG_DEBUG("Adding Branches to custom AANT");

    m_theTree = new TTree("MCEventInfoTree","MCEventInfoTree");

    m_theTree->Branch("NGenEventWithoutVertex",m_NGenEventWithoutVertex,"NGenEventWithoutVertex/I");

    m_theTree->Branch("NHardPrimVertices",m_NHardPrimVertices,"NHardPrimVertices/I");
    m_theTree->Branch("hardPrimVtxX",m_hardPrimVtxX);
    m_theTree->Branch("hardPrimVtxY",m_hardPrimVtxY);
    m_theTree->Branch("hardPrimVtxZ",m_hardPrimVtxZ);

    m_theTree->Branch("NPileupVertices",m_NPileupVertices,"NPileupVertices/I");
    m_theTree->Branch("NPrimPileupVertices",m_NPrimPileupVertices,"NPrimPileupVertices/I");
    m_theTree->Branch("pileupVtxX",m_pileupVtxX);
    m_theTree->Branch("pileupVtxY",m_pileupVtxY);
    m_theTree->Branch("pileupVtxZ",m_pileupVtxZ);

    if (StatusCode::SUCCESS == m_thistSvc->regTree("/AANT/MCEventInfoTree",m_theTree)) {
    ATH_MSG_INFO("Booked module ntuple");
  } else {
    ATH_MSG_ERROR("Unable to register module ntuple");
  }


    return StatusCode::SUCCESS;
  }

} //end of namespace declaration
