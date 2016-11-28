/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     JetEfficienciesMonTool.h
// PACKAGE:  TrigT1CaloMonitoring
//
// AUTHOR:   Hardeep Bansil
//           Adapted for monitoring: Peter Faulkner
//	     
//
// ********************************************************************
#ifndef TRIGT1CALOMONITORING_JETEFFICIENCIESMONTOOL_H
#define TRIGT1CALOMONITORING_JETEFFICIENCIESMONTOOL_H

#include <string>
#include <vector>
#include <map>

#include "AthContainers/DataVector.h"
#include "GaudiKernel/ToolHandle.h"
#include "Identifier/Identifier.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

class TH1F_LW;
class TH2F_LW;
class StatusCode;

class EventInfo;
class CondAttrListCollection;
class LVL1_ROI;
//class JetCollection;
//class Jet;
class IJetSelector;
class VxContainer;
class TileID;
class CaloLVL1_ID;
class TileCablingService;


namespace Trig {
  class TrigDecisionTool;
}
// ============================================================================
namespace LVL1 {
// ============================================================================
// Forward declarations:
// ============================================================================
class IL1TriggerTowerTool;
class IL1CaloLArTowerEnergy;
class TriggerTower;
class ITrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramToolV1;

/** L1 Jet trigger efficiency monitoring
 *
 *  <b>ROOT Histogram Directories:</b>
 *
 *  <table>
 *  <tr><th> Directory
 *    </th><th> Contents                                                          </th></tr>
 *  <tr><td> @c L1Calo/Reco/JetEfficiencies/JetEmScale_Et
 *    </td><td> Efficiencies for each Jet trigger                                 <br>
 *              Overall efficiency                                                <br>
 *              Overall central efficiency                                        <br>
 *              Overall forward efficiency                                        <br>
 *              Overall efficiency eta/phi                                        <br>
 *              Primary vertex multiplicity                                       </td></tr>
 *  <tr><td> @c L1Calo/Reco/JetEfficiencies/JetEmScale_Et/numerator
 *    </td><td> Number of jets passing each trigger                               <br>
 *              Number of jets passing any trigger                                <br>
 *              Number of central jets passing any trigger                        <br>
 *              Number of forward jets passing any trigger                        <br>
 *              Number of jets passing any trigger eta/phi                        </td></tr>
 *  <tr><td> @c L1Calo/Reco/JetEfficiencies/JetEmScale_Et/denominator
 *    </td><td> Total number of jets                                              <br>
 *              Total number of central jets                                      <br>
 *              Total number of forward jets                                      <br>
 *              Total number of jets eta/phi                                      </td></tr>
 *  <tr><td> @c L1Calo/Reco/JetEfficiencies/JetEmScale_50GeV_EtaVsPhi
 *    </td><td> Eta/phi maps of efficiencies for each trigger for Et>50GeV        </td></tr>
 *  <tr><td> @c L1Calo/Reco/JetEfficiencies/JetEmScale_50GeV_EtaVsPhi/numerator
 *    </td><td> Eta/phi maps of number of jets passing each trigger for Et>50GeV  </td></tr>
 *  <tr><td> @c L1Calo/Reco/JetEfficiencies/JetEmScale_50GeV_EtaVsPhi/denominator
 *    </td><td> Eta/phi maps of number of jets for Et>50GeV                       </td></tr>
 *  <tr><td> @c L1Calo/Reco/JetEfficiencies/JetEmScale_100GeV_EtaVsPhi
 *    </td><td> Eta/phi maps of efficiencies for each trigger for Et>100GeV       </td></tr>
 *  <tr><td> @c L1Calo/Reco/JetEfficiencies/JetEmScale_100GeV_EtaVsPhi/numerator
 *    </td><td> Eta/phi maps of number of jets passing each trigger for Et>100GeV </td></tr>
 *  <tr><td> @c L1Calo/Reco/JetEfficiencies/JetEmScale_100GeV_EtaVsPhi/denominator
 *    </td><td> Eta/phi maps of number of jets for Et>100GeV                      </td></tr>
 *  <tr><td> @c L1Calo/Reco/JetEfficiencies/JetEmScale_200GeV_EtaVsPhi
 *    </td><td> Eta/phi maps of efficiencies for each trigger for Et>200GeV       </td></tr>
 *  <tr><td> @c L1Calo/Reco/JetEfficiencies/JetEmScale_200GeV_EtaVsPhi/numerator
 *    </td><td> Eta/phi maps of number of jets passing each trigger for Et>200GeV </td></tr>
 *  <tr><td> @c L1Calo/Reco/JetEfficiencies/JetEmScale_200GeV_EtaVsPhi/denominator
 *    </td><td> Eta/phi maps of number of jets for Et>200GeV                      </td></tr>
 *  <tr><td> @c L1Calo/Reco/JetEfficiencies/DeadOrBadChannels
 *    </td><td> Towers with dead channels                                         <br>
 *              Towers affected by missing FEBs or bad Tile quality               <br>
 *              Error bits for LAr noise burst events                             </td></tr>
 *  </table>
 *
 *  <!--
 *  <b>Notes on Particular Histograms:</b>
 *
 *  <table>
 *  <tr><th> Histogram                        </th><th> Comment                           </th></tr>
 *  <tr><td>                                  </td><td>                                   </td></tr>
 *  </table>
 *  -->
 *
 *  <b>Custom Merges Used (Tier0):</b>
 *
 *  <table>
 *  <tr><th> Merge                                             </th><th> Used For                   </th></tr>
 *  <tr><td> @ref MergesUsedsection     "@c lowerLB "          </td><td> Dead channels and bad calo </td></tr>
 *  <tr><td> @ref MergesUsedsection     "@c perBinEffPerCent " </td><td> Efficiency plots           </td></tr>
 *  <tr><td> @ref PostProcessingsection "@c PostProcessing "   </td><td> Eta/phi efficiency plots   <br>
 *                                                                Set bins with few entries to 100% </td></tr>
 *  </table>
 *
 *  <b>StoreGate Containers Used:</b>
 *
 *  <table>
 *  <tr><th> Container                 </th><th> Comment                          </th></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::TriggerTower>   </td><td> @copydoc m_triggerTowers         </td></tr>
 *  <tr><td> @c CondAttrListCollection </td><td> @copydoc m_dbPpmDeadChannels     </td></tr>
 *  <tr><td> @c CaloCellContainer      </td><td> For tile cells, first event only </td></tr>
 *  <tr><td> @c EventInfo              </td><td> @copydoc m_eventInfo             </td></tr>
 *  <tr><td> @c LVL1_ROI               </td><td> @copydoc m_lvl1RoIs              </td></tr>
 *  <tr><td> @c JetCollection          </td><td> @copydoc m_offlineJets           </td></tr>
 *  <tr><td> @c VxContainer            </td><td> @copydoc m_primaryVertex         </td></tr>
 *  </table>
 *
 *  <b>Tools Used:</b>
 *
 *  <table>
 *  <tr><th> Tool                           </th><th> Description          </th></tr>
 *  <tr><td> @c TrigT1CaloMonErrorTool      </td><td> @copydoc m_errorTool </td></tr>
 *  <tr><td> @c TrigT1CaloLWHistogramToolV1   </td><td> @copydoc m_histTool  </td></tr>
 *  <tr><td> @c LVL1::IL1TriggerTowerTool   </td><td> @copydoc m_ttTool    </td></tr>
 *  <tr><td> @c LVL1::IL1CaloLArTowerEnergy </td><td> @copydoc m_larEnergy </td></tr>
 *  </table>
 *
 *  <b>JobOption Properties:</b>
 *
 *  <table>
 *  <tr><th> Property                     </th><th> Description                          </th></tr>
 *  <tr><td> @c HistogramTool             </td><td> @copydoc m_histTool                  </td></tr>
 *  <tr><td> @c TriggerTowerTool          </td><td> @copydoc m_ttTool                    </td></tr>
 *  <tr><td> @c LArTowerEnergyTool        </td><td> @copydoc m_larEnergy                 </td></tr>
 *  <tr><td> @c DeadChannelsFolder        </td><td> @copydoc m_dbPpmDeadChannelsFolder   </td></tr>
 *  <tr><td> @c TriggerTowersLocation     </td><td> @copydoc m_triggerTowersLocation     </td></tr>
 *  <tr><td> @c RoIsLocation              </td><td> @copydoc m_lvl1RoIsLocation          </td></tr>
 *  <tr><td> @c OfflineJetsLocation       </td><td> @copydoc m_offlineJetsLocation       </td></tr>
 *  <tr><td> @c PrimaryVertexLocation     </td><td> @copydoc m_primaryVertexLocation     </td></tr>
 *  <tr><td> @c CaloCellContainerLocation </td><td> @copydoc m_caloCellContainerLocation </td></tr>
 *  <tr><td> @c RootDirectory             </td><td> @copydoc m_rootDir                   </td></tr>
 *  <tr><td> @c UseTrigger                </td><td> @copydoc m_useTrigger                </td></tr>
 *  <tr><td> @c TriggerStrings            </td><td> @copydoc m_triggerStrings            </td></tr>
 *  <tr><td> @c goodEMDeltaRMatch_Cut     </td><td> @copydoc m_goodEMDeltaRMatch_Cut     </td></tr>
 *  <tr><td> @c goodHadDeltaRMatch_Cut    </td><td> @copydoc m_goodHadDeltaRMatch_Cut    </td></tr>
 *  <tr><td> @c goodHadDeltaPhiMatch_Cut  </td><td> @copydoc m_goodHadDeltaPhiMatch_Cut  </td></tr>
 *  <tr><td> @c UseEmThresholdsOnly       </td><td> @copydoc m_useEmThresholdsOnly       </td></tr>
 *  <tr><td> @c JetQualityLevel           </td><td> @copydoc m_jetQualityLevel           </td></tr>
 *  <tr><td> @c NtracksAtPrimaryVertex    </td><td> @copydoc m_nTracksAtPrimaryVertex    </td></tr>
 *  <tr><td> @c HadCoreVHCut              </td><td> @copydoc m_hadCoreVHCut              </td></tr>
 *  <tr><td> @c RemoveNoiseBursts         </td><td> @copydoc m_removeNoiseBursts         </td></tr>
 *  </table>
 *
 *  <b>Related Documentation:</b>
 *
 *  <a href="https://cdsweb.cern.ch/record/1444500/files/ATL-COM-DAQ-2012-030.pdf">
 *  Tier-0 Monitoring of the ATLAS Level-1 1 Calorimeter Trigger Jet and
 *  Electromagnetic Trigger Item Efficiencies</a>
 *
 *  @author Hardeep Bansil, adapted for monitoring by Peter Faulkner
 *
 */

class JetEfficienciesMonTool: public ManagedMonitorToolBase
{

public:
  
  JetEfficienciesMonTool(const std::string & type, const std::string & name,
   		         const IInterface* parent);
    

  virtual ~JetEfficienciesMonTool();

  virtual StatusCode initialize();
    
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

private:

  //HSB - functions
  //----------------------------------
  /// Convert offline analysis phi into value more suitable for L1Calo
  double l1caloPhi(const double atlasPhi) const;

  /// Check to see if an object is triggered w.r.t. a RoI
  bool deltaMatch(double etaJet, double etaRoi, double dR, double dPhi, bool fRoi);
  /// Correct the value of deltaPhi so that it falls in +-PI range
  double correctDeltaPhi(double dPhi);
  /// Calculate delta R quickly between two objects
  double calcDeltaR(double eta1, double phi1, double eta2, double phi2);
	        
  /// Check for electron that it is of the right jet quality type as required from jobOptions
  bool correctJetQuality(const xAOD::Jet* jet);
  
  /// Ask if object has no EmTau RoIs nearby at L1
  bool isolatedJetObjectL1(double phi, double eta);
  /// Ask if object has no jets or jet RoIs nearby at EF
  bool isolatedJetObjectEF(double phi, double eta);    
								        
  /// Analysis code for offline reconstructed jets
  StatusCode analyseOfflineJets();
									    
  /// Trigger Tower Analysis
  StatusCode triggerTowerAnalysis();
  /// Trigger Chain Analysis
  StatusCode triggerChainAnalysis();  
  /// Load important containers
  StatusCode loadContainers();
  /// Return number of primary vertices that have at least a number of tracks
  unsigned int nPrimaryVertex();
  /// Map Tile quality
  StatusCode mapTileQuality();

  //----------------------------------    

  /// Number of Main Jet RoI bits
  static const int JET_ROI_BITS = 8;
  /// Number of Forward Jet RoI bits
  static const int FJET_ROI_BITS = 4;

  /// Corrupt event veto tool
  ToolHandle<ITrigT1CaloMonErrorTool>    m_errorTool;
  /// Histogram helper tool
  ToolHandle<TrigT1CaloLWHistogramToolV1> m_histTool;
  /// TT simulation tool for Identifiers. Used first event only
  ToolHandle<LVL1::IL1TriggerTowerTool> m_ttTool;
  /// Tool for Missing FEB. Used first event only
  ToolHandle<LVL1::IL1CaloLArTowerEnergy> m_larEnergy;
  /// JetSelect
  ToolHandle<IJetSelector> m_looseJetSelector;
  ToolHandle<IJetSelector> m_mediumJetSelector;
  ToolHandle<IJetSelector> m_tightJetSelector;
  
  /// Tile ID helper
  const TileID*             m_tileID;
  /// Calo ID helper
  const CaloLVL1_ID*        m_TT_ID;
  /// Tile cabling service
  const TileCablingService* m_tileCablingService;

  /// Configured chains
  std::vector<std::string> m_configuredChains; 
  /// Trigger strings
  std::vector<std::string> m_triggerStrings;

  // Container StoreGate keys
  /// Dead channels folder StoreGate key
  std::string m_dbPpmDeadChannelsFolder;
  /// TriggerTower collection StoreGate key
  std::string m_triggerTowersLocation;
  /// CaloCell container StoreGate key
  std::string m_caloCellContainerLocation;
  /// LVL1_ROIs StoreGate key
  std::string m_lvl1RoIsLocation;
  /// Offline jets container StoreGate key
  std::string m_offlineJetsLocation;
  /// Vertex container StoreGate key
  std::string m_primaryVertexLocation;

  // Container pointers
  /// For noise burst error bits
  const EventInfo* m_eventInfo;
  /// Dead channels info from DB, first event only
  const CondAttrListCollection* m_dbPpmDeadChannels;
  /// PPM data, first event only, for eta/phi
  const DataVector<LVL1::TriggerTower>* m_triggerTowers;
  /// For EmTau_ROIs and Jet_ROIs
  const LVL1_ROI* m_lvl1RoIs;
  /// For offline jets
  //const JetCollection* m_offlineJets;
  const xAOD::JetContainer* m_offlineJets;
  /// For primary vertices
  const VxContainer* m_primaryVertex;

  // Tile Calorimeter quality map
  typedef std::map<Identifier, uint16_t> IdTileQualityMapType;
  IdTileQualityMapType m_idTileQualityMap;

  /// Root directory
  std::string m_rootDir;

  // Counters for number of events
  unsigned int m_numEvents;
  unsigned int m_numOffJets;
  unsigned int m_numOffJetsInContainer;
  unsigned int m_numOffJetsPassCuts;
  unsigned int m_numOffJetsTriggered;
  unsigned int m_numJetObjPassTrigger;
  unsigned int m_numJetObjTotal;
  
  // Variables for L1 & EF trigger chains 
  bool m_passed_L1_EM_Trigger;
  bool m_passed_EF_Trigger;
  bool m_passed_EF_SingleJet_Trigger;
  bool m_passed_EF_SingleEgamma_Trigger;
  bool m_passed_EF_SingleEgamma_Trigger_HighestVH;
  bool m_passed_EF_MultiJet_Trigger;
  bool m_passed_EF_Tau_Trigger;
  bool m_passed_EF_MissingEnergy_Trigger;
  std::vector<int> m_wantedTriggers;
  static const int s_L1_EM_Trigger_mask                     = 0x1;
  static const int s_EF_Trigger_mask                        = 0x2;
  static const int s_EF_SingleJet_Trigger_mask              = 0x4;
  static const int s_EF_MultiJet_Trigger_mask               = 0x8;
  static const int s_EF_SingleEgamma_Trigger_mask           = 0x10;
  static const int s_EF_SingleEgamma_Trigger_HighestVH_mask = 0x20;
  static const int s_EF_Tau_Trigger_mask                    = 0x40;
  static const int s_EF_MissingEnergy_Trigger_mask          = 0x80;

  // Python settable cuts  
  /// Only use EM thresholds for isolation test
  bool m_useEmThresholdsOnly;
  /// Isolation cut
  double m_goodEMDeltaRMatch_Cut;
  /// Cut on jet/RoI delta R
  double m_goodHadDeltaRMatch_Cut;
  /// Cut on jet/RoI phi difference
  double m_goodHadDeltaPhiMatch_Cut;
  /// Had core maximum
  double m_hadCoreVHCut;
  /// Jet quality type
  int m_jetQualityLevel;
  /// Flag to ignore LAr noise burst events
  bool m_removeNoiseBursts;
  /// Flag to check trigger menu
  bool m_useTrigger;
  /// Minimum number of primary tracks
  unsigned int m_nTracksAtPrimaryVertex;

  /// Flag for disabled channel/bad calo analysis
  bool m_firstEvent;

  //=======================
  //   Histograms
  //=======================

  // Em Scale Jet plots
  TH1F_LW* m_h_JetEmScale_Et;                              ///< Raw Jet Et
  TH1F_LW* m_h_JetEmScale_Et_central;                      ///< Raw Jet Et Central
  TH1F_LW* m_h_JetEmScale_Et_forward;                      ///< Raw Jet Et Forward
  TH1F_LW* m_h_JetEmScale_Et_triggered;                    ///< Raw Jet Et (Triggered)
  TH1F_LW* m_h_JetEmScale_Et_triggered_central;            ///< Raw Jet Et Central (Triggered)
  TH1F_LW* m_h_JetEmScale_Et_triggered_forward;            ///< Raw Jet Et Forward (Triggered)
  TH1F_LW* m_h_JetEmScale_Et_triggered_Eff;                ///< Raw Jet Et (Triggered) Efficiency
  TH1F_LW* m_h_JetEmScale_Et_triggered_central_Eff;        ///< Raw Jet Et Central (Triggered) Efficiency
  TH1F_LW* m_h_JetEmScale_Et_triggered_forward_Eff;        ///< Raw Jet Et Forward (Triggered) Efficiency
  TH1F_LW* m_h_JetEmScale_Et_J_item[JET_ROI_BITS];         ///< Raw Et for Triggered Jets passing each Main threshold
  TH1F_LW* m_h_JetEmScale_Et_FJ_J_item[FJET_ROI_BITS];     ///< Raw Et for Triggered Jets passing each Forward or Main threshold
  TH1F_LW* m_h_JetEmScale_Et_J_Eff_item[JET_ROI_BITS];     ///< Raw Et for Triggered Jets passing each Main threshold Efficiency
  TH1F_LW* m_h_JetEmScale_Et_FJ_J_Eff_item[FJET_ROI_BITS]; ///< Raw Et for Triggered Jets passing each Forward or Main threshold Efficiency
  TH2F_LW* m_h_JetEmScale_Eta_vs_Phi;                      ///< Raw Jet eta v phi
  TH2F_LW* m_h_JetEmScale_Eta_vs_Phi_triggered;            ///< Raw Jet eta v phi (Triggered)
  TH2F_LW* m_h_JetEmScale_Eta_vs_Phi_triggered_Eff;        ///< Raw Jet eta v phi (Triggered) Efficiency
  TH2F_LW* m_h_JetEmScale_50GeV_Eta_vs_Phi;                               ///< Raw Jet eta v phi (Et > 50 GeV)
  TH2F_LW* m_h_JetEmScale_50GeV_Eta_vs_Phi_J_item[JET_ROI_BITS];          ///< Raw Jet eta v phi (triggered on each Main threshold with Et > 50 GeV)
  TH2F_LW* m_h_JetEmScale_50GeV_Eta_vs_Phi_FJ_J_item[FJET_ROI_BITS];      ///< Raw Jet eta v phi (triggered on each Forward or Main threshold with Et > 50 GeV)
  TH2F_LW* m_h_JetEmScale_50GeV_Eta_vs_Phi_J_Eff_item[JET_ROI_BITS];      ///< Raw Jet eta v phi (triggered on each Main threshold with Et > 50 GeV) Efficiency
  TH2F_LW* m_h_JetEmScale_50GeV_Eta_vs_Phi_FJ_J_Eff_item[FJET_ROI_BITS];  ///< Raw Jet eta v phi (triggered on each Forward or Main threshold with Et > 50 GeV) Efficiency
  TH2F_LW* m_h_JetEmScale_100GeV_Eta_vs_Phi;                              ///< Raw Jet eta v phi (Et > 100 GeV)
  TH2F_LW* m_h_JetEmScale_100GeV_Eta_vs_Phi_J_item[JET_ROI_BITS];         ///< Raw Jet eta v phi (triggered on each Main threshold with Et > 100 GeV)
  TH2F_LW* m_h_JetEmScale_100GeV_Eta_vs_Phi_FJ_J_item[FJET_ROI_BITS];     ///< Raw Jet eta v phi (triggered on each Forward or Main threshold with Et > 100 GeV)
  TH2F_LW* m_h_JetEmScale_100GeV_Eta_vs_Phi_J_Eff_item[JET_ROI_BITS];     ///< Raw Jet eta v phi (triggered on each Main threshold with Et > 100 GeV) Efficiency
  TH2F_LW* m_h_JetEmScale_100GeV_Eta_vs_Phi_FJ_J_Eff_item[FJET_ROI_BITS]; ///< Raw Jet eta v phi (triggered on each Forward or Main threshold with Et > 100 GeV) Efficiency
  TH2F_LW* m_h_JetEmScale_200GeV_Eta_vs_Phi;                              ///< Raw Jet eta v phi (Et > 200 GeV)
  TH2F_LW* m_h_JetEmScale_200GeV_Eta_vs_Phi_J_item[JET_ROI_BITS];         ///< Raw Jet eta v phi (triggered on each Main threshold with Et > 200 GeV)
  TH2F_LW* m_h_JetEmScale_200GeV_Eta_vs_Phi_FJ_J_item[FJET_ROI_BITS];     ///< Raw Jet eta v phi (triggered on each Forward or Main threshold with Et > 200 GeV)
  TH2F_LW* m_h_JetEmScale_200GeV_Eta_vs_Phi_J_Eff_item[JET_ROI_BITS];     ///< Raw Jet eta v phi (triggered on each Main threshold with Et > 200 GeV) Efficiency
  TH2F_LW* m_h_JetEmScale_200GeV_Eta_vs_Phi_FJ_J_Eff_item[FJET_ROI_BITS]; ///< Raw Jet eta v phi (triggered on each Forward or Main threshold with Et > 200 GeV) Efficiency

  //Trigger tower bad/dead
  TH2F_LW* m_h_TrigTower_jetBadCalo;     ///< Jet Trigger Towers - Missing FEBs/Tile Quality - eta against phi (Et > 5 GeV)
  TH2F_LW* m_h_TrigTower_jetDeadChannel; ///< Jet Trigger Towers with dead channels - eta against phi (Et > 5 GeV)
  TH1F_LW* m_h_LAr_jetNoisy;             ///< LAr Error Bits in Rejected Events

  TH1F_LW* m_h_nPriVtx;                  ///< Primary Vertex Multiplicity

  /// Link to L1_Jet histogram corresponding to L1_ForwardJet histogram
  int m_linkedHistos[FJET_ROI_BITS];

};
// ============================================================================
}  // end namespace
// ============================================================================

#endif
