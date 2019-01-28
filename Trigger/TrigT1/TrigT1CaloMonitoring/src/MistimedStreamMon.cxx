/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "SGTools/StlVectorClids.h"

#include "LWHists/LWHist.h"
#include "LWHists/TH1F_LW.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigT1Interfaces/TrigT1CaloDefs.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "TrigT1CaloEvent/TriggerTower_ClassDEF.h"
#include "TrigT1CaloUtils/DataError.h"

//for the 80MHz functionality (db access)
#include "TrigT1CaloCondSvc/L1CaloCondSvc.h"
#include "TrigT1CaloCalibConditions/L1CaloRunParameters.h"  
#include "TrigT1CaloCalibConditions/L1CaloRunParametersContainer.h"  
#include "TrigT1CaloCalibConditions/L1CaloReadoutConfig.h"  
#include "TrigT1CaloCalibConditions/L1CaloReadoutConfigContainer.h"  

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
      m_errorTool("LVL1::TrigT1CaloMonErrorTool/TrigT1CaloMonErrorTool"),
      m_histTool("LVL1::TrigT1CaloLWHistogramTool/TrigT1CaloLWHistogramTool"),            m_ttTool("LVL1::L1TriggerTowerTool/L1TriggerTowerTool"), // can provide coolID, prob. not needed
      m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
      m_histBooked(false),
      m_h_1d_cutFlow_mistimedStreamAna(0),
      m_h_1d_selectedEvents_mistimedStreamAna(0),
      m_l1CondSvc("L1CaloCondSvc",name),
      m_maxHistos(1),
      m_curHistos(0),
      m_thistSvc("THistSvc", name)
{
     declareProperty("PathInRootFile", m_PathInRootFile = "L1Calo/MistimedStream");
     declareProperty("TrigDecisionTool", m_trigDec, "The tool to access TrigDecision" );
     declareProperty("BS_xAODTriggerTowerContainer", m_xAODTriggerTowerContainerName = LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation);
     declareProperty("maxHistos",m_maxHistos = 1);
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
  ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);

  CHECK(ManagedMonitorToolBase::initialize());
  CHECK(m_errorTool.retrieve());
  CHECK(m_histTool.retrieve());
  CHECK(m_ttTool.retrieve());
  CHECK(m_trigDec.retrieve());
  CHECK(m_l1CondSvc.retrieve());

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
StatusCode MistimedStreamMon:: retrieveConditions()
/*---------------------------------------------------------*/
{
  ATH_MSG_VERBOSE("PPMSimBSMon::retrieveConditions");
  if (m_l1CondSvc) {
    ATH_MSG_VERBOSE( "Retrieving Run Parameters Containers" );
    CHECK_WITH_CONTEXT(m_l1CondSvc->retrieve(m_runParametersContainer),"MistimedStreamMon");
    if (std::cbegin(*m_runParametersContainer) == std::cend(*m_runParametersContainer)) {
      ATH_MSG_ERROR("Empty L1CaloRunParametersContainer");
      return StatusCode::FAILURE;
    }
    ATH_MSG_VERBOSE( "Retrieving Readount Configuration Containers" );
    CHECK_WITH_CONTEXT(m_l1CondSvc->retrieve(m_readoutConfigContainer),"MistimedStreamMon");
    if (std::cbegin(*m_readoutConfigContainer) == std::cend(*m_readoutConfigContainer)) {
      ATH_MSG_ERROR("Empty L1CaloReadoutConfigContainer");
      return StatusCode::FAILURE;
    }

  }else{
    ATH_MSG_ERROR("L1CondSvc not present!");
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void MistimedStreamMon::fillEtaPhiMap(TH2F* hist, double eta, double phi, double weight, bool shrinkEtaBins)
/*---------------------------------------------------------*/
{
  double phiMod = phi * (32./M_PI);
  double etaMod = eta;
  const double absEta = fabs(eta);
  if (absEta > 3.2) {
    if (shrinkEtaBins) {
      etaMod = 2.9 + 0.1*(absEta-3.2)/0.425;
      if (eta < 0.) etaMod = -etaMod;
    }
    // Fill four bins in phi
    phiMod = floor(phiMod/4)*4. + 2.;
    hist->Fill(etaMod, phiMod + 1.5, weight);
    hist->Fill(etaMod, phiMod + 0.5, weight);
    hist->Fill(etaMod, phiMod - 0.5, weight);
    hist->Fill(etaMod, phiMod - 1.5, weight);
  } else if (absEta > 2.5) {
    if (shrinkEtaBins) {
      etaMod = (absEta > 3.1) ? 2.85 : 2.5 + (absEta-2.5)/2.;
      if (eta < 0.) etaMod = -etaMod;
    }
    // Fill two bins in phi
    phiMod = floor(phiMod/2)*2. + 1.;
    hist->Fill(etaMod, phiMod + 0.5, weight);
    hist->Fill(etaMod, phiMod - 0.5, weight);
  } else hist->Fill(eta, phiMod, weight);
}


/*---------------------------------------------------------*/
TH2F* MistimedStreamMon::createEtaPhiMap(std::string name, std::string title, bool isHADLayer, bool shrinkEtaBins) 
/*---------------------------------------------------------*/
{
  // construct ppmEtaPhi Histo (modified from TrigT1CaloLWHistogramTool, but without the booking
  TH2F* hist = 0;
  TAxis* axis = 0;
  if (shrinkEtaBins) {
     hist = new TH2F(name.c_str(), title.c_str(), 66, -3.3, 3.3, 64, 0., 64.); 
     hist->SetOption("colz");
     axis = hist->GetXaxis();
     for (int ch = -25; ch < 25; ch+=4) {
       int chan = ch;
       if (chan >= -1) ++chan;
       const double eta = (chan/10.)+0.05;
       std::ostringstream cnum;
       cnum << chan << "/" << std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setprecision(2) << eta;       
       axis->SetBinLabel(chan+34, cnum.str().c_str());
     }
     axis->SetBinLabel(1, "-49/-4.41");
     axis->SetBinLabel(5, "-32/-3.15");
     axis->SetBinLabel(62, "31/3.15");
     axis->SetBinLabel(66, "44/4.41");
  } else {
    const int nxbins = 66;
    const double xbins[nxbins+1] = {-4.9,-4.475,-4.050,-3.625,-3.2,-3.1,-2.9,
                                    -2.7,-2.5,-2.4,-2.3,-2.2,-2.1,-2.0,-1.9,
  				    -1.8,-1.7,-1.6,-1.5,-1.4,-1.3,-1.2,-1.1,
				    -1.0,-0.9,-0.8,-0.7,-0.6,-0.5,-0.4,-0.3,
				    -0.2,-0.1,0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,
				    0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,
				    1.8,1.9,2.0,2.1,2.2,2.3,2.4,2.5,2.7,2.9,
				    3.1,3.2,3.625,4.050,4.475,4.9};
    hist = new TH2F(name.c_str(), title.c_str(), nxbins, xbins, 64, 0., 64.); 
    hist->SetOption("colz");                  
    hist->SetXTitle("eta");
  }

  axis = hist->GetYaxis();
  const double phiBin     = M_PI/32.;
  const double halfPhiBin = M_PI/64.;
  for (int chan = 0; chan < 64; chan += 4 ) {
    const double rad = chan*phiBin + halfPhiBin;
    std::ostringstream lab;
    lab << chan << "/" << std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setprecision(2) << rad;          
    axis->SetBinLabel(chan+1, lab.str().c_str());
  }
  if (shrinkEtaBins) axis->SetBinLabel(64, "etaVphi");
  else               axis->SetBinLabel(64, "phi");
  
  // now relabel the outer bins for the HAD Layer
  if (shrinkEtaBins && isHADLayer) {
    axis = hist->GetXaxis();    
    axis->SetBinLabel(1, "-49/-4.17");
    axis->SetBinLabel(66, "44/4.19");
  }
  
  return hist;  
}

/*---------------------------------------------------------*/
StatusCode MistimedStreamMon::bookHistogramsRecurrent() // based on bookHistogramsRecurrent from PPrMon
/*---------------------------------------------------------*/
{
  if (msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "in MistimedStreamMon::bookHistograms" << endmsg;
  
  MgmtAttr_t attr = ATTRIB_UNMANAGED;

  if (newRunFlag()) {

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
                           10, 0, 10); // overall cutflow plot
   
    m_h_1d_cutFlow_mistimedStreamAna->GetXaxis()->SetBinLabel( 1, "All" );
    m_h_1d_cutFlow_mistimedStreamAna->GetXaxis()->SetBinLabel( 2, "unsuitable readout" );
    m_h_1d_cutFlow_mistimedStreamAna->GetXaxis()->SetBinLabel( 3, "HLT_mistimemonj400" );
    m_h_1d_cutFlow_mistimedStreamAna->GetXaxis()->SetBinLabel( 4, "L1_J100" );
    m_h_1d_cutFlow_mistimedStreamAna->GetXaxis()->SetBinLabel( 5, "<= 4 bad peak TT" );
    m_h_1d_cutFlow_mistimedStreamAna->GetXaxis()->SetBinLabel( 6, "<= 4 bad central TT" );
    m_h_1d_cutFlow_mistimedStreamAna->GetXaxis()->SetBinLabel( 7, "<= 4 bad late TT" );
    m_h_1d_cutFlow_mistimedStreamAna->GetXaxis()->SetBinLabel( 8, ">= 2 late TT" );
    m_h_1d_cutFlow_mistimedStreamAna->GetXaxis()->SetBinLabel( 9, "<= 3 in-time" );
    m_h_1d_cutFlow_mistimedStreamAna->GetXaxis()->SetBinLabel(10, ">= 1 significant TT in EM layer" );
      
    m_histTool->unsetMonGroup();
    if (newRunFlag())
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
  
  //Get readout info from data base
  if(this->retrieveConditions().isFailure()) {
      REPORT_MESSAGE(MSG::WARNING);
      return StatusCode::FAILURE;
  }
  
  m_readoutConfigContainer->readoutConfig(6);
  
  unsigned int readoutConfigID = std::cbegin(*m_runParametersContainer)->readoutConfigID();
  unsigned int channelID = 0;
  unsigned int numFadcSlices = 0; 
  unsigned int l1aFadcSlice = 0; 
  unsigned int readout80ModePpm = 0;
  //the readout config ID tells you, which readoutConfig is loaded. now you can retrieve the l1aFadcSlice from this DB entry
  for(const auto& it: *m_readoutConfigContainer){
    if (it.channelId() == readoutConfigID){
        channelID = it.channelId();
        numFadcSlices = it.numFadcSlices();
        l1aFadcSlice = it.l1aFadcSlice();
        readout80ModePpm = it.readout80ModePpm();
    }
  }
  
  if(msgLvl(MSG::DEBUG)){
    ATH_MSG_VERBOSE("ReadoutConfigID = " << readoutConfigID );
    ATH_MSG_VERBOSE("ChannelID = " << channelID );
    ATH_MSG_VERBOSE("numFadcSlices = " << numFadcSlices );
    ATH_MSG_VERBOSE("l1aFadcSlice = " << l1aFadcSlice );
    ATH_MSG_VERBOSE("readout80ModePpm = " << readout80ModePpm );
  } 

//   std::cout<<"readoutConfigID = "<<readoutConfigID<<std::endl;
//   std::cout<<"ChannelID = "<<channelID<<std::endl;
//   std::cout<<"numFadcSlices = "<<numFadcSlices<<std::endl;
//   std::cout<<"l1aFadcSlice = "<<l1aFadcSlice<<std::endl;
//   std::cout<<"readout80ModePpm = "<<readout80ModePpm<<std::endl;
  
  // Retrieve EventInfo from SG and save lumi block number, global event number and run number
  unsigned int lumiNo = 0;
  unsigned int currentRunNo = 0;
  unsigned int currentEventNo = 0;
  const EventInfo *evInfo = 0;
  sc = evtStore()->retrieve(evInfo);
  if (sc.isFailure() || !evInfo) {
    if (debug)
      msg(MSG::DEBUG) << "No EventInfo found" << endmsg;
  } else {
    const EventID *evID = evInfo->event_ID();
    if (evID) {
      lumiNo = evID->lumi_block();
      currentRunNo = evID->run_number();
      currentEventNo = evID->event_number();
    }
  }
  m_h_1d_cutFlow_mistimedStreamAna->Fill(0.5);

  //for the algorithm to run, we need the 2 adc slices before and after the l1a-slice
  //add some readout compatibility checks of the algo here
  if(readout80ModePpm){
     if(numFadcSlices < 9){
       msg(MSG::DEBUG) << "Number of ADC slices < 9 for 80 MHz readout, algorithm cannot run, aborting..." << endmsg;
       return StatusCode::SUCCESS;
     }
     if(l1aFadcSlice < 4){
       msg(MSG::DEBUG) << "L1a readout pointer < 4 for 80 MHz readout, algorithm cannot run, aborting..." << endmsg;
       return StatusCode::SUCCESS;
     }
     if(numFadcSlices - l1aFadcSlice < 4){
       msg(MSG::DEBUG) << "L1a readout pointer is at "<< l1aFadcSlice << " with "<< numFadcSlices << "slices at 80 MHz readout mode, algorithm cannot run, aborting..." << endmsg;
       return StatusCode::SUCCESS;
     } 
  }else{
     if(numFadcSlices < 5){
       msg(MSG::DEBUG) << "Number of ADC slices < 5 for 40 MHz readout, algorithm cannot run, aborting..." << endmsg;
       return StatusCode::SUCCESS;
     }
     if(l1aFadcSlice < 2){
       msg(MSG::DEBUG) << "L1a readout pointer < 2 for 40 MHz readout, algorithm cannot run, aborting..." << endmsg;
       return StatusCode::SUCCESS;
     } 
     if(numFadcSlices - l1aFadcSlice < 2){
       msg(MSG::DEBUG) << "L1a readout pointer is at "<< l1aFadcSlice << " with "<< numFadcSlices << "slices at 40 MHz readout mode, algorithm cannot run, aborting..." << endmsg;
       return StatusCode::SUCCESS;
     }  
  }   
  m_h_1d_cutFlow_mistimedStreamAna->Fill(1.5);
  
  //Select events that fired HLT_mistimedmonj400
  if(! (m_trigDec->isPassed("HLT_mistimemonj400",TrigDefs::requireDecision))){
    return StatusCode::SUCCESS;
  }
  m_h_1d_cutFlow_mistimedStreamAna->Fill(2.5);

  //Only select events which passed the L1_J100
  if(! (m_trigDec->isPassed("L1_J100"))){
    return StatusCode::SUCCESS;
  }
  m_h_1d_cutFlow_mistimedStreamAna->Fill(3.5);

  // now classify the tower signals by looking at their FADC counts, if it exceeds 70
  int satCounter = 0; // saturated TT
  int badCounter = 0; // category 2 really bad
  int bad2Counter = 0; // category 4 bad peak 2 
  int bad3Counter = 0; // category 6 bad peak 3 
  int good3Counter = 0; // category 5 good peak 3 
  int good2Counter = 0; // category 5 good peak 3 
  int emActivityCounter = 0; //count number of TT in EM layer with ADC > 70 

  // =====================================================================
  // ================= Container: TriggerTower ===========================
  // =====================================================================
  
  xAOD::TriggerTowerContainer_v2::const_iterator TriggerTowerIterator = 
      TriggerTowerTES->begin();
  xAOD::TriggerTowerContainer_v2::const_iterator TriggerTowerIteratorEnd =
      TriggerTowerTES->end();
  
  for (; TriggerTowerIterator != TriggerTowerIteratorEnd;
       ++TriggerTowerIterator) {
    auto vecIndexTT = std::distance( TriggerTowerTES->begin(), TriggerTowerIterator ) ;
    float ttPulseCategory = 0;
    std::vector<uint16_t> ttADC = TriggerTowerTES->at(vecIndexTT)->adc();
    //Check which RunParameters are set in COOL database -> modify acd input accordingly
    // need to know: - readout mode (40 or 80MHz)
    //               - central slice position
    //               - number of readout slices (== .adc().size())
 
    std::vector<uint16_t> readoutCorrectedADC; //this is the standard readout ADC vector: 5 40MHz samples with l1A in the middle
    if(!readout80ModePpm){//40 MHz
       //just acess the acd vector, as the sanity checks where done above
       readoutCorrectedADC.push_back(ttADC.at(l1aFadcSlice-2));
       readoutCorrectedADC.push_back(ttADC.at(l1aFadcSlice-1));
       readoutCorrectedADC.push_back(ttADC.at(l1aFadcSlice));
       readoutCorrectedADC.push_back(ttADC.at(l1aFadcSlice+1));
       readoutCorrectedADC.push_back(ttADC.at(l1aFadcSlice+2));
    }else{//80 MHz
       readoutCorrectedADC.push_back(ttADC.at(l1aFadcSlice-4));
       readoutCorrectedADC.push_back(ttADC.at(l1aFadcSlice-2));
       readoutCorrectedADC.push_back(ttADC.at(l1aFadcSlice));
       readoutCorrectedADC.push_back(ttADC.at(l1aFadcSlice+2));
       readoutCorrectedADC.push_back(ttADC.at(l1aFadcSlice+4));
    }
  
    // retrieve max ADC value and position, this seems to be buggy in the DAOD
    auto maxValIterator = std::max_element(readoutCorrectedADC.begin(), readoutCorrectedADC.end());
    int maxADCval = *maxValIterator;
    int adcPeakPositon = std::distance(std::begin(readoutCorrectedADC), maxValIterator);
    
    if(maxADCval < 70){
       ttPulseCategory = 0.1;
    }else if(maxADCval == 1023) {
        satCounter++;
        ttPulseCategory = 1;
        if(!TriggerTowerTES->at(vecIndexTT)->layer()) emActivityCounter++;
    }else{
        bool goodQual = pulseQuality(readoutCorrectedADC, adcPeakPositon);
        if(!TriggerTowerTES->at(vecIndexTT)->layer()) emActivityCounter++;
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
  
  if(!emActivityCounter){
    //reject events with no activity in the EM layer
    return StatusCode::SUCCESS;
  }
  m_h_1d_cutFlow_mistimedStreamAna->Fill(9.5);

  if(m_curHistos < m_maxHistos){

    //adding the histos dynamically using THistSvc into an event-specific sub-directory
    // Construct a subdirectory name for the histograms:
    std::ostringstream fullPathInFile;
    fullPathInFile  << "/" << m_fileKey << "/" << "run_" << AthenaMonManager::runNumber() << "/" << m_PathInRootFile << "/run_" << std::to_string(currentRunNo) << "_event_" << std::to_string(currentEventNo) << "/";
  
    std::string name, title;
    name = "em_2d_etaPhi_tt_classification_mistimedStreamAna";
    title = "#eta - #phi Map of TT classification, EM layer: event no. " + std::to_string(currentEventNo) + " in lb. " + std::to_string(lumiNo) + " of run " + std::to_string(currentRunNo);
    TH2F *emClass = createEtaPhiMap(name, title);
    CHECK(m_thistSvc->regHist(fullPathInFile.str() + name.c_str(), emClass));
    
    name = "had_2d_etaPhi_tt_classification_mistimedStreamAna";
    title = "#eta - #phi Map of TT classification, HAD layer: event no. " + std::to_string(currentEventNo) + " in lb. " + std::to_string(lumiNo) + " of run " + std::to_string(currentRunNo);
    TH2F *hadClass = createEtaPhiMap(name, title, true);
    CHECK(m_thistSvc->regHist(fullPathInFile.str() + name.c_str(), hadClass));
  
    name = "em_2d_etaPhi_tt_pseBits_mistimedStreamAna";
    title = "#eta - #phi Map of TT PSE Bits, EM layer: event no. " + std::to_string(currentEventNo) + " in lb. " + std::to_string(lumiNo) + " of run " + std::to_string(currentRunNo);
    TH2F *emPSE = createEtaPhiMap(name, title);
    CHECK(m_thistSvc->regHist(fullPathInFile.str() + name.c_str(), emPSE));
  
    name = "had_2d_etaPhi_tt_pseBits_mistimedStreamAna";
    title = "#eta - #phi Map of TT PSE Bits, HAD layer: event no. " + std::to_string(currentEventNo) + " in lb. " + std::to_string(lumiNo) + " of run " + std::to_string(currentRunNo);
    TH2F *hadPSE = createEtaPhiMap(name, title, true);
    CHECK(m_thistSvc->regHist(fullPathInFile.str() + name.c_str(), hadPSE));
  
    // histos for the LUT values. They show a combination of CP (central) and JEP (forward) output 
    name = "em_2d_etaPhi_tt_lut0_mistimedStreamAna";
    title = "#eta - #phi Map of TT LUT in timeslice 0 = BCID-1, EM layer: event no. " + std::to_string(currentEventNo) + " in lb. " + std::to_string(lumiNo) + " of run " + std::to_string(currentRunNo);
    TH2F *emLUT0 = createEtaPhiMap(name, title);
    CHECK(m_thistSvc->regHist(fullPathInFile.str() + name.c_str(), emLUT0));
    
    name = "had_2d_etaPhi_tt_lut0_mistimedStreamAna";
    title = "#eta - #phi Map of TT LUT in timeslice 0 = BCID-1, HAD layer: event no. " + std::to_string(currentEventNo) + " in lb. " + std::to_string(lumiNo) + " of run " + std::to_string(currentRunNo);
    TH2F *hadLUT0 = createEtaPhiMap(name, title, true);
    CHECK(m_thistSvc->regHist(fullPathInFile.str() + name.c_str(), hadLUT0));
    
    name = "em_2d_etaPhi_tt_lut1_mistimedStreamAna";
    title = "#eta - #phi Map of TT LUT in timeslice 1 = BCID, EM layer: event no. " + std::to_string(currentEventNo) + " in lb. " + std::to_string(lumiNo) + " of run " + std::to_string(currentRunNo);
    TH2F *emLUT1 = createEtaPhiMap(name, title);
    CHECK(m_thistSvc->regHist(fullPathInFile.str() + name.c_str(), emLUT1));
  
    name = "had_2d_etaPhi_tt_lut1_mistimedStreamAna";
    title = "#eta - #phi Map of TT LUT in timeslice 1 = BCID, HAD layer: event no. " + std::to_string(currentEventNo) + " in lb. " + std::to_string(lumiNo) + " of run " + std::to_string(currentRunNo);
    TH2F *hadLUT1 = createEtaPhiMap(name, title, true);
    CHECK(m_thistSvc->regHist(fullPathInFile.str() + name.c_str(), hadLUT1));
  
    name = "em_2d_etaPhi_tt_lut2_mistimedStreamAna";
    title = "#eta - #phi Map of TT LUT in timeslice 2 = BCID+1, EM layer: event no. " + std::to_string(currentEventNo) + " in lb. " + std::to_string(lumiNo) + " of run " + std::to_string(currentRunNo);
    TH2F *emLUT2 = createEtaPhiMap(name, title);
    CHECK(m_thistSvc->regHist(fullPathInFile.str() + name.c_str(), emLUT2));
  
    name = "had_2d_etaPhi_tt_lut2_mistimedStreamAna";
    title = "#eta - #phi Map of TT LUT in timeslice 2 = BCID+1, HAD layer: event no. " + std::to_string(currentEventNo) + " in lb. " + std::to_string(lumiNo) + " of run " + std::to_string(currentRunNo);
    TH2F *hadLUT2 = createEtaPhiMap(name, title, true);
    CHECK(m_thistSvc->regHist(fullPathInFile.str() + name.c_str(), hadLUT2));
    
    
    // loop over the decorated TTcollection to fill the classification and pse histos
    for(auto decoratedIterator = ttContainer->begin() ; decoratedIterator != ttContainer->end() ; ++decoratedIterator ){
  
        const int layer = (*decoratedIterator)->layer();
        const double eta = (*decoratedIterator)->eta();
        const double phi = (*decoratedIterator)->phi();
        const float pulseCat = (*decoratedIterator)->auxdata<float>("pulseClassification");
        uint8_t bcidWord = (*decoratedIterator)->bcidVec()[0]; // look at the status bit in the central time slice. Hard coded for now as we do the 5 slice check above
          
        // Check if TT is in EM or HAD layer:
        if (layer == 0) { //========== ELECTROMAGNETIC LAYER =========================
            fillEtaPhiMap(emClass, eta, phi, pulseCat);
            if(pulseCat > 0.5) fillEtaPhiMap(emPSE, eta, phi, (unsigned int)bcidWord);
        }
        else if(layer == 1) { //========== HADRONIC LAYER ===============================
            fillEtaPhiMap(hadClass, eta, phi, pulseCat);
            if(pulseCat > 0.5) fillEtaPhiMap(hadPSE, eta, phi, (unsigned int)bcidWord);
        }
     }
  
     //loop over the cpm tower container to fill the lut histos 
     for(auto thisCT:*cpmTowCon){
        double eta = thisCT->eta();
        double phi = thisCT->phi();
        std::vector<uint8_t> cpmEMenergy = thisCT->emEnergyVec();
        std::vector<uint8_t> cpmHADenergy = thisCT->hadEnergyVec();
  
        if(cpmEMenergy.size() > 2){ // expect 3 slices to be read out
            fillEtaPhiMap(emLUT0, eta, phi, (int) cpmEMenergy.at(0));
            fillEtaPhiMap(emLUT1, eta, phi, (int) cpmEMenergy.at(1));
            fillEtaPhiMap(emLUT2, eta, phi, (int) cpmEMenergy.at(2));
        }
        if(cpmHADenergy.size() > 2){
            fillEtaPhiMap(hadLUT0, eta, phi, (int) cpmHADenergy.at(0));
            fillEtaPhiMap(hadLUT1, eta, phi, (int) cpmHADenergy.at(1));
            fillEtaPhiMap(hadLUT2, eta, phi, (int) cpmHADenergy.at(2));
        }
      }
      // and the jet element container for the forward region
      for(auto thisJE:*jetEleCon){
        double eta = thisJE->eta();
        double phi = thisJE->phi();
        int signeta = 1;
        if(eta < 0) signeta = -1;
        std::vector<uint16_t> jepEMenergy = thisJE->emJetElementETVec();
        std::vector<uint16_t> jepHADenergy = thisJE->hadJetElementETVec();
        
        if (eta < -2.5 || eta > 2.5) {// Use JEP info to fill the forward part of the lut plots, but since this has TT granularity we have to play some tricks
           if(jepEMenergy.size() > 2){
             fillEtaPhiMap(emLUT0, eta, phi, (int)jepEMenergy.at(0));
             fillEtaPhiMap(emLUT1, eta, phi, (int)jepEMenergy.at(1));
             fillEtaPhiMap(emLUT2, eta, phi, (int)jepEMenergy.at(2));           
             if (eta < -3.2 || eta > 3.2) {//for the FCal, the jep elements eta will be 4.05 -> to mimic this in the PPM histo histos fill 3 more bins in eta 
                fillEtaPhiMap(emLUT0, signeta*4.7, phi, (int)jepEMenergy.at(0));
                fillEtaPhiMap(emLUT1, signeta*4.7, phi, (int)jepEMenergy.at(1));
                fillEtaPhiMap(emLUT2, signeta*4.7, phi, (int)jepEMenergy.at(2));
                fillEtaPhiMap(emLUT0, signeta*3.7, phi, (int)jepEMenergy.at(0));
                fillEtaPhiMap(emLUT1, signeta*3.7, phi, (int)jepEMenergy.at(1));
                fillEtaPhiMap(emLUT2, signeta*3.7, phi, (int)jepEMenergy.at(2));
                fillEtaPhiMap(emLUT0, signeta*3.5, phi, (int)jepEMenergy.at(0));
                fillEtaPhiMap(emLUT1, signeta*3.5, phi, (int)jepEMenergy.at(1));
                fillEtaPhiMap(emLUT2, signeta*3.5, phi, (int)jepEMenergy.at(2));    
             }else if (eta < -2.9 || eta > 2.9) {//here the jep element eta will be 3.05 -> to mimic this in the PPM histo fill one more eta bin
                fillEtaPhiMap(emLUT0, signeta*3.15, phi, (int)jepEMenergy.at(0));
                fillEtaPhiMap(emLUT1, signeta*3.15, phi, (int)jepEMenergy.at(1));
                fillEtaPhiMap(emLUT2, signeta*3.15, phi, (int)jepEMenergy.at(2));   
              } 
           }
           if(jepHADenergy.size()> 2){
             fillEtaPhiMap(hadLUT0, eta, phi, (int)jepHADenergy.at(0));
             fillEtaPhiMap(hadLUT1, eta, phi, (int)jepHADenergy.at(1));
             fillEtaPhiMap(hadLUT2, eta, phi, (int)jepHADenergy.at(2));   
             if (eta < -3.2 || eta > 3.2) {//for the FCal, the jep elements are summed horizontally, mimic this in the TH2TT histos -> fill 3 more bins in eta
               fillEtaPhiMap(hadLUT0, signeta*4.7, phi, (int)jepHADenergy.at(0));
               fillEtaPhiMap(hadLUT1, signeta*4.7, phi, (int)jepHADenergy.at(1));
               fillEtaPhiMap(hadLUT2, signeta*4.7, phi, (int)jepHADenergy.at(2));
               fillEtaPhiMap(hadLUT0, signeta*3.7, phi, (int)jepHADenergy.at(0));
               fillEtaPhiMap(hadLUT1, signeta*3.7, phi, (int)jepHADenergy.at(1));
               fillEtaPhiMap(hadLUT2, signeta*3.7, phi, (int)jepHADenergy.at(2));
               fillEtaPhiMap(hadLUT0, signeta*3.5, phi, (int)jepHADenergy.at(0));
               fillEtaPhiMap(hadLUT1, signeta*3.5, phi, (int)jepHADenergy.at(1));
               fillEtaPhiMap(hadLUT2, signeta*3.5, phi, (int)jepHADenergy.at(2));     
             }else if (eta < -2.9 || eta > 2.9) {
               fillEtaPhiMap(hadLUT0, signeta*3.15, phi, (int)jepHADenergy.at(0));
               fillEtaPhiMap(hadLUT1, signeta*3.15, phi, (int)jepHADenergy.at(1));
               fillEtaPhiMap(hadLUT2, signeta*3.15, phi, (int)jepHADenergy.at(2));
	     } 
	   }
	}
      }
      
      m_curHistos++;
  }


  m_h_1d_selectedEvents_mistimedStreamAna->Fill(lumiNo);
  
  //dont forget to delete the decorated TTcontainers
  delete ttContainer;
  delete ttAuxContainer;
 
  return StatusCode::SUCCESS;
}

// ============================================================================
} // end namespace
// ============================================================================
