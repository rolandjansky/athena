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

#include "AthenaPoolUtilities/AthenaAttributeList.h"


#include "MistimedStreamMon.h"
// ============================================================================
namespace LVL1 {
// ============================================================================
MistimedStreamMon::MistimedStreamMon(const std::string & type, const std::string & name, const IInterface* parent): ManagedMonitorToolBase ( type, name, parent ),
      m_histBooked(false),
      m_errorTool("LVL1::TrigT1CaloMonErrorTool/TrigT1CaloMonErrorTool"),
      m_histTool("LVL1::TrigT1CaloLWHistogramTool/TrigT1CaloLWHistogramTool"),
      m_ttTool("LVL1::L1TriggerTowerTool/L1TriggerTowerTool"),
      m_h_em_2d_etaPhi_tt_classification_mistimedStreamAna(0),
      m_h_1d_selectedEvents_mistimedStreamAna(0),
      m_h_1d_cutFlow_mistimedStreamAna(0)
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

  std::cout<<"Hello Julia :)"<<std::endl;
  
  return StatusCode::SUCCESS;

}

/*---------------------------------------------------------*/

StatusCode MistimedStreamMon::finalize()
{
  //delete things here
  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode MistimedStreamMon::bookHistogramsRecurrent() // based on bookHistogramsRecurrent from PPrMon
/*---------------------------------------------------------*/
{
  if (msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "in MistimedStreamMon::bookHistograms" << endmsg;
  
  MgmtAttr_t attr = ATTRIB_UNMANAGED;

  if (newRun) {

    //create MonGroup with the folder to store histos
    MonGroup TT_MistimedMon(this, m_PathInRootFile, run, attr);
    m_histTool->setMonGroup(&TT_MistimedMon);

    std::string name, title;
    title = "#eta - #phi Map of TT classification in the EM layer";
    m_h_em_2d_etaPhi_tt_classification_mistimedStreamAna =
        m_histTool->bookPPMEmEtaVsPhi("em_2d_etaPhi_tt_classification_mistimedStreamAna", title);

    m_h_1d_selectedEvents_mistimedStreamAna =
        m_histTool->bookPPMEmEta("1d_selectedEvents_mistimedStreamAna",
                                 "Selected events per lumi block by the MistimedStream analysis");
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

  // Get Bunch crossing number from EventInfo
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

  // =====================================================================
  // ================= Container: TriggerTower ===========================
  // =====================================================================

  xAOD::TriggerTowerContainer_v2::const_iterator TriggerTowerIterator =
      TriggerTowerTES->begin();
  xAOD::TriggerTowerContainer_v2::const_iterator TriggerTowerIteratorEnd =
      TriggerTowerTES->end();

  for (; TriggerTowerIterator != TriggerTowerIteratorEnd;
       ++TriggerTowerIterator) {

    const int layer = (*TriggerTowerIterator)->layer();
    const double eta = (*TriggerTowerIterator)->eta();
    const double phi = (*TriggerTowerIterator)->phi();
//     const int cpET = (*TriggerTowerIterator)->cpET();
//     int jepET = 0;
//     const std::vector<uint_least8_t> jepETvec =
//         (*TriggerTowerIterator)->lut_jep();
//     if (jepETvec.size() > 0) {
//       jepET = (*TriggerTowerIterator)->jepET();
//     }

    // Check if TT is in EM or HAD layer:
    
    if (layer == 0) { //========== ELECTROMAGNETIC LAYER =========================
      //---------------------------- ADC HitMaps per timeslice -----------------
      unsigned int tslice = (*TriggerTowerIterator)->adcPeak();

      if (tslice < ((*TriggerTowerIterator)->adc()).size()) {
//         const int ADC = ((*TriggerTowerIterator)->adc())[tslice];
//           m_h_1d_selectedEvents_mistimedStreamAna->Fill(eta,1);
          m_histTool->fillPPMEmEtaVsPhi(m_h_em_2d_etaPhi_tt_classification_mistimedStreamAna, 
                                        eta, phi, 1);
      }
    }
//     if (layer == 1) { //========== HADRONIC LAYER ===============================
//       //---------------------------- ADC HitMaps per timeslice -----------------
//       unsigned int tslice = (*TriggerTowerIterator)->adcPeak();
// 
//       if (tslice < ((*TriggerTowerIterator)->adc()).size()) {
//         const int ADC = ((*TriggerTowerIterator)->adc())[tslice];
//         if (ADC > m_TT_ADC_HitMap_Thresh) {
//           m_histTool->fillPPMHadEtaVsPhi(m_h_ppm_had_2d_etaPhi_tt_adc_HitMap,
//                                          eta, phi, 1);
//         }
//       }
//     }
  }
  
  return StatusCode::SUCCESS;
}

// ============================================================================
} // end namespace
// ============================================================================

