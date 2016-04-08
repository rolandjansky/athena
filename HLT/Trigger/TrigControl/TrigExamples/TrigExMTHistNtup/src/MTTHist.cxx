/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigExMTHistNtup/MTTHist.h"

#include "GaudiKernel/ISvcLocator.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "GaudiKernel/IHistogramSvc.h"

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ThreadGaudi.h"

#include <cmath>
#include <cstdlib>

#include "TH1F.h"
#include "TH2D.h"
#include "TTree.h"

#include "TrigMonitorBase/TrigLockedHist.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  
MTTHist::MTTHist(const std::string & name, ISvcLocator * pSvcLocator) :
   AthAlgorithm(name, pSvcLocator),
   m_h1fTest(0),
   m_h2muMon(0),
   m_timing(0),
   m_evdata(0),
   m_hGlobal(0),
   m_hCustom(0),
   m_previousEvent(0),
   m_event(0),
   m_eventDistance(0),
   m_testDouble(0.),
   m_testFloat(0.),
   m_dataTime(0.),
   m_processTime(0.),
   m_eventCounter(0)
{
  declareProperty("addBinEveryNEvents", m_addBin = 0,
                  "Double the number of bins every N events");

  declareProperty("nHistsForBinTest", m_nHistsBinTest = 1,
                  "Number of histograms for binning test");

  declareProperty("useMutex", m_useMutex = true,
                  "Use histmon mutex to lock histogram access");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  

StatusCode MTTHist::beginRun() {
    StatusCode sc = StatusCode::SUCCESS;

    // get the  histogramming service 
    ITHistSvc *rootHistSvc;
    if (service("THistSvc", rootHistSvc).isFailure()) {
	ATH_MSG_ERROR("Unable to locate THistSvc");
	return StatusCode::FAILURE;
    }
    /**
     * create histograms 
     */
    m_h1fTest   = new TH1F("h1ftest", "Mu eta / mu phi;;entries", 100, -0.6, 0.6);
    m_h2muMon   = new TH2D("muMon", "Mu eta .vs. mu phi;eta[rad];phi[rad];entries", 11, 0, 11, 36, 0, 36);
    m_hGlobal   = new TH1F("hGlobal", "Global events info;strange random value;entries", 32, 0, 32);  

    m_h1binTest.reserve(m_nHistsBinTest);
    for (size_t i=0; i<m_nHistsBinTest; ++i) {
      TString name("binTest");
      name += i;
      m_h1binTest.push_back(new TH1F(name.Data(), "Variable number of bins;;entries", 1, 0, 1));
    }
    
    /**
     * book the histograms then
     */
    std::string monPath = "/SHIFT/"+getGaudiThreadGenericName(name())+"/";
    std::string exmonPath = "/EXPERT/"+getGaudiThreadGenericName(name())+"/";
    std::string summaryPath = "/RUNSTAT/"+getGaudiThreadGenericName(name())+"/";

    if ( rootHistSvc->regHist( monPath + m_h1fTest->GetName(), m_h1fTest ).isFailure() ) 
	ATH_MSG_WARNING("Can't book " 
	    << monPath + m_h1fTest->GetName());

    if ( rootHistSvc->regHist( exmonPath + m_h2muMon->GetName(), m_h2muMon ).isFailure() )
	ATH_MSG_WARNING("Can't book " 
	    << exmonPath + m_h2muMon->GetName());

    for (size_t i=0; i<m_nHistsBinTest; ++i) {
      if ( rootHistSvc->regHist( monPath + m_h1binTest[i]->GetName(), m_h1binTest[i] ).isFailure() ) 
	ATH_MSG_WARNING("Can't book " 
	    << monPath + m_h1binTest[i]->GetName());
    }
    
    // or just statically choosen name
    if ( rootHistSvc->regHist( summaryPath + m_hGlobal->GetName(), m_hGlobal ).isFailure() )
	ATH_MSG_WARNING("Can't book " 
	    << summaryPath + m_hGlobal->GetName());

    /**
     * The example below is showing how to make use of the status returned by 
     * THistSvc in order to save on space/fill time.
     * In order to see this hisograms one needs to modify TrigMonTHistSvc setup 
     * (let it pass though IncludeName regex, and set right configuration in partition)
     * In Lvl2THistSvc regex needs to be adjusted and output file asigned.
     */
    std::string myCustomPath = "/MMTHistCustomDir/";
    m_hCustom  = new TH1F("hCustom", "Custom information", 32, 0, 32);
    if ( rootHistSvc->regHist( myCustomPath + m_hCustom->GetName(), m_hCustom ).isFailure() ) {
       ATH_MSG_WARNING("Can't book " <<  myCustomPath + m_hCustom->GetName() << " will delete it and save on filling");
       delete m_hCustom;
       m_hCustom = 0;
    }




    /**
     * create & book ntuple ( TTree in ROOT language )
     */
    m_timing = new TTree("timing", "events processing timing");
    m_timing->Branch("dataTime", &m_dataTime, "dataTime/F");
    m_timing->Branch("processTime", &m_processTime, "processTime/F");

    if ( rootHistSvc->regTree(exmonPath+"timing", m_timing).isFailure() )
	ATH_MSG_WARNING("Can't book " 
	    << exmonPath+"timing");

    m_evdata = new TTree("evdata", "event base data");
    m_evdata->Branch("eventDistance", &m_eventDistance, "eventDistance/I");
    m_evdata->Branch("testDouble", &m_testDouble, "testDouble/D");
    m_evdata->Branch("testFloat", &m_testFloat, "testFloat/F");
    m_evdata->Branch("event", &m_event, "event/F");
  
    if ( rootHistSvc->regTree(exmonPath+"evdata", m_evdata).isFailure() )
	ATH_MSG_WARNING("Can't book " << exmonPath+"evdata");
    return sc;
}

StatusCode MTTHist::initialize() {
    ATH_MSG_INFO("in initialize()");
    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MTTHist::execute() {

   m_eventCounter++;

   ATH_MSG_DEBUG("in execute()");

   // Get the Event
   const EventInfo *pevt;
   if (StatusCode::SUCCESS != evtStore()->retrieve(pevt, ""))
      return StatusCode::FAILURE;


   // some fake calulations
   int run;
   m_event = pevt->event_ID()->event_number();
   run = pevt->event_ID()->run_number();

   m_eventDistance = m_event - m_previousEvent;

   // fill the histogram
   m_h2muMon->Fill(m_eventDistance, run);
   m_h1fTest->Fill(float (m_eventDistance) / 100., 1.);

   // calculate some more quantities
   m_testDouble = (m_event%2) ? 5.11 : 1.234;
   m_testFloat = ((m_eventDistance+m_event)%1000)/1000.;

   m_evdata->Fill();

   // adding bins on the fly
   for (size_t i=0; i<m_nHistsBinTest; ++i) {
      TString label("");
      if (m_addBin>0) label += (m_eventCounter/m_addBin);
      else label = "1";
      if (m_useMutex) {
         lock_histogram_operation<TH1F>(m_h1binTest[i])->Fill(label, random() % 10);
      }
      else {
         m_h1binTest[i]->Fill(label, random() % 10);
      }
   }

   // some fake data to be put in m_timing TTree
   m_dataTime = 0.1*m_eventDistance;
   m_processTime = pow(m_testFloat, 2);
   m_timing->Fill();

   // you can do more fills per one execute() call
   m_processTime = pow(m_testFloat, 3);
   m_timing->Fill();

   ATH_MSG_DEBUG(" fill the TTree " << m_timing->GetEntries());
   m_previousEvent = m_event;


   // fill of custom histogram (notice we check if it is not null)
   if ( m_hCustom ) m_hCustom->Fill(random()%32);

   return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MTTHist::finalize(){
   ATH_MSG_INFO("in finalize()");
   return StatusCode::SUCCESS;
}
