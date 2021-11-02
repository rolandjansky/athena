//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArCoverage
 * @author Jessica Leveque <jleveque@in2p3.fr>
 *
 */

#ifndef LARMONTOOLS_LARCOVERAGE_H
#define LARMONTOOLS_LARCOVERAGE_H

#include "LArOnlineIDStrHelper.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloGeoHelpers/CaloPhiRange.h"

#include "Identifier/HWIdentifier.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRecConditions/LArBadChannelMask.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "CaloConditions/CaloNoise.h"

#include <string>
#include <map>
#include <memory>

class LArEM_ID;
class LArOnlineID;
class CaloDetDescrElement;
class ITHistSvc;
class LArCoverageFCALBins;

#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile_LW.h"
class TProfile_LW;

class LArCoverage: public ManagedMonitorToolBase
{
 public:
  LArCoverage(const std::string& type, 
                      const std::string& name,
		      const IInterface* parent);

  /** @brief Default destructor */
  virtual ~LArCoverage();

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

private:

  // services
  const LArOnlineID* m_LArOnlineIDHelper;
  const LArEM_ID*    m_LArEM_IDHelper;
  const LArFCAL_ID*  m_LArFCAL_IDHelper;
  const LArHEC_ID*   m_LArHEC_IDHelper;
  const CaloIdManager*       m_caloIdMgr;

  std::unique_ptr<LArOnlineIDStrHelper> m_strHelper;
  ITHistSvc* m_rootStore;
  /** Handle to bad-channel tools */
  LArBadChannelMask m_bcMask;
  Gaudi::Property<std::vector<std::string> > m_problemsToMask{this,"ProblemsToMask",{}, "Bad-Channel categories to mask"}; 

  SG::ReadHandleKey<xAOD::EventInfo> m_EventInfoKey{this, "EventInfoKey", "EventInfo"};
  SG::ReadHandleKey<LArRawChannelContainer> m_rawChannelsKey{this, "LArRawChannelKey", "LArRawChannels"};
  SG::ReadCondHandleKey<LArBadChannelCont> m_BCKey{this, "BadChanKey", "LArBadChannel", "SG bad channels key"};
  SG::ReadCondHandleKey<LArBadFebCont> m_BFKey{this, "MFKey", "LArBadFeb", "SG missing FEBs key"};
  SG::ReadCondHandleKey<CaloNoise> m_noiseCDOKey{this,"CaloNoiseKey","electronicNoise","SG Key of CaloNoise data object"};
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey
    {this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
      , "CaloDetDescrManager"
      , "CaloDetDescrManager"
      , "SG Key for CaloDetDescrManager in the Condition Store" };

  // To retrieve bad channel DB keywords 
  int DBflag(const EventContext& ctx, HWIdentifier onID);

  // To set histos Style
  static void SetHWCoverageStyle(TH2I_LW* h);
  static void SetPartCoverageStyle(TH2I_LW* h);
  static void SetBadChannelZaxisLabels(TH2I_LW* h);

  // To keep track of known disabled FEBs
  void FillKnownMissingFEBs(const EventContext& ctx,
                            const CaloDetDescrManager* caloDetDescrMgr);

  // To fix empty bins in histos with variable bin size
  void FixEmptyBins();

  // Properties
  int m_nevents;

  // Other things
  int m_eventsCounter;

  // Coverage Maps 
  TH2I_LW* m_hCoverageEMBA[4];  TH2I_LW* m_hCoverageEMBC[4];   
  TH2I_LW* m_hCoverageEMECA[4]; TH2I_LW* m_hCoverageEMECC[4]; 
  TH2I_LW* m_hCoverageHECA[4];  TH2I_LW* m_hCoverageHECC[4]; 
  TH2I_LW* m_hCoverageFCALA[4]; TH2I_LW* m_hCoverageFCALC[4]; 

  TH2I_LW* m_hCoverageHWEMBA;  TH2I_LW* m_hCoverageHWEMBC;
  TH2I_LW* m_hCoverageHWEMECA; TH2I_LW* m_hCoverageHWEMECC;
  TH2I_LW* m_hCoverageHWHECA;  TH2I_LW* m_hCoverageHWHECC;
  TH2I_LW* m_hCoverageHWFCALA; TH2I_LW* m_hCoverageHWFCALC;

  // CaloNoise histograms
  TProfile_LW* m_hCaloNoiseEM[4];
  TProfile_LW* m_hCaloNoiseHEC[4];
  TProfile_LW* m_hCaloNoiseFCAL[4];

  // Bad Channels from DB
  TH2I_LW*   m_hBadChannelsBarrelA;
  TH2I_LW*   m_hBadChannelsBarrelC;
  TH2I_LW*   m_hBadChannelsEndcapA;
  TH2I_LW*   m_hBadChannelsEndcapC;


  const std::array<CaloGain::CaloGain,CaloCell_Base_ID::NSUBCALO> m_highestGain{ 
      CaloGain::LARHIGHGAIN,     //LAREM
      CaloGain::LARMEDIUMGAIN,   //LARHEC
      CaloGain::LARHIGHGAIN,     //LARFCAL
      CaloGain::TILEHIGHHIGH,    //TILE
      CaloGain::LARHIGHGAIN      //LARMINIFCAL   
	};


};

#endif
