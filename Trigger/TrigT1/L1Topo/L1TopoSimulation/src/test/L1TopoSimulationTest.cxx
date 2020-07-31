/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./L1TopoSimulationTest.h"
#include "../AthenaL1TopoHistSvc.h"

#include "L1TopoCoreSim/TopoSteering.h"
#include "L1TopoConfig/L1TopoMenu.h"
#include "L1TopoEvent/TopoInputEvent.h"
#include "L1TopoConfig/L1TopoXMLParser.h"
#include "L1TopoInterfaces/IL1TopoHistSvc.h"

#include "GaudiKernel/ITHistSvc.h"


using namespace std;


LVL1::L1TopoSimulationTest::L1TopoSimulationTest(const std::string &name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_OffhistSvc( "THistSvc/THistSvc", name),
  m_OfftopoSteering( unique_ptr<TCS::TopoSteering>(new TCS::TopoSteering()) )
{
  declareProperty( "InputXMLFile", m_OffinputXMLFile, "File name for menu XML");
  declareProperty( "InputASCIIFile", m_OffinputASCIIFile, "File name for ASCII TOB vector");
  declareProperty( "HistSvc", m_OffhistSvc, "Histogramming service for L1Topo algorithms");
  declareProperty( "MonHistBaseDir", m_OffhistBaseDir = "L1TopoAlgorithms", "Base directory for monitoring histograms will be /EXPERT/<MonHistBaseDir>" );
  declareProperty( "TopoOutputLevel", m_OfftopoOutputLevel, "OutputLevel for L1Topo algorithms" );
  declareProperty( "TopoSteeringOutputLevel", m_OfftopoSteeringOutputLevel, "OutputLevel for L1Topo steering" );
}


LVL1::L1TopoSimulationTest::~L1TopoSimulationTest()
{}


StatusCode
LVL1::L1TopoSimulationTest::initialize() {
  ATH_MSG_INFO("initialize");

  m_OfftopoSteering->setMsgLevel( TrigConf::MSGTC::Level(m_OfftopoSteeringOutputLevel) );

  if (m_OffinputXMLFile.empty()){
    ATH_MSG_FATAL("No L1 Topo menu from XML " << m_OffinputXMLFile);
    return StatusCode::FAILURE;
  }
  TXC::L1TopoXMLParser XMLParser;
  XMLParser.msg().setLevel( TrigConf::MSGTC::Level(m_OfftopoOutputLevel) );
  XMLParser.readConfiguration(m_OffinputXMLFile);
  XMLParser.parseConfiguration();

  m_OfftopoSteering->setupFromConfiguration(XMLParser.takeMenu());
  m_OfftopoSteering->setAlgMsgLevel( TrigConf::MSGTC::Level(m_OfftopoOutputLevel) );

  
  std::shared_ptr<IL1TopoHistSvc> topoHistSvc = std::shared_ptr<IL1TopoHistSvc>( new AthenaL1TopoHistSvc(m_OffhistSvc) );
   topoHistSvc->setBaseDir("/EXPERT/" + m_OffhistBaseDir.value());
  
   
   m_OfftopoSteering->setHistSvc(topoHistSvc);
   
   try {
      m_OfftopoSteering->initializeAlgorithms();
   }
   catch(exception & e) {
      ATH_MSG_FATAL("Caught exception when initializing topo algorithms" << e.what() );
      return StatusCode::FAILURE;
   }

   m_Offreader.setVerbosity(0); // disable print to screen

   // load ascii event file
   m_Offreader.loadInput(m_OffinputASCIIFile);
   m_Offreader.validateInput();

   // instantiate input event
   TCS::TopoInputEvent & inputEvent = m_OfftopoSteering->inputEvent();
   m_Offreader.setInputEvent(&inputEvent);

   return StatusCode::SUCCESS;
}

StatusCode
LVL1::L1TopoSimulationTest::execute() {

   // reset input and internal state
   m_OfftopoSteering->reset();

   // Obtain next events in the dump txt
   m_Offreader.getNextEvent();

   // execute the toposteering
   m_OfftopoSteering->executeEvent();

   return StatusCode::SUCCESS;
}

StatusCode
LVL1::L1TopoSimulationTest::finalize() {
   return StatusCode::SUCCESS;
}
