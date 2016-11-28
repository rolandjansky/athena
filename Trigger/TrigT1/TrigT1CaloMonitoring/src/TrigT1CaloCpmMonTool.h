/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigT1CaloCpmMonTool.h
// PACKAGE:  TrigT1CaloMonitoring
//
// AUTHOR:   Peter Faulkner
//	     
//
// ********************************************************************
#ifndef TRIGT1CALOCPMMONTOOL_H
#define TRIGT1CALOCPMMONTOOL_H

#include <map>
#include <string>
#include <vector>

#include "GaudiKernel/ToolHandle.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "AthContainers/DataVector.h"

class TH1F_LW;
class TH2F_LW;
class TH2I_LW;
class StatusCode;

// ============================================================================
namespace LVL1 {
// ============================================================================
// Forward declarations:
// ============================================================================
class CPMTower;
class CPMHits;
class CMMCPHits;
class CPMRoI;
class TriggerTower;
class ITrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramToolV1;
// ============================================================================
/** Monitoring of the Cluster Processor.
 *
 *  Produces histograms of CPM and CMM-CP input data, output data and hardware errors.
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
 *  <tr><td> @c L1Calo/CPM/Output/RoI             </td><td> Eta/phi hitmaps all RoIs                 <br>
 *                                                          Eta/phi hitmaps EM RoIs only             <br>
 *                                                          Eta/phi hitmaps Tau RoIs only            <br>
 *                                                          CPM RoI saturation                       <br>
 *                                                          RoI thresholds by crate/module       </td></tr>
 *  <tr><td> @c L1Calo/CPM/Output/Thresholds      </td><td> CPM hits thresholds by crate/module 
 *                                                          weighted                             </td></tr>
 *  <tr><td> @c L1Calo/CPM/Errors/Hardware        </td><td> Hardware error summary and overview  </td></tr>
 *  <tr><td> @c L1Calo/CPM/Errors/Hardware/Detail </td><td> Parity and linkdown errors by eta/phi    <br>
 *                                                          Substatus errors by crate/module         <br>
 *                                                          CPM and CMM error event numbers      </td></tr>
 *  <tr><td> @c L1Calo/CPM_CMM/Input              </td><td> Number of slices/triggered slice         <br>
 *                                                          CPM/CMM hits slice match                 <br>
 *                                                          CMM hits thresholds by crate/module
 *                                                          weighted                             </td></tr>
 *  <tr><td> @c L1Calo/CPM_CMM/Output             </td><td> Hit sums thresholds weighted         </td></tr>
 *  <tr><td> @c L1Calo/CPM_CMM/Errors/Hardware    </td><td> Substatus errors                         <br>
 *                                                          Parity errors                        </td></tr>
 *  </table>
 *
 *  <b>Notes on Particular Histograms:</b>
 *
 *  <table>
 *  <tr><th> Histogram                      </th><th> Comment                                </th></tr>
 *  <tr><td> @c L1Calo/CPM/Output/RoI/      <br>
 *           @c cpm_2d_etaPhi_roi_Hitmap    <br>
 *           @c cpm_2d_etaPhi_roi_EmHitmap  <br>
 *           @c cpm_2d_etaPhi_roi_TauHitmap </td><td> These are duplicates and should be 
 *                                                    deleted once DQ config file is updated </td></tr>
 *  </table>
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
 *           @c <LVL1::CPMRoI>         </td><td> CPM RoI data                             </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CPMHits>        </td><td> CPM Hits data                            </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CMMCPHits>      </td><td> CMM-CP Hits data                         </td></tr>
 *  <tr><td> @c std::vector<int>       <br>
 *           @c "L1CaloCPMErrorVector" </td><td> Output.
 *                                               Error summary bits for global histograms </td></tr>
 *  </table>
 *
 *  <b>Tools Used:</b>
 *
 *  <table>
 *  <tr><th> Tool                         </th><th> Description          </th></tr>
 *  <tr><td> @c TrigT1CaloMonErrorTool    </td><td> @copydoc m_errorTool </td></tr>
 *  <tr><td> @c TrigT1CaloLWHistogramToolV1 </td><td> @copydoc m_histTool  </td></tr>
 *  </table>
 *
 *  <b>JobOption Properties:</b>
 *
 *  <table>
 *  <tr><th> Property                   </th><th> Description                        </th></tr>
 *  <tr><td> @c CPMTowerLocation        </td><td> @copydoc m_cpmTowerLocation        </td></tr>
 *  <tr><td> @c CPMTowerLocationOverlap </td><td> @copydoc m_cpmTowerLocationOverlap </td></tr>
 *  <tr><td> @c CPMHitsLocation         </td><td> @copydoc m_cpmHitsLocation         </td></tr>
 *  <tr><td> @c CMMCPHitsLocation       </td><td> @copydoc m_cmmCpHitsLocation       </td></tr>
 *  <tr><td> @c CPMRoILocation          </td><td> @copydoc m_cpmRoiLocation          </td></tr>
 *  <tr><td> @c TriggerTowerLocation    </td><td> @copydoc m_triggerTowerLocation    </td></tr>
 *  <tr><td> @c RootDirectory           </td><td> @copydoc m_rootDir                 </td></tr>
 *  <tr><td> @c MaxEnergyRange          </td><td> @copydoc m_maxEnergyRange          </td></tr>
 *  </table>
 *
 *  <b>Related Documentation:</b>
 *
 *  <a href="http://hepwww.rl.ac.uk/Atlas-L1/Modules/ROD/ROD-spec-version1_2_2.pdf">
 *  ATLAS Level-1 Calorimeter Trigger - Read-out Driver</a>
 *
 *  @author Peter Faulkner
 *
 */

class TrigT1CaloCpmMonTool: public ManagedMonitorToolBase
{

public:
  
  TrigT1CaloCpmMonTool(const std::string & type, const std::string & name,
		       const IInterface* parent);
    

  virtual ~TrigT1CaloCpmMonTool();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
    
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

private:

  /// Error summary plot bins
  enum SummaryErrors { EMParity, EMLink, HadParity, HadLink, CPMStatus,
                       RoIParity, CMMParity, CMMStatus, NumberOfSummaryBins };

  typedef DataVector<LVL1::CPMTower>     CpmTowerCollection;
  typedef DataVector<LVL1::CPMHits>      CpmHitsCollection;
  typedef DataVector<LVL1::CMMCPHits>    CmmCpHitsCollection;
  typedef DataVector<LVL1::CPMRoI>       CpmRoiCollection;
  typedef DataVector<LVL1::TriggerTower> TriggerTowerCollection;

  typedef std::map<unsigned int, LVL1::TriggerTower*> TriggerTowerMap;
  typedef std::map<unsigned int, LVL1::CPMTower*>     CpmTowerMap;
  typedef std::map<unsigned int, LVL1::CPMHits*>      CpmHitsMap;
  typedef std::map<unsigned int, LVL1::CMMCPHits*>    CmmCpHitsMap;
  
  static const int s_crates     = 4;   ///< Number of CPM crates
  static const int s_modules    = 14;  ///< Number of modules per crate (modules numbered 1-14)
  static const int s_maxSlices  = 5;   ///< Maximum number of slices
  static const int s_thresholds = 16;  ///< Number of EM/Tau threshold bits
  static const int s_threshBits = 3;   ///< Number of bits per threshold for hit sums
  static const int s_threshMask = 0x7; ///< Hit sums mask

  /// Tool to retrieve bytestream errors
  ToolHandle<ITrigT1CaloMonErrorTool>    m_errorTool;
  /// Histogram helper tool
  ToolHandle<TrigT1CaloLWHistogramToolV1> m_histTool;

  /// Core CPM tower container StoreGate key
  std::string m_cpmTowerLocation;
  /// Overlap CPM tower container StoreGate key
  std::string m_cpmTowerLocationOverlap;
  /// CPM hits container StoreGate key
  std::string m_cpmHitsLocation;
  /// CMM-CP hits container StoreGate key
  std::string m_cmmCpHitsLocation;
  /// DAQ CPM RoI container StoreGate key
  std::string m_cpmRoiLocation;
  /// Trigger Tower container StoreGate key
  std::string m_triggerTowerLocation;
  
  /// Root directory
  std::string m_rootDir;

  /// Maximum energy plotted
  int m_maxEnergyRange;
  /// Number of events
  int m_events;
  /// EM RoI bitmask
  unsigned int m_emBitMask;
  /// Tau RoI bitmask
  unsigned int m_tauBitMask;
  /// Histograms booked flag
  bool m_histBooked;

  //=======================
  //   Timeslice plots
  //=======================

  TH2F_LW* m_h_cpm_2d_tt_Slices;                ///< CPM Slices and Triggered Slice
  TH2F_LW* m_h_cmm_2d_thresh_Slices;            ///< CMM Slices and Triggered Slice
  TH2F_LW* m_h_cpm_2d_tt_SliceMatch;            ///< PPM/CPM Tower Slice Match
  TH2F_LW* m_h_cmm_2d_thresh_SliceMatch;        ///< CPM/CMM Hits Slice Match

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
  //  CPM RoIs
  //=============================================

  TH2F_LW* m_h_cpm_2d_roi_Thresholds;           ///< CPM RoI Thresholds
  TH2F_LW* m_h_cpm_2d_etaPhi_roi_HitmapAll;     ///< CPM RoIs All Eta-Phi Hit Map
  TH2F_LW* m_h_cpm_2d_etaPhi_roi_HitmapEm;      ///< CPM RoIs EM Eta-Phi Hit Map
  TH2F_LW* m_h_cpm_2d_etaPhi_roi_HitmapTau;     ///< CPM RoIs Tau Eta-Phi Hit Map
  //To be deleted:
  TH2F_LW* m_h_cpm_2d_etaPhi_roi_Hitmap;        ///< CPM RoIs All Eta-Phi Hit Map (Duplicate)
  TH2F_LW* m_h_cpm_2d_etaPhi_roi_EmHitmap;      ///< CPM RoIs EM Eta-Phi Hit Map (Duplicate)
  TH2F_LW* m_h_cpm_2d_etaPhi_roi_TauHitmap;     ///< CPM RoIs Tau Eta-Phi Hit Map (Duplicate)
  // Tower saturation
  TH2F_LW* m_h_cpm_2d_etaPhi_roi_Saturation;    ///< CPM RoI Tower Saturation
  // Parity errors
  TH2F_LW* m_h_cpm_2d_etaPhi_roi_Parity;        ///< CPM RoI Parity Errors

  //=============================================
  //  CPM Hits
  //=============================================

  TH2F_LW* m_h_cpm_2d_thresh_Weighted;          ///< CPM Hits Thresholds Weighted

  //=============================================
  //  CMM-CP Hits
  //=============================================

  TH2F_LW* m_h_cmm_2d_thresh_Weighted;          ///< CMM-CP Hits Thresholds Weighted
  TH2F_LW* m_h_cmm_2d_thresh_SumsWeighted;      ///< CMM-CP Hit Sums Thresholds Weighted
  // Errors
  TH2F_LW* m_h_cmm_2d_thresh_Parity;            ///< CMM Parity Errors
  TH2F_LW* m_h_cmm_2d_Status;                   ///< CMM Sub-status bits

  //=============================================
  //  Error summary
  //=============================================

  TH1F_LW* m_h_cpm_1d_ErrorSummary;             ///< CP Error Summary
  TH2F_LW* m_h_cpm_2d_ErrorOverview;            ///< CP Error Overview
  TH2I_LW* m_h_cpm_2d_ErrorEventNumbers;        ///< CP Error Event Numbers

};
// ============================================================================
}  // end namespace
// ============================================================================
#endif
