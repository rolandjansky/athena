/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     CPMSimBSMon.h
// PACKAGE:  TrigT1CaloMonitoring
//
// AUTHOR:   Peter Faulkner
//	     
//
// ********************************************************************
#ifndef CPMSIMBSMON_H
#define CPMSIMBSMON_H

#include <map>
#include <string>
#include <vector>

#include "GaudiKernel/ToolHandle.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "DataModel/DataVector.h"

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
class CPMTower;
class CPMHits;
class CMMCPHits;
class CPMRoI;
class RODHeader;
class TriggerTower;
class IL1EmTauTools;
class IL1CPHitsTools;
class ITrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramToolV1;
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
 *                                Ditto by crate/module/chip/local coordinate        <br>
 *                                Ditto by threshold                               </td></tr>
 *  <tr><td> @c L1Calo/CPM/Errors/Transmission_Simulation/RoIs2Hits                </td><td>
 *                                CPM hits data/simulation matches and mismatches    <br>
 *                                Ditto by threshold                               </td></tr>
 *  <tr><td> @c L1Calo/CPM/Errors/Transmission_Simulation/MismatchEventNumbers     </td><td>
 *                                CPM mismatch event numbers                       </td></tr>
 *  <tr><td> @c L1Calo/CPM_CMM/Errors/Transmission_Simulation/CPM2CMMHits          </td><td>
 *                                CPM/CMM hits matches and mismatches                <br>
 *                                Ditto by threshold                               </td></tr>
 *  <tr><td> @c L1Calo/CPM_CMM/Errors/Transmission_Simulation/Hits2Sums            </td><td>
 *                                Hit sums data/simulation matches and mismatches    <br>
 *                                Ditto by threshold                               </td></tr>
 *  <tr><td> @c L1Calo/CPM_CMM/Errors/Transmission_Simulation/MismatchEventNumbers </td><td>
 *                                CMM mismatch event numbers                       </td></tr>
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
 *           @c <LVL1::CPMRoI>            </td><td> CPM RoI data                             </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CPMHits>           </td><td> CPM Hits data                            </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CMMCPHits>         </td><td> CMM-CP Hits data                         </td></tr>
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
 *  <tr><th> Tool                         </th><th> Description           </th></tr>
 *  <tr><td> @c LVL1::IL1EmTauTools       </td><td> @copydoc m_emTauTool  </td></tr>
 *  <tr><td> @c LVL1::IL1CPHitsTools      </td><td> @copydoc m_cpHitsTool </td></tr>
 *  <tr><td> @c TrigT1CaloMonErrorTool    </td><td> @copydoc m_errorTool  </td></tr>
 *  <tr><td> @c TrigT1CaloLWHistogramToolV1 </td><td> @copydoc m_histTool   </td></tr>
 *  </table>
 *
 *  <b>JobOption Properties:</b>
 *
 *  <table>
 *  <tr><th> Property                   </th><th> Description                        </th></tr>
 *  <tr><td> @c EmTauTool               </td><td> @copydoc m_emTauTool               </td></tr>
 *  <tr><td> @c CPHitsTool              </td><td> @copydoc m_cpHitsTool              </td></tr>
 *  <tr><td> @c CPMTowerLocation        </td><td> @copydoc m_cpmTowerLocation        </td></tr>
 *  <tr><td> @c CPMTowerLocationOverlap </td><td> @copydoc m_cpmTowerLocationOverlap </td></tr>
 *  <tr><td> @c CPMHitsLocation         </td><td> @copydoc m_cpmHitsLocation         </td></tr>
 *  <tr><td> @c CMMCPHitsLocation       </td><td> @copydoc m_cmmCpHitsLocation       </td></tr>
 *  <tr><td> @c CPMRoILocation          </td><td> @copydoc m_cpmRoiLocation          </td></tr>
 *  <tr><td> @c TriggerTowerLocation    </td><td> @copydoc m_triggerTowerLocation    </td></tr>
 *  <tr><td> @c RodHeaderLocation       </td><td> @copydoc m_rodHeaderLocation       </td></tr>
 *  <tr><td> @c RootDirectory           </td><td> @copydoc m_rootDir                 </td></tr>
 *  </table>
 *
 *  <b>Related Documentation:</b>
 *
 *  <a href="http://hepwww.rl.ac.uk/Atlas-L1/Modules/CPM/CPM_Specification_2_03.pdf">
 *  ATLAS Level-1 Calorimeter Trigger Cluster Processor Module</a><br>
 *  <a href="http://hepwww.rl.ac.uk/Atlas-L1/Modules/CMM/CMM_V1_8.pdf">
 *  ATLAS Calorimeter First Level Trigger - Common Merger Module</a><br>
 *  <a href="http://hepwww.rl.ac.uk/Atlas-L1/Modules/ROD/ROD-spec-version1_2_2.pdf">
 *  ATLAS Level-1 Calorimeter Trigger - Read-out Driver</a>
 *
 *  @author Peter Faulkner
 *
 */

class CPMSimBSMon: public ManagedMonitorToolBase
{

public:
  
  CPMSimBSMon(const std::string & type, const std::string & name,
		                        const IInterface* parent);
    

  virtual ~CPMSimBSMon();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
    
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

private:

  /// Mismatch summary bins
  enum SummaryErrors { EMTowerMismatch, HadTowerMismatch, RoIMismatch,
                       CPMHitsMismatch, CMMHitsMismatch, LocalSumMismatch,
		       RemoteSumMismatch, TotalSumMismatch,
		       NumberOfSummaryBins };

  typedef DataVector<LVL1::CPMTower>     CpmTowerCollection;
  typedef DataVector<LVL1::CPMHits>      CpmHitsCollection;
  typedef DataVector<LVL1::CMMCPHits>    CmmCpHitsCollection;
  typedef DataVector<LVL1::CPMRoI>       CpmRoiCollection;
  typedef DataVector<LVL1::TriggerTower> TriggerTowerCollection;
  typedef DataVector<LVL1::CPAlgorithm>  InternalRoiCollection;
  typedef DataVector<LVL1::RODHeader>    RodHeaderCollection;
  
  typedef std::vector<int> ErrorVector;

  typedef std::map<int, LVL1::TriggerTower*> TriggerTowerMap;
  typedef std::map<int, LVL1::CPMTower*>     CpmTowerMap;
  typedef std::map<int, LVL1::CPMRoI*>       CpmRoiMap;
  typedef std::map<int, LVL1::CPMHits*>      CpmHitsMap;
  typedef std::map<int, LVL1::CMMCPHits*>    CmmCpHitsMap;
  
  /// Compare Trigger Towers and CPM Towers
  bool  compare(const TriggerTowerMap& ttMap, const CpmTowerMap& cpMap,
                      ErrorVector& errors, bool overlap);
  /// Compare Simulated RoIs with data
  void  compare(const CpmRoiMap& roiSimMap, const CpmRoiMap& roiMap,
                                                 ErrorVector& errors);
  /// Compare simulated CPM Hits with data
  void  compare(const CpmHitsMap& cpmSimMap, const CpmHitsMap& cpmMap,
                                             ErrorVector& errors);
  /// Compare CPM Hits and CMM Hits
  void  compare(const CpmHitsMap& cpmMap, const CmmCpHitsMap& cmmMap,
                      ErrorVector& errorsCPM, ErrorVector& errorsCMM);
  /// Compare Simulated CMM Hit Sums and Data CMM Hit Sums
  void  compare(const CmmCpHitsMap& cmmSimMap, const CmmCpHitsMap& cmmMap,
                                          ErrorVector& errors, int selection);
  /// Set labels for Overview and summary histograms
  void  setLabels(LWHist* hist, bool xAxis = true);
  /// Set up TriggerTower map
  void  setupMap(const TriggerTowerCollection* coll, TriggerTowerMap& map);
  /// Set up CpmTower map
  void  setupMap(const CpmTowerCollection* coll, CpmTowerMap& map);
  /// Set up CpmRoi map
  void  setupMap(const CpmRoiCollection* coll, CpmRoiMap& map);
  /// Set up CpmHits map
  void  setupMap(const CpmHitsCollection* coll, CpmHitsMap& map);
  /// Set up CmmCpHits map
  void  setupMap(const CmmCpHitsCollection* coll, CmmCpHitsMap& map);
  /// Simulate CPM RoIs from CPM Towers
  void  simulate(const CpmTowerMap towers, const CpmTowerMap towersOv,
                       CpmRoiCollection* rois);
  /// Simulate CPM RoIs from CPM Towers quick version
  void  simulate(const CpmTowerMap towers, CpmRoiCollection* rois);
  /// Simulate CPM Hits from CPM RoIs
  void  simulate(const CpmRoiCollection* rois, CpmHitsCollection* hits);
  /// Simulate CMM Hit sums from CMM Hits
  void  simulate(const CmmCpHitsCollection* hitsIn,
                       CmmCpHitsCollection* hitsOut, int selection);
  /// Return EM FPGA for given crate/phi
  int   fpga(int crate, double phi);
  /// Return a tower with zero energy if parity bit is set
  LVL1::CPMTower* ttCheck(LVL1::CPMTower* tt, CpmTowerCollection* coll);
  /// Check if LimitedRoISet bit is set
  bool  limitedRoiSet(int crate);

  /// CP RoI simulation tool
  ToolHandle<LVL1::IL1EmTauTools>       m_emTauTool;
  /// CPM and CP-CMM hits simulation tool
  ToolHandle<LVL1::IL1CPHitsTools>      m_cpHitsTool;
  /// Event veto error tool
  ToolHandle<ITrigT1CaloMonErrorTool>    m_errorTool;
  /// Histogram helper tool
  ToolHandle<TrigT1CaloLWHistogramToolV1> m_histTool;
  // Debug printout flag
  bool m_debug;

  /// Root directory
  std::string m_rootDir;

  /// CPM core tower container StoreGate key
  std::string m_cpmTowerLocation;
  /// CPM overlap tower container StoreGate key
  std::string m_cpmTowerLocationOverlap;
  /// CPM hits container StoreGate key
  std::string m_cpmHitsLocation;
  /// CMM-CP hits container StoreGate key
  std::string m_cmmCpHitsLocation;
  /// CPM RoI container StoreGate key
  std::string m_cpmRoiLocation;
  /// Trigger Tower container StoreGate key
  std::string m_triggerTowerLocation;
  /// ROD header container StoreGate key
  std::string m_rodHeaderLocation;
  /// Pointer to ROD header container
  const RodHeaderCollection* m_rodTES;

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
  TH2F_LW* m_h_cpm_2d_roi_SimEqData;              ///< CPM RoI Data/Simulation Non-zero Matches
  TH2F_LW* m_h_cpm_2d_roi_SimNeData;              ///< CPM RoI Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_cpm_2d_roi_SimNoData;              ///< CPM RoI Simulation but no Data
  TH2F_LW* m_h_cpm_2d_roi_DataNoSim;              ///< CPM RoI Data but no Simulation
  TH2F_LW* m_h_cpm_2d_roi_ThreshSimEqData;        ///< CPM RoI Data/Simulation Threshold Matches
  TH2F_LW* m_h_cpm_2d_roi_ThreshSimNeData;        ///< CPM RoI Data/Simulation Threshold Mismatches
  TH2F_LW* m_h_cpm_2d_etaPhi_roi_SimEqData;       ///< CPM RoI Data/Simulation Non-zero Matches
  TH2F_LW* m_h_cpm_2d_etaPhi_roi_SimNeData;       ///< CPM RoI Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_cpm_2d_etaPhi_roi_SimNoData;       ///< CPM RoI Simulation but no Data
  TH2F_LW* m_h_cpm_2d_etaPhi_roi_DataNoSim;       ///< CPM RoI Data but no Simulation

  // CPM Hits
  TH2F_LW* m_h_cpm_2d_thresh_SimEqData;           ///< CPM Hits Data/Simulation Non-zero Matches
  TH2F_LW* m_h_cpm_2d_thresh_SimNeData;           ///< CPM Hits Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_cpm_2d_thresh_SimNoData;           ///< CPM Hits Simulation but no Data
  TH2F_LW* m_h_cpm_2d_thresh_DataNoSim;           ///< CPM Hits Data but no Simulation
  TH2F_LW* m_h_cpm_2d_thresh_ThreshSimEqData;     ///< CPM Hits Data/Simulation Threshold Matches
  TH2F_LW* m_h_cpm_2d_thresh_ThreshSimNeData;     ///< CPM Hits Data/Simulation Threshold Mismatches

  // CMM-CP Hits
  TH2F_LW* m_h_cmm_2d_thresh_CpmEqCmm;            ///< CMM Hits/CPM Hits Non-zero Matches
  TH2F_LW* m_h_cmm_2d_thresh_CpmNeCmm;            ///< CMM Hits/CPM Hits Non-zero Mismatches
  TH2F_LW* m_h_cmm_2d_thresh_CpmNoCmm;            ///< CPM Hits but no CMM Hits
  TH2F_LW* m_h_cmm_2d_thresh_CmmNoCpm;            ///< CMM Hits but no CPM Hits
  TH2F_LW* m_h_cmm_2d_thresh_ThreshCpmEqCmm;      ///< CMM Hits/CPM Hits Threshold Matches
  TH2F_LW* m_h_cmm_2d_thresh_ThreshCpmNeCmm;      ///< CMM Hits/CPM Hits Threshold Mismatches

  // CMM-CP Hit Sums
  TH1F_LW* m_h_cmm_1d_thresh_SumsSimEqData;       ///< CMM Hit Sums Data/Simulation Non-zero Matches
  TH1F_LW* m_h_cmm_1d_thresh_SumsSimNeData;       ///< CMM Hit Sums Data/Simulation Non-zero Mismatches
  TH1F_LW* m_h_cmm_1d_thresh_SumsSimNoData;       ///< CMM Hit Sums Simulation but no Data
  TH1F_LW* m_h_cmm_1d_thresh_SumsDataNoSim;       ///< CMM Hit Sums Data but no Simulation
  TH2F_LW* m_h_cmm_2d_thresh_SumsThreshSimEqData; ///< CMM Hit Sums Data/Simulation Threshold Matches
  TH2F_LW* m_h_cmm_2d_thresh_SumsThreshSimNeData; ///< CMM Hit Sums Data/Simulation Threshold Mismatches
  
  // Summary
  TH2F_LW* m_h_cpm_2d_SimEqDataOverview;          ///< CP Transmission/Comparison with Simulation Overview - Events with Matches
  TH2F_LW* m_h_cpm_2d_SimNeDataOverview;          ///< CP Transmission/Comparison with Simulation Overview - Events with Mismatches
  TH1F_LW* m_h_cpm_1d_SimNeDataSummary;           ///< CP Transmission/Comparison with Simulation Mismatch Summary

  std::vector<TH2I_LW*> m_v_2d_MismatchEvents;    ///< Mismatch Event Number Samples

};
// ============================================================================
}  // end namespace
// ============================================================================
#endif
