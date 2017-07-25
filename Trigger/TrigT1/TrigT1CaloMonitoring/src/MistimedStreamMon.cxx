/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "SGTools/StlVectorClids.h"

#include "LWHists/LWHist.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile_LW.h"
#include "LWHists/TProfile2D_LW.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "TrigT1CaloEvent/TriggerTower_ClassDEF.h"
#include "TrigT1CaloUtils/DataError.h"

#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"
#include "xAODTrigL1Calo/CPMTowerContainer.h"
#include "xAODTrigL1Calo/JetElementContainer.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"


#include "MistimedStreamMon.h"
// ============================================================================
namespace LVL1 {
// ============================================================================
MistimedStreamMon::MistimedStreamMon(const std::string & type, const std::string & name, const IInterface* parent): ManagedMonitorToolBase ( type, name, parent ),
      m_histBooked(false),
      m_selectedEventCounter(0),
      m_errorTool("LVL1::TrigT1CaloMonErrorTool/TrigT1CaloMonErrorTool"),
      m_histTool("LVL1::TrigT1CaloLWHistogramTool/TrigT1CaloLWHistogramTool"),
      m_ttTool("LVL1::L1TriggerTowerTool/L1TriggerTowerTool"), // can provide coolID, prob. not needed
      m_h_1d_cutFlow_mistimedStreamAna(0),
      m_h_1d_selectedEvents_mistimedStreamAna(0),
      m_v_em_2d_etaPhi_tt_classification_mistimedStreamAna(0),
      m_v_had_2d_etaPhi_tt_classification_mistimedStreamAna(0),
      m_v_em_2d_etaPhi_tt_pseBits_mistimedStreamAna(0),
      m_v_had_2d_etaPhi_tt_pseBits_mistimedStreamAna(0),
      m_v_em_2d_etaPhi_tt_lut_cp0_mistimedStreamAna(0),
      m_v_em_2d_etaPhi_tt_lut_cp1_mistimedStreamAna(0),
      m_v_em_2d_etaPhi_tt_lut_cp2_mistimedStreamAna(0),
      m_v_had_2d_etaPhi_tt_lut_cp0_mistimedStreamAna(0),
      m_v_had_2d_etaPhi_tt_lut_cp1_mistimedStreamAna(0),
      m_v_had_2d_etaPhi_tt_lut_cp2_mistimedStreamAna(0),
      m_v_em_2d_etaPhi_tt_lut_jep0_mistimedStreamAna(0),
      m_v_em_2d_etaPhi_tt_lut_jep1_mistimedStreamAna(0),
      m_v_em_2d_etaPhi_tt_lut_jep2_mistimedStreamAna(0),
      m_v_had_2d_etaPhi_tt_lut_jep0_mistimedStreamAna(0),
      m_v_had_2d_etaPhi_tt_lut_jep1_mistimedStreamAna(0),
      m_v_had_2d_etaPhi_tt_lut_jep2_mistimedStreamAna(0) 
      
{
     declareProperty("PathInRootFile", m_PathInRootFile = "L1Calo/MistimedStream");
     declareProperty("BS_xAODTriggerTowerContainer",
                  m_xAODTriggerTowerContainerName =                     LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation);
}

MistimedStreamMon::~MistimedStreamMon()
{
}

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode MistimedStreamMon::initialize()
/*---------------------------------------------------------*/
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endmsg;

  CHECK(ManagedMonitorToolBase::initialize());
  CHECK(m_errorTool.retrieve());
  CHECK(m_histTool.retrieve());
  CHECK(m_ttTool.retrieve());

  std::cout<<"Here comes the MistimedStream Analysis"<<std::endl;
  
  return StatusCode::SUCCESS;

}

/*---------------------------------------------------------*/

StatusCode MistimedStreamMon::finalize()
{
  //delete things here
  return StatusCode::SUCCESS;
}

bool MistimedStreamMon::pulseQuality(std::vector<uint16_t> ttPulse, int peakSlice) {
     
    bool goodPulse = true;
    
    int a = ttPulse[peakSlice-1];
    int b = ttPulse[peakSlice];
    int c = ttPulse[peakSlice+1];
    double tim = (0.5*a-0.5*c)/(a+c-2*b);
    double wid = (a+c-64.0)/(b-32.0);
    if ( tim < 0.0 ) goodPulse = false;
    if ( tim > 0.3 ) goodPulse = false;
    if ( wid < 1.0 ) goodPulse = false;
    if ( wid > 1.6 ) goodPulse = false;             
//     std::cout<<"Pulse qual= "<< goodPulse<<" tim = "<<tim<<" wid = "<<wid <<std::endl;
//     std::cout<<"a = "<< a <<" b = "<<b<<" c = "<<c <<std::endl;
    return goodPulse;
}


/*---------------------------------------------------------*/
void MistimedStreamMon::bookEventHistograms(std::string number) 
/*---------------------------------------------------------*/
{
  // detailed histograms per selected event. Ideally booked before filling. if not possible, write method and create histos for the first 10 events (0 - 9), but not so pretty
  std::string name, title;
  name = "em_2d_etaPhi_tt_classification_mistimedStreamAna_event_" + number;
  title = "#eta - #phi Map of TT classification in the EM layer of selected event number " + number;
  TH2F_LW *emClass = m_histTool->bookPPMEmEtaVsPhi(name, title);
  m_v_em_2d_etaPhi_tt_classification_mistimedStreamAna.push_back(emClass);
  name = "had_2d_etaPhi_tt_classification_mistimedStreamAna_event_" + number;
  title = "#eta - #phi Map of TT classification in the HAD layer of selected event number " + number;
  TH2F_LW *hadClass = m_histTool->bookPPMEmEtaVsPhi(name, title);
  m_v_had_2d_etaPhi_tt_classification_mistimedStreamAna.push_back(hadClass);

  name = "em_2d_etaPhi_tt_pseBits_mistimedStreamAna_event_" + number;
  title = "#eta - #phi Map of TT PSE Bits in the EM layer of selected event number " + number;
  TH2F_LW *emPSE = m_histTool->bookPPMEmEtaVsPhi(name, title);
  m_v_em_2d_etaPhi_tt_pseBits_mistimedStreamAna.push_back(emPSE);
  name = "had_2d_etaPhi_tt_pseBits_mistimedStreamAna_event_" + number;
  title = "#eta - #phi Map of TT PSE Bits in the HAD layer of selected event number " + number;
  TH2F_LW *hadPSE = m_histTool->bookPPMEmEtaVsPhi(name, title);
  m_v_had_2d_etaPhi_tt_pseBits_mistimedStreamAna.push_back(hadPSE);
  
  name = "em_2d_etaPhi_tt_lut_cp0_mistimedStreamAna_event_" + number;
  title = "#eta - #phi Map of TT LUT CP in timeslice 0 = BCID-1 in the EM layer of selected event number " + number;
  TH2F_LW *emLUTcp0 = m_histTool->bookPPMEmEtaVsPhi(name, title);
  m_v_em_2d_etaPhi_tt_lut_cp0_mistimedStreamAna.push_back(emLUTcp0);
  name = "had_2d_etaPhi_tt_lut_cp0_mistimedStreamAna_event_" + number;
  title = "#eta - #phi Map of TT LUT CP in timeslice 0 = BCID-1 in the HAD layer of selected event number " + number;
  TH2F_LW *hadLUTcp0 = m_histTool->bookPPMEmEtaVsPhi(name, title);
  m_v_had_2d_etaPhi_tt_lut_cp0_mistimedStreamAna.push_back(hadLUTcp0);
  
  name = "em_2d_etaPhi_tt_lut_cp1_mistimedStreamAna_event_" + number;
  title = "#eta - #phi Map of TT LUT CP in timeslice 1 = BCID in the EM layer of selected event number " + number;
  TH2F_LW *emLUTcp1 = m_histTool->bookPPMEmEtaVsPhi(name, title);
  m_v_em_2d_etaPhi_tt_lut_cp1_mistimedStreamAna.push_back(emLUTcp1);
  name = "had_2d_etaPhi_tt_lut_cp1_mistimedStreamAna_event_" + number;
  title = "#eta - #phi Map of TT LUT CP in timeslice 1 = BCID in the HAD layer of selected event number " + number;
  TH2F_LW *hadLUTcp1 = m_histTool->bookPPMEmEtaVsPhi(name, title);
  m_v_had_2d_etaPhi_tt_lut_cp1_mistimedStreamAna.push_back(hadLUTcp1);

  name = "em_2d_etaPhi_tt_lut_cp2_mistimedStreamAna_event_" + number;
  title = "#eta - #phi Map of TT LUT CP in timeslice 2 = BCID+1 in the EM layer of selected event number " + number;
  TH2F_LW *emLUTcp2 = m_histTool->bookPPMEmEtaVsPhi(name, title);
  m_v_em_2d_etaPhi_tt_lut_cp2_mistimedStreamAna.push_back(emLUTcp2);
  name = "had_2d_etaPhi_tt_lut_cp2_mistimedStreamAna_event_" + number;
  title = "#eta - #phi Map of TT LUT CP in timeslice 2 = BCID+1 in the HAD layer of selected event number " + number;
  TH2F_LW *hadLUTcp2 = m_histTool->bookPPMEmEtaVsPhi(name, title);
  m_v_had_2d_etaPhi_tt_lut_cp2_mistimedStreamAna.push_back(hadLUTcp2);

  name = "em_2d_etaPhi_tt_lut_jep0_mistimedStreamAna_event_" + number;
  title = "#eta - #phi Map of TT LUT JEP in timeslice 0 = BCID-1 in the EM layer of selected event number " + number;
  TH2F_LW *emLUTjep0 = m_histTool->bookJEMEtaVsPhi(name, title);
  m_v_em_2d_etaPhi_tt_lut_jep0_mistimedStreamAna.push_back(emLUTjep0);
  name = "had_2d_etaPhi_tt_lut_jep0_mistimedStreamAna_event_" + number;
  title = "#eta - #phi Map of TT LUT JEP in timeslice 0 = BCID-1 in the HAD layer of selected event number " + number;
  TH2F_LW *hadLUTjep0 = m_histTool->bookJEMEtaVsPhi(name, title);
  m_v_had_2d_etaPhi_tt_lut_jep0_mistimedStreamAna.push_back(hadLUTjep0);
  
  name = "em_2d_etaPhi_tt_lut_jep1_mistimedStreamAna_event_" + number;
  title = "#eta - #phi Map of TT LUT JEP in timeslice 1 = BCID in the EM layer of selected event number " + number;
  TH2F_LW *emLUTjep1 = m_histTool->bookJEMEtaVsPhi(name, title);
  m_v_em_2d_etaPhi_tt_lut_jep1_mistimedStreamAna.push_back(emLUTjep1);
  name = "had_2d_etaPhi_tt_lut_jep1_mistimedStreamAna_event_" + number;
  title = "#eta - #phi Map of TT LUT JEP in timeslice 1 = BCID in the HAD layer of selected event number " + number;
  TH2F_LW *hadLUTjep1 = m_histTool->bookJEMEtaVsPhi(name, title);
  m_v_had_2d_etaPhi_tt_lut_jep1_mistimedStreamAna.push_back(hadLUTjep1);

  name = "em_2d_etaPhi_tt_lut_jep2_mistimedStreamAna_event_" + number;
  title = "#eta - #phi Map of TT LUT JEP in timeslice 2 = BCID+1 in the EM layer of selected event number " + number;
  TH2F_LW *emLUTjep2 = m_histTool->bookJEMEtaVsPhi(name, title);
  m_v_em_2d_etaPhi_tt_lut_jep2_mistimedStreamAna.push_back(emLUTjep2);
  name = "had_2d_etaPhi_tt_lut_jep2_mistimedStreamAna_event_" + number;
  title = "#eta - #phi Map of TT LUT JEP in timeslice 2 = BCID+1 in the HAD layer of selected event number " + number;
  TH2F_LW *hadLUTjep2 = m_histTool->bookJEMEtaVsPhi(name, title);
  m_v_had_2d_etaPhi_tt_lut_jep2_mistimedStreamAna.push_back(hadLUTjep2);
  
  return;
}

/*---------------------------------------------------------*/
StatusCode MistimedStreamMon::bookHistogramsRecurrent() // based on bookHistogramsRecurrent from PPrMon
/*---------------------------------------------------------*/
{
  if (msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "in MistimedStreamMon::bookHistograms" << endmsg;
  
  MgmtAttr_t attr = ATTRIB_UNMANAGED;

  if (newRun) {

    // create MonGroup with the folder to store histos
    MonGroup TT_MistimedMon(this, m_PathInRootFile, run, attr);
    m_histTool->setMonGroup(&TT_MistimedMon);
  
    // overview histograms (only one per run)
    m_h_1d_selectedEvents_mistimedStreamAna =
        m_histTool->book1F("1d_selectedEvents_mistimedStreamAna",
                           "Selected events per lumi block by the MistimedStream analysis",
                           9999, 0, 9999); // this plot will "only" cover 9999 lumi blocks
    m_h_1d_cutFlow_mistimedStreamAna =
        m_histTool->book1F("1d_cutFlow_mistimedStreamAna",
                           "Total events selected by the MistimedStream analysis for this run",
                           9, 0, 9); // overall cutflow plot
   
    m_h_1d_cutFlow_mistimedStreamAna->GetXaxis()->SetBinLabel( 1, "All" );
    m_h_1d_cutFlow_mistimedStreamAna->GetXaxis()->SetBinLabel( 2, "5 slice readout" );
    m_h_1d_cutFlow_mistimedStreamAna->GetXaxis()->SetBinLabel( 3, "HLT_mistimemonj400" );
    m_h_1d_cutFlow_mistimedStreamAna->GetXaxis()->SetBinLabel( 4, "L1_J100" );
    m_h_1d_cutFlow_mistimedStreamAna->GetXaxis()->SetBinLabel( 5, "<= 4 bad peak TT" );
    m_h_1d_cutFlow_mistimedStreamAna->GetXaxis()->SetBinLabel( 6, "<= 4 bad central TT" );
    m_h_1d_cutFlow_mistimedStreamAna->GetXaxis()->SetBinLabel( 7, "<= 4 bad late TT" );
    m_h_1d_cutFlow_mistimedStreamAna->GetXaxis()->SetBinLabel( 8, ">= 2 late TT" );
    m_h_1d_cutFlow_mistimedStreamAna->GetXaxis()->SetBinLabel( 9, "<= 3 in-time" );
    
    //book 10 events for each run, more mistimed events will be discarded and need to be checked by hand
    m_v_em_2d_etaPhi_tt_classification_mistimedStreamAna.clear();
    m_v_had_2d_etaPhi_tt_classification_mistimedStreamAna.clear();
    m_v_em_2d_etaPhi_tt_pseBits_mistimedStreamAna.clear();
    m_v_had_2d_etaPhi_tt_pseBits_mistimedStreamAna.clear();
    m_v_em_2d_etaPhi_tt_lut_cp0_mistimedStreamAna.clear();
    m_v_em_2d_etaPhi_tt_lut_cp1_mistimedStreamAna.clear();
    m_v_em_2d_etaPhi_tt_lut_cp2_mistimedStreamAna.clear();
    m_v_had_2d_etaPhi_tt_lut_cp0_mistimedStreamAna.clear();
    m_v_had_2d_etaPhi_tt_lut_cp1_mistimedStreamAna.clear();
    m_v_had_2d_etaPhi_tt_lut_cp2_mistimedStreamAna.clear();
    m_v_em_2d_etaPhi_tt_lut_jep0_mistimedStreamAna.clear();
    m_v_em_2d_etaPhi_tt_lut_jep1_mistimedStreamAna.clear();
    m_v_em_2d_etaPhi_tt_lut_jep2_mistimedStreamAna.clear();
    m_v_had_2d_etaPhi_tt_lut_jep0_mistimedStreamAna.clear();
    m_v_had_2d_etaPhi_tt_lut_jep1_mistimedStreamAna.clear();
    m_v_had_2d_etaPhi_tt_lut_jep2_mistimedStreamAna.clear();
    
    for(int histNo = 0; histNo < 10; histNo++){
        bookEventHistograms(std::to_string(histNo));
    }
  
    m_histTool->unsetMonGroup();
    if (newRun)
      m_histBooked = true;
  }

  return StatusCode::SUCCESS;
}


StatusCode MistimedStreamMon::fillHistograms()
{
 const bool debug = msgLvl(MSG::DEBUG);
  if (debug)
    msg(MSG::DEBUG) << "in fillHistograms()" << endmsg;

  if (!m_histBooked) {
    if (debug)
      msg(MSG::DEBUG) << "Histogram(s) not booked" << endmsg;
    return StatusCode::SUCCESS;
  }

  // Skip events believed to be corrupt
  if (m_errorTool->corrupt()) {
    if (debug)
      msg(MSG::DEBUG) << "Skipping corrupt event" << endmsg;
    return StatusCode::SUCCESS;
  }

  // Retrieve TriggerTowers from SG
  const xAOD::TriggerTowerContainer_v2 *TriggerTowerTES = 0;
  StatusCode sc =
      evtStore()->retrieve(TriggerTowerTES, m_xAODTriggerTowerContainerName);
  if (sc.isFailure() || !TriggerTowerTES) {
    if (debug)
      msg(MSG::DEBUG) << "No TriggerTower found in TES at "
                      << m_xAODTriggerTowerContainerName << endmsg;
    return StatusCode::SUCCESS;
  }

  //Create container for the decorated TT, as well as auxiliary container holding the actual properties
  xAOD::TriggerTowerContainer*    ttContainer = new xAOD::TriggerTowerContainer;
  xAOD::TriggerTowerAuxContainer* ttAuxContainer = new xAOD::TriggerTowerAuxContainer;
  // To tell the TT container in which auxiliary container it should write its member variables
  ttContainer->setStore(ttAuxContainer); //gives it a new associated aux container
  //Object holding the decoration, to optimize access speed
  xAOD::TriggerTower::Decorator<float> myDecoration("pulseClassification");
  
  //Get CPM Towers for LUT_CP
  const xAOD::CPMTowerContainer* cpmTowCon = 0;
  CHECK(evtStore()->retrieve(cpmTowCon,"CPMTowers")); 
  
  //Get Jet Elements for LUT-JEP
  const xAOD::JetElementContainer* jetEleCon = 0;
  CHECK(evtStore()->retrieve(jetEleCon,"JetElements")); 
  
  // Retrieve EventInfo from SG and save lumi block number, global event number and run number
  uint32_t bunchCrossing = 0;
  const EventInfo *evInfo = 0;
  sc = evtStore()->retrieve(evInfo);
  if (sc.isFailure() || !evInfo) {
    if (debug)
      msg(MSG::DEBUG) << "No EventInfo found" << endmsg;
  } else {
    const EventID *evID = evInfo->event_ID();
    if (evID) {
      bunchCrossing = evID->bunch_crossing_id();
    }
  }
  m_h_1d_cutFlow_mistimedStreamAna->Fill(0.5);
  
  // =====================================================================
  // ================= Container: TriggerTower ===========================
  // =====================================================================

  xAOD::TriggerTowerContainer_v2::const_iterator TriggerTowerIterator = 
      TriggerTowerTES->begin();
  xAOD::TriggerTowerContainer_v2::const_iterator TriggerTowerIteratorEnd =
      TriggerTowerTES->end();

   // right now this code only works on 5 slice readout:
   if((*TriggerTowerIterator)->adc().size() != 5){
       msg(MSG::DEBUG) << "Number of ADC slices != 5, aborting..." << endmsg;
       return StatusCode::SUCCESS;
   }
   
  m_h_1d_cutFlow_mistimedStreamAna->Fill(1.5);
  //Select events that fired HLT_mistimedmonj400
  m_h_1d_cutFlow_mistimedStreamAna->Fill(2.5);
  //Only select events which passed the L1_J100
  m_h_1d_cutFlow_mistimedStreamAna->Fill(3.5);


  // now classify the tower signals by looking at their FADC counts, if it exceeds 70
  int satCounter = 0; // saturated TT
  int badCounter = 0; // category 2 really bad
  int bad2Counter = 0; // category 4 bad peak 2 
  int bad3Counter = 0; // category 6 bad peak 3 
  int good3Counter = 0; // category 5 good peak 3 
  int good2Counter = 0; // category 5 good peak 3 
  
  for (; TriggerTowerIterator != TriggerTowerIteratorEnd;
       ++TriggerTowerIterator) {
    auto vecIndexTT = std::distance( TriggerTowerTES->begin(), TriggerTowerIterator ) ;
    float ttPulseCategory = 0;
    // retrieve max ADC value and position, this seems to be buggy in the DAOD
//     std::cout<<vecIndexTT<< " TT index; lumi block "<<lumiBlock<<std::endl;
//     << " event Number " <<eventNumber<<" run number "<<runNumber 
    auto maxValIterator = std::max_element(TriggerTowerTES->at(vecIndexTT)->adc().begin(), TriggerTowerTES->at(vecIndexTT)->adc().end());
    int maxADCval = *maxValIterator;
    int adcPeakPositon = std::distance(std::begin(TriggerTowerTES->at(vecIndexTT)->adc()), maxValIterator);

    if(maxADCval < 70){
       ttPulseCategory = 0.1;
    }else if(maxADCval == 1023) {
        satCounter++;
        ttPulseCategory = 1;
    }else{
        bool goodQual = pulseQuality(TriggerTowerTES->at(vecIndexTT)->adc(), adcPeakPositon);
        //look at any of the five FADC values
        if(adcPeakPositon == 2){ // can be class 3 or 4 now
          if(goodQual){
             good2Counter++;
             ttPulseCategory = 3;
          }else{
             bad2Counter++;
             ttPulseCategory = 4;
          }
        }else if(adcPeakPositon == 3){ // can be class 5 or 6 now
           if(goodQual){
             good3Counter++;
             ttPulseCategory = 5;
           }else{
             bad3Counter++;
             ttPulseCategory = 6;
           }
        }else{
          //this is class 2 - really bad
          badCounter++;
          ttPulseCategory = 2;
      }
    }
    // decorate the TT in order to have to recompute the pulse categorisation
    xAOD::TriggerTower* newTT = new xAOD::TriggerTower; //create a new TT object
    ttContainer->push_back(newTT); // add the newTT to new output TT container (at the end of it)
    *newTT = *TriggerTowerTES->at(vecIndexTT); // copy over all information from TT to newTT
    newTT->auxdata<float>("pulseClassification") = ttPulseCategory; //decorate
//     myDecoration(*newJet) = 10.0; // same decoration with a Decorator (faster)
  }
  
  if(badCounter > 4){
    //reject events with more than 4 wrongly peaked towers
    return StatusCode::SUCCESS;
  }
  m_h_1d_cutFlow_mistimedStreamAna->Fill(4.5);
 
  if(bad2Counter > 4){
    //reject events with more than 4 pulses peaking in slice 2 that are badly timed or mis-shapen
    return StatusCode::SUCCESS;
  }
  m_h_1d_cutFlow_mistimedStreamAna->Fill(5.5);
  
  if(bad3Counter > 4){
    //reject events with more than 4 pulses peaking in slice 3 that are badly timed or mis-shapen
    return StatusCode::SUCCESS;
  }
  m_h_1d_cutFlow_mistimedStreamAna->Fill(6.5);
  
  if(good3Counter < 2){
    //reject events with less than 2 pulses nicely peaking in slice 3 
    return StatusCode::SUCCESS;
  }
  m_h_1d_cutFlow_mistimedStreamAna->Fill(7.5);
   
  if(good2Counter > 2){
    //reject events with more than 2 pulses nicely peaking in slice 2 to avoid event being triggered by pileup 
    return StatusCode::SUCCESS;
  }
  
  m_h_1d_cutFlow_mistimedStreamAna->Fill(8.5);
  m_selectedEventCounter++;

  //loop over the decorated TTcollection, if less than 10 events are selected for this run
  if(m_selectedEventCounter < 10){
    for(auto decoratedIterator = ttContainer->begin() ; decoratedIterator != ttContainer->end() ; ++decoratedIterator ){

        const int layer = (*decoratedIterator)->layer();
        const double eta = (*decoratedIterator)->eta();
        const double phi = (*decoratedIterator)->phi();
        const float pulseCat = (*decoratedIterator)->auxdata<float>("pulseClassification");
        uint8_t bcidWord = (*decoratedIterator)->bcidVec()[0]; // look at the status bit in the central time slice. Hard coded for now as we do the 5 slice check above
        
        // Check if TT is in EM or HAD layer:
        if (layer == 0) { //========== ELECTROMAGNETIC LAYER =========================
            m_histTool->fillPPMEmEtaVsPhi(m_v_em_2d_etaPhi_tt_classification_mistimedStreamAna[m_selectedEventCounter], eta, phi, pulseCat);
            if(pulseCat > 0.5){
               m_histTool->fillPPMEmEtaVsPhi(m_v_em_2d_etaPhi_tt_pseBits_mistimedStreamAna[m_selectedEventCounter], eta, phi, (unsigned int)bcidWord);
                
            }

        }
        else if(layer == 1) { //========== HADRONIC LAYER ===============================
            m_histTool->fillPPMEmEtaVsPhi(m_v_had_2d_etaPhi_tt_classification_mistimedStreamAna[m_selectedEventCounter], eta, phi, pulseCat);
            if(pulseCat > 0.5){
               m_histTool->fillPPMEmEtaVsPhi(m_v_had_2d_etaPhi_tt_pseBits_mistimedStreamAna[m_selectedEventCounter], eta, phi, (unsigned int)bcidWord);
            }            
         }
     }
   }
   
   //loop over the cpm tower container to fill the lut_cp histos 
   for(auto thisCT:*cpmTowCon){
      double eta = thisCT->eta();
      double phi = thisCT->phi();
      std::vector<uint8_t> cpmEMenergy = thisCT->emEnergyVec();
      std::vector<uint8_t> cpmHADenergy = thisCT->hadEnergyVec();

      if(cpmEMenergy.size() > 2){ // expect 3 slicesto be read out
          m_histTool->fillPPMEmEtaVsPhi(m_v_em_2d_etaPhi_tt_lut_cp0_mistimedStreamAna[m_selectedEventCounter], eta, phi, (int) cpmEMenergy.at(0));
          m_histTool->fillPPMEmEtaVsPhi(m_v_em_2d_etaPhi_tt_lut_cp1_mistimedStreamAna[m_selectedEventCounter], eta, phi, (int) cpmEMenergy.at(1));
          m_histTool->fillPPMEmEtaVsPhi(m_v_em_2d_etaPhi_tt_lut_cp2_mistimedStreamAna[m_selectedEventCounter], eta, phi, (int) cpmEMenergy.at(2));
      }
      if(cpmHADenergy.size() > 2){
          m_histTool->fillPPMEmEtaVsPhi(m_v_had_2d_etaPhi_tt_lut_cp0_mistimedStreamAna[m_selectedEventCounter], eta, phi, (int) cpmHADenergy.at(0));
          m_histTool->fillPPMEmEtaVsPhi(m_v_had_2d_etaPhi_tt_lut_cp1_mistimedStreamAna[m_selectedEventCounter], eta, phi, (int) cpmHADenergy.at(1));
          m_histTool->fillPPMEmEtaVsPhi(m_v_had_2d_etaPhi_tt_lut_cp2_mistimedStreamAna[m_selectedEventCounter], eta, phi, (int) cpmHADenergy.at(2));
      }
   }
   
   // the jet element container
   for(auto thisJE:*jetEleCon){
      double eta = thisJE->eta();
      double phi = thisJE->phi();
      std::vector<uint16_t> jepEMenergy = thisJE->emJetElementETVec();
      std::vector<uint16_t> jepHADenergy = thisJE->hadJetElementETVec();
      if(jepEMenergy.size() > 2){
          m_histTool->fillJEMEtaVsPhi(m_v_em_2d_etaPhi_tt_lut_jep0_mistimedStreamAna[m_selectedEventCounter], eta, phi, (int)jepEMenergy.at(0));
          m_histTool->fillJEMEtaVsPhi(m_v_em_2d_etaPhi_tt_lut_jep1_mistimedStreamAna[m_selectedEventCounter], eta, phi, (int)jepEMenergy.at(1));
          m_histTool->fillJEMEtaVsPhi(m_v_em_2d_etaPhi_tt_lut_jep2_mistimedStreamAna[m_selectedEventCounter], eta, phi, (int)jepEMenergy.at(2));

      }
      if(jepHADenergy.size()){
          m_histTool->fillJEMEtaVsPhi(m_v_had_2d_etaPhi_tt_lut_jep0_mistimedStreamAna[m_selectedEventCounter], eta, phi, (int)jepHADenergy.at(0));
          m_histTool->fillJEMEtaVsPhi(m_v_had_2d_etaPhi_tt_lut_jep1_mistimedStreamAna[m_selectedEventCounter], eta, phi, (int)jepHADenergy.at(1));
          m_histTool->fillJEMEtaVsPhi(m_v_had_2d_etaPhi_tt_lut_jep2_mistimedStreamAna[m_selectedEventCounter], eta, phi, (int)jepHADenergy.at(2));

      }
  }
//   //adding the histos dynamically does not seem to work... too bad!!!
//   std::stringstream buffer;
//   m_histTool->setMonGroup(&TT_MistimedMon);
//   m_v_em_2d_etaPhi_tt_lut_cp1_mistimedStreamAna.clear();
//   for (int i = 0; i < 12; i++) {
//         buffer.str("");
//         buffer << i; // here the lumi block, event number and run number should go
//         std::string name = "em_2d_etaPhi_tt_lut_cp1_mistimedStreamAna_event_" + buffer.str();
//         std::string title = "EM Layer, LUT-CP in time slice 1 for Event No. " + buffer.str();
//         TH2F_LW *hist =
//             m_histTool->bookPPMEmEtaVsPhi(name, title);
//         m_histTool->fillPPMEmEtaVsPhi(hist, 1, 1, 1);
// //         m_histTool->numbers(hist, 0, 15, 2);
// //         m_histTool->ppmCrateModule(hist, crate, crate + 1, 0, false);
//         m_v_em_2d_etaPhi_tt_lut_cp1_mistimedStreamAna.push_back(hist);
//       }
  
  m_h_1d_selectedEvents_mistimedStreamAna->Fill(lumiBlock);
  
  //dont forget to delete the decorated TTcontainers
  delete ttContainer;
  delete ttAuxContainer;
  
  return StatusCode::SUCCESS;
}

// ============================================================================
} // end namespace
// ============================================================================

