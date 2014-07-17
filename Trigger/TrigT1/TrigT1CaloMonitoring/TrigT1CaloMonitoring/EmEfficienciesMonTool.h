/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     EmEfficienciesMonTool.h
// PACKAGE:  TrigT1CaloMonitoring
//
// AUTHOR:   Hardeep Bansil
//           Adapted for monitoring: Peter Faulkner
//	     
//
// ********************************************************************
#ifndef TRIGT1CALOMONITORING_EMEFFICIENCIESMONTOOL_H
#define TRIGT1CALOMONITORING_EMEFFICIENCIESMONTOOL_H

#include <string>
#include <vector>

#include "DataModel/DataVector.h"
#include "GaudiKernel/ToolHandle.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

class LWHist;
class TH1F_LW;
class TH2F_LW;
class StatusCode;
class CaloCluster;
class TrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramTool;
class EventInfo;
class CondAttrListCollection;
class LVL1_ROI;
class ElectronContainer;
class PhotonContainer;
class VxContainer;

namespace Analysis {
  class Electron;
  class Photon;
}
namespace LVL1 {
  class IL1TriggerTowerTool;
  class IL1CaloLArTowerEnergy;
  class TriggerTower;
}
namespace Trig {
  class TrigDecisionTool;
}

/** L1 EM trigger efficiency monitoring
 *
 *  <b>ROOT Histogram Directories:</b>
 *
 *  <table>
 *  <tr><th> Directory
 *    </th><th> Contents                                                             </th></tr>
 *  <tr><td> @c L1Calo/Reco/EmEfficiencies/ClusterRaw_Et
 *    </td><td> Efficiencies for each EM trigger                                     <br>
 *              Overall efficiency                                                   <br>
 *              Efficiency in transition region                                      </td></tr>
 *  <tr><td> @c L1Calo/Reco/EmEfficiencies/ClusterRaw_Et/numerator
 *    </td><td> Number of clusters passing each trigger                              <br>
 *              Number of clusters passing any trigger                               <br>
 *              Number of clusters passing any trigger in transition region          </td></tr>
 *  <tr><td> @c L1Calo/Reco/EmEfficiencies/ClusterRaw_Et/denominator
 *    </td><td> Total number of clusters                                             <br>
 *              Total number of clusters in transition region                        </td></tr>
 *  <tr><td> @c L1Calo/Reco/EmEfficiencies/ClusterRaw_10GeV_EtaVsPhi
 *    </td><td> Eta/phi maps of efficiencies for each trigger for Et>10GeV           </td></tr>
 *  <tr><td> @c L1Calo/Reco/EmEfficiencies/ClusterRaw_10GeV_EtaVsPhi/numerator
 *    </td><td> Eta/phi maps of number of clusters passing each trigger for Et>10GeV </td></tr>
 *  <tr><td> @c L1Calo/Reco/EmEfficiencies/ClusterRaw_10GeV_EtaVsPhi/denominator
 *    </td><td> Eta/phi maps of number of clusters for Et>10GeV                      </td></tr>
 *  <tr><td> @c L1Calo/Reco/EmEfficiencies/ClusterRaw_20GeV_EtaVsPhi
 *    </td><td> Eta/phi maps of efficiencies for each trigger for Et>20GeV           </td></tr>
 *  <tr><td> @c L1Calo/Reco/EmEfficiencies/ClusterRaw_20GeV_EtaVsPhi/numerator
 *    </td><td> Eta/phi maps of number of clusters passing each trigger for Et>20GeV </td></tr>
 *  <tr><td> @c L1Calo/Reco/EmEfficiencies/ClusterRaw_20GeV_EtaVsPhi/denominator
 *    </td><td> Eta/phi maps of number of clusters for Et>20GeV                      </td></tr>
 *  <tr><td> @c L1Calo/Reco/EmEfficiencies/ClusterRaw_30GeV_EtaVsPhi
 *    </td><td> Eta/phi maps of efficiencies for each trigger for Et>30GeV           </td></tr>
 *  <tr><td> @c L1Calo/Reco/EmEfficiencies/ClusterRaw_30GeV_EtaVsPhi/numerator
 *    </td><td> Eta/phi maps of number of clusters passing each trigger for Et>30GeV </td></tr>
 *  <tr><td> @c L1Calo/Reco/EmEfficiencies/ClusterRaw_30GeV_EtaVsPhi/denominator
 *    </td><td> Eta/phi maps of number of clusters for Et>30GeV                      </td></tr>
 *  <tr><td> @c L1Calo/Reco/EmEfficiencies/DeadOrBadChannels
 *    </td><td> Em towers with dead channels                                         <br>
 *              Em towers affected by missing FEBs                                   <br>
 *              Error bits for LAr noise burst events                                </td></tr>
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
 *  <tr><th> Container                 </th><th> Comment                      </th></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::TriggerTower>   </td><td> @copydoc m_triggerTowers     </td></tr>
 *  <tr><td> @c CondAttrListCollection </td><td> @copydoc m_dbPpmDeadChannels </td></tr>
 *  <tr><td> @c EventInfo              </td><td> @copydoc m_eventInfo         </td></tr>
 *  <tr><td> @c LVL1_ROI               </td><td> @copydoc m_lvl1RoIs          </td></tr>
 *  <tr><td> @c ElectronContainer      </td><td> @copydoc m_offlineElectrons  </td></tr>
 *  <tr><td> @c PhotonContainer        </td><td> @copydoc m_offlinePhotons    </td></tr>
 *  <tr><td> @c VxContainer            </td><td> @copydoc m_primaryVtx        </td></tr>
 *  </table>
 *
 *  <b>Tools Used:</b>
 *
 *  <table>
 *  <tr><th> Tool                           </th><th> Description          </th></tr>
 *  <tr><td> @c TrigT1CaloMonErrorTool      </td><td> @copydoc m_errorTool </td></tr>
 *  <tr><td> @c TrigT1CaloLWHistogramTool   </td><td> @copydoc m_histTool  </td></tr>
 *  <tr><td> @c LVL1::IL1TriggerTowerTool   </td><td> @copydoc m_ttTool    </td></tr>
 *  <tr><td> @c LVL1::IL1CaloLArTowerEnergy </td><td> @copydoc m_larEnergy </td></tr>
 *  <tr><td> @c Trig::TrigDecisionTool      </td><td> @copydoc m_trigger   </td></tr>
 *  </table>
 *
 *  <b>JobOption Properties:</b>
 *
 *  <table>
 *  <tr><th> Property                    </th><th> Description                         </th></tr>
 *  <tr><td> @c HistogramTool            </td><td> @copydoc m_histTool                 </td></tr>
 *  <tr><td> @c TriggerTowerTool         </td><td> @copydoc m_ttTool                   </td></tr>
 *  <tr><td> @c LArTowerEnergyTool       </td><td> @copydoc m_larEnergy                </td></tr>
 *  <tr><td> @c TrigDecisionTool         </td><td> @copydoc m_trigger                  </td></tr>
 *  <tr><td> @c DeadChannelsFolder       </td><td> @copydoc m_dbPpmDeadChannelsFolder  </td></tr>
 *  <tr><td> @c TriggerTowersLocation    </td><td> @copydoc m_triggerTowersLocation    </td></tr>
 *  <tr><td> @c RoIsLocation             </td><td> @copydoc m_lvl1RoIsLocation         </td></tr>
 *  <tr><td> @c OfflineElectronsLocation </td><td> @copydoc m_offlineElectronsLocation </td></tr>
 *  <tr><td> @c OfflinePhotonsLocation   </td><td> @copydoc m_offlinePhotonsLocation   </td></tr>
 *  <tr><td> @c PrimaryVertexLocation    </td><td> @copydoc m_primaryVertexLocation    </td></tr>
 *  <tr><td> @c RootDirectory            </td><td> @copydoc m_rootDir                  </td></tr>
 *  <tr><td> @c UseTrigger               </td><td> @copydoc m_useTrigger               </td></tr>
 *  <tr><td> @c TriggerStrings           </td><td> @copydoc m_triggerStrings           </td></tr>
 *  <tr><td> @c useDeltaRMatch           </td><td> @copydoc m_useDeltaRMatch           </td></tr>
 *  <tr><td> @c goodEMDeltaRMatch_Cut    </td><td> @copydoc m_goodEMDeltaRMatch_Cut    </td></tr>
 *  <tr><td> @c goodHadDeltaRMatch_Cut   </td><td> @copydoc m_goodHadDeltaRMatch_Cut   </td></tr>
 *  <tr><td> @c useDeltaEtaPhiMatch      </td><td> @copydoc m_useDeltaEtaPhiMatch      </td></tr>
 *  <tr><td> @c goodEMDeltaPhiMatch_Cut  </td><td> @copydoc m_goodEMDeltaPhiMatch_Cut  </td></tr>
 *  <tr><td> @c goodEMDeltaEtaMatch_Cut  </td><td> @copydoc m_goodEMDeltaEtaMatch_Cut  </td></tr>
 *  <tr><td> @c UseEmThresholdsOnly      </td><td> @copydoc m_useEmThresholdsOnly      </td></tr>
 *  <tr><td> @c RemoveNoiseBursts        </td><td> @copydoc m_removeNoiseBursts        </td></tr>
 *  <tr><td> @c IsEmType                 </td><td> @copydoc m_isEmType                 </td></tr>
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

class EmEfficienciesMonTool: public ManagedMonitorToolBase
{

public:
  
  EmEfficienciesMonTool(const std::string & type, const std::string & name,
  		        const IInterface* parent);
    

  virtual ~EmEfficienciesMonTool();

  virtual StatusCode initialize();
    
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

private:

  //HSB - functions
  //----------------------------------
  /// Asks if there is at least one primary vertex with 3 tracks coming off it
  bool vertexRequirementsPassed(int &numVtx, int &bestNumTracks);  

  /// Check to see if an object is triggered w.r.t. an RoI
  bool deltaMatch(double dR, double dEta, double dPhi, double gdR, double gdEta, double gdPhi); 
  /// Correct the value of deltaPhi so that it falls in +-M_PI range
  double correctDeltaPhi(double dPhi);
  /// Calculate delta R quickly between two objects
  double calcDeltaR(double eta1, double phi1, double eta2, double phi2);
	        
  /// Determine if corresponding tower is dead or bad (not currently used)
  bool emObjInDeadBadTower(double eta, double phi);
			        
  /// Ask if object is within well defined eta range for EM performance
  bool inEgammaGoodEtaRange(double eta);    
  /// Ask if object is within EM barrel
  bool inEMBarrel(double eta, int sign);
  /// Ask if object is within EM Transition region
  bool inEMTransR(double eta, int sign);
  /// Ask if object is within EM endcap
  bool inEMEndcap(double eta, int sign);
				        
  /// Check for electron that it is of the right isEm type as required from jobOptions
  bool correctIsEmElectron(const Analysis::Electron* el);
  /// Check for photon that it is of the right isEm type as required from jobOptions
  bool correctIsEmPhoton(const Analysis::Photon* ph);
  /// Check for electron that it is of the right isEm type as required from jobOptions
  std::string isEmLevelElectron(const Analysis::Electron* el, int &code);
  /// Check for photon that it is of the right isEm type as required from jobOptions
  std::string isEmLevelPhoton(const Analysis::Photon* ph, int &code);    
				    
  /// Ask if object is has no jets or jet RoIs nearby at L1
  bool isolatedEmObjectL1(double phi, double eta);
  /// Ask if object is has no jets or jet RoIs nearby at EF
  bool isolatedEmObjectEF(double phi, double eta);    
								        
  /// Get the raw values of the CaloCluster
  void getRawClusterValuesFromCells(CaloCluster* cc, double& et, double& eta, double& phi);
								                
  /// Analysis code for offline reconstructed electrons
  StatusCode analyseOfflineElectrons();
  /// Analysis code for offline reconstructed photons
  StatusCode analyseOfflinePhotons();  
									    
  /// Trigger Tower Analysis
  StatusCode triggerTowerAnalysis();
  /// Trigger Chain Analysis
  StatusCode triggerChainAnalysis();  
  /// Load important containers
  StatusCode loadContainers();

  /// Return true if threshold number is an EM threshold
  bool emType(int bitNumber);
  //----------------------------------    

  /// Number of EM/Tau RoI bits (any can be EM)
  static const int ROI_BITS = 16;

  /// Corrupt event veto tool
  ToolHandle<TrigT1CaloMonErrorTool>    m_errorTool;
  /// Histogram helper tool
  ToolHandle<TrigT1CaloLWHistogramTool> m_histTool;
  /// TT simulation tool for Identifiers. Used first event only
  ToolHandle<LVL1::IL1TriggerTowerTool> m_ttTool;
  /// Tool for Missing FEB. Used first event only
  ToolHandle<LVL1::IL1CaloLArTowerEnergy> m_larEnergy;
  /// Trigger Decision tool
  ToolHandle<Trig::TrigDecisionTool> m_trigger;

  /// Configured chains
  std::vector<std::string> m_configuredChains; 
  /// Trigger strings
  std::vector<std::string> m_triggerStrings;

  // Container StoreGate keys
  /// Dead channels folder StoreGate key
  std::string m_dbPpmDeadChannelsFolder;
  /// TriggerTower collection StoreGate key
  std::string m_triggerTowersLocation;
  /// LVL1_ROIs StoreGate key
  std::string m_lvl1RoIsLocation;
  /// Offline electrons container StoreGate key
  std::string m_offlineElectronsLocation;
  /// Offline photons container StoreGate key
  std::string m_offlinePhotonsLocation;
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
  /// For offline electrons
  const ElectronContainer* m_offlineElectrons;
  /// For offline photons
  const PhotonContainer* m_offlinePhotons;
  /// For primary vertices
  const VxContainer* m_primaryVtx;

  /// Root directory
  std::string m_rootDir;

  // Counters for number of events
  unsigned int m_numEvents;
  unsigned int m_numOffElec;
  unsigned int m_numOffPhot;    
  unsigned int m_numOffElecInContainer;
  unsigned int m_numOffPhotInContainer;    
  unsigned int m_numOffElecPassCuts;
  unsigned int m_numOffPhotPassCuts;
  unsigned int m_numOffElecTriggered;
  unsigned int m_numOffPhotTriggered;
  unsigned int m_numEmObjPassTrigger;
  unsigned int m_numEmObjTotal;
  
  // Variables for L1 & EF trigger chains 
  bool m_passed_L1_Jet_Trigger;
  bool m_passed_EF_SingleJet_Trigger;
  bool m_passed_EF_MultiJet_Trigger;
  bool m_passed_EF_egTau_Trigger;
  bool m_passed_EF_Trigger;
  std::vector<int> m_wantedTriggers;
  static const int s_L1_Jet_Trigger_mask       = 0x1;
  static const int s_EF_Trigger_mask           = 0x2;
  static const int s_EF_SingleJet_Trigger_mask = 0x4;
  static const int s_EF_MultiJet_Trigger_mask  = 0x8;
  static const int s_EF_egTau_Trigger_mask     = 0x10;

  // Python settable cuts  
  /// Only check EM thresholds in EmTauRoIs
  bool m_useEmThresholdsOnly;
  /// Cut on electron/RoI delta R
  double m_goodEMDeltaRMatch_Cut;
  /// Cut on electron/RoI eta difference
  double m_goodEMDeltaEtaMatch_Cut;
  /// Cut on electron/RoI phi difference
  double m_goodEMDeltaPhiMatch_Cut;        
  /// Isolation cut from jets
  double m_goodHadDeltaRMatch_Cut;
  /// Use delta R match check
  bool m_useDeltaRMatch;
  /// Use eta, phi difference checks
  bool m_useDeltaEtaPhiMatch;
  /// Electron type required
  int m_isEmType;
  /// Flag to ignore LAr noise burst events
  bool m_removeNoiseBursts;
  /// Flag to check trigger menu
  bool m_useTrigger;

  /// Mask giving EM bits from Em/Tau RoI
  unsigned int m_emBitMask;

  /// disabled channel/bad calo flag
  bool m_firstEvent;

  //=======================
  //   Histograms
  //=======================

  TH1F_LW* m_h_ClusterRaw_Et_gdEta;                            ///< Raw Cluster Et
  TH1F_LW* m_h_ClusterRaw_Et_triggered_gdEta;                  ///< Raw Cluster Et (Triggered)
  TH1F_LW* m_h_ClusterRaw_Et_triggered_Eff;                    ///< Raw Cluster Et (Triggered) Efficiency
  TH1F_LW* m_h_ClusterRaw_Et_transR;                           ///< Raw Cluster Et (in transition region)
  TH1F_LW* m_h_ClusterRaw_Et_triggered_transR;                 ///< Raw Cluster Et (Triggered in transition region)
  TH1F_LW* m_h_ClusterRaw_Et_triggered_transR_Eff;             ///< Raw Cluster Et (Triggered in transition region) Efficiency
  TH1F_LW* m_h_ClusterRaw_Et_bitcheck[ROI_BITS];               ///< Raw Et for Triggered Clusters passing each threshold
  TH1F_LW* m_h_ClusterRaw_Et_bitcheck_Eff[ROI_BITS];           ///< Raw Et for Triggered Clusters passing each threshold Efficiency
  TH2F_LW* m_h_ClusterRaw_10GeV_Eta_vs_Phi;                    ///< Raw Cluster eta against phi (Et_raw > 10 GeV)
  TH2F_LW* m_h_ClusterRaw_10GeV_Eta_vs_Phi_trig[ROI_BITS];     ///< Raw Cluster eta V phi (Triggered on each threshold with Et_raw > 10 GeV)
  TH2F_LW* m_h_ClusterRaw_10GeV_Eta_vs_Phi_trig_Eff[ROI_BITS]; ///< Raw Cluster eta V phi (Triggered on each threshold with Et_raw > 10 GeV) Efficiency
  TH2F_LW* m_h_ClusterRaw_20GeV_Eta_vs_Phi;                    ///< Raw Cluster eta against phi (Et_raw > 20 GeV)
  TH2F_LW* m_h_ClusterRaw_20GeV_Eta_vs_Phi_trig[ROI_BITS];     ///< Raw Cluster eta V phi (Triggered on each threshold with Et_raw > 20 GeV)
  TH2F_LW* m_h_ClusterRaw_20GeV_Eta_vs_Phi_trig_Eff[ROI_BITS]; ///< Raw Cluster eta V phi (Triggered on each threshold with Et_raw > 20 GeV) Efficiency
  TH2F_LW* m_h_ClusterRaw_30GeV_Eta_vs_Phi;                    ///< Raw Cluster eta against phi (Et_raw > 30 GeV)
  TH2F_LW* m_h_ClusterRaw_30GeV_Eta_vs_Phi_trig[ROI_BITS];     ///< Raw Cluster eta V phi (Triggered on each threshold with Et_raw > 30 GeV)
  TH2F_LW* m_h_ClusterRaw_30GeV_Eta_vs_Phi_trig_Eff[ROI_BITS]; ///< Raw Cluster eta V phi (Triggered on each threshold with Et_raw > 30 GeV) Efficiency
   
  TH2F_LW* m_h_TrigTower_emBadCalo;                            ///< EM Trigger Towers affected by Missing FEBs - eta V phi
  TH2F_LW* m_h_TrigTower_emDeadChannel;                        ///< EM Trigger Towers with dead channels - eta V phi
  TH1F_LW* m_h_LAr_emNoisy;                                    ///< LAr Error Bits in Rejected Events


};

#endif
