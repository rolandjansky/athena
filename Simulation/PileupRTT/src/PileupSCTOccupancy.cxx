/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///==================================================
///
/// PileupSCTOccupancy.h, (c) ATLAS Detector software
///
/// Validation for Pileup Production
///
/// Authors : David W. Miller
/// In Collaboration with  : Sven Vahsen
///
/// Date : 16 April 2007
///
///==================================================

#include "PileupRTT/PileupSCTOccupancy.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "InDetIdentifier/SCT_ID.h"
#include "TH2.h"

#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"

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
  PileupRTT::PileupSCTOccupancy::PileupSCTOccupancy(const std::string& name,ISvcLocator* pSvcLocator) : 
    AthAlgorithm(name, pSvcLocator),

    m_pileupRTTTool(0),

    m_storeGate(0),
    m_detStore(0),
    m_sctID(0),

    m_thistSvc(0),

    h_sct_lay0_occupancy(0),
    h_sct_lay1_occupancy(0),
    h_sct_lay2_occupancy(0),
    h_sct_lay3_occupancy(0)

  {
    //+++ Container Names
    declareProperty("SCT_RDOName",       m_SCT_RDOName   = "SCT_RDOs");
  }

  ///============
  /// DESTRUCTOR
  ///============
  PileupRTT::PileupSCTOccupancy::~PileupSCTOccupancy() { }

  ///=============================================
  /// Initialize
  /// --> initialize StoreGate
  /// --> get a handle on the analysis tools
  /// --> book histograms
  ///=============================================
  StatusCode PileupRTT::PileupSCTOccupancy::initialize()
  {
    ATH_MSG_INFO("Initializing PileupSCTOccupancy Algorithm");

    ///+++ Get handle to StoreGate (Persistency) Service
    StatusCode sc = service("StoreGateSvc", m_storeGate);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Unable to retrieve pointer to StoreGateSvc");
      return StatusCode::FAILURE;
    }

    ///+++ get DetectorStore service
    sc=service("DetectorStore",m_detStore);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("DetectorStore service not found !");
      return StatusCode::FAILURE;
    } 

    ///+++ Get handle to Histogram Service
    sc = service("THistSvc", m_thistSvc);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Unable to retrieve pointer to THistSvc");
      return StatusCode::FAILURE;
    }
    
    ///+++ Get handle on Tools Service
    IToolSvc* toolSvc;
    sc = service("ToolSvc",toolSvc);
    if (StatusCode::SUCCESS != sc) {
      ATH_MSG_ERROR("Can't get ToolSvc");
      return StatusCode::FAILURE;
    }
    
    ///+++ Grab the JetTrackVertexHelperTool
    if ( toolSvc->retrieveTool("PileupRTT::PileupValidationTools", m_pileupRTTTool).isFailure() ) {
      ATH_MSG_ERROR("Creation of algTool PileupValidationTools FAILED !!");
      return StatusCode::RECOVERABLE;
    }

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

    ATH_MSG_INFO("PileupRTT::PileupSCTOccupancy::CBNT_initialize() successful");
    return StatusCode::SUCCESS;
  }

  ///=============================================
  /// Finalize:
  /// delete any memory allocation from the heap
  ///=============================================
  StatusCode PileupRTT::PileupSCTOccupancy::finalize() {
    ATH_MSG_INFO("In PileupRTT::PileupSCTOccupancy::finalize()");


  ///=============================================
  /// Clear:
  /// clear CBNT members
  ///=============================================
  
    ATH_MSG_DEBUG("clear members");
    return StatusCode::SUCCESS;
  }

  ///=============================================
  /// Execute:
  /// on event by event
  ///=============================================
  StatusCode PileupRTT::PileupSCTOccupancy::execute()
  {

    ATH_MSG_DEBUG("in execute()");
    StatusCode sc;

    ///+++ Do SCT Occupancy
    sc = doSCTOccupancy();
    if (sc.isFailure()) {
      ATH_MSG_ERROR("The method doSCTOccupancy() failed");
      return StatusCode::RECOVERABLE;
    }
    else ATH_MSG_INFO("doSCTOccupancy() successful!");

    return StatusCode::SUCCESS;
  }

  ///=============================================
  /// Pileup RDO Validation
  ///     "doSCTOccupancy"
  /// --> Get SCT Occupancy Info
  ///=============================================

  StatusCode PileupRTT::PileupSCTOccupancy::doSCTOccupancy() {

    ATH_MSG_DEBUG("doSCTOccupancy()");
    StatusCode sc;

    // Get the SCT helper from the detector store
    sc = m_detStore->retrieve(m_sctID, "SCT_ID");
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Could not get SCT_ID helper !");
      return StatusCode::SUCCESS;
    }
    else {
      ATH_MSG_DEBUG(" Found the SCT_ID helper. ");
    }

    // retrieve SCT RDO container from storegate
    ATH_MSG_DEBUG("Now trying to grab the SCT RDO Container");
    const DataHandle<SCT_RDO_Container> sct_rdocontainer;
    sc = m_storeGate->retrieve(sct_rdocontainer, m_SCT_RDOName);
    if (sc != StatusCode::SUCCESS || !sct_rdocontainer){
      ATH_MSG_ERROR("Could not find the data object " << m_SCT_RDOName << " !");
      return StatusCode::SUCCESS;
    } else {
      ATH_MSG_DEBUG("SCT RDO Container found");
    }

    //loop on the RDO collections
    SCT_RDO_Container::const_iterator rdo_it = sct_rdocontainer->begin();

    for(; rdo_it != sct_rdocontainer->end(); ++rdo_it){
    
      const InDetRawDataCollection<SCT_RDORawData>* rdo_coll = &(**rdo_it);
        if (!rdo_coll){
          continue;
        }
        //get the vector of the RDOs
        DataVector<SCT_RDORawData>::const_iterator sct_rdo;
        for(sct_rdo=rdo_coll->begin(); sct_rdo!=rdo_coll->end(); ++sct_rdo){
          Identifier rdo_Id = (*sct_rdo)->identify();
          if(m_sctID->is_barrel(rdo_Id)){
            int layer = m_sctID->layer_disk(rdo_Id);
            int eta_module = m_sctID->eta_module(rdo_Id);
            int phi_module = m_sctID->phi_module(rdo_Id);
            int cluster_size = (*sct_rdo)->getGroupSize();
            if(layer==0) h_sct_lay0_occupancy->Fill(eta_module,phi_module,cluster_size);
            if(layer==1) h_sct_lay1_occupancy->Fill(eta_module,phi_module,cluster_size);
            if(layer==2) h_sct_lay2_occupancy->Fill(eta_module,phi_module,cluster_size);
            if(layer==3) h_sct_lay3_occupancy->Fill(eta_module,phi_module,cluster_size);
          }
        }
    }
    return StatusCode::SUCCESS;
  }

  ///=================
  /// Book Histograms
  ///=================
  StatusCode PileupRTT::PileupSCTOccupancy::bookHistograms() {

    StatusCode sc;
    ATH_MSG_DEBUG("In PileupRTT::PileupSCTOccupancy::bookHistograms()");

    h_sct_lay0_occupancy = new TH2D("sct_B0_occupancy", "Barrel layer 0 occupancy", 13,-6.5,6.5,32,0,32);
    h_sct_lay1_occupancy = new TH2D("sct_B1_occupancy", "barrel layer 1 occupancy", 13,-6.5,6.5,40,0,40);
    h_sct_lay2_occupancy = new TH2D("sct_B2_occupancy", "Barrel layer 2 occupancy", 13,-6.5,6.5,48,0,48);
    h_sct_lay3_occupancy = new TH2D("sct_B3_occupancy", "Barrel layer 3 occupancy", 13,-6.5,6.5,56,0,56);

    sc = m_thistSvc->regHist("/AANT/SCT/lay0_sct_occ",h_sct_lay0_occupancy);
    sc = m_thistSvc->regHist("/AANT/SCT/lay1_sct_occ",h_sct_lay1_occupancy);
    sc = m_thistSvc->regHist("/AANT/SCT/lay2_sct_occ",h_sct_lay2_occupancy);
    sc = m_thistSvc->regHist("/AANT/SCT/lay3_sct_occ",h_sct_lay3_occupancy);
    
    if (sc.isFailure()) {
      ATH_MSG_ERROR("ROOT Hist registration failed!");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;

  }

  ///======================
  /// Add Branches to AANT
  ///======================
  StatusCode PileupRTT::PileupSCTOccupancy::addBranches() {

    ///=======================================
    ///+++ Add branches and leaves to the tree
    ATH_MSG_DEBUG("Adding Branches to custom AANT");
    return StatusCode::SUCCESS;
  }

} //end of namespace declaration
