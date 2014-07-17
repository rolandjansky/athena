/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     JEPSimBSMon.h
// PACKAGE:  TrigT1CaloMonitoring
//
// AUTHOR:   Peter Faulkner
//	     
//
// ********************************************************************
#ifndef JEPSIMBSMON_H
#define JEPSIMBSMON_H

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
class TrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramTool;

namespace LVL1 {
  class CMMEtSums;
  class CMMJetHits;
  class CMMRoI;
  class JEMEtSums;
  class JEMHits;
  class JEMRoI;
  class JetAlgorithm;
  class JetElement;
  class RODHeader;
  class TriggerTower;
  class IL1JEPHitsTools;
  class IL1JetElementTools;
  class IL1JetTools;
  class IL1JEPEtSumsTools;
}

/** Cross-check of JEM and JEM-CMM data with simulation.
 *
 *  Compares data step-by-step with data simulated from the previous step.
 *
 *  <b>ROOT Histogram Directories:</b>
 *
 *  <table>
 *  <tr><th> Directory                                                             </th><th>
 *                 Contents                                                        </th></tr>
 *  <tr><td> @c L1Calo/JEM/Errors/Transmission_Simulation                          </td><td>
 *                 Summary mismatch plot                                           <br>
 *                 Match and mismatch overviews                                    </td></tr>
 *  <tr><td> @c L1Calo/JEM/Errors/Transmission_Simulation/PPM2Elements             </td><td>
 *                 Jet element core/overlap data/simulation matches and mismatches </td></tr>
 *  <tr><td> @c L1Calo/JEM/Errors/Transmission_Simulation/Elements2RoIs            </td><td>
 *                 RoI data/simulation matches and mismatches eta/phi              <br>
 *                 Ditto by crate/module/frame/local coordinate                    <br>
 *                 Ditto by threshold                                              </td></tr>
 *  <tr><td> @c L1Calo/JEM/Errors/Transmission_Simulation/RoIs2Hits                </td><td>
 *                 JEM hits data/simulation matches and mismatches                 <br>
 *                 Ditto by threshold                                              </td></tr>
 *  <tr><td> @c L1Calo/JEM/Errors/Transmission_Simulation/Elements2Energy          </td><td>
 *                 JEM Energy sums data/simulation matches and mismatches          </td></tr>
 *  <tr><td> @c L1Calo/JEM/Errors/Transmission_Simulation/MismatchEventNumbers     </td><td>
 *                 JEM mismatch event numbers                                      </td></tr>
 *  <tr><td> @c L1Calo/JEM_CMM/Errors/Transmission_Simulation/JEM2CMMHits          </td><td>
 *                 JEM/CMM hits matches and mismatches                             <br>
 *                 Ditto by threshold                                              </td></tr>
 *  <tr><td> @c L1Calo/JEM_CMM/Errors/Transmission_Simulation/Hits2Sums            </td><td>
 *                 Hit sums data/simulation matches and mismatches                 <br>
 *                 Ditto by threshold                                              </td></tr>
 *  <tr><td> @c L1Calo/JEM_CMM/Errors/Transmission_Simulation/JEM2CMMEnergy        </td><td>
 *                 JEM/CMM Energy sums matches and mismatches                      </td></tr>
 *  <tr><td> @c L1Calo/JEM_CMM/Errors/Transmission_Simulation/Energy2Sums          </td><td>
 *                 Energy totals data/simulation matches and mismatches            <br>
 *                 Ditto for Et maps by threshold                                  </td></tr>
 *  <tr><td> @c L1Calo/JEM_CMM/Errors/Transmission_Simulation/MismatchEventNumbers </td><td>
 *                 JEM CMM mismatch event numbers                                  </td></tr>
 *  </table>
 *
 *  <b>Notes on Particular Histograms:</b>
 *
 *  <table>
 *  <tr><th> Histogram                                     </th><th> Comment                                 </th></tr>
 *  <tr><td> @c L1Calo/JEM/Errors/Transmission_Simulation/ <br>
 *           @c jem_1d_SimNeDataSummary                    <br>
 *           @c jem_2d_Sim{Eq|Ne}DataOverview              </td><td> Simulation steps labelled in red depend on the trigger menu </td></tr>
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
 *  <tr><th> Container                    </th><th> Comment                                     </th></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::TriggerTower>      </td><td> PPM data                                    </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::JetElement>        </td><td> Jet element core data                       </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::JetElement>        </td><td> Jet element overlap data                    </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::JEMRoI>            </td><td> Jet RoI data                                </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::JEMHits>           </td><td> Jet hits data                               </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::JEMEtSums>         </td><td> Energy sums data                            </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CMMJetHits>        </td><td> CMM Jet hits data                           </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CMMEtSums>         </td><td> CMM Energy sums data                        </td></tr>
 *  <tr><td> @c LVL1::CMMRoI              </td><td> CMM RoI data                                </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::RODHeader>         </td><td> ROD header data for LimitedRoISet bit
 *                                                  and MinorVersion number                     </td></tr>
 *  <tr><td> @c std::vector<int>          <br>
 *           @c "L1CaloJEMMismatchVector" </td><td> Output.
 *                                                  Mismatch summary bits for global histograms </td></tr>
 *  </table>
 *
 *  <b>Tools Used:</b>
 *
 *  <table>
 *  <tr><th> Tool                         </th><th> Description               </th></tr>
 *  <tr><td> @c LVL1::IL1JEPHitsTools     </td><td> @copydoc m_jepHitsTool    </td></tr>
 *  <tr><td> @c LVL1::IL1JetTools         </td><td> @copydoc m_jetTool        </td></tr>
 *  <tr><td> @c LVL1::IL1JetElementTools  </td><td> @copydoc m_jetElementTool </td></tr>
 *  <tr><td> @c LVL1::IL1JEPEtSumsTools   </td><td> @copydoc m_etSumsTool     </td></tr>
 *  <tr><td> @c TrigT1CaloMonErrorTool    </td><td> @copydoc m_errorTool      </td></tr>
 *  <tr><td> @c TrigT1CaloLWHistogramTool </td><td> @copydoc m_histTool       </td></tr>
 *  </table>
 *
 *  <b>JobOption Properties:</b>
 *
 *  <table>
 *  <tr><th> Property                     </th><th> Description                          </th></tr>
 *  <tr><td> @c JEPHitsTool               </td><td> @copydoc m_jepHitsTool               </td></tr>
 *  <tr><td> @c JetTool                   </td><td> @copydoc m_jetTool                   </td></tr>
 *  <tr><td> @c JetElementTool            </td><td> @copydoc m_jetElementTool            </td></tr>
 *  <tr><td> @c JEPEtSumsTool             </td><td> @copydoc m_etSumsTool                </td></tr>
 *  <tr><td> @c JetElementLocation        </td><td> @copydoc m_jetElementLocation        </td></tr>
 *  <tr><td> @c JetElementLocationOverlap </td><td> @copydoc m_jetElementLocationOverlap </td></tr>
 *  <tr><td> @c JEMHitsLocation           </td><td> @copydoc m_jemHitsLocation           </td></tr>
 *  <tr><td> @c CMMJetHitsLocation        </td><td> @copydoc m_cmmJetHitsLocation        </td></tr>
 *  <tr><td> @c JEMRoILocation            </td><td> @copydoc m_jemRoiLocation            </td></tr>
 *  <tr><td> @c CMMRoILocation            </td><td> @copydoc m_cmmRoiLocation            </td></tr>
 *  <tr><td> @c JEMEtSumsLocation         </td><td> @copydoc m_jemEtSumsLocation         </td></tr>
 *  <tr><td> @c CMMEtSumsLocation         </td><td> @copydoc m_cmmEtSumsLocation         </td></tr>
 *  <tr><td> @c TriggerTowerLocation      </td><td> @copydoc m_triggerTowerLocation      </td></tr>
 *  <tr><td> @c RodHeaderLocation         </td><td> @copydoc m_rodHeaderLocation         </td></tr>
 *  <tr><td> @c RootDirectory             </td><td> @copydoc m_rootDir                   </td></tr>
 *  </table>
 *
 *  <b>Related Documentation:</b>
 *
 *  <a href="http://hepwww.rl.ac.uk/Atlas-L1/Modules/JEM/JEMspec12d.pdf">
 *  ATLAS Level-1 Calorimeter Trigger Jet / Energy Processor Module</a><br>
 *  <a href="http://hepwww.rl.ac.uk/Atlas-L1/Modules/CMM/CMM_V1_8.pdf">
 *  ATLAS Calorimeter First Level Trigger - Common Merger Module</a><br>
 *  <a href="http://hepwww.rl.ac.uk/Atlas-L1/Modules/ROD/ROD-spec-version1_2_2.pdf">
 *  ATLAS Level-1 Calorimeter Trigger - Read-out Driver</a>
 *
 *  @author Peter Faulkner
 *
 */

class JEPSimBSMon: public ManagedMonitorToolBase
{

public:
  
  JEPSimBSMon(const std::string & type, const std::string & name,
		       const IInterface* parent);
    

  virtual ~JEPSimBSMon();

  virtual StatusCode initialize();
    
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

private:

  // Error summary bins
  enum SummaryErrors { EMElementMismatch, HadElementMismatch, RoIMismatch,
                       JEMHitsMismatch, CMMJetHitsMismatch, LocalJetMismatch,
		       RemoteJetMismatch, TotalJetMismatch, JetEtMismatch,
		       JetEtRoIMismatch, JEMEtSumsMismatch, CMMEtSumsMismatch,
		       LocalEnergyMismatch, RemoteEnergyMismatch,
		       TotalEnergyMismatch, SumEtMismatch, MissingEtMismatch,
		       MissingEtSigMismatch, EnergyRoIMismatch,
		       NumberOfSummaryBins };

  typedef DataVector<LVL1::JetElement>   JetElementCollection;
  typedef DataVector<LVL1::JEMHits>      JemHitsCollection;
  typedef DataVector<LVL1::CMMJetHits>   CmmJetHitsCollection;
  typedef DataVector<LVL1::JEMRoI>       JemRoiCollection;
  typedef DataVector<LVL1::TriggerTower> TriggerTowerCollection;
  typedef DataVector<LVL1::JetAlgorithm> InternalRoiCollection;
  typedef DataVector<LVL1::JEMEtSums>    JemEtSumsCollection;
  typedef DataVector<LVL1::CMMEtSums>    CmmEtSumsCollection;
  typedef DataVector<LVL1::RODHeader>    RodHeaderCollection;
  
  typedef std::vector<int> ErrorVector;

  typedef std::map<int, LVL1::JetElement*>   JetElementMap;
  typedef std::map<int, LVL1::JEMRoI*>       JemRoiMap;
  typedef std::map<int, LVL1::JEMHits*>      JemHitsMap;
  typedef std::map<int, LVL1::CMMJetHits*>   CmmJetHitsMap;
  typedef std::map<int, LVL1::JEMEtSums*>    JemEtSumsMap;
  typedef std::map<int, LVL1::CMMEtSums*>    CmmEtSumsMap;
  
  /// Compare Simulated JetElements with data
  bool  compare(const JetElementMap& jeSimMap, const JetElementMap& jeMap,
                      ErrorVector& errors, bool overlap);
  /// Compare Simulated RoIs with data
  void  compare(const JemRoiMap& roiSimMap, const JemRoiMap& roiMap,
                                                 ErrorVector& errors);
  /// Compare simulated JEM Hits with data
  void  compare(const JemHitsMap& jemSimMap, const JemHitsMap& jemMap,
                                             ErrorVector& errors);
  /// Compare JEM Hits and CMM Hits
  void  compare(const JemHitsMap& jemMap, const CmmJetHitsMap& cmmMap,
                      ErrorVector& errorsJEM, ErrorVector& errorsCMM);
  /// Compare Simulated CMM Hit Sums and Data CMM Hit Sums
  void  compare(const CmmJetHitsMap& cmmSimMap, const CmmJetHitsMap& cmmMap,
                                          ErrorVector& errors, int selection);
  /// Compare JetEt Map with JetEt RoI from data
  void  compare(const CmmJetHitsMap& cmmMap, const LVL1::CMMRoI* cmmRoi,
                                             ErrorVector& errors);
  /// Compare simulated JEM Et Sums with data
  void  compare(const JemEtSumsMap& jemSimMap, const JemEtSumsMap& jemMap,
                                               ErrorVector& errors);
  /// Compare JEM EtSums and CMM EtSums
  void  compare(const JemEtSumsMap& jemMap, const CmmEtSumsMap& cmmMap,
                      ErrorVector& errorsJEM, ErrorVector& errorsCMM);
  /// Compare Simulated CMM EtSums and Data CMM EtSums
  void  compare(const CmmEtSumsMap& cmmSimMap, const CmmEtSumsMap& cmmMap,
                                          ErrorVector& errors, int selection);
  /// Compare Et Maps and Energy Totals with Energy RoIs from data
  void  compare(const CmmEtSumsMap& cmmMap, const LVL1::CMMRoI* cmmRoi,
                                              ErrorVector& errors);
  /// Fill error event number histogram
  void  fillEventSample(int err, int loc, bool isJem);
  /// Set overview and summary histogram labels
  void  setLabels(LWHist* hist, bool xAxis = true);
  /// Set hit sums histogram labels
  void  setLabelsSH(LWHist* hist);
  /// Set hit threshold names histogram labels
  void  setLabelsSHF(LWHist* hist);
  /// Set energy sums histogram labels
  void  setLabelsEnTot(LWHist* hist);
  /// Set energy threshold names histogram labels
  void  setLabelsEnTotThr(LWHist* hist);
  /// Set up JetElement map
  void  setupMap(const JetElementCollection* coll, JetElementMap& map);
  /// Set up JemRoi map
  void  setupMap(const JemRoiCollection* coll, JemRoiMap& map);
  /// Set up JemHits map
  void  setupMap(const JemHitsCollection* coll, JemHitsMap& map);
  /// Set up CmmJetHits map
  void  setupMap(const CmmJetHitsCollection* coll, CmmJetHitsMap& map);
  /// Set up JemEtSums map
  void  setupMap(const JemEtSumsCollection* coll, JemEtSumsMap& map);
  /// Set up CmmEtSums map
  void  setupMap(const CmmEtSumsCollection* coll, CmmEtSumsMap& map);
  /// Simulate Jet Elements from Trigger Towers
  void  simulate(const TriggerTowerCollection* towers,
                       JetElementCollection* elements);
  /// Simulate JEM RoIs from Jet Elements
  void  simulate(const JetElementCollection* elements,
                 const JetElementCollection* elementsOv,
		       JemRoiCollection* rois);
  /// Simulate JEM RoIs from Jet Elements quick version
  void  simulate(const JetElementCollection* elements,
		       JemRoiCollection* rois);
  /// Simulate JEM Hits from JEM RoIs
  void  simulate(const JemRoiCollection* rois, JemHitsCollection* hits);
  /// Simulate CMM-Jet Hit sums from CMM-Jet Hits
  void  simulate(const CmmJetHitsCollection* hitsIn,
                       CmmJetHitsCollection* hitsOut, int selection);
  /// Simulate JEM EtSums from JetElements
  void  simulate(const JetElementCollection* elements,
                       JemEtSumsCollection* sums);
  /// Simulate CMM-Energy Total sums from CMM-Energy Sums
  void  simulate(const CmmEtSumsCollection* sumsIn,
                       CmmEtSumsCollection* sumsOut, int selection);
  /// Check if LimitedRoISet bit set
  bool  limitedRoiSet(int crate);
  /// Return true if version with Missing-Et-Sig
  bool  hasMissingEtSig();
  /// Load ROD Headers
  void  loadRodHeaders();

  /// JEP hits simulation tool
  ToolHandle<LVL1::IL1JEPHitsTools>      m_jepHitsTool;
  /// JEM RoI simulation tool
  ToolHandle<LVL1::IL1JetTools>          m_jetTool;
  /// Jet element simulation tool
  ToolHandle<LVL1::IL1JetElementTools>   m_jetElementTool;
  /// JEP Et sums simulation tool
  ToolHandle<LVL1::IL1JEPEtSumsTools>    m_etSumsTool;
  /// Event veto tool
  ToolHandle<TrigT1CaloMonErrorTool>     m_errorTool;
  /// Histogram helper tool
  ToolHandle<TrigT1CaloLWHistogramTool>  m_histTool;

  /// Debug printout flag
  bool m_debug;
  /// Directory in ROOT
  std::string m_rootDir;

  /// Core Jet Element container StoreGate key
  std::string m_jetElementLocation;
  /// Overlap Jet Element container StoreGate key
  std::string m_jetElementLocationOverlap;
  /// JEM hits container StoreGate key
  std::string m_jemHitsLocation;
  /// CMM-Jet hits container StoreGate key
  std::string m_cmmJetHitsLocation;
  /// JEM RoI container StoreGate key
  std::string m_jemRoiLocation;
  /// CMM RoI container StoreGate key
  std::string m_cmmRoiLocation;
  /// JEM Et sums container StoreGate key
  std::string m_jemEtSumsLocation;
  /// CMM Et sums container StoreGate key
  std::string m_cmmEtSumsLocation;
  /// Trigger Tower container StoreGate key
  std::string m_triggerTowerLocation;
  /// ROD header container StoreGate key
  std::string m_rodHeaderLocation;
  /// Pointer to ROD header container
  const RodHeaderCollection* m_rodTES;
  /// LimitedRoISet flags
  int m_limitedRoi;
  /// Version with Missimg-Et-Sig flag
  bool m_versionSig;
  /// Histograms booked flag
  bool m_histBooked;

  //=======================
  //   Match/Mismatch plots
  //=======================

  // Jet Elements
  TH2F_LW* m_h_jem_em_2d_etaPhi_jetEl_SimEqCore;     ///< Core Jet Elements EM Data/Simulation Non-zero Matches
  TH2F_LW* m_h_jem_em_2d_etaPhi_jetEl_SimNeCore;     ///< Core Jet Elements EM Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_jem_em_2d_etaPhi_jetEl_SimNoCore;     ///< Core Jet Elements EM Simulation but no Data
  TH2F_LW* m_h_jem_em_2d_etaPhi_jetEl_CoreNoSim;     ///< Core Jet Elements EM Data but no Simulation
  TH2F_LW* m_h_jem_had_2d_etaPhi_jetEl_SimEqCore;    ///< Core Jet Elements HAD Data/Simulation Non-zero Matches
  TH2F_LW* m_h_jem_had_2d_etaPhi_jetEl_SimNeCore;    ///< Core Jet Elements HAD Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_jem_had_2d_etaPhi_jetEl_SimNoCore;    ///< Core Jet Elements HAD Simulation but no Data
  TH2F_LW* m_h_jem_had_2d_etaPhi_jetEl_CoreNoSim;    ///< Core Jet Elements HAD Data but no Simulation
  TH2F_LW* m_h_jem_em_2d_etaPhi_jetEl_SimEqOverlap;  ///< Overlap Jet Elements EM Data/Simulation Non-zero Matches
  TH2F_LW* m_h_jem_em_2d_etaPhi_jetEl_SimNeOverlap;  ///< Overlap Jet Elements EM Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_jem_em_2d_etaPhi_jetEl_SimNoOverlap;  ///< Overlap Jet Elements EM Simulation but no Data
  TH2F_LW* m_h_jem_em_2d_etaPhi_jetEl_OverlapNoSim;  ///< Overlap Jet Elements EM Data but no Simulation
  TH2F_LW* m_h_jem_had_2d_etaPhi_jetEl_SimEqOverlap; ///< Overlap Jet Elements HAD Data/Simulation Non-zero Matches
  TH2F_LW* m_h_jem_had_2d_etaPhi_jetEl_SimNeOverlap; ///< Overlap Jet Elements HAD Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_jem_had_2d_etaPhi_jetEl_SimNoOverlap; ///< Overlap Jet Elements HAD Simulation but no Data
  TH2F_LW* m_h_jem_had_2d_etaPhi_jetEl_OverlapNoSim; ///< Overlap Jet Elements HAD Data but no Simulation

  // RoIs
  TH2F_LW* m_h_jem_2d_roi_SimEqData;                 ///< JEM RoI Data/Simulation Non-zero Matches
  TH2F_LW* m_h_jem_2d_roi_SimNeData;                 ///< JEM RoI Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_jem_2d_roi_SimNoData;                 ///< JEM RoI Simulation but no Data
  TH2F_LW* m_h_jem_2d_roi_DataNoSim;                 ///< JEM RoI Data but no Simulation
  TH2F_LW* m_h_jem_2d_roi_ThreshSimEqData;           ///< JEM RoI Data/Simulation Threshold Matches
  TH2F_LW* m_h_jem_2d_roi_ThreshSimNeData;           ///< JEM RoI Data/Simulation Threshold Mismatches
  TH2F_LW* m_h_jem_2d_etaPhi_roi_SimEqData;          ///< JEM RoI Data/Simulation Non-zero Matches
  TH2F_LW* m_h_jem_2d_etaPhi_roi_SimNeData;          ///< JEM RoI Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_jem_2d_etaPhi_roi_SimNoData;          ///< JEM RoI Simulation but no Data
  TH2F_LW* m_h_jem_2d_etaPhi_roi_DataNoSim;          ///< JEM RoI Data but no Simulation

  // JEM Hits
  TH2F_LW* m_h_jem_2d_thresh_SimEqData;              ///< JEM Hits Data/Simulation Non-zero Matches
  TH2F_LW* m_h_jem_2d_thresh_SimNeData;              ///< JEM Hits Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_jem_2d_thresh_SimNoData;              ///< JEM Hits Simulation but no Data
  TH2F_LW* m_h_jem_2d_thresh_DataNoSim;              ///< JEM Hits Data but no Simulation
  TH2F_LW* m_h_jem_2d_thresh_ThreshSimEqData;        ///< JEM Hits Data/Simulation Threshold Matches
  TH2F_LW* m_h_jem_2d_thresh_ThreshSimNeData;        ///< JEM Hits Data/Simulation Threshold Mismatches

  // CMM-Jet Hits
  TH2F_LW* m_h_cmm_2d_thresh_JemEqCmm;               ///< CMM Hits/JEM Hits Non-zero Matches
  TH2F_LW* m_h_cmm_2d_thresh_JemNeCmm;               ///< CMM Hits/JEM Hits Non-zero Mismatches
  TH2F_LW* m_h_cmm_2d_thresh_JemNoCmm;               ///< JEM Hits but no CMM Hits
  TH2F_LW* m_h_cmm_2d_thresh_CmmNoJem;               ///< CMM Hits but no JEM Hits
  TH2F_LW* m_h_cmm_2d_thresh_ThreshJemEqCmm;         ///< CMM Hits/JEM Hits Threshold Matches
  TH2F_LW* m_h_cmm_2d_thresh_ThreshJemNeCmm;         ///< CMM Hits/JEM Hits Threshold Mismatches

  // CMM-Jet Hit Sums
  TH1F_LW* m_h_cmm_1d_thresh_SumsSimEqData;          ///< CMM Hit Sums Data/Simulation Non-zero Matches
  TH1F_LW* m_h_cmm_1d_thresh_SumsSimNeData;          ///< CMM Hit Sums Data/Simulation Non-zero Mismatches
  TH1F_LW* m_h_cmm_1d_thresh_SumsSimNoData;          ///< CMM Hit Sums Simulation but no Data
  TH1F_LW* m_h_cmm_1d_thresh_SumsDataNoSim;          ///< CMM Hit Sums Data but no Simulation
  TH2F_LW* m_h_cmm_2d_thresh_SumsThreshSimEqData;    ///< CMM Hit Sums Data/Simulation Threshold Matches
  TH2F_LW* m_h_cmm_2d_thresh_SumsThreshSimNeData;    ///< CMM Hit Sums Data/Simulation Threshold Mismatches

  // JEMEtSums
  TH2F_LW* m_h_jem_2d_energy_SimEqData;              ///< JEM EtSums Data/Simulation Non-zero Matches
  TH2F_LW* m_h_jem_2d_energy_SimNeData;              ///< JEM EtSums Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_jem_2d_energy_SimNoData;              ///< JEM EtSums Simulation but no Data
  TH2F_LW* m_h_jem_2d_energy_DataNoSim;              ///< JEM EtSums Data but no Simulation

  // CMMEtSums
  TH2F_LW* m_h_cmm_2d_energy_JemEqCmm;               ///< CMM EtSums/JEM EtSums Non-zero Matches
  TH2F_LW* m_h_cmm_2d_energy_JemNeCmm;               ///< CMM EtSums/JEM EtSums Non-zero Mismatches
  TH2F_LW* m_h_cmm_2d_energy_JemNoCmm;               ///< JEM EtSums but no CMM EtSums
  TH2F_LW* m_h_cmm_2d_energy_CmmNoJem;               ///< CMM EtSums but no JEM EtSums

  // Energy Crate/System sums
  TH2F_LW* m_h_cmm_2d_energy_SumsSimEqData;          ///< Energy Totals Data/Simulation Non-zero Matches
  TH2F_LW* m_h_cmm_2d_energy_SumsSimNeData;          ///< Energy Totals Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_cmm_2d_energy_SumsSimNoData;          ///< Energy Totals Simulation but no Data
  TH2F_LW* m_h_cmm_2d_energy_SumsDataNoSim;          ///< Energy Totals Data but no Simulation
  TH2F_LW* m_h_cmm_2d_energy_EtMapsThreshSimEqData;  ///< Et Maps Data/Simulation Threshold Matches
  TH2F_LW* m_h_cmm_2d_energy_EtMapsThreshSimNeData;  ///< Et Maps Data/Simulation Threshold Mismatches

  // Summary
  TH2F_LW* m_h_jem_2d_SimEqDataOverview;             ///< JEP Transmission/Comparison with Simulation Overview - Events with Matches
  TH2F_LW* m_h_jem_2d_SimNeDataOverview;             ///< JEP Transmission/Comparison with Simulation Overview - Events with Mismatches
  TH1F_LW* m_h_jem_1d_SimNeDataSummary;              ///< JEP Transmission/Comparison with Simulation Mismatch Summary

  std::vector<TH2I_LW*> m_v_2d_MismatchEvents;       ///< Mismatch Event Number Samples

};

#endif
