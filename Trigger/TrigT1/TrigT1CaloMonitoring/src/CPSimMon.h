/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     CPSimMon.h
// PACKAGE:  TrigT1CaloMonitoring
//
// AUTHOR:   Peter Faulkner
//	     
//
// ********************************************************************
#ifndef TRIGT1CALOMONITORING_CPSIMMON_H
#define TRIGT1CALOMONITORING_CPSIMMON_H

#include <map>
#include <string>
#include <vector>

#include "GaudiKernel/ToolHandle.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "AthContainers/DataVector.h"

#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/CPMTower.h"
#include "xAODTrigL1Calo/CMXCPTob.h"
#include "xAODTrigL1Calo/CMXCPHits.h"

#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/CPMTowerContainer.h"
#include "xAODTrigL1Calo/CMXCPTobContainer.h"
#include "xAODTrigL1Calo/CMXCPHitsContainer.h"
#include "xAODTrigL1Calo/RODHeaderContainer.h"
#include "xAODTrigL1Calo/CPMTobRoIContainer.h"


class LWHist;
class TH1F_LW;
class TH2F_LW;
class TH2I_LW;
class StatusCode;

// ============================================================================
namespace LVL1 {
// ============================================================================
// Forward declarations:
// ============================================================================
class CPAlgorithm;


//class TriggerTower;
class IL1EmTauTools;
class IL1CPCMXTools;
class IL1CPMTools;
class ITrigT1CaloMonErrorTool; 
class TrigT1CaloLWHistogramTool;
// ============================================================================

/** Cross-check of CPM data with simulation.
 *
 *  Compares data step-by-step with data simulated from the previous step.
 *
 *  <b>ROOT Histogram Directories:</b>
 *
 *  <table>
 *  <tr><th> Directory  </th><th> Contents                                         </th></tr>
 *  <tr><td> @c L1Calo/CPM/Errors/Transmission_Simulation                          </td><td>
 *                                Summary mismatch plot                              <br>
 *                                Match and mismatch overviews                     </td></tr>
 *  <tr><td> @c L1Calo/CPM/Errors/Transmission_Simulation/PPM2CPMTowers            </td><td>
 *                                TriggerTower/CPMTower eta/phi matches and
 *                                mismatches for core and overlap towers             <br>
 *                                TriggerTower/CPMTower matches and mismatches by FPGA </td></tr>
 *  <tr><td> @c L1Calo/CPM/Errors/Transmission_Simulation/Towers2RoIs              </td><td>
 *                                RoI data/simulation matches and mismatches eta/phi <br>
 *                                Ditto by crate/module/chip/local coordinate      </td></tr>
 *  <tr><td> @c L1Calo/CPM/Errors/Transmission_Simulation/MismatchEventNumbers     </td><td>
 *                                CPM mismatch event numbers                       </td></tr>
 *  <tr><td> @c L1Calo/CPM_CMX/Errors/Transmission_Simulation/RoIs2TOBs            </td><td>
 *                                CPM RoI/CMX TOBs matches and mismatches eta/phi    <br>
 *                                Ditto by crate/module/chip/local coordinate        <br>
 *                                RoI Overflow matches and mismatches              </td></tr>
 *  <tr><td> @c L1Calo/CPM_CMX/Errors/Transmission_Simulation/TOBs2HitSums         </td><td>
 *                                Hit sums data/simulation matches and mismatches    <br>
 *                                Ditto by threshold                               </td></tr>
 *  <tr><td> @c L1Calo/CPM_CMX/Errors/Transmission_Simulation/TOBs2Topo            </td><td>
 *                                Topo output info matches and mismatches          </td></tr>
 *  <tr><td> @c L1Calo/CPM_CMX/Errors/Transmission_Simulation/MismatchEventNumbers </td><td>
 *                                CMX mismatch event numbers                       </td></tr>
 *  </table>
 *
 *  <b>Notes on Particular Histograms:</b>
 *
 *  <table>
 *  <tr><th> Histogram                                     </th><th> Comment       </th></tr>
 *  <tr><td> @c L1Calo/CPM/Errors/Transmission_Simulation/ <br>
 *           @c cpm_1d_SimNeDataSummary                    <br>
 *           @c cpm_2d_Sim{Eq|Ne}DataOverview              </td><td> Simulation steps labelled in red depend on the trigger menu </td></tr>
 *  <tr><td> @c L1Calo/CPM/Errors/Transmission_Simulation/PPM2CPMTowers/ <br>
 *           @c cpm_2d_tt_Ppm{*}CpmFpga                    </td><td> FPGAs labelled in blue are overlap channels </td></tr>
 *  </table>
 *
 *  <b>Custom Merges Used (Tier0):</b>
 *
 *  <table>
 *  <tr><th> Merge                                    </th><th> Used For                    </th></tr>
 *  <tr><td> @ref MergesUsedsection "@c eventSample " </td><td> Mismatch event number plots </td></tr>
 *  </table>
 *
 *  <b>StoreGate Containers Used:</b>
 *
 *  <table>
 *  <tr><th> Container                    </th><th> Comment                                  </th></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::TriggerTower>      </td><td> PPM data                                 </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CPMTower>          </td><td> CPM Tower data                           </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CPMTower>          </td><td> CPM Tower overlap data                   </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CPMTobRoI>         </td><td> CPM RoI data                             </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CMXCPTob>          </td><td> CMX-CP TOB data                          </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CMXCPHits>         </td><td> CMX-CP Hits data and Topo info           </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::RODHeader>         </td><td> ROD header data for LimitedRoISet bit    </td></tr>
 *  <tr><td> @c std::vector<int>          <br>
 *           @c "L1CaloCPMMismatchVector" </td><td> Output.
 *                                               Mismatch summary bits for global histograms </td></tr>
 *  </table>
 *
 *  <b>Tools Used:</b>
 *
 *  <table>
 *  <tr><th> Tool                               </th><th> Description           </th></tr>
 *  <tr><td> @c LVL1::IL1EmTauTools             </td><td> @copydoc m_emTauTool  </td></tr>
 *  <tr><td> @c LVL1::IL1CPCMXTools             </td><td> @copydoc m_cpCmxTool  </td></tr>
 *  <tr><td> @c LVL1::ITrigT1CaloMonErrorTool   </td><td> @copydoc m_errorTool  </td></tr>
 *  <tr><td> @c LVL1::TrigT1CaloLWHistogramTool </td><td> @copydoc m_histTool   </td></tr>
 *  </table>
 *
 *  <b>JobOption Properties:</b>
 *
 *  <table>
 *  <tr><th> Property                   </th><th> Description                        </th></tr>
 *  <tr><td> @c EmTauTool               </td><td> @copydoc m_emTauTool               </td></tr>
 *  <tr><td> @c CPCMXTool               </td><td> @copydoc m_cpCmxTool               </td></tr>
 *  <tr><td> @c ErrorTool               </td><td> @copydoc m_errorTool               </td></tr>
 *  <tr><td> @c HistogramTool           </td><td> @copydoc m_histTool                </td></tr>
 *  <tr><td> @c CPMTowerLocation        </td><td> @copydoc m_cpmTowerLocation        </td></tr>
 *  <tr><td> @c CPMTowerLocationOverlap </td><td> @copydoc m_cpmTowerLocationOverlap </td></tr>
 *  <tr><td> @c CMXCPTobLocation        </td><td> @copydoc m_cmxCpTobLocation        </td></tr>
 *  <tr><td> @c CMXCPHitsLocation       </td><td> @copydoc m_cmxCpHitsLocation       </td></tr>
 *  <tr><td> @c CPMTobRoILocation       </td><td> @copydoc m_cpmTobRoiLocation       </td></tr>
 *  <tr><td> @c TriggerTowerLocation    </td><td> @copydoc m_triggerTowerLocation    </td></tr>
 *  <tr><td> @c RodHeaderLocation       </td><td> @copydoc m_rodHeaderLocation       </td></tr>
 *  <tr><td> @c ErrorLocation           </td><td> @copydoc m_errorLocation           </td></tr>
 *  <tr><td> @c RootDirectory           </td><td> @copydoc m_rootDir                 </td></tr>
 *  </table>
 *
 *  <b>Related Documentation:</b>                                 <!-- UPDATE!! -->
 *
 *  <a href="http://hepwww.rl.ac.uk/Atlas-L1/Modules/CPM/CPM_Specification_2_03.pdf">
 *  ATLAS Level-1 Calorimeter Trigger Cluster Processor Module</a><br>
 *  <!--
 *  <a href="http://hepwww.rl.ac.uk/Atlas-L1/Modules/CMM/CMM_V1_8.pdf">
 *  ATLAS Calorimeter First Level Trigger - Common Merger Module</a><br>
 *  -->
 *  <a href="http://hepwww.rl.ac.uk/Atlas-L1/Modules/ROD/ROD-spec-version1_2_2.pdf">
 *  ATLAS Level-1 Calorimeter Trigger - Read-out Driver</a>
 *
 *  @author Peter Faulkner
 *
 */

class CPSimMon: public ManagedMonitorToolBase
{

public:
  
  CPSimMon(const std::string & type, const std::string & name,
		                     const IInterface* parent);
    

  virtual ~CPSimMon();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
    
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();
  
private:

  /// Mismatch summary bins
  enum SummaryErrors { EMTowerMismatch, HadTowerMismatch, EMRoIMismatch,
		       TauRoIMismatch, LeftCMXTobMismatch, RightCMXTobMismatch,
                       LocalSumMismatch, RemoteSumMismatch, TotalSumMismatch,
		       TopoMismatch, NumberOfSummaryBins };

  //typedef DataVector<LVL1::CPMTower>     CpmTowerCollection;
  //typedef DataVector<LVL1::CMXCPTob>     CmxCpTobCollection;
  //typedef DataVector<LVL1::CMXCPHits>    CmxCpHitsCollection;  
  //typedef DataVector<LVL1::TriggerTower> TriggerTowerCollection;  

  typedef DataVector<LVL1::CPAlgorithm>  InternalRoiCollection;
  typedef std::vector<int> ErrorVector;

  //typedef maps @@
  //typedef std::map<int, LVL1::TriggerTower*> TriggerTowerMap;
  typedef std::map<int, const xAOD::TriggerTower*> TriggerTowerMapEm;
  typedef std::map<int, const xAOD::TriggerTower*> TriggerTowerMapHad;
  typedef std::map<int, xAOD::CPMTower*>     CpmTowerMap;
  //  typedef std::map<int, const xAOD::CPMTower*>*    CpmTowerMapP;
  typedef std::map<int, const xAOD::CMXCPTob*>     CmxCpTobMap;
  typedef std::map<int, const xAOD::CMXCPHits*>    CmxCpHitsMap;
  typedef std::map<int, xAOD::CPMTobRoI*>    CpmTobRoiMap;

  static const int s_crates  = 4;
  static const int s_modules = 14;
  static const int s_cmxs    = 2;
  
  /// Compare Trigger Towers and CPM Towers
  bool  compareEm(const TriggerTowerMapEm& ttMap, const CpmTowerMap& cpMap,
                      ErrorVector& errors, bool overlap);
  bool  compareHad(const TriggerTowerMapHad& ttMap, const CpmTowerMap& cpMap,
                      ErrorVector& errors, bool overlap);
  /// Compare Simulated RoIs with data
  void  compare(const CpmTobRoiMap& roiSimMap, const CpmTobRoiMap& roiMap,
                                                 ErrorVector& errors);
  /// Compare simulated CMX TOBs with data
  void  compare(const CmxCpTobMap& simMap, const CmxCpTobMap& datMap,
                      const std::vector<int> parityMap,
                      ErrorVector& errorsCPM, ErrorVector& errorsCMX);
  /// Compare Simulated CMX Hit Sums and Data CMX Hit Sums
  void  compare(const CmxCpHitsMap& cmxSimMap, const CmxCpHitsMap& cmxMap,
                                          ErrorVector& errors, int selection);
  /// Set labels for Overview and summary histograms
  void  setLabels(LWHist* hist, bool xAxis = true);
  /// Set labels for Topo histograms
  void  setLabelsTopo(TH2F_LW* hist);
  /// Set up TriggerTower map
  void  setupMap(const xAOD::TriggerTowerContainer* coll,
                 TriggerTowerMapEm& emmap, TriggerTowerMapHad& hadmap);
  /// Set up CpmTower map
  void  setupMap(const xAOD::CPMTowerContainer* coll, CpmTowerMap& map);
  /// Set up CpmTobRoi map
  void  setupMap(const xAOD::CPMTobRoIContainer* coll, CpmTobRoiMap& map);
  /// Set up CmxCpTob map
  void  setupMap(const xAOD::CMXCPTobContainer* coll, CmxCpTobMap& map,
                                           std::vector<int>* parityMap = 0);
  /// Set up CmxCpHits map
  void  setupMap(const xAOD::CMXCPHitsContainer* coll, CmxCpHitsMap& map);
  /// Simulate CPM RoIs from CPM Towers
  void  simulate(const CpmTowerMap *towers, const CpmTowerMap *towersOv,
                       xAOD::CPMTobRoIContainer* rois);
  /// Simulate CPM RoIs from CPM Towers quick version
  void  simulate(const CpmTowerMap* towers, xAOD::CPMTobRoIContainer* rois);
  /// Simulate CMX-CP TOBs from CPM RoIs
  void  simulate(const xAOD::CPMTobRoIContainer* rois, xAOD::CMXCPTobContainer* tobs);
  /// Simulate CMX Hit sums from CMX TOBs
  void  simulate(const xAOD::CMXCPTobContainer* tobs,
		 xAOD::CMXCPHitsContainer* hits, int selection);
  /// Simulate CMX Total Hit sums from Remote/Local
  void  simulate(const xAOD::CMXCPHitsContainer* hitsIn,
		 xAOD::CMXCPHitsContainer* hitsOut);
  /// Return EM FPGA for given crate/phi
  int   fpga(int crate, double phi);
  /// Return a tower with zero energy if parity bit is set
  xAOD::CPMTower* ttCheck( xAOD::CPMTower* tt, xAOD::CPMTowerContainer* coll);
  /// Check if LimitedRoISet bit is set
  bool  limitedRoiSet(int crate);

  /// CP RoI simulation tool
  ToolHandle<LVL1::IL1EmTauTools>        m_emTauTool;
  /// CP-CMX simulation tool
  ToolHandle<LVL1::IL1CPCMXTools>        m_cpCmxTool;
  /// CP simulation tool
  ToolHandle<LVL1::IL1CPMTools>          m_cpmTool;
  /// Event veto error tool
  ToolHandle<ITrigT1CaloMonErrorTool>    m_errorTool;  
  /// Histogram helper tool
  ToolHandle<TrigT1CaloLWHistogramTool>  m_histTool;
  // Debug printout flag
  bool m_debug;

  /// Root directory
  std::string m_rootDir;

  /// CPM core tower container StoreGate key
  std::string m_cpmTowerLocation;
  /// CPM overlap tower container StoreGate key
  std::string m_cpmTowerLocationOverlap;
  /// CMX-CP TOBs container StoreGate key
  std::string m_cmxCpTobLocation;
  /// CMX-CP hits container StoreGate key
  std::string m_cmxCpHitsLocation;
  /// CPM RoI container StoreGate key
  std::string m_cpmTobRoiLocation;
  /// Trigger Tower container StoreGate key
  //std::string m_triggerTowerLocation;
  /// xAODTriggerTower Container key
  std::string m_triggerTowerLocation;
  /// ROD header container StoreGate key
  std::string m_rodHeaderLocation;
  /// Error vector StoreGate key
  std::string m_errorLocation;
  /// Pointer to ROD header container
  const xAOD::RODHeaderContainer* m_rodTES;

  /// CPM overlap tower container present
  bool m_overlapPresent;
  /// LimitedRoISet flags
  int m_limitedRoi;
  /// Histograms booked flag
  bool m_histBooked;

  //=======================
  //   Match/Mismatch plots
  //=======================

  // CPM Towers
  TH2F_LW* m_h_cpm_em_2d_etaPhi_tt_PpmEqCore;     ///< EM Core CPM Tower/PPM Tower Non-zero Matches
  TH2F_LW* m_h_cpm_em_2d_etaPhi_tt_PpmNeCore;     ///< EM Core CPM Tower/PPM Tower Non-zero Mismatches
  TH2F_LW* m_h_cpm_em_2d_etaPhi_tt_PpmNoCore;     ///< EM PPM Towers but no Core CPM Towers
  TH2F_LW* m_h_cpm_em_2d_etaPhi_tt_CoreNoPpm;     ///< EM Core CPM Towers but no PPM Towers
  TH2F_LW* m_h_cpm_had_2d_etaPhi_tt_PpmEqCore;    ///< HAD Core CPM Tower/PPM Tower Non-zero Matches
  TH2F_LW* m_h_cpm_had_2d_etaPhi_tt_PpmNeCore;    ///< HAD Core CPM Tower/PPM Tower Non-zero Mismatches
  TH2F_LW* m_h_cpm_had_2d_etaPhi_tt_PpmNoCore;    ///< HAD PPM Towers but no Core CPM Towers
  TH2F_LW* m_h_cpm_had_2d_etaPhi_tt_CoreNoPpm;    ///< HAD Core CPM Towers but no PPM Towers
  TH2F_LW* m_h_cpm_em_2d_etaPhi_tt_PpmEqOverlap;  ///< EM Overlap CPM Tower/PPM Tower Non-zero Matches
  TH2F_LW* m_h_cpm_em_2d_etaPhi_tt_PpmNeOverlap;  ///< EM Overlap CPM Tower/PPM Tower Non-zero Mismatches
  TH2F_LW* m_h_cpm_em_2d_etaPhi_tt_PpmNoOverlap;  ///< EM PPM Towers but no Overlap CPM Towers
  TH2F_LW* m_h_cpm_em_2d_etaPhi_tt_OverlapNoPpm;  ///< EM Overlap CPM Towers but no PPM Towers
  TH2F_LW* m_h_cpm_had_2d_etaPhi_tt_PpmEqOverlap; ///< HAD Overlap CPM Tower/PPM Tower Non-zero Matches
  TH2F_LW* m_h_cpm_had_2d_etaPhi_tt_PpmNeOverlap; ///< HAD Overlap CPM Tower/PPM Tower Non-zero Mismatches
  TH2F_LW* m_h_cpm_had_2d_etaPhi_tt_PpmNoOverlap; ///< HAD PPM Towers but no Overlap CPM Towers
  TH2F_LW* m_h_cpm_had_2d_etaPhi_tt_OverlapNoPpm; ///< HAD Overlap CPM Towers but no PPM Towers
  TH2F_LW* m_h_cpm_2d_tt_PpmEqCpmFpga;            ///< CPM Tower/PPM Tower Non-zero Matches by FPGA
  TH2F_LW* m_h_cpm_2d_tt_PpmNeCpmFpga;            ///< CPM Tower/PPM Tower Non-zero Mismatches by FPGA
  TH2F_LW* m_h_cpm_2d_tt_PpmNoCpmFpga;            ///< PPM Towers but no CPM Towers by FPGA
  TH2F_LW* m_h_cpm_2d_tt_CpmNoPpmFpga;            ///< CPM Towers but no PPM Towers by FPGA

  // RoI
  TH2F_LW* m_h_cpm_2d_roi_EmEnergySimEqData;      ///< CPM RoI EM Energy Data/Simulation Non-zero Matches
  TH2F_LW* m_h_cpm_2d_roi_EmEnergySimNeData;      ///< CPM RoI EM Energy Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_cpm_2d_roi_EmEnergySimNoData;      ///< CPM RoI EM Energy Simulation but no Data
  TH2F_LW* m_h_cpm_2d_roi_EmEnergyDataNoSim;      ///< CPM RoI EM Energy Data but no Simulation
  TH2F_LW* m_h_cpm_2d_roi_TauEnergySimEqData;     ///< CPM RoI Tau Energy Data/Simulation Non-zero Matches
  TH2F_LW* m_h_cpm_2d_roi_TauEnergySimNeData;     ///< CPM RoI Tau Energy Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_cpm_2d_roi_TauEnergySimNoData;     ///< CPM RoI Tau Energy Simulation but no Data
  TH2F_LW* m_h_cpm_2d_roi_TauEnergyDataNoSim;     ///< CPM RoI Tau Energy Data but no Simulation
  TH2F_LW* m_h_cpm_2d_roi_EmIsolSimEqData;        ///< CPM RoI EM Isolation Data/Simulation Non-zero Matches
  TH2F_LW* m_h_cpm_2d_roi_EmIsolSimNeData;        ///< CPM RoI EM Isolation Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_cpm_2d_roi_EmIsolSimNoData;        ///< CPM RoI EM Isolation Simulation but no Data
  TH2F_LW* m_h_cpm_2d_roi_EmIsolDataNoSim;        ///< CPM RoI EM Isolation Data but no Simulation
  TH2F_LW* m_h_cpm_2d_roi_TauIsolSimEqData;       ///< CPM RoI Tau Isolation Data/Simulation Non-zero Matches
  TH2F_LW* m_h_cpm_2d_roi_TauIsolSimNeData;       ///< CPM RoI Tau Isolation Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_cpm_2d_roi_TauIsolSimNoData;       ///< CPM RoI Tau Isolation Simulation but no Data
  TH2F_LW* m_h_cpm_2d_roi_TauIsolDataNoSim;       ///< CPM RoI Tau Isolation Data but no Simulation
  TH2F_LW* m_h_cpm_2d_etaPhi_roi_SimEqData;       ///< CPM RoI Data/Simulation Non-zero Matches
  TH2F_LW* m_h_cpm_2d_etaPhi_roi_SimNeData;       ///< CPM RoI Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_cpm_2d_etaPhi_roi_SimNoData;       ///< CPM RoI Simulation but no Data
  TH2F_LW* m_h_cpm_2d_etaPhi_roi_DataNoSim;       ///< CPM RoI Data but no Simulation

  // CMX-CP TOBs
  TH2F_LW* m_h_cmx_2d_tob_LeftEnergySimEqData;    ///< CMX TOB Left Energy Data/Simulation Non-zero Matches
  TH2F_LW* m_h_cmx_2d_tob_LeftEnergySimNeData;    ///< CMX TOB Left Energy Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_cmx_2d_tob_LeftEnergySimNoData;    ///< CMX TOB Left Energy Simulation but no Data
  TH2F_LW* m_h_cmx_2d_tob_LeftEnergyDataNoSim;    ///< CMX TOB Left Energy Data but no Simulation
  TH2F_LW* m_h_cmx_2d_tob_RightEnergySimEqData;   ///< CMX TOB Right Energy Data/Simulation Non-zero Matches
  TH2F_LW* m_h_cmx_2d_tob_RightEnergySimNeData;   ///< CMX TOB Right Energy Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_cmx_2d_tob_RightEnergySimNoData;   ///< CMX TOB Right Energy Simulation but no Data
  TH2F_LW* m_h_cmx_2d_tob_RightEnergyDataNoSim;   ///< CMX TOB Right Energy Data but no Simulation
  TH2F_LW* m_h_cmx_2d_tob_LeftIsolSimEqData;      ///< CMX TOB Left Isolation Data/Simulation Non-zero Matches
  TH2F_LW* m_h_cmx_2d_tob_LeftIsolSimNeData;      ///< CMX TOB Left Isolation Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_cmx_2d_tob_LeftIsolSimNoData;      ///< CMX TOB Left Isolation Simulation but no Data
  TH2F_LW* m_h_cmx_2d_tob_LeftIsolDataNoSim;      ///< CMX TOB Left Isolation Data but no Simulation
  TH2F_LW* m_h_cmx_2d_tob_RightIsolSimEqData;     ///< CMX TOB Right Isolation Data/Simulation Non-zero Matches
  TH2F_LW* m_h_cmx_2d_tob_RightIsolSimNeData;     ///< CMX TOB Right Isolation Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_cmx_2d_tob_RightIsolSimNoData;     ///< CMX TOB Right Isolation Simulation but no Data
  TH2F_LW* m_h_cmx_2d_tob_RightIsolDataNoSim;     ///< CMX TOB Right Isolation Data but no Simulation
  TH2F_LW* m_h_cmx_2d_tob_OverflowSimEqData;      ///< CMX TOB RoI Overflow Bit Data/Simulation Matches
  TH2F_LW* m_h_cmx_2d_tob_OverflowSimNeData;      ///< CMX TOB RoI Overflow Bit Data/Simulation Mismatches
  TH2F_LW* m_h_cmx_2d_etaPhi_tob_SimEqData;       ///< CMX TOB Data/Simulation Non-zero Matches
  TH2F_LW* m_h_cmx_2d_etaPhi_tob_SimNeData;       ///< CMX TOB Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_cmx_2d_etaPhi_tob_SimNoData;       ///< CMX TOB Simulation but no Data
  TH2F_LW* m_h_cmx_2d_etaPhi_tob_DataNoSim;       ///< CMX TOB Data but no Simulation

  // CMX-CP Hit Sums
  TH1F_LW* m_h_cmx_1d_thresh_SumsSimEqData;       ///< CMX Hit Sums Data/Simulation Non-zero Matches
  TH1F_LW* m_h_cmx_1d_thresh_SumsSimNeData;       ///< CMX Hit Sums Data/Simulation Non-zero Mismatches
  TH1F_LW* m_h_cmx_1d_thresh_SumsSimNoData;       ///< CMX Hit Sums Simulation but no Data
  TH1F_LW* m_h_cmx_1d_thresh_SumsDataNoSim;       ///< CMX Hit Sums Data but no Simulation
  TH2F_LW* m_h_cmx_2d_thresh_LeftSumsSimEqData;   ///< CMX Hit Sums Left Data/Simulation Threshold Non-zero Matches
  TH2F_LW* m_h_cmx_2d_thresh_LeftSumsSimNeData;   ///< CMX Hit Sums Left Data/Simulation Threshold Non-zero Mismatches
  TH2F_LW* m_h_cmx_2d_thresh_RightSumsSimEqData;  ///< CMX Hit Sums Right Data/Simulation Threshold Non-zero Matches
  TH2F_LW* m_h_cmx_2d_thresh_RightSumsSimNeData;  ///< CMX Hit Sums Right Data/Simulation Threshold Non-zero Mismatches

  // Topo output information
  TH2F_LW* m_h_cmx_2d_topo_SimEqData;             ///< CMX Topo Output Data/Simulation Non-zero Matches
  TH2F_LW* m_h_cmx_2d_topo_SimNeData;             ///< CMX Topo Output Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_cmx_2d_topo_SimNoData;             ///< CMX Topo Output Simulation but no Data
  TH2F_LW* m_h_cmx_2d_topo_DataNoSim;             ///< CMX Topo Output Data but no Simulation
  
  // Summary
  TH2F_LW* m_h_cpm_2d_SimEqDataOverview;          ///< CP Transmission/Comparison with Simulation Overview - Events with Matches
  TH2F_LW* m_h_cpm_2d_SimNeDataOverview;          ///< CP Transmission/Comparison with Simulation Overview - Events with Mismatches
  TH1F_LW* m_h_cpm_1d_SimNeDataSummary;           ///< CP Transmission/Comparison with Simulation Mismatch Summary

  std::vector<TH2I_LW*> m_v_2d_MismatchEvents;    ///< Mismatch Event Number Samples

};
// ============================================================================
} // end namespace
// ============================================================================

#endif
