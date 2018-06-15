/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArCoverage
 * @author Jessica Leveque <jleveque@in2p3.fr>
 *
 */

#ifndef LARCOVERAGE_H
#define LARCOVERAGE_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloInterface/ICaloNoiseTool.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"

#include "Identifier/HWIdentifier.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArMonTools/SelectAllLArRawChannels.h"
#include "LArMonTools/LArOnlineIDStrHelper.h"
#include "LArRawEvent/LArRawChannel.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "EventContainers/SelectAllObject.h" 
#include "LArTools/LArCablingService.h"
#include "LArRecConditions/ILArBadChannelMasker.h"
#include "LArRecConditions/ILArBadChanTool.h"

#include <string>
#include <map>

class LArEM_ID;
class LArOnlineID;
class CaloDetDescrManager;
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
  StatusCode initialize();

  /** Book general histograms
   *  Implement pure virtual methods of IMonitorToolBase */
  StatusCode bookHistograms();

  /** Called each event */
  StatusCode fillHistograms();

  /** Regularly called to fill noise maps
   *  Overwrite dummy method from MonitorToolBase */
  StatusCode procHistograms();

 protected:

  // services
  const LArOnlineID* m_LArOnlineIDHelper;
  const LArEM_ID*    m_LArEM_IDHelper;
  const LArFCAL_ID*  m_LArFCAL_IDHelper;
  const LArHEC_ID*   m_LArHEC_IDHelper;
  const CaloIdManager*       m_caloIdMgr;
  const CaloDetDescrManager* m_CaloDetDescrMgr;
  const LArCoverageFCALBins* m_FCALBins;

  LArOnlineIDStrHelper* m_strHelper;
  ITHistSvc* m_rootStore;
  /** Handle to LArCablingService */
  ToolHandle<LArCablingService> m_larCablingService;  
  /** Handle to bad-channel tools */
  ToolHandle<ILArBadChannelMasker> m_badChannelMask;
  ToolHandle<ILArBadChanTool> m_badChannelTool;
  ToolHandle<ILArBadChanTool> m_badMNBFEBsTool;
 /** Handle to caloNoiseTool */
  ToolHandle < ICaloNoiseTool > m_caloNoiseTool ;

 private:

  // To retrieve bad channel DB keywords 
  int DBflag(HWIdentifier onID);

  // To set histos Style
  void SetHWCoverageStyle(TH2I_LW* h);
  void SetPartCoverageStyle(TH2I_LW* h);
  void SetBadChannelZaxisLabels(TH2I_LW* h);

  // To keep track of known disabled FEBs
  void FillKnownMissingFEBs(const CaloDetDescrManager* caloDetDescrMgr);

  // To fix empty bins in histos with variable bin size
  void FixEmptyBins();

  // Properties
  std::string m_LArDigitContainerKey;
  std::string m_channelKey;
  int m_nsigma;
  int m_nevents;

  bool m_monitorMNBFEBs;

  // Other things
  int m_eventsCounter;
  std::map<HWIdentifier,int> m_noisycells;

  // Coverage Maps 
  TH2I_LW* m_hCoverageEMBA[4];  TH2I_LW* m_hCoverageEMBC[4];   
  TH2I_LW* m_hCoverageEMECA[4]; TH2I_LW* m_hCoverageEMECC[4]; 
  TH2I_LW* m_hCoverageHECA[4];  TH2I_LW* m_hCoverageHECC[4]; 
  TH2I_LW* m_hCoverageFCALA[4]; TH2I_LW* m_hCoverageFCALC[4]; 

  TH2I_LW* m_hCoverageHWEMBA;  TH2I_LW* m_hCoverageHWEMBC;
  TH2I_LW* m_hCoverageHWEMECA; TH2I_LW* m_hCoverageHWEMECC;
  TH2I_LW* m_hCoverageHWHECA;  TH2I_LW* m_hCoverageHWHECC;
  TH2I_LW* m_hCoverageHWFCALA; TH2I_LW* m_hCoverageHWFCALC;

  // CaloNoiseTool histograms
  TProfile_LW* m_hCaloNoiseToolEM[4];
  TProfile_LW* m_hCaloNoiseToolHEC[4];
  TProfile_LW* m_hCaloNoiseToolFCAL[4];

  // Bad Channels from DB
  TH2I_LW*   m_hBadChannelsBarrelA;
  TH2I_LW*   m_hBadChannelsBarrelC;
  TH2I_LW*   m_hBadChannelsEndcapA;
  TH2I_LW*   m_hBadChannelsEndcapC;

  // Bad MNB FEBs from DB
  TH2I_LW*   m_hSuspiciousMNBFEBsBarrelA;
  TH2I_LW*   m_hSuspiciousMNBFEBsBarrelC;
  TH2I_LW*   m_hSuspiciousMNBFEBsEndcapA;
  TH2I_LW*   m_hSuspiciousMNBFEBsEndcapC;

};

#endif
