/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALOMONITORING_L1CALOL1TOPOMONITORALGORITHM_H
#define TRIGT1CALOMONITORING_L1CALOL1TOPOMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"

#include "xAODTrigL1Calo/CMXCPTobContainer.h"
#include "xAODTrigL1Calo/CMXJetTobContainer.h"
#include "xAODTrigL1Calo/L1TopoRawDataContainer.h"
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/RoIBResult.h"
#include "L1TopoRDO/L1TopoRDOCollection.h"
#include "L1TopoRDO/Header.h"
#include "L1TopoRDO/L1TopoTOB.h"
#include "TrigT1Interfaces/FrontPanelCTP.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

/**
 *  @class L1CaloL1TopoMonitorAlgorithm
 *  @brief Monitor legacy CMX to L1Topo transmission of TOBs and timing of the L1Topo trigger bits with respect to L1 accept.
 *  @author Joergen Sjoelin
 **/

class L1CaloL1TopoMonitorAlgorithm : public AthMonitorAlgorithm {
public:L1CaloL1TopoMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~L1CaloL1TopoMonitorAlgorithm()=default;
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

private:
  void jem2Coord(const int crate, const int jem,
		 const int frame, const int location,
		 int &x, int &y, double &eta, double &phi) const;
  void cpm2Coord(const int crate, const int cpm,
		 const int chip, const int location,
		 int &x, int &y, double &eta, double &phi) const;

  StringProperty m_packageName{this,"PackageName","L1CaloL1TopoMonitor",
      "group name for histograming"};
  
  enum class SummaryErrors : uint8_t {
    CALO_CONV=0, NO_CMX, DAQ_CONV,
      NO_DAQ, ROI_CONV, NO_ROI, F_CRC, PAYL_CRC, CMX_MATCH, NUMBEROFBITS
      };
  enum TOB_TYPE {JETL_TOB=0, JETS_TOB, TAU_TOB, EM_TOB, MU_TOB, TOB_TYPES};
  static const int MAXTOBS=30;
   
  // container keys including steering parameter and description
  SG::ReadHandleKey<CTP_RDO> m_ctpRdoKey {this, "CTPRDOLocation",
      LVL1CTP::DEFAULT_RDOOutputLocation,"Key of the CTP RDO object"};
  SG::ReadHandleKey<xAOD::CMXCPTobContainer> m_cmxCpTobLocation {this,
      "CMXCPTobLocation", LVL1::TrigT1CaloDefs::CMXCPTobLocation,
      "CMXCPTob container"};
  SG::ReadHandleKey<xAOD::CMXJetTobContainer> m_cmxJetTobLocation {this,
      "CMXJetTobLocation", LVL1::TrigT1CaloDefs::CMXJetTobLocation,
      "CMXJetTob container"};
  SG::ReadHandleKey<xAOD::L1TopoRawDataContainer> m_l1TopoKey {this,
      "L1TopoRawData", "L1TopoRawData",
      "Key of the L1Topo DAQ object"};
  /*
  SG::ReadHandleKey<ROIB::RoIBResult> m_RoIBResultKey {this,
      "RoIBResult", "RoIBResult",
      "Key to RoIBResult holding decoded L1Topo information from RoIBuilder"};
  SG::ReadHandleKey<LVL1::FrontPanelCTP> m_simTopoCTPKey {this,
      "SimTopoCTPLocation", LVL1::DEFAULT_L1TopoLegacyCTPLocation,
      "Key to simulated topo decision output for CTP"};
  SG::ReadHandleKey<LVL1::FrontPanelCTP> m_simTopoOverflowCTPKey {this,
      "SimTopoOverflowCTPLocation",
      LVL1::DEFAULT_L1TopoLegacyOverflowCTPLocation,
      "Key of the simulated topo overflow output for CTP"};
  */
};
#endif
