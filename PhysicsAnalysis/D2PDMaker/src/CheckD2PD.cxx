/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//===========================================================================
// Name    : CheckD2PD.h
// Package : offline/PhysicsAnalysis/D2PDMaker
// Author  : Karsten Koeneke
// Created : April 2009
//
// DESCRIPTION:
//
// Check some properties of the D2PD.
//
//===========================================================================

#define XXX std::cout << "I am here: " << __FILE__ << ":" << __LINE__ << std::endl;

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"
#include "TMath.h"

// The composite particle
#include "ParticleEvent/CompositeParticle.h"
#include "ParticleEvent/CompositeParticleContainer.h"

/// this class' header file
#include "D2PDMaker/CheckD2PD.h"

#include <stdint.h>
#include <algorithm>
#include <math.h>
#include <functional>

#include "AthenaKernel/Units.h"

using namespace Analysis;
using Athena::Units::GeV;


//===========================================================================
// Constructor
//===========================================================================
CheckD2PD::CheckD2PD(const std::string& name,
                     ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_storeGate(0),
  m_thistSvc(0),
 m_dpd_composite1_number(0),
 m_dpd_composite1_mass(0),
 m_dpd_composite1_pt(0),
 m_dpd_composite1_eta(0),
 m_dpd_composite1_nDaughter(0),
 m_dpd_composite1_daughters_pt(0),
 m_dpd_composite1_daughters_eta(0),
 m_dpd_composite2_number(0),
 m_dpd_composite2_mass(0),
 m_dpd_composite2_pt(0),
 m_dpd_composite2_eta(0),
 m_dpd_composite3_number(0),
 m_dpd_composite3_mass(0),
 m_dpd_composite3_pt(0),
 m_dpd_composite3_eta(0),
 m_dpd_composite4_number(0),
 m_dpd_composite4_mass(0),
 m_dpd_composite4_pt(0),
 m_dpd_composite4_eta(0),
 m_dpd_composite5_number(0),
 m_dpd_composite5_mass(0),
 m_dpd_composite5_pt(0),
 m_dpd_composite5_eta(0)
{
  /// switches to control the analysis through job options :: these are the default
  /// to changed in the job options

  /// The container name & selection cuts
  declareProperty( "CompositeParticleContainer1",  m_compPartCollName1 = "",
                   "The name (key) of the first composite particle collection" );
  declareProperty( "CompositeParticleContainer2",  m_compPartCollName2 = "",
                   "The name (key) of the second composite particle collection" );
  declareProperty( "CompositeParticleContainer3",  m_compPartCollName3 = "",
                   "The name (key) of the third composite particle collection" );
  declareProperty( "CompositeParticleContainer4",  m_compPartCollName4 = "",
                   "The name (key) of the fourth composite particle collection" );
  declareProperty( "CompositeParticleContainer5",  m_compPartCollName5 = "",
                   "The name (key) of the fifth composite particle collection" );
}




//===========================================================================
// Destructor - check up memory allocation
// delete any memory allocation on the heap
//===========================================================================

CheckD2PD::~CheckD2PD() {}




//===========================================================================
// Initialize
// initialize StoreGate
// get a handle on the analysis tools
// book histograms
//===========================================================================

StatusCode CheckD2PD::initialize() 
{

  MsgStream mLog( messageService(), name() );

  mLog << MSG::INFO
       << "Initializing CheckD2PD"
       << endreq;

  // StoreGate for EventStore
  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) 
    {
      mLog << MSG::ERROR
           << "Unable to retrieve pointer to StoreGateSvc"
           << endreq;
      return sc;
    }

  // The histogramming service
  sc = service("THistSvc", m_thistSvc);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR
           << "Unable to retrieve pointer to THistSvc"
           << endreq;
      return sc;
    }



  //====================================
  // the histograms
  //====================================
  // composite 1
  m_dpd_composite1_number = new TH1F("dpd_composite1_number", "composite1_number", 50, 0.0, 50.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite1_number", m_dpd_composite1_number);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite1_number registration failed" << endreq;
      return sc;
    }
  
  m_dpd_composite1_mass = new TH1F("dpd_composite1_mass", "composite1_mass", 200, 0.0, 200.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite1_mass", m_dpd_composite1_mass);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite1_mass registration failed" << endreq;
      return sc;
    }
  
  m_dpd_composite1_pt = new TH1F("dpd_composite1_pt", "composite1_pt", 200, 0.0, 200.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite1_pt", m_dpd_composite1_pt);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite1_pt registration failed" << endreq;
      return sc;
    }
  
  m_dpd_composite1_eta = new TH1F("dpd_composite1_eta", "composite1_eta", 100, -10.0, 10.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite1_eta", m_dpd_composite1_eta);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite1_eta registration failed" << endreq;
      return sc;
    }

  // composite 1 daughters
  m_dpd_composite1_nDaughter = new TH1F("dpd_composite1_nDaughter", "composite1_nDaughter", 50, 0.0, 50.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite1_nDaughter", m_dpd_composite1_nDaughter);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite1_nDaughter registration failed" << endreq;
      return sc;
    }
  
  m_dpd_composite1_daughters_pt = new TH1F("dpd_composite1_daughters_pt", "composite1_daughters_pt", 200, 0.0, 200.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite1_daughters_pt", m_dpd_composite1_daughters_pt);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite1_daughters_pt registration failed" << endreq;
      return sc;
    }
  
  m_dpd_composite1_daughters_eta = new TH1F("dpd_composite1_daughters_eta", "composite1_daughters_eta", 100, -10.0, 10.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite1_daughters_eta", m_dpd_composite1_daughters_eta);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite1_daughters_eta registration failed" << endreq;
      return sc;
    }
  


  // composite 2
  m_dpd_composite2_number = new TH1F("dpd_composite2_number", "composite2_number", 50, 0.0, 50.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite2_number", m_dpd_composite2_number);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite2_number registration failed" << endreq;
      return sc;
    }
  
  m_dpd_composite2_mass = new TH1F("dpd_composite2_mass", "composite2_mass", 200, 0.0, 200.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite2_mass", m_dpd_composite2_mass);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite2_mass registration failed" << endreq;
      return sc;
    }
  
  m_dpd_composite2_pt = new TH1F("dpd_composite2_pt", "composite2_pt", 200, 0.0, 200.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite2_pt", m_dpd_composite2_pt);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite2_pt registration failed" << endreq;
      return sc;
    }
  
  m_dpd_composite2_eta = new TH1F("dpd_composite2_eta", "composite2_eta", 100, -10.0, 10.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite2_eta", m_dpd_composite2_eta);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite2_eta registration failed" << endreq;
      return sc;
    }
  


  // composite 3
  m_dpd_composite3_number = new TH1F("dpd_composite3_number", "composite3_number", 50, 0.0, 50.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite3_number", m_dpd_composite3_number);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite3_number registration failed" << endreq;
      return sc;
    }
  
  m_dpd_composite3_mass = new TH1F("dpd_composite3_mass", "composite3_mass", 200, 0.0, 200.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite3_mass", m_dpd_composite3_mass);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite3_mass registration failed" << endreq;
      return sc;
    }
  
  m_dpd_composite3_pt = new TH1F("dpd_composite3_pt", "composite3_pt", 200, 0.0, 200.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite3_pt", m_dpd_composite3_pt);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite3_pt registration failed" << endreq;
      return sc;
    }
  
  m_dpd_composite3_eta = new TH1F("dpd_composite3_eta", "composite3_eta", 100, -10.0, 10.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite3_eta", m_dpd_composite3_eta);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite3_eta registration failed" << endreq;
      return sc;
    }
  


  // composite 4
  m_dpd_composite4_number = new TH1F("dpd_composite4_number", "composite4_number", 50, 0.0, 50.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite4_number", m_dpd_composite4_number);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite4_number registration failed" << endreq;
      return sc;
    }
  
  m_dpd_composite4_mass = new TH1F("dpd_composite4_mass", "composite4_mass", 200, 0.0, 200.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite4_mass", m_dpd_composite4_mass);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite4_mass registration failed" << endreq;
      return sc;
    }
  
  m_dpd_composite4_pt = new TH1F("dpd_composite4_pt", "composite4_pt", 200, 0.0, 200.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite4_pt", m_dpd_composite4_pt);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite4_pt registration failed" << endreq;
      return sc;
    }
  
  m_dpd_composite4_eta = new TH1F("dpd_composite4_eta", "composite4_eta", 100, -10.0, 10.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite4_eta", m_dpd_composite4_eta);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite4_eta registration failed" << endreq;
      return sc;
    }
  


  // composite 5
  m_dpd_composite5_number = new TH1F("dpd_composite5_number", "composite5_number", 50, 0.0, 50.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite5_number", m_dpd_composite5_number);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite5_number registration failed" << endreq;
      return sc;
    }
  
  m_dpd_composite5_mass = new TH1F("dpd_composite5_mass", "composite5_mass", 200, 0.0, 200.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite5_mass", m_dpd_composite5_mass);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite5_mass registration failed" << endreq;
      return sc;
    }
  
  m_dpd_composite5_pt = new TH1F("dpd_composite5_pt", "composite5_pt", 200, 0.0, 200.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite5_pt", m_dpd_composite5_pt);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite5_pt registration failed" << endreq;
      return sc;
    }
  
  m_dpd_composite5_eta = new TH1F("dpd_composite5_eta", "composite5_eta", 100, -10.0, 10.0);
  sc = m_thistSvc->regHist("/AANT/d2pd/dpd_composite5_eta", m_dpd_composite5_eta);
  if (sc.isFailure())
    {
      mLog << MSG::ERROR << "ROOT Hist m_dpd_composite5_eta registration failed" << endreq;
      return sc;
    }
  



  return sc;
}		 




//===========================================================================
// Finalize - delete any memory allocation from the heap
//===========================================================================

StatusCode CheckD2PD::finalize() 
{
  //-----------------------------------------
  // Declare the simple StatusCode
  //-----------------------------------------
  StatusCode sc = StatusCode::SUCCESS ;

  MsgStream mLog( messageService(), name() );
  
  return sc;
  
}




//===========================================================================
// Execute - called by the event loop on event by event
//===========================================================================

StatusCode CheckD2PD::execute()
{
  MsgStream mLog( messageService(), name() );

  mLog << MSG::DEBUG << "execute()" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  // do the actual checking of the D2PD
  sc = checkD2PD();
  if ( sc.isFailure() )
    {
      mLog << MSG::ERROR << "The check of the D2PD failed" << endreq;
      sc = StatusCode::SUCCESS;
    }

  return sc;
  
}




//===========================================================================
// zee on esd: called by execute()
//===========================================================================

StatusCode CheckD2PD::checkD2PD()
{
  MsgStream mLog( messageService(), name() );

  mLog << MSG::DEBUG << "checkD2PD()" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  // read the DPD composite particle container from persistecy storage
  const CompositeParticleContainer* compPartCont1(0);
  const CompositeParticleContainer* compPartCont2(0);
  const CompositeParticleContainer* compPartCont3(0);
  const CompositeParticleContainer* compPartCont4(0);
  const CompositeParticleContainer* compPartCont5(0);
  sc=m_storeGate->retrieve( compPartCont1, m_compPartCollName1 );
  if( sc.isFailure()  ||  !compPartCont1 )
    {
      mLog << MSG::WARNING
           << "No DPD Composite Particle Container 1 found"
           << endreq; 
      return sc;
    }  
  mLog << MSG::DEBUG << "CompositeParticleContainer1 successfully retrieved! It has " << compPartCont1->size() << " entries." << endreq;
  
  if ( m_compPartCollName2 != "" )
    {
      sc=m_storeGate->retrieve( compPartCont2, m_compPartCollName2 );
      if( sc.isFailure()  ||  !compPartCont2 )
        {
          mLog << MSG::WARNING
               << "No DPD Composite Particle Container 2 found"
               << endreq; 
          return sc;
        }  
      mLog << MSG::DEBUG << "CompositeParticleContainer2 successfully retrieved! It has " << compPartCont2->size() << " entries." << endreq;
    }

  if ( m_compPartCollName3 != "" )
    {
      sc=m_storeGate->retrieve( compPartCont3, m_compPartCollName3 );
      if( sc.isFailure()  ||  !compPartCont3 )
        {
          mLog << MSG::WARNING
               << "No DPD Composite Particle Container 3 found"
               << endreq; 
          return sc;
        }  
      mLog << MSG::DEBUG << "CompositeParticleContainer3 successfully retrieved! It has " << compPartCont3->size() << " entries." << endreq;
    }

  if ( m_compPartCollName4 != "" )
    {
      sc=m_storeGate->retrieve( compPartCont4, m_compPartCollName4 );
      if( sc.isFailure()  ||  !compPartCont4 )
        {
          mLog << MSG::WARNING
               << "No DPD Composite Particle Container 4 found"
               << endreq; 
          return sc;
        }  
      mLog << MSG::DEBUG << "CompositeParticleContainer4 successfully retrieved! It has " << compPartCont4->size() << " entries." << endreq;
    }

  if ( m_compPartCollName5 != "" )
    {
      sc=m_storeGate->retrieve( compPartCont5, m_compPartCollName5 );
      if( sc.isFailure()  ||  !compPartCont5 ) 
        {
          mLog << MSG::WARNING
               << "No DPD Composite Particle Container 5 found"
               << endreq; 
          return sc;
        }  
      mLog << MSG::DEBUG << "CompositeParticleContainer5 successfully retrieved! It has " << compPartCont5->size() << " entries." << endreq;
    }



  // Get the number of composite particles per event
  unsigned int nCompPart1 = compPartCont1->size();
  m_dpd_composite1_number->Fill( nCompPart1, 1.);

  // iterators over the container 
  CompositeParticleContainer::const_iterator compPart1Itr    = compPartCont1->begin();
  CompositeParticleContainer::const_iterator compPart1ItrEnd = compPartCont1->end();

  // loop over the composite particle container
  // and fill the mass, pt, and eta histograms
  for (; compPart1Itr != compPart1ItrEnd; ++compPart1Itr )
    {
      // Get the current electron
      const CompositeParticle* compPart1 = *compPart1Itr;

      // Fill basic electron properties into a histogram
      m_dpd_composite1_mass->Fill( compPart1->m()/GeV, 1.0 );
      m_dpd_composite1_pt->Fill( compPart1->pt()/GeV, 1.0 );
      m_dpd_composite1_eta->Fill( compPart1->eta(), 1.0 );

      // Get the number of daughters
      m_dpd_composite1_nDaughter->Fill( compPart1->navigableBase().size(), 1.0 );

      // Loop over the daughters and fill the daughter histograms
      CompositeParticle::ConstituentsIter_t comp1DaughterItr    = compPart1->constituents_begin();
      CompositeParticle::ConstituentsIter_t comp1DaughterItrEnd = compPart1->constituents_end();
      for (; comp1DaughterItr != comp1DaughterItrEnd; ++comp1DaughterItr )
        {
          // Get the current daughter particle
          const INavigable4Momentum* daughterPart = *comp1DaughterItr;

          // Fill the daughter histograms
          m_dpd_composite1_daughters_pt->Fill( daughterPart->pt()/GeV, 1.0 );
          m_dpd_composite1_daughters_eta->Fill( daughterPart->eta(), 1.0 );
        }
    }


  if ( m_compPartCollName2 != "" )
    {
      // Get the number of composite particles per event
      unsigned int nCompPart2 = compPartCont2->size();
      m_dpd_composite2_number->Fill( nCompPart2, 1.);
  
      // iterators over the container 
      CompositeParticleContainer::const_iterator compPart2Itr    = compPartCont2->begin();
      CompositeParticleContainer::const_iterator compPart2ItrEnd = compPartCont2->end();
      
      // loop over the composite particle container
      // and fill the mass, pt, and eta histograms
      for (; compPart2Itr != compPart2ItrEnd; ++compPart2Itr )
        {
          // Get the current electron
          const CompositeParticle* compPart2 = *compPart2Itr;
	  
          // Fill basic electron properties into a histogram
          m_dpd_composite2_mass->Fill( compPart2->m()/GeV, 1.);
          m_dpd_composite2_pt->Fill( compPart2->pt()/GeV, 1.);
          m_dpd_composite2_eta->Fill( compPart2->eta(), 1.);
        }
    }

  if ( m_compPartCollName3 != "" )
    {
      // Get the number of composite particles per event
      unsigned int nCompPart3 = compPartCont3->size();
      m_dpd_composite3_number->Fill( nCompPart3, 1.);
  
      // iterators over the container 
      CompositeParticleContainer::const_iterator compPart3Itr    = compPartCont3->begin();
      CompositeParticleContainer::const_iterator compPart3ItrEnd = compPartCont3->end();
      
      // loop over the composite particle container
      // and fill the mass, pt, and eta histograms
      for (; compPart3Itr != compPart3ItrEnd; ++compPart3Itr )
        {
          // Get the current electron
          const CompositeParticle* compPart3 = *compPart3Itr;
	  
          // Fill basic electron properties into a histogram
          m_dpd_composite3_mass->Fill( compPart3->m()/GeV, 1.);
          m_dpd_composite3_pt->Fill( compPart3->pt()/GeV, 1.);
          m_dpd_composite3_eta->Fill( compPart3->eta(), 1.);
        }
    }


  if ( m_compPartCollName4 != "" )
    {
      // Get the number of composite particles per event
      unsigned int nCompPart4 = compPartCont4->size();
      m_dpd_composite4_number->Fill( nCompPart4, 1.);
  
      // iterators over the container 
      CompositeParticleContainer::const_iterator compPart4Itr    = compPartCont4->begin();
      CompositeParticleContainer::const_iterator compPart4ItrEnd = compPartCont4->end();
      
      // loop over the composite particle container
      // and fill the mass, pt, and eta histograms
      for (; compPart4Itr != compPart4ItrEnd; ++compPart4Itr )
        {
          // Get the current electron
          const CompositeParticle* compPart4 = *compPart4Itr;
	  
          // Fill basic electron properties into a histogram
          m_dpd_composite4_mass->Fill( compPart4->m()/GeV, 1.);
          m_dpd_composite4_pt->Fill( compPart4->pt()/GeV, 1.);
          m_dpd_composite4_eta->Fill( compPart4->eta(), 1.);
        }
    }


  if ( m_compPartCollName5 != "" )
    {
      // Get the number of composite particles per event
      unsigned int nCompPart5 = compPartCont5->size();
      m_dpd_composite5_number->Fill( nCompPart5, 1.);
  
      // iterators over the container 
      CompositeParticleContainer::const_iterator compPart5Itr    = compPartCont5->begin();
      CompositeParticleContainer::const_iterator compPart5ItrEnd = compPartCont5->end();
      
      // loop over the composite particle container
      // and fill the mass, pt, and eta histograms
      for (; compPart5Itr != compPart5ItrEnd; ++compPart5Itr )
        {
          // Get the current electron
          const CompositeParticle* compPart5 = *compPart5Itr;
	  
          // Fill basic electron properties into a histogram
          m_dpd_composite5_mass->Fill( compPart5->m()/GeV, 1.);
          m_dpd_composite5_pt->Fill( compPart5->pt()/GeV, 1.);
          m_dpd_composite5_eta->Fill( compPart5->eta(), 1.);
        }
    }



  return sc;
}

