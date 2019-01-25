/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     JEPSimMon.h
// PACKAGE:  TrigT1CaloMonitoring
//
// AUTHOR:   Peter Faulkner
//
//
// ********************************************************************
#ifndef TRIGT1CALOMONITORING_JEPSIMMON_H
#define TRIGT1CALOMONITORING_JEPSIMMON_H

#include <map>
#include <string>
#include <vector>

#include "GaudiKernel/ToolHandle.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "AthContainers/DataVector.h"

// ============================================================================
// xAOD
// ============================================================================
#include "xAODTrigL1Calo/CMXEtSumsContainer.h"
#include "xAODTrigL1Calo/CMXJetTobContainer.h"
#include "xAODTrigL1Calo/CMXJetHitsContainer.h"
#include "xAODTrigL1Calo/CMXRoIContainer.h"
#include "xAODTrigL1Calo/JEMEtSumsContainer.h"
#include "xAODTrigL1Calo/JEMTobRoIContainer.h"
#include "xAODTrigL1Calo/JetElementContainer.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/RODHeaderContainer.h"
// ============================================================================

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
class JEMJetAlgorithm;
class IL1JetCMXTools;
class IL1JetElementTools;
class IL1JEMJetTools;
class IL1EnergyCMXTools;
class ITrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramTool;
// ============================================================================

/** Cross-check of JEM and JEM-CMX data with simulation.
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
 *                 Ditto by crate/module/frame/local coordinate                    </td></tr>
 *  <tr><td> @c L1Calo/JEM/Errors/Transmission_Simulation/Elements2Energy          </td><td>
 *                 JEM Energy sums data/simulation matches and mismatches          </td></tr>
 *  <tr><td> @c L1Calo/JEM/Errors/Transmission_Simulation/MismatchEventNumbers     </td><td>
 *                 JEM mismatch event numbers                                      </td></tr>
 *  <tr><td> @c L1Calo/JEM_CMX/Errors/Transmission_Simulation/RoIs2TOBs            </td><td>
 *                 JEM RoI/CMX TOBs matches and mismatches eta/phi                     <br>
 *                 Ditto by crate/module frame/local coord                         </td></tr>
 *  <tr><td> @c L1Calo/JEM_CMX/Errors/Transmission_Simulation/TOBs2HitSums         </td><td>
 *                 Hit sums data/simulation matches and mismatches                 <br>
 *                 Ditto by threshold                                              <br>
 *                 RoI overflow matches and mismatches                             </td></tr>
 *  <tr><td> @c L1Calo/JEM_CMX/Errors/Transmission_Simulation/TOBs2Topo            </td><td>
 *                 Topo info matches and mismatches                                </td></tr>
 *  <tr><td> @c L1Calo/JEM_CMX/Errors/Transmission_Simulation/JEM2CMXEnergy        </td><td>
 *                 JEM/CMX Energy sums matches and mismatches                      </td></tr>
 *  <tr><td> @c L1Calo/JEM_CMX/Errors/Transmission_Simulation/Energy2Sums          </td><td>
 *                 Energy totals data/simulation matches and mismatches            <br>
 *                 Ditto for Et maps by threshold                                  </td></tr>
 *  <tr><td> @c L1Calo/JEM_CMX/Errors/Transmission_Simulation/MismatchEventNumbers </td><td>
 *                 JEM CMX mismatch event numbers                                  </td></tr>
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
 *           @c <LVL1::JEMTobRoI>         </td><td> Jet RoI data                                </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::JEMEtSums>         </td><td> Energy sums data                            </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CMXJetTob>         </td><td> CMX Jet TOBs data                           </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CMXJetHits>        </td><td> CMX Jet hits data and Topo info             </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CMXEtSums>         </td><td> CMX Energy sums data                        </td></tr>
 *  <tr><td> @c LVL1::CMXRoI              </td><td> CMX RoI data                                </td></tr>
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
 *  <tr><th> Tool                               </th><th> Description               </th></tr>
 *  <tr><td> @c LVL1::IL1JetCMXTools            </td><td> @copydoc m_jetCmxTool     </td></tr>
 *  <tr><td> @c LVL1::IL1JetTools               </td><td> @copydoc m_jetTool        </td></tr>
 *  <tr><td> @c LVL1::IL1JetElementTools        </td><td> @copydoc m_jetElementTool </td></tr>
 *  <tr><td> @c LVL1::IL1EnergyCMXTools         </td><td> @copydoc m_energyCmxTool  </td></tr>
 *  <tr><td> @c LVL1::ITrigT1CaloMonErrorTool   </td><td> @copydoc m_errorTool      </td></tr>
 *  <tr><td> @c LVL1::TrigT1CaloLWHistogramTool </td><td> @copydoc m_histTool       </td></tr>
 *  </table>
 *
 *  <b>JobOption Properties:</b>
 *
 *  <table>
 *  <tr><th> Property                     </th><th> Description                          </th></tr>
 *  <tr><td> @c JetCMXTool                </td><td> @copydoc m_jetCmxTool                </td></tr>
 *  <tr><td> @c JetTool                   </td><td> @copydoc m_jetTool                   </td></tr>
 *  <tr><td> @c JetElementTool            </td><td> @copydoc m_jetElementTool            </td></tr>
 *  <tr><td> @c EnergyCMXTool             </td><td> @copydoc m_energyCmxTool             </td></tr>
 *  <tr><td> @c ErrorTool                 </td><td> @copydoc m_errorTool                 </td></tr>
 *  <tr><td> @c HistogramTool             </td><td> @copydoc m_histTool                  </td></tr>
 *  <tr><td> @c JetElementLocation        </td><td> @copydoc m_jetElementLocation        </td></tr>
 *  <tr><td> @c JetElementLocationOverlap </td><td> @copydoc m_jetElementLocationOverlap </td></tr>
 *  <tr><td> @c CMXJetTobLocation         </td><td> @copydoc m_cmxJetTobLocation         </td></tr>
 *  <tr><td> @c CMXJetHitsLocation        </td><td> @copydoc m_cmxJetHitsLocation        </td></tr>
 *  <tr><td> @c JEMTobRoILocation         </td><td> @copydoc m_jemRoiLocation            </td></tr>
 *  <tr><td> @c CMXRoILocation            </td><td> @copydoc m_cmxRoiLocation            </td></tr>
 *  <tr><td> @c JEMEtSumsLocation         </td><td> @copydoc m_jemEtSumsLocation         </td></tr>
 *  <tr><td> @c CMXEtSumsLocation         </td><td> @copydoc m_cmxEtSumsLocation         </td></tr>
 *  <tr><td> @c TriggerTowerLocation      </td><td> @copydoc m_triggerTowerLocation      </td></tr>
 *  <tr><td> @c RodHeaderLocation         </td><td> @copydoc m_rodHeaderLocation         </td></tr>
 *  <tr><td> @c ErrorLocation             </td><td> @copydoc m_errorLocation             </td></tr>
 *  <tr><td> @c RootDirectory             </td><td> @copydoc m_rootDir                   </td></tr>
 *  </table>
 *
 *  <b>Related Documentation:</b>                        <!-- UPDATE!! -->
 *
 *  <a href="http://hepwww.rl.ac.uk/Atlas-L1/Modules/JEM/JEMspec12d.pdf">
 *  ATLAS Level-1 Calorimeter Trigger Jet / Energy Processor Module</a><br>
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

class JEPSimMon: public ManagedMonitorToolBase
{

public:

  JEPSimMon(const std::string & type, const std::string & name,
            const IInterface* parent);


  static const InterfaceID& interfaceID();

  virtual ~JEPSimMon();

  virtual StatusCode initialize();

  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

private:

  // Error summary bins
  enum SummaryErrors { EMElementMismatch,   HadElementMismatch,
                       RoIMismatch,         CMXJetTobMismatch,
                       LocalJetMismatch,    RemoteJetMismatch,
                       TotalJetMismatch,    CMXJetTopoMismatch,
                       JEMEtSumsMismatch,   CMXEtSumsMismatch,
                       LocalEnergyMismatch, RemoteEnergyMismatch,
                       TotalEnergyMismatch, SumEtMismatch,
                       MissingEtMismatch,   MissingEtSigMismatch,
                       EnergyRoIMismatch,   NumberOfSummaryBins
                     };


  typedef std::vector<int> ErrorVector;
  typedef DataVector<LVL1::JEMJetAlgorithm> InternalRoiCollection;

  typedef xAOD::JetElementMap_t   JetElementMap;
  typedef xAOD::JEMTobRoIMap_t    JEMTobRoIMap;
  typedef xAOD::CMXJetTobMap_t    CmxJetTobMap;
  typedef xAOD::CMXJetHitsMap_t   CmxJetHitsMap;
  typedef xAOD::JEMEtSumsMap_t    JemEtSumsMap;
  typedef xAOD::CMXEtSumsMap_t    CmxEtSumsMap;

  static const int s_crates    = 2;
  static const int s_modules   = 16;
  static const int s_cmxs      = 2;
  static const int s_locCoords = 4;

  /// Compare Simulated JetElements with data
  bool  compare(const JetElementMap& jeSimMap, const JetElementMap& jeMap,
                ErrorVector& errors, bool overlap);
  /// Compare Simulated RoIs with data
  void  compare(const JEMTobRoIMap& roiSimMap, const JEMTobRoIMap& roiMap,
                ErrorVector& errors);
  /// Compare simulated CMX TOBs with data
  void  compare(const CmxJetTobMap& tobSimMap, const CmxJetTobMap& tobMap,
                ErrorVector& errorsJEM, ErrorVector& errorsCMX);
  /// Compare Simulated CMX Hit Sums and Data CMX Hit Sums
  void  compare(const CmxJetHitsMap& cmxSimMap, const CmxJetHitsMap& cmxMap,
                ErrorVector& errors, int selection);
  /// Compare simulated JEM Et Sums with data
  void  compare(const JemEtSumsMap& jemSimMap, const JemEtSumsMap& jemMap,
                ErrorVector& errors);
  /// Compare JEM EtSums and CMX EtSums
  void  compare(const JemEtSumsMap& jemMap, const CmxEtSumsMap& cmxMap,
                ErrorVector& errorsJEM, ErrorVector& errorsCMX);
  /// Compare Simulated CMX EtSums and Data CMX EtSums
  void  compare(const CmxEtSumsMap& cmxSimMap, const CmxEtSumsMap& cmxMap,
                ErrorVector& errors, int selection);
  /// Compare Et Maps and Energy Totals with Energy RoIs from data
  void  compare(const CmxEtSumsMap& cmxMap, const xAOD::CMXRoI* cmxRoi,
                ErrorVector& errors);
  /// Fill error event number histogram
  void  fillEventSample(int err, int loc, bool isJem);
  /// Set overview and summary histogram labels
  void  setLabels(LWHist* hist, bool xAxis = true);
  /// Set hit sums histogram labels
  void  setLabelsSH(LWHist* hist);
  /// Set hit threshold names histogram labels
  void  setLabelsSHF(LWHist* hist);
  /// Set Topo Info histogram labels
  void  setLabelsTopo(TH2F_LW* hist);
  /// Set energy sums histogram labels
  void  setLabelsEnTot(LWHist* hist);
  /// Set energy threshold names histogram labels
  void  setLabelsEnTotThr(LWHist* hist);
  /// Set up JetElement map
  void  setupMap(const xAOD::JetElementContainer* coll, JetElementMap& map);
  /// Set up JemTobRoi map
  void  setupMap(const xAOD::JEMTobRoIContainer* coll, JEMTobRoIMap& map);
  /// Set up CmxJetTob map
  void  setupMap(const xAOD::CMXJetTobContainer* coll, CmxJetTobMap& map);
  /// Set up CmxJetHits map
  void  setupMap(const xAOD::CMXJetHitsContainer* coll, CmxJetHitsMap& map);
  /// Set up JemEtSums map
  void  setupMap(const xAOD::JEMEtSumsContainer* coll, JemEtSumsMap& map);
  /// Set up CmxEtSums map
  void  setupMap(const xAOD::CMXEtSumsContainer* coll, CmxEtSumsMap& map);
  /// Simulate Jet Elements from Trigger Towers
  void  simulate(const xAOD::TriggerTowerContainer* towers,
                 xAOD::JetElementContainer* elements);
  /// Simulate JEM TOB RoIs from Jet Elements
  void  simulate(const xAOD::JetElementContainer* elements,
                 const xAOD::JetElementContainer* elementsOv,
                 xAOD::JEMTobRoIContainer* rois);
  /// Simulate JEM RoIs from Jet Elements quick version
  void  simulate(const xAOD::JetElementContainer* elements,
                 xAOD::JEMTobRoIContainer* rois);
  /// Simulate CMX TOBs from JEM RoIs
  void  simulate(const xAOD::JEMTobRoIContainer* rois, xAOD::CMXJetTobContainer* hits);
  /// Simulate CMX-Jet Hit sums from CMX-Jet TOBs
  void  simulate(const xAOD::CMXJetTobContainer* tobs,
                 xAOD::CMXJetHitsContainer* hits, int selection);
  /// Simulate CMX Total Hit sums from Remote/Local
  void  simulate(const xAOD::CMXJetHitsContainer* hitsIn,
                 xAOD::CMXJetHitsContainer* hitsOut);
  /// Simulate JEM EtSums from JetElements
  void  simulate(const xAOD::JetElementContainer* elements,
                 xAOD::JEMEtSumsContainer* sums);
  /// Simulate CMX-Energy Total sums from CMX-Energy Sums
  void  simulate(const xAOD::CMXEtSumsContainer* sumsIn,
                 xAOD::CMXEtSumsContainer* sumsOut, int selection);
  /// Check if LimitedRoISet bit set
  bool  limitedRoiSet(int crate);
  /// Load ROD Headers
  void  loadRodHeaders();

  template <typename T>
  void dumpDataAndSim(const std::string& msg, const std::map<int, T*>& data,
                         const std::map<int, T*>& sim);

  /// CMX-Jet simulation tool
  ToolHandle<LVL1::IL1JetCMXTools>            m_jetCmxTool;
  /// JEM RoI simulation tool
  ToolHandle<LVL1::IL1JEMJetTools>               m_jetTool;
  /// Jet element simulation tool
  ToolHandle<LVL1::IL1JetElementTools>        m_jetElementTool;
  /// CMX-Energy simulation tool
  ToolHandle<LVL1::IL1EnergyCMXTools>         m_energyCmxTool;
  /// Event veto tool
  ToolHandle<ITrigT1CaloMonErrorTool>   m_errorTool;
  /// Histogram helper tool
  ToolHandle<TrigT1CaloLWHistogramTool> m_histTool;

  /// Debug printout flag
  bool m_debug;
  /// Directory in ROOT
  std::string m_rootDir;

  /// Core Jet Element container StoreGate key
  std::string m_jetElementLocation;
  /// Overlap Jet Element container StoreGate key
  std::string m_jetElementLocationOverlap;
  /// CMX-Jet TOBs container StoreGate key
  std::string m_cmxJetTobLocation;
  /// CMX-Jet hits container StoreGate key
  std::string m_cmxJetHitsLocation;
  /// JEM RoI container StoreGate key
  std::string m_jemRoiLocation;
  /// CMX RoI container StoreGate key
  std::string m_cmxRoiLocation;
  /// JEM Et sums container StoreGate key
  std::string m_jemEtSumsLocation;
  /// CMX Et sums container StoreGate key
  std::string m_cmxEtSumsLocation;
  /// Trigger Tower container StoreGate key
  std::string m_triggerTowerLocation;
  /// ROD header container StoreGate key
  std::string m_rodHeaderLocation;
  /// Error vector StoreGate key
  std::string m_errorLocation;
  /// Pointer to ROD header container
  const xAOD::RODHeaderContainer* m_rodTES;
  /// LimitedRoISet flags
  int m_limitedRoi;
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
  TH2F_LW* m_h_jem_2d_roi_EnergyLgSimEqData;         ///< JEM RoI Energy Large Data/Simulation Non-zero Matches
  TH2F_LW* m_h_jem_2d_roi_EnergyLgSimNeData;         ///< JEM RoI Energy Large Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_jem_2d_roi_EnergyLgSimNoData;         ///< JEM RoI Energy Large Simulation but no Data
  TH2F_LW* m_h_jem_2d_roi_EnergyLgDataNoSim;         ///< JEM RoI Energy Large Data but no Simulation
  TH2F_LW* m_h_jem_2d_roi_EnergySmSimEqData;         ///< JEM RoI Energy Small Data/Simulation Non-zero Matches
  TH2F_LW* m_h_jem_2d_roi_EnergySmSimNeData;         ///< JEM RoI Energy Small Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_jem_2d_roi_EnergySmSimNoData;         ///< JEM RoI Energy Small Simulation but no Data
  TH2F_LW* m_h_jem_2d_roi_EnergySmDataNoSim;         ///< JEM RoI Energy Small Data but no Simulation
  TH2F_LW* m_h_jem_2d_etaPhi_roi_SimEqData;          ///< JEM RoI Data/Simulation Non-zero Matches
  TH2F_LW* m_h_jem_2d_etaPhi_roi_SimNeData;          ///< JEM RoI Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_jem_2d_etaPhi_roi_SimNoData;          ///< JEM RoI Simulation but no Data
  TH2F_LW* m_h_jem_2d_etaPhi_roi_DataNoSim;          ///< JEM RoI Data but no Simulation

  // CMX-Jet TOBs
  TH2F_LW* m_h_cmx_2d_tob_EnergyLgSimEqData;         ///< CMX TOB Energy Large Data/Simulation Non-zero Matches
  TH2F_LW* m_h_cmx_2d_tob_EnergyLgSimNeData;         ///< CMX TOB Energy Large Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_cmx_2d_tob_EnergyLgSimNoData;         ///< CMX TOB Energy Large Simulation but no Data
  TH2F_LW* m_h_cmx_2d_tob_EnergyLgDataNoSim;         ///< CMX TOB Energy Large Data but no Simulation
  TH2F_LW* m_h_cmx_2d_tob_EnergySmSimEqData;         ///< CMX TOB Energy Small Data/Simulation Non-zero Matches
  TH2F_LW* m_h_cmx_2d_tob_EnergySmSimNeData;         ///< CMX TOB Energy Small Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_cmx_2d_tob_EnergySmSimNoData;         ///< CMX TOB Energy Small Simulation but no Data
  TH2F_LW* m_h_cmx_2d_tob_EnergySmDataNoSim;         ///< CMX TOB Energy Small Data but no Simulation
  TH2F_LW* m_h_cmx_2d_etaPhi_tob_SimEqData;          ///< CMX TOB Data/Simulation Non-zero Matches
  TH2F_LW* m_h_cmx_2d_etaPhi_tob_SimNeData;          ///< CMX TOB Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_cmx_2d_etaPhi_tob_SimNoData;          ///< CMX TOB Simulation but no Data
  TH2F_LW* m_h_cmx_2d_etaPhi_tob_DataNoSim;          ///< CMX TOB Data but no Simulation

  // CMX-Jet Hit Sums
  TH1F_LW* m_h_cmx_1d_thresh_SumsSimEqData;          ///< CMX Hit Sums Data/Simulation Non-zero Matches
  TH1F_LW* m_h_cmx_1d_thresh_SumsSimNeData;          ///< CMX Hit Sums Data/Simulation Non-zero Mismatches
  TH1F_LW* m_h_cmx_1d_thresh_SumsSimNoData;          ///< CMX Hit Sums Simulation but no Data
  TH1F_LW* m_h_cmx_1d_thresh_SumsDataNoSim;          ///< CMX Hit Sums Data but no Simulation
  TH1F_LW* m_h_cmx_1d_thresh_SumsOvfSimEqData;       ///< CMX Hit Sums RoI Overflow Bit Data/Simulation Matches
  TH1F_LW* m_h_cmx_1d_thresh_SumsOvfSimNeData;       ///< CMX Hit Sums RoI Overflow Bit Data/Simulation Mismatches
  TH2F_LW* m_h_cmx_2d_thresh_SumsThreshSimEqData;    ///< CMX Hit Sums Data/Simulation Threshold Matches
  TH2F_LW* m_h_cmx_2d_thresh_SumsThreshSimNeData;    ///< CMX Hit Sums Data/Simulation Threshold Mismatches

  // Topo output information
  TH2F_LW* m_h_cmx_2d_topo_SimEqData;                ///< CMX Topo Output Data/Simulation Non-zero Matches
  TH2F_LW* m_h_cmx_2d_topo_SimNeData;                ///< CMX Topo Output Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_cmx_2d_topo_SimNoData;                ///< CMX Topo Output Simulation but no Data
  TH2F_LW* m_h_cmx_2d_topo_DataNoSim;                ///< CMX Topo Output Data but no Simulation

  // JEMEtSums
  TH2F_LW* m_h_jem_2d_energy_SimEqData;              ///< JEM EtSums Data/Simulation Non-zero Matches
  TH2F_LW* m_h_jem_2d_energy_SimNeData;              ///< JEM EtSums Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_jem_2d_energy_SimNoData;              ///< JEM EtSums Simulation but no Data
  TH2F_LW* m_h_jem_2d_energy_DataNoSim;              ///< JEM EtSums Data but no Simulation

  // CMXEtSums
  TH2F_LW* m_h_cmx_2d_energy_JemEqCmx;               ///< CMX EtSums/JEM EtSums Non-zero Matches
  TH2F_LW* m_h_cmx_2d_energy_JemNeCmx;               ///< CMX EtSums/JEM EtSums Non-zero Mismatches
  TH2F_LW* m_h_cmx_2d_energy_JemNoCmx;               ///< JEM EtSums but no CMX EtSums
  TH2F_LW* m_h_cmx_2d_energy_CmxNoJem;               ///< CMX EtSums but no JEM EtSums

  // Energy Crate/System sums
  TH2F_LW* m_h_cmx_2d_energy_SumsSimEqData;          ///< Energy Totals Data/Simulation Non-zero Matches
  TH2F_LW* m_h_cmx_2d_energy_SumsSimNeData;          ///< Energy Totals Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_cmx_2d_energy_SumsSimNoData;          ///< Energy Totals Simulation but no Data
  TH2F_LW* m_h_cmx_2d_energy_SumsDataNoSim;          ///< Energy Totals Data but no Simulation
  TH2F_LW* m_h_cmx_2d_energy_EtMapsThreshSimEqData;  ///< Et Maps Data/Simulation Threshold Matches
  TH2F_LW* m_h_cmx_2d_energy_EtMapsThreshSimNeData;  ///< Et Maps Data/Simulation Threshold Mismatches

  // Summary
  TH2F_LW* m_h_jem_2d_SimEqDataOverview;             ///< JEP Transmission/Comparison with Simulation Overview - Events with Matches
  TH2F_LW* m_h_jem_2d_SimNeDataOverview;             ///< JEP Transmission/Comparison with Simulation Overview - Events with Mismatches
  TH1F_LW* m_h_jem_1d_SimNeDataSummary;              ///< JEP Transmission/Comparison with Simulation Mismatch Summary

  std::vector<TH2I_LW*> m_v_2d_MismatchEvents;       ///< Mismatch Event Number Samples

};

template <typename T> inline
void JEPSimMon::dumpDataAndSim(const std::string& msg, const std::map<int, T*>& data,
                    const std::map<int, T*>& sim) {
  if (!m_debug) return;

  ATH_MSG_DEBUG(msg);
  for (const auto& p : data) {
    ATH_MSG_DEBUG(" DAT " << *p.second);
    auto itSim =  sim.find(p.first);
    if (itSim != sim.end()) {
      ATH_MSG_DEBUG(" SIM " << *itSim->second << std::endl);
    }
  }
  ATH_MSG_DEBUG("End Compare");
  }

} // end namespace

#endif
