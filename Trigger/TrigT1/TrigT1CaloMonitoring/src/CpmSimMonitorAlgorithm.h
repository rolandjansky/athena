/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALOMONITORING_CPMSIMMONITORALGORITHM_H
#define TRIGT1CALOMONITORING_CPMSIMMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrigT1CaloToolInterfaces/IL1CPMTools.h"
#include "TrigT1CaloToolInterfaces/IL1CPCMXTools.h"
#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/CPMTowerContainer.h" 
#include "xAODTrigL1Calo/CMXCPTobContainer.h"
#include "xAODTrigL1Calo/CMXCPHitsContainer.h"
#include "xAODTrigL1Calo/RODHeaderContainer.h"
//
#include "xAODTrigL1Calo/CMXCPHitsAuxContainer.h"
#include "xAODTrigL1Calo/CMXCPTobAuxContainer.h"
#include "xAODTrigL1Calo/CPMTowerAuxContainer.h"
//
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
//
#include "TrigT1CaloUtils/DataError.h"
#include "xAODTrigL1Calo/CPMTobRoIAuxContainer.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1Interfaces/CPRoIDecoder.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"

#include "TrigConfData/L1Menu.h"

class IL1CPCMXTools;
class IL1CPMTools;
class ITrigT1CaloMonErrorTool; 

class CpmSimMonitorAlgorithm : public AthMonitorAlgorithm {
 public:CpmSimMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~CpmSimMonitorAlgorithm()=default;
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

 private:

  // to deal with L1 menu
  ServiceHandle<TrigConf::ITrigConfigSvc> m_configSvc{this, "TrigConfigSvc", "TrigConf::xAODConfigSvc/xAODConfigSvc"};
  const TrigConf::L1Menu* getL1Menu(const EventContext& ctx) const;

  StringProperty m_packageName{this,"PackageName","CpmSimMonitor","group name for histograming"};

  // Error vector StoreGate key
  StringProperty m_errorLocation{this,"ErrorLocation","L1CaloCPMMismatchVector","ErrorVector"};

  Gaudi::Property<int> m_crates{this,"s_crates", 4,  "Number of CPM crates"};
  Gaudi::Property<int> m_modules{this,"s_modules", 14, "Number of modules per crate (modules numbered 1-14)"};
  Gaudi::Property<int> m_maxSlices{this,"s_maxSlices", 5,  "Maximum number of slices"};
  Gaudi::Property<int> m_cmxs{this,"s_cmxs", 2,  "Number of CMXs"};

  // CPM overlap tower container present
  bool m_overlapPresent;

  // Mismatch summary bins
  enum SummaryErrors { EMTowerMismatch, HadTowerMismatch, EMRoIMismatch,
		       TauRoIMismatch, LeftCMXTobMismatch, RightCMXTobMismatch,
                       LocalSumMismatch, RemoteSumMismatch, TotalSumMismatch,
		       TopoMismatch, NumberOfSummaryBins };

  typedef std::vector<int> ErrorVector;

  // container keys including steering parameter and description
  SG::ReadHandleKey<xAOD::TriggerTowerContainer>m_triggerTowerLocation{this, "BS_xAODTriggerTowerContainer",LVL1::TrigT1CaloDefs::xAODTriggerTowerLocation,"TriggerTower Location"};
  SG::ReadHandleKey<xAOD::CPMTowerContainer> m_cpmTowerLocation{this, "CPMTowerLocation", LVL1::TrigT1CaloDefs::CPMTowerLocation, "CPM container"};
  SG::ReadHandleKey<xAOD::CPMTowerContainer> m_cpmTowerLocationOverlap{this, "CPMTowerLocationOverlap",LVL1::TrigT1CaloDefs::CPMTowerLocation + "Overlap", "CPM Overlap container"};
  SG::ReadHandleKey<xAOD::CPMTobRoIContainer> m_cpmTobRoiLocation{this, "CPMTobRoILocation", LVL1::TrigT1CaloDefs::CPMTobRoILocation, "CPM RoI container"};
  SG::ReadHandleKey<xAOD::CMXCPTobContainer> m_cmxCpTobLocation{this, "CMXCPTobLocation", LVL1::TrigT1CaloDefs::CMXCPTobLocation, "CMX CP Tob container"};
  SG::ReadHandleKey<xAOD::CMXCPHitsContainer> m_cmxCpHitsLocation{this, "CMXCPHitsLocation", LVL1::TrigT1CaloDefs::CMXCPHitsLocation, "CMX CP Hits container"};
  SG::ReadHandleKey<xAOD::RODHeaderContainer> m_rodHeaderLocation{this, "RodHeaderLocation", LVL1::TrigT1CaloDefs::RODHeaderLocation, "Rod header container"};

  SG::ReadHandleKey<TrigConf::L1Menu> m_L1MenuKey  { this, "L1TriggerMenu", "DetectorStore+L1TriggerMenu", "L1 Menu" };


  // CP-CMX simulation tool
  ToolHandle<LVL1::IL1CPCMXTools>        m_cpCmxTool;
  // CP simulation tool
  ToolHandle<LVL1::IL1CPMTools>          m_cpmTool;
  // Event veto error tool 
  ToolHandle<LVL1::ITrigT1CaloMonErrorTool>    m_errorTool;  

  //typedef maps
  typedef std::map<int, const xAOD::TriggerTower*> TriggerTowerMapEm;
  typedef std::map<int, const xAOD::TriggerTower*> TriggerTowerMapHad;
  typedef xAOD::CPMTowerMap_t                      CpmTowerMap;
  typedef std::map<int, const xAOD::CMXCPTob*>     CmxCpTobMap;
  typedef std::map<int, const xAOD::CMXCPHits*>    CmxCpHitsMap;
  typedef xAOD::CPMTobRoIMap_t                     CpmTobRoiMap;
 
  // Set up TriggerTower map
  StatusCode setupMap(const xAOD::TriggerTowerContainer* coll,
		      TriggerTowerMapEm& emmap, TriggerTowerMapHad& hadmap) const;

  // Set up CpmTower map
  StatusCode  setupMap(const xAOD::CPMTowerContainer* coll, CpmTowerMap& map) const;

  // Set up CpmTobRoi map - one for read handles and one without?
  StatusCode setupMap(const xAOD::CPMTobRoIContainer* coll, CpmTobRoiMap& map) const;

  // Set up CmxCpTob map
  StatusCode  setupMap(const xAOD::CMXCPTobContainer* coll, CmxCpTobMap& map,
                                           std::vector<int>* parityMap = 0) const;
  // Set up CmxCpHits map
  StatusCode  setupMap(const xAOD::CMXCPHitsContainer* coll, CmxCpHitsMap& map) const;

  // Compare Trigger Towers and CPM Towers
  bool  compareEm(const TriggerTowerMapEm& ttMap, const CpmTowerMap& cpMap,
                      ErrorVector& errors, bool overlap) const;
  bool  compareHad(const TriggerTowerMapHad& ttMap, const CpmTowerMap& cpMap,
                      ErrorVector& errors, bool overlap) const;
 
  // Compare Simulated RoIs with data
  void  compare(const CpmTobRoiMap& roiSimMap, const CpmTobRoiMap& roiMap,
		ErrorVector& errors,
		const xAOD::RODHeaderContainer* rodTES) const;

  // Compare simulated CMX TOBs with data
  void  compare(const CmxCpTobMap& simMap, const CmxCpTobMap& datMap,
		const std::vector<int> & parityMap,
		ErrorVector& errorsCPM, ErrorVector& errorsCMX,
		const xAOD::RODHeaderContainer* rodTES) const;

  // Compare Simulated CMX Hit Sums and Data CMX Hit Sums
  void  compare(const CmxCpHitsMap& cmxSimMap, const CmxCpHitsMap& cmxMap,
		ErrorVector& errors, int selection) const;


  // simulation
  // Simulate CPM RoIs from CPM Towers
  void  simulate(const CpmTowerMap *towers, const CpmTowerMap *towersOv,
		 xAOD::CPMTobRoIContainer* rois, const EventContext& ctx) const;
  // Simulate CPM RoIs from CPM Towers quick version
  void  simulate(const CpmTowerMap* towers, xAOD::CPMTobRoIContainer* rois, const EventContext& ctx) const;


  // Simulate CMX-CP TOBs from CPM RoIs
  void  simulate(const xAOD::CPMTobRoIContainer* rois, xAOD::CMXCPTobContainer* tobs) const;

  // Simulate CMX Hit sums from CMX TOBs
  void  simulate(const xAOD::CMXCPTobContainer* tobs,
		 xAOD::CMXCPHitsContainer* hits, int selection,
		 const EventContext& ctx) const;

  // Simulate CMX Total Hit sums from Remote/Local
  void  simulate(const xAOD::CMXCPHitsContainer* hitsIn,
		 xAOD::CMXCPHitsContainer* hitsOut) const;

  // Return EM FPGA for given crate/phi
  int   fpga(int crate, double phi) const;

  // Return a tower with zero energy if parity bit is set
  CpmTowerMap::mapped_type ttCheck( CpmTowerMap::mapped_type tt, xAOD::CPMTowerContainer* coll) const;

  // Check if LimitedRoISet bit is set
  bool  limitedRoiSet(int crate, SG::ReadHandle<xAOD::RODHeaderContainer> &rodTES) const;
  bool  limitedRoiSet(int crate, const xAOD::RODHeaderContainer* rodTES) const;

  // Flag which threshold hit values are non-zero and the same
  int thresholdsSame(int val1, int val2, int nThresh, int nBits) const;

  //  Flag which threshold hit values are different
  int thresholdsDiff(int val1, int val2, int nThresh, int nBits) const;

  void fillXVsThresholds(Monitored::Scalar<int> &xitem, Monitored::Scalar<int> &yitem,
			 Monitored::Scalar<int> &witem,
			 int x, int val, int nThresh,
			 int nBits, int offset = 0) const;

};
#endif
