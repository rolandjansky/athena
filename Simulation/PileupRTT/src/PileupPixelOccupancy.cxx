/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///==================================================
///
/// PileupPixelOccupancy.h, (c) ATLAS Detector software
///
/// Validation for Pileup Production
///
/// Authors : David W. Miller
/// In Collaboration with  : Sven Vahsen
///
/// Date : 16 April 2007
///
///==================================================

#include "PileupRTT/PileupPixelOccupancy.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TH2.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetRawData/PixelRDO_Container.h"

#include "InDetRawData/InDetRawDataContainer.h"

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
  PileupRTT::PileupPixelOccupancy::PileupPixelOccupancy(const std::string& name,ISvcLocator* pSvcLocator) : 
    AthAlgorithm(name, pSvcLocator),

    m_pix_BCID(0),
    m_pix_ToT(0),
    m_pix_Lvl1ID(0),
    m_pix_Lvl1A(0),
    m_pix_word(0),
    m_pix_num_hits(0),

    m_theTree(0),
    m_thistSvc(0),
    
    m_Pixel_RDOName("PixelRDOs") 
  
  {

    //+++ Container Names
    declareProperty("PIX_RDOName", m_Pixel_RDOName = "PixelRDOs");
    
    //+++ Flag for saving all pixel information
    declareProperty("DoAllPixels", m_doAllPixels = true);
  }

  ///============
  /// DESTRUCTOR
  ///============
  PileupRTT::PileupPixelOccupancy::~PileupPixelOccupancy() { }


  ///=============================================
  /// Initialize
  /// --> initialize StoreGate
  /// --> get a handle on the analysis tools
  /// --> book histograms
  ///=============================================
  StatusCode PileupRTT::PileupPixelOccupancy::initialize()
  {

    ATH_MSG_INFO("In PileupRTT::PileupPixelOccupancy::CBNT_initialize()");

    //+++ Get handle to StoreGate (Persistency) Service
    StatusCode sc = service("StoreGateSvc", m_storeGate);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Unable to retrieve pointer to StoreGateSvc");
        return StatusCode::FAILURE;
    }

    //+++ get DetectorStore service
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

    m_pix_BCID = new std::vector<double>;
    m_pix_ToT = new std::vector<double>;
    m_pix_Lvl1ID= new std::vector<double>;
    m_pix_Lvl1A = new std::vector<double>;
    m_pix_word = new std::vector<double>;
    m_pix_num_hits = new std::vector<int>;

    ///+++ Book ROOT histograms
    sc = this->bookHistograms();
    if (sc.isFailure()) {
      ATH_MSG_ERROR("bookHistograms failed!");
      return StatusCode::FAILURE;
    }

    //+++ Add branches and leaves to the tree
    sc = this->addBranches();
    if (sc.isFailure()) {
      ATH_MSG_ERROR("addBranches failed!");
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("PileupRTT::PileupPixelOccupancy::CBNT_initialize() successful");
    return StatusCode::SUCCESS;
  }

  ///=============================================
  /// Finalize:
  /// delete any memory allocation from the heap
  ///=============================================
  StatusCode PileupRTT::PileupPixelOccupancy::finalize() {
    ATH_MSG_INFO("In PileupRTT::PileupPixelOccupancy::CBNT_finalize()");   

  ///=============================================
  /// Clear:
  /// clear CBNT members
  ///=============================================

  ATH_MSG_DEBUG("clear members");
    
    m_pix_BCID->clear();
    m_pix_ToT->clear();
    m_pix_Lvl1ID->clear();
    m_pix_Lvl1A->clear();
    m_pix_word->clear();
    m_pix_num_hits->clear();
    
    return StatusCode::SUCCESS;
  }

  ///=============================================
  /// Execute:
  /// on event by event
  ///=============================================
  StatusCode PileupRTT::PileupPixelOccupancy::execute()
  {

    ATH_MSG_DEBUG("in execute()");
    StatusCode sc;

    ///+++ Read the Pileup RDO
    sc = doPixelOccupancy();
    if (sc.isFailure()) {
      ATH_MSG_ERROR("The method doPixelOccupancy() failed");
      return StatusCode::RECOVERABLE;
    }
    else ATH_MSG_INFO("doPixelOccupancy() successful!");

    m_theTree->Fill();

    return StatusCode::SUCCESS;
  }

  ///=============================================
  /// Pileup RDO Validation
  ///     "doPixelOccupancy"
  /// --> Get Pixel Occupancy Info
  ///=============================================

  StatusCode PileupRTT::PileupPixelOccupancy::doPixelOccupancy() {

    ATH_MSG_INFO("doPixelOccupancy()");

    // Get the pixel helper from the detector store
    StatusCode sc = m_detStore->retrieve(m_pixelID,"PixelID");
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Could not get PixelID helper !");
      return StatusCode::SUCCESS;
    }
    else {
      ATH_MSG_DEBUG(" Found the PixelID helper. ");
    }

    // retrieve Pixel RDO container from storegate
    ATH_MSG_DEBUG("Now trying to grab the Pixel RDO Container");
    const DataHandle<PixelRDO_Container> p_rdocontainer;
    if(m_storeGate->contains<PixelRDO_Container>(m_Pixel_RDOName))
    {
      sc=m_storeGate->retrieve(p_rdocontainer,m_Pixel_RDOName);
      if (sc.isFailure() || !p_rdocontainer) {
        ATH_MSG_ERROR("Could not find the data object " << m_Pixel_RDOName << " !");
        return StatusCode::SUCCESS;
      } else {
        ATH_MSG_DEBUG("Pixel RDO Container found");
      }

      //loop over all elements in pixel RDO
      int nhits = 0;
      PixelRDO_Container::const_iterator colNext   = p_rdocontainer->begin();
      PixelRDO_Container::const_iterator lastCol   = p_rdocontainer->end();
      DataVector<PixelRDORawData>::const_iterator p_rdo;

      for (; colNext != lastCol; ++colNext) 
      {
          const InDetRawDataCollection<PixelRDORawData>* PixelCollection(*colNext);
          if (!PixelCollection) continue;
          for(p_rdo=PixelCollection->begin(); p_rdo!=PixelCollection->end(); ++p_rdo)
          {
              if (m_doAllPixels) {

                m_pix_BCID->push_back((*p_rdo)->getBCID() - (*p_rdo)->getToT());               //BCID       
                m_pix_ToT->push_back((*p_rdo)->getToT());                 //Time over Threshold
                m_pix_Lvl1ID->push_back((*p_rdo)->getLVL1ID());
                m_pix_Lvl1A->push_back((*p_rdo)->getLVL1A());
                m_pix_word->push_back((*p_rdo)->getWord());

              }
                
              Identifier rdoID = (*p_rdo)->identify();
              h_pix_occupancy->Fill(m_pixelID->eta_index(rdoID),m_pixelID->phi_index(rdoID)); //2D histo row vs. col for ALL modules

              if(m_pixelID->barrel_ec(rdoID)==2)
              {
                  h_ecA_occupancy->Fill(m_pixelID->layer_disk(rdoID),m_pixelID->phi_module(rdoID));
              }
              if(m_pixelID->barrel_ec(rdoID)==-2)
              {
                  h_ecC_occupancy->Fill(m_pixelID->layer_disk(rdoID),m_pixelID->phi_module(rdoID));
              }
              if(m_pixelID->barrel_ec(rdoID)==0)
              {
                  if(m_pixelID->layer_disk(rdoID)==0) h_bar_lay0_occupancy->Fill(m_pixelID->eta_module(rdoID),m_pixelID->phi_module(rdoID));
                  if(m_pixelID->layer_disk(rdoID)==1) h_bar_lay1_occupancy->Fill(m_pixelID->eta_module(rdoID),m_pixelID->phi_module(rdoID));
                  if(m_pixelID->layer_disk(rdoID)==2) h_bar_lay2_occupancy->Fill(m_pixelID->eta_module(rdoID),m_pixelID->phi_module(rdoID));
              }

          nhits++;
          }
      }
      m_pix_num_hits->push_back(nhits);          //total number of pixel hits per event

    }
    return StatusCode::SUCCESS;
  }

  ///=================
  /// Book Histograms
  ///=================
  StatusCode PileupRTT::PileupPixelOccupancy::bookHistograms() {

    StatusCode sc;
    ATH_MSG_DEBUG("Booking histograms");

    h_ecA_occupancy      = new TH2D("pixel_A_occupancy", "EC A occupancy", 3,0,3,48,0,48);
    h_ecC_occupancy      = new TH2D("pixel_C_occupancy", "EC C occupancy", 3,0,3,48,0,48);
    h_bar_lay0_occupancy = new TH2D("pixel_B0_occupancy", "Barrel layer 0 occupancy", 13,-6.5,6.5,22,0,22);
    h_bar_lay1_occupancy = new TH2D("pixel_B1_occupancy", "Barrel layer 1 occupancy", 13,-6.5,6.5,38,0,38);
    h_bar_lay2_occupancy = new TH2D("pixel_B2_occupancy", "Barrel layer 2 occupancy", 13,-6.5,6.5,52,0,52);
    h_pix_occupancy      = new TH2D("pixel_occupancy", "Pixel occupancy integrated over all modules", 144,0.,144.,328,0.,328.);

    sc = m_thistSvc->regHist("/AANT/Pixel/ecA_occ",     h_ecA_occupancy);
    sc = m_thistSvc->regHist("/AANT/Pixel/ecC_occ",     h_ecC_occupancy);
    sc = m_thistSvc->regHist("/AANT/Pixel/lay0_bar_occ",h_bar_lay0_occupancy);
    sc = m_thistSvc->regHist("/AANT/Pixel/lay1_bar_occ",h_bar_lay1_occupancy);
    sc = m_thistSvc->regHist("/AANT/Pixel/lay2_bar_occ",h_bar_lay2_occupancy);
    sc = m_thistSvc->regHist("/AANT/Pixel/pix_occ",     h_pix_occupancy);
    
    if (sc.isFailure()) {
      ATH_MSG_ERROR("ROOT Hist registration failed!");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  ///======================
  /// Add Branches to AANT
  ///======================
  StatusCode PileupRTT::PileupPixelOccupancy::addBranches() {

    ///=======================================
    ///+++ Add branches and leaves to the tree

    ATH_MSG_DEBUG("Adding Branches to custom AANT");

    m_theTree = new TTree("PixelOccupancyTree","PixelOccupancyTree");

    m_theTree->Branch("pix_ToT",m_pix_ToT);
    m_theTree->Branch("pix_Lvl1ID",m_pix_Lvl1ID);
    m_theTree->Branch("pix_Lvl1A",m_pix_Lvl1A);
    m_theTree->Branch("pix_word",m_pix_word);
    m_theTree->Branch("pix_BCID",m_pix_BCID);
    m_theTree->Branch("pix_num_hits",m_pix_num_hits);

    if (StatusCode::SUCCESS == m_thistSvc->regTree("/AANT/PixelOccupancyTree",m_theTree)) {
    ATH_MSG_INFO("Booked module ntuple");
  } else {
    ATH_MSG_ERROR("Unable to register module ntuple");
  }


    return StatusCode::SUCCESS;
  }

} //end of namespace declaration
