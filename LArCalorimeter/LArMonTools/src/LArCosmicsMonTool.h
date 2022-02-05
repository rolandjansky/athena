/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArCosmicsMonTool
 * @author Jessica Leveque <jleveque@in2p3.fr>
 *
 */

#ifndef LARMONTOOLS_LARCOSMICSMONTOOL_H
#define LARMONTOOLS_LARCOSMICSMONTOOL_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "Identifier/HWIdentifier.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "LArElecCalib/ILArPedestal.h"
#include "LArRawEvent/LArDigit.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawChannel.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArRecConditions/LArBadChannelMask.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "StoreGate/ReadCondHandleKey.h"


#include <map>
#include <string>
#include <bitset>
#include <vector>
#include "TH1.h"
#include "TH2I.h"
#include "TH2F.h"
#include "TMath.h"
#include "TProfile2D.h"
#include "TTree.h"

class LArEM_ID;
class LArOnlineID;
class CaloDetDescrElement;
class StoreGateSvc;
class ITHistSvc;

class TH1I;
class TH2I;
class TH2F;
class TProfile2D;
class TTree;

class LArCosmicsMonTool: public ManagedMonitorToolBase
{
 public:
  LArCosmicsMonTool(const std::string& type, 
                      const std::string& name,
		      const IInterface* parent);

  /** @brief Default destructor */
  virtual ~LArCosmicsMonTool();

  /** @brief Overwrite dummy method from AlgTool */
  virtual StatusCode initialize() override;

  /** Book general histograms
   *  Implement pure virtual methods of IMonitorToolBase */
  virtual StatusCode bookHistograms() override;

  /** Called each event */
  virtual StatusCode fillHistograms() override;

  /** Regularly called to fill noise maps
   *  Overwrite dummy method from MonitorToolBase */
  virtual StatusCode procHistograms() override;

 protected:

  // services
  const LArOnlineID* m_LArOnlineIDHelper;
  const LArEM_ID*    m_LArEM_IDHelper;
  const LArFCAL_ID*  m_LArFCAL_IDHelper;
  const LArHEC_ID*   m_LArHEC_IDHelper;
  const CaloIdManager*       m_caloIdMgr;


  //LArOnlineIDStrHelper* m_strHelper;
  ITHistSvc* m_rootStore;
  /** Handle to bad-channel mask */
  LArBadChannelMask m_bcMask;
  SG::ReadCondHandleKey<LArBadChannelCont> m_bcContKey {this, "BadChanKey", "LArBadChannel", "SG key for LArBadChan object"};
  Gaudi::Property<std::vector<std::string> > m_problemsToMask{this,"ProblemsToMask",{}, "Bad-Channel categories to mask"}; 
  
 private:

  // initialize monitoring bookkeeping info
  StatusCode initMonInfo();

  // Properties
  std::string m_LArDigitContainerKey;
  SG::ReadCondHandleKey<ILArPedestal> m_larPedestalKey
  { this, "LArPedestalKey", "Pedestal", "" };
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey
    {this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
      , "CaloDetDescrManager"
      , "CaloDetDescrManager"
      , "SG Key for CaloDetDescrManager in the Condition Store" };
  std::string m_channelKey;
  float m_muonADCthreshold_EM_barrel;
  float m_muonADCthreshold_EM_endcap;
  float m_muonADCthreshold_HEC;
  float m_muonADCthreshold_FCAL;

  // utilities
  int channelInSlotMax(HWIdentifier hardwareID);
  int feedthroughSlotMax(HWIdentifier id);
  static std::string gain_str(CaloGain::CaloGain gain);
  static std::string sampling_str(int sampling);

  // To get physical coordinates
  static StatusCode returnEtaPhiCoord(const CaloDetDescrManager* ddmgr,
                               Identifier offlineID,float& eta,float& phi);

  // Muon Seeds with Digits
  TH2F* m_hMuonMapEMDig; TH2F* m_hMuonMapHECDig; TH2F* m_hMuonMapFCALDig;
  TH1F* m_hMuonTimeEMDig[4]{}; TH1F* m_hMuonTimeHECDig[2]{}; TH1F* m_hMuonTimeFCALDig[2]{};
  TH1F* m_hMuonEnergyEMDig[4]{}; TH1F* m_hMuonEnergyHECDig[2]{}; TH1F* m_hMuonEnergyFCALDig[2]{};
  TH2F* m_hMuonEvsTimeEMDig[4]{}; TH2F* m_hMuonEvsTimeHECDig[2]{}; TH2F* m_hMuonEvsTimeFCALDig[2]{};
  TProfile* m_hMuonShapeEMDig[4]{}; TProfile* m_hMuonShapeHECDig[2]{}; TProfile* m_hMuonShapeFCALDig[2]{};

  // Counters
  int m_eventsCounter;

  bool m_newrun;

};

#endif
