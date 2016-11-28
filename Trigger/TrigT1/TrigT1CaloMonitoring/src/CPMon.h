/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     CPMon.h
// PACKAGE:  TrigT1CaloMonitoring
//
// AUTHOR:   Peter Faulkner
//	     
//
// ********************************************************************
#ifndef TRIGT1CALOMONITORING_CPMON_H
#define TRIGT1CALOMONITORING_CPMON_H

#include <map>
#include <string>
#include <vector>

#include "GaudiKernel/ToolHandle.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "AthContainers/DataVector.h"

#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/CPMTower.h"


class TH1F_LW;
class TH2F_LW;
class TH2I_LW;
class StatusCode;

// ============================================================================
namespace LVL1 {
// ============================================================================
// Forward declarations:
// ============================================================================
  class ITrigT1CaloMonErrorTool;
  class TrigT1CaloLWHistogramTool;
// ============================================================================
/** Monitoring of the Cluster Processor.
 *
 *  Produces histograms of CPM and CMX-CP input data, output data and hardware errors.
 *
 *  <b>ROOT Histogram Directories:</b>
 *
 *  <table>
 *  <tr><th> Directory                            </th><th> Contents                             </th></tr>
 *  <tr><td> @c L1Calo/CPM/Input                  </td><td> Et, eta, phi distributions               <br>
 *                                                          CPM tower eta/phi hitmaps                <br>
 *                                                          CPM tower eta/phi hitmaps weighted by Et <br>
 *                                                          PPM tower eta/phi hitmaps                <br>
 *                                                          Number of slices/triggered slice         <br>
 *                                                          PPM/CPM tower slice match            </td></tr>
 *  <tr><td> @c L1Calo/CPM/Output                 </td><td> Eta/phi hitmaps Em and Tau               <br>
 *                                                          Eta/phi hitmaps non-zero isolation       <br>
 *                                                          Cluster energy EM and Tau                <br>
 *                                                          Isolation EM and Tau                     <br>
 *                                                          TOBs per CPM                         </td></tr>
 *  <tr><td> @c L1Calo/CPM/Errors/Hardware        </td><td> Hardware error summary and overview  </td></tr>
 *  <tr><td> @c L1Calo/CPM/Errors/Hardware/Detail </td><td> Parity and linkdown errors by eta/phi    <br>
 *                                                          Substatus errors by crate/module         <br>
 *                                                          CPM and CMX error event numbers      </td></tr>
 *  <tr><td> @c L1Calo/CPM_CMX/Input              </td><td> Eta/phi hitmaps Left and Right           <br>
 *                                                          Eta/phi hitmaps non-zero isolation       <br>
 *                                                          Cluster energy Left and Right            <br>
 *                                                          Isolation Left and Right                 <br>
 *                                                          TOBs per CPM                             <br>
 *                                                          TOBs per CMX                             <br>
 *                                                          TOBs overflow                        </td></tr>
 *  <tr><td> @c L1Calo/CPM_CMX/Output             </td><td> Hit sums thresholds weighted             <br>
 *                                                          Number of slices/triggered slice         <br>
 *                                                          Topo output checksum, occupancy map,
 *                                                          occupancy counts                         <br>
 *                                                          Topo TOBs per CPM                        <br>
 *                                                          Topo TOBs per CMX                    </td></tr>
 *  <tr><td> @c L1Calo/CPM_CMX/Errors/Hardware    </td><td> Substatus errors                         <br>
 *                                                          Parity errors                        </td></tr>
 *  </table>
 *
 *  <!--
 *  <b>Notes on Particular Histograms:</b>
 *
 *  <table>
 *  <tr><th> Histogram                      </th><th> Comment                       </th></tr>
 *  <tr><td>                                <br>                                    </td></tr>
 *  </table>
 *  -->
 *
 *  <b>Custom Merges Used (Tier0):</b>
 *
 *  <table>
 *  <tr><th> Merge                                    </th><th> Used For                 </th></tr>
 *  <tr><td> @ref MergesUsedsection "@c eventSample " </td><td> Error event number plots </td></tr>
 *  </table>
 *
 *  <b>StoreGate Containers Used:</b>
 *
 *  <table>
 *  <tr><th> Container                 </th><th> Comment                                  </th></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::TriggerTower>   </td><td> PPM data                                 </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CPMTower>       </td><td> CPM Tower data                           </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CPMTower>       </td><td> CPM Tower overlap data                   </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CPMTobRoI>      </td><td> CPM RoI data (TOBs)                      </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CMXCPTob>       </td><td> CMX-CP TOB data                          </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CMXCPHits>      </td><td> CMX-CP Hits data and Topo info           </td></tr>
 *  <tr><td> @c std::vector<int>       <br>
 *           @c "L1CaloCPMErrorVector" </td><td> Output.
 *                                               Error summary bits for global histograms </td></tr>
 *  </table>
 *
 *  <b>Tools Used:</b>
 *
 *  <table>
 *  <tr><th> Tool                               </th><th> Description          </th></tr>
 *  <tr><td> @c TrigT1CaloMonErrorTool   </td><td> @copydoc m_errorTool </td></tr>
 *  <tr><td> @c TrigT1CaloLWHistogramTool </td><td> @copydoc m_histTool  </td></tr>
 *  </table>
 *
 *  <b>JobOption Properties:</b>
 *
 *  <table>
 *  <tr><th> Property                   </th><th> Description                        </th></tr>
 *  <tr><td> @c ErrorTool               </td><td> @copydoc m_errorTool               </td></tr>
 *  <tr><td> @c HistogramTool           </td><td> @copydoc m_histTool                </td></tr>
 *  <tr><td> @c TriggerTowerLocation    </td><td> @copydoc m_triggerTowerLocation    </td></tr>
 *  <tr><td> @c BS_xAODTriggerTowerContainer </td><td> @copydoc m_xAODTriggerTowerContainerName </td></tr>
 *  <tr><td> @c CPMTowerLocation        </td><td> @copydoc m_cpmTowerLocation        </td></tr>
 *  <tr><td> @c CPMTowerLocationOverlap </td><td> @copydoc m_cpmTowerLocationOverlap </td></tr>
 *  <tr><td> @c CMXCPTobLocation        </td><td> @copydoc m_cmxCpTobLocation        </td></tr>
 *  <tr><td> @c CMXCPHitsLocation       </td><td> @copydoc m_cmxCpHitsLocation       </td></tr>
 *  <tr><td> @c CPMTobRoILocation       </td><td> @copydoc m_cpmTobRoiLocation       </td></tr>
 *  <tr><td> @c ErrorLocation           </td><td> @copydoc m_errorLocation           </td></tr>
 *  <tr><td> @c RootDirectory           </td><td> @copydoc m_rootDir                 </td></tr>
 *  <tr><td> @c MaxEnergyRange          </td><td> @copydoc m_maxEnergyRange          </td></tr>
 *  <tr><td> @c MaxTOBsPerCMX           </td><td> @copydoc m_maxTobsPerCmx           </td></tr>
 *  </table>
 *
 *  <b>Related Documentation:</b>                           <!-- UPDATE!! -->
 *
 *  <a href="http://hepwww.rl.ac.uk/Atlas-L1/Modules/ROD/ROD-spec-version1_2_2.pdf">
 *  ATLAS Level-1 Calorimeter Trigger - Read-out Driver</a>
 *
 *  @author Peter Faulkner
 *
 */

  class CPMon : public ManagedMonitorToolBase {
    
  public:
    
  CPMon(const std::string & type, const std::string & name,
		                  const IInterface* parent);
    

  virtual ~CPMon();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
    
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

private:

  /// Error summary plot bins
  enum SummaryErrors { EMParity, EMLink, HadParity, HadLink, CPMStatus,
                       TOBParity, SumParity, CMXStatus, NumberOfSummaryBins };

  //typedef DataVector<LVL1::TriggerTower> TriggerTowerCollection;
  // typedef DataVector<LVL1::CPMTower>     CpmTowerCollection;
  // typedef DataVector<LVL1::CMXCPTob>     CmxCpTobCollection;
  // typedef DataVector<LVL1::CMXCPHits>    CmxCpHitsCollection;
  // typedef DataVector<LVL1::CPMTobRoI>    CpmTobRoiCollection;

  //typedef std::map<unsigned int, LVL1::TriggerTower*> TriggerTowerMap;
  typedef std::map<const unsigned int, const xAOD::TriggerTower*> TriggerTowerMap;
  typedef std::map<unsigned int, const xAOD::CPMTower*> CpmTowerMap;
  
  static const int s_crates     = 4;   ///< Number of CPM crates
  static const int s_modules    = 14;  ///< Number of modules per crate (modules numbered 1-14)
  static const int s_maxSlices  = 5;   ///< Maximum number of slices
  static const int s_thresholds = 16;  ///< Number of EM/Tau threshold bits
  static const int s_threshBits = 3;   ///< Number of bits per threshold for hit sums
  static const int s_threshMask = 0x7; ///< Hit sums mask
  static const int s_tobsPerCPM = 5;   ///< Maximum number of TOBs per CPM sent to CMX
  static const int s_isolRange  = 32;  ///< Maximum range for encoded isolation
  static const int s_isolBits   = 5;   ///< Number of bits for encoded isolation

  /// Tool to retrieve bytestream errors
  ToolHandle<ITrigT1CaloMonErrorTool>   m_errorTool;  
  /// Histogram helper tool
  ToolHandle<TrigT1CaloLWHistogramTool> m_histTool;

  /// Trigger Tower container StoreGate key
  std::string m_triggerTowerLocation; //has been replaced by the xAODTriggerTower
  /// xAODTriggerTower Container key
  std::string m_xAODTriggerTowerContainerName;
  /// Core CPM tower container StoreGate key
  std::string m_cpmTowerLocation;
  /// Overlap CPM tower container StoreGate key
  std::string m_cpmTowerLocationOverlap;
  /// CMX-CP TOBs container StoreGate key
  std::string m_cmxCpTobLocation;
  /// CMX-CP hits container StoreGate key
  std::string m_cmxCpHitsLocation;
  /// DAQ CPM TOB RoI container StoreGate key
  std::string m_cpmTobRoiLocation;
  /// Error vector StoreGate key
  std::string m_errorLocation;
  
  /// Root directory
  std::string m_rootDir;

  /// Maximum energy plotted
  int m_maxEnergyRange;
  /// Maximum number of TOBs per CMX plotted
  int m_maxTobsPerCmx;
  /// Histograms booked flag
  bool m_histBooked;

  //=======================
  //   Timeslice plots
  //=======================

  TH2F_LW* m_h_cpm_2d_tt_Slices;                ///< CPM Slices and Triggered Slice
  TH2F_LW* m_h_cmx_2d_thresh_Slices;            ///< CMX Slices and Triggered Slice
  TH2F_LW* m_h_cpm_2d_tt_SliceMatch;            ///< PPM/CPM Tower Slice Match

  //=============================================
  //   CPM Tower - Trigger Tower plots
  //=============================================

  // TriggerTower plots
  TH2F_LW* m_h_ppm_em_2d_etaPhi_tt_Hitmap;      ///< PPM Trigger Tower EM eta/phi
  TH2F_LW* m_h_ppm_had_2d_etaPhi_tt_Hitmap;     ///< PPM Trigger Tower HAD eta/phi
  // CPMTower plots
  TH1F_LW* m_h_cpm_em_1d_tt_Et;                 ///< CPM Tower EM Et
  TH1F_LW* m_h_cpm_had_1d_tt_Et;                ///< CPM Tower HAD Et
  TH1F_LW* m_h_cpm_em_1d_tt_Eta;                ///< CPM Tower EM eta
  TH1F_LW* m_h_cpm_had_1d_tt_Eta;               ///< CPM Tower HAD eta
  TH1F_LW* m_h_cpm_em_1d_tt_Phi;                ///< CPM Tower EM phi
  TH1F_LW* m_h_cpm_had_1d_tt_Phi;               ///< CPM Tower HAD phi
  TH2F_LW* m_h_cpm_em_2d_etaPhi_tt_Hitmap;      ///< CPM Tower EM eta/phi
  TH2F_LW* m_h_cpm_had_2d_etaPhi_tt_Hitmap;     ///< CPM Tower HAD eta/phi
  TH2F_LW* m_h_cpm_em_2d_etaPhi_tt_EtWeighted;  ///< CPM Tower EM eta/phi weighted
  TH2F_LW* m_h_cpm_had_2d_etaPhi_tt_EtWeighted; ///< CPM Tower HAD eta/phi weighted
  // Errors
  TH2F_LW* m_h_cpm_em_2d_etaPhi_tt_Parity;      ///< CPM Tower EM Parity Errors
  TH2F_LW* m_h_cpm_had_2d_etaPhi_tt_Parity;     ///< CPM Tower HAD Parity Errors
  TH2F_LW* m_h_cpm_em_2d_etaPhi_tt_LinkDown;    ///< CPM Tower EM Link Down Errors
  TH2F_LW* m_h_cpm_had_2d_etaPhi_tt_LinkDown;   ///< CPM Tower HAD Link Down Errors
  TH2F_LW* m_h_cpm_2d_Status;                   ///< CPM Sub-status bits

  //=============================================
  //  CPM TOB RoIs
  //=============================================

  TH1F_LW* m_h_cpm_1d_roi_EnergyEm;             ///< CPM TOB RoI Cluster Energy EM
  TH1F_LW* m_h_cpm_1d_roi_EnergyTau;            ///< CPM TOB RoI Cluster Energy Tau
  TH1F_LW* m_h_cpm_1d_roi_IsolationEm;          ///< CPM TOB RoI Encoded Isolation Value EM
  TH1F_LW* m_h_cpm_1d_roi_IsolationTau;         ///< CPM TOB RoI Encoded Isolation Value Tau
  TH1F_LW* m_h_cpm_1d_roi_IsolationBitsEm;      ///< CPM TOB RoI Encoded Isolation Bits EM
  TH1F_LW* m_h_cpm_1d_roi_IsolationBitsTau;     ///< CPM TOB RoI Encoded Isolation Bits Tau
  TH1F_LW* m_h_cpm_1d_roi_TOBsPerCPMEm;         ///< CPM TOB RoI TOBs per CPM EM
  TH1F_LW* m_h_cpm_1d_roi_TOBsPerCPMTau;        ///< CPM TOB RoI TOBs per CPM Tau
  TH2F_LW* m_h_cpm_2d_etaPhi_roi_HitmapEm;      ///< CPM TOB RoIs EM Hit Map
  TH2F_LW* m_h_cpm_2d_etaPhi_roi_HitmapTau;     ///< CPM TOB RoIs Tau Hit Map
  TH2F_LW* m_h_cpm_2d_etaPhi_roi_EtWeightedEm;  ///< CPM TOB RoIs EM Weighted by Energy
  TH2F_LW* m_h_cpm_2d_etaPhi_roi_EtWeightedTau; ///< CPM TOB RoIs Tau Weighted by Energy
  TH2F_LW* m_h_cpm_2d_etaPhi_roi_HitmapIsolEm;  ///< CPM TOB RoIs EM Non-zero Isolation Hit Map
  TH2F_LW* m_h_cpm_2d_etaPhi_roi_HitmapIsolTau; ///< CPM TOB RoIs Tau Non-zero Isolation Hit Map

  //=============================================
  //  CMX-CP TOBs
  //=============================================

  TH1F_LW* m_h_cmx_1d_tob_EnergyLeft;           ///< CMX-CP TOBs Cluster Energy Left CMX
  TH1F_LW* m_h_cmx_1d_tob_EnergyRight;          ///< CMX-CP TOBs Cluster Energy Right CMX
  TH1F_LW* m_h_cmx_1d_tob_IsolationLeft;        ///< CMX-CP TOBs Encoded Isolation Value Left CMX
  TH1F_LW* m_h_cmx_1d_tob_IsolationRight;       ///< CMX-CP TOBs Encoded Isolation Value Right CMX
  TH1F_LW* m_h_cmx_1d_tob_IsolationBitsLeft;    ///< CMX-CP TOBs Encoded Isolation Bits Left CMX
  TH1F_LW* m_h_cmx_1d_tob_IsolationBitsRight;   ///< CMX-CP TOBs Encoded Isolation Bits Right CMX
  TH1F_LW* m_h_cmx_1d_tob_TOBsPerCPMLeft;       ///< CMX-CP TOBs per CPM Left CMX
  TH1F_LW* m_h_cmx_1d_tob_TOBsPerCPMRight;      ///< CMX-CP TOBs per CPM Right CMX
  TH1F_LW* m_h_cmx_1d_tob_TOBsPerCMXLeft;       ///< CMX-CP TOBs per CMX Left
  TH1F_LW* m_h_cmx_1d_tob_TOBsPerCMXRight;      ///< CMX-CP TOBs per CMX Right
  TH2F_LW* m_h_cmx_2d_tob_HitmapLeft;           ///< CMX-CP TOBs Left CMX Hit Map
  TH2F_LW* m_h_cmx_2d_tob_HitmapRight;          ///< CMX-CP TOBs Right CMX Hit Map
  TH2F_LW* m_h_cmx_2d_tob_HitmapIsolLeft;       ///< CMX-CP TOBs Left CMX Non-zero Isolation Hit Map
  TH2F_LW* m_h_cmx_2d_tob_HitmapIsolRight;      ///< CMX-CP TOBs Right CMX Non-zero Isolation Hit Map
  TH2F_LW* m_h_cmx_2d_tob_Overflow;             ///< CMX-CP TOBs Overflow

  //=============================================
  //  CMX-CP Hits
  //=============================================

  TH2F_LW* m_h_cmx_2d_thresh_SumsWeightedLeft;  ///< CMX-CP Hit Sums Thresholds Weighted Left CMX
  TH2F_LW* m_h_cmx_2d_thresh_SumsWeightedRight; ///< CMX-CP Hit Sums Thresholds Weighted Right CMX
  TH1F_LW* m_h_cmx_1d_topo_OutputChecksum;      ///< CMX-CP Topo Output Checksum Non-zero
  TH2F_LW* m_h_cmx_2d_topo_CPMOccupancyMap;     ///< CMX-CP Topo CPM Occupancy Maps
  TH2F_LW* m_h_cmx_2d_topo_CPMOccupancyCounts;  ///< CMX-CP Topo CPM Occupancy Counts Weighted
  TH1F_LW* m_h_cmx_1d_topo_TOBsPerCPMLeft;      ///< CMX-CP Topo TOBs per CPM Left CMX
  TH1F_LW* m_h_cmx_1d_topo_TOBsPerCPMRight;     ///< CMX-CP Topo TOBs per CPM Right CMX
  TH1F_LW* m_h_cmx_1d_topo_TOBsPerCMXLeft;      ///< CMX-CP Topo TOBs per CMX Left
  TH1F_LW* m_h_cmx_1d_topo_TOBsPerCMXRight;     ///< CMX-CP Topo TOBs per CMX Right

  //=============================================
  //  CMX Errors
  //=============================================

  TH2F_LW* m_h_cmx_2d_tob_Parity;               ///< CMX-CP TOB Parity Errors
  TH2F_LW* m_h_cmx_2d_thresh_Parity;            ///< CMX Remote Hit Sums Parity Errors
  TH2F_LW* m_h_cmx_2d_Status;                   ///< CMX Sub-status bits

  //=============================================
  //  Error summary
  //=============================================

  TH1F_LW* m_h_cpm_1d_ErrorSummary;             ///< CP Error Summary
  TH2F_LW* m_h_cpm_2d_ErrorOverview;            ///< CP Error Overview
  TH2I_LW* m_h_cpm_2d_ErrorEventNumbers;        ///< CP Error Event Numbers

};
// ============================================================================
} // end namespace
// ============================================================================
#endif
