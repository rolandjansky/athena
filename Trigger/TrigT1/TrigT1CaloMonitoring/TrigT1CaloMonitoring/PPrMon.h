/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     PPrMon.h
// PACKAGE:  TrigT1CaloMonitoring
//
// AUTHOR:   Ethan-Etienne Woehrling (eew@hep.ph.bham.ac.uk)
//           Johanna Fleckner (Johanna.Fleckner@uni-mainz.de)
//	     
//
// ********************************************************************
#ifndef PPRMON_H
#define PPRMON_H

#include <string>
#include <vector>

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "GaudiKernel/ToolHandle.h"

class TH1F_LW;
class TH2F_LW;
class TH2I_LW;
class TProfile_LW;
class TProfile2D_LW;

class StatusCode;

class TrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramTool;

namespace LVL1 {
  class IL1TriggerTowerTool;
}

/** Monitoring of the Preprocessor
 *
 *  Produces histograms of PPM FADC and LUT data and hardware errors.
 *
 *  <b>ROOT Histogram Directories:</b>
 *
 *  <table>
 *  <tr><th> Directory                       </th><th> Contents                             </th></tr>
 *  <tr><td> @c L1Calo/PPM/ADC/EtaPhiMaps    </td><td> Eta-phi ADC hitmaps and profiles for
 *                                                     ADC counts > ADCHitMap_Thresh        </td></tr>
 *  <tr><td> @c L1Calo/PPM/ADC/Timeslices    </td><td> Triggered timeslice                  <br>
 *                                                     Maximum timeslice                    <br>
 *                                                     Eta-phi maximum timeslice profile    <br>
 *                                                     Signal shape profiles by partition   </td></tr>
 *  <tr><td> @c L1Calo/PPM/LUT/Distributions </td><td> LUT Et, eta, phi distributions       <br>
 *                                                     Number of LUT>5 by bunchcrossing     <br>
 *                                                     BCID bits VS LUT Et                  </td></tr>
 *  <tr><td> @c L1Calo/PPM/LUT/EtaPhiMaps    </td><td> Eta-phi LUT hitmaps for various Et
 *                                                     thresholds.
 *                                                     Produced every lumiblock on Tier0    <br>
 *                                                     Eta-phi average Et for Et>5          </td></tr>
 *  <tr><td> <tt>L1Calo/PPM/LUT/EtaPhiMaps/lumi_0,1,... </tt></td><td>
 *                                                     Similar for last few lumiblocks.
 *                                                     Online only                          </td></tr>
 *  <tr><td> @c L1Calo/PPM/Errors            </td><td> SubStatus word errors                <br>
 *                                                     ASIC error field errors              </td></tr>
 *  <tr><td> @c L1Calo/PPM/Errors/Detail     </td><td> ASIC error field errors by MCM       </td></tr>
 *  </table>
 *
 *  <b>Notes on Particular Histograms:</b>
 *
 *  <table>
 *  <tr><th> Histogram                                     </th><th> Comment                                 </th></tr>
 *  <tr><td> @c L1Calo/PPM/ADC/Timeslices/                 <br>
 *           @c ppm_{em|had}_2d_etaPhi_tt_adc_MaxTimeslice </td><td> Timeslices are numbered 1-nslice so empty bins can be distinguished </td></tr>
 *  <tr><td> @c ppm_{em|had}_1d_tt_adc_SignalProfileXXXXXX </td><td> Average ADC values each slice for Lut>0 </td></tr>
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
 *  <tr><td> @c EventInfo              </td><td> For bunch crossing number via @c EventID </td></tr>
 *  <tr><td> @c std::vector<int>       <br>
 *           @c "L1CaloPPMErrorVector" </td><td> Output.
 *                                               Error summary bits for global histograms </td></tr>
 *  </table>
 *
 *  <b>Tools Used:</b>
 *
 *  <table>
 *  <tr><th> Tool                         </th><th> Description          </th></tr>
 *  <tr><td> @c TrigT1CaloMonErrorTool    </td><td> @copydoc m_errorTool </td></tr>
 *  <tr><td> @c TrigT1CaloLWHistogramTool </td><td> @copydoc m_histTool  </td></tr>
 *  <tr><td> @c LVL1::IL1TriggerTowerTool </td><td> @copydoc m_ttTool    </td></tr>
 *  </table>
 *
 *  <b>JobOption Properties:</b>
 *
 *  <table>
 *  <tr><th> Property                    </th><th> Description                          </th></tr>
 *  <tr><td> @c BS_TriggerTowerContainer </td><td> @copydoc m_TriggerTowerContainerName </td></tr>
 *  <tr><td> @c LUTHitMap_LumiBlocks     </td><td> @copydoc m_TT_HitMap_LumiBlocks      </td></tr>
 *  <tr><td> @c ADCHitMap_Thresh         </td><td> @copydoc m_TT_ADC_HitMap_Thresh      </td></tr>
 *  <tr><td> @c MaxEnergyRange           </td><td> @copydoc m_MaxEnergyRange            </td></tr>
 *  <tr><td> @c ADCPedestal              </td><td> @copydoc m_TT_ADC_Pedestal           </td></tr>
 *  <tr><td> @c HADFADCCut               </td><td> @copydoc m_HADFADCCut                </td></tr>
 *  <tr><td> @c EMFADCCut                </td><td> @copydoc m_EMFADCCut                 </td></tr>
 *  <tr><td> @c PathInRootFile           </td><td> @copydoc m_PathInRootFile            </td></tr>
 *  <tr><td> @c ErrorPathInRootFile      </td><td> @copydoc m_ErrorPathInRootFile       </td></tr>
 *  <tr><td> @c OnlineTest               </td><td> @copydoc m_onlineTest                </td></tr>
 *  <tr><td> @c LUTHitMap_ThreshVec      </td><td> @copydoc m_TT_HitMap_ThreshVec       </td></tr>
 *  </table>
 *
 *  <b>Related Documentation:</b>
 *
 *  <a href="http://hepwww.rl.ac.uk/Atlas-L1/Modules/ROD/ROD-spec-version1_2_2.pdf">
 *  ATLAS Level-1 Calorimeter Trigger - Read-out Driver</a>
 *
 *  @authors Johanna Fleckner, Andrea Neusiedl, Peter Faulkner, Ivana Hristova
 *
 */

class PPrMon: public ManagedMonitorToolBase
{

 public:
  
  PPrMon(const std::string & type, const std::string & name,
		                   const IInterface* parent);

  virtual ~PPrMon();

  virtual StatusCode initialize();
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
private:
  
  /// Subdetector partitions
  enum CaloPartitions { LArFCAL1C, LArEMECC, LArOverlapC, LArEMBC, LArEMBA,
      LArOverlapA, LArEMECA, LArFCAL1A, LArFCAL23C, LArHECC,
      TileEBC, TileLBC, TileLBA, TileEBA, LArHECA, LArFCAL23A,
      MaxPartitions };

  /// Find signal maximum FADC slice
  double recTime(const std::vector<int>& vFAdc, int cut);
  /// Return subdetector partition
  int partition(int layer, double eta);
  /// Return subdetector partition name
  std::string partitionName(int part);

  /// TriggerTower Container key
  std::string m_TriggerTowerContainerName;
  /// Et cuts for hitmaps
  std::vector<unsigned int> m_TT_HitMap_ThreshVec;
  /// The number of back lumiblocks for separate LUT hitmaps online
  int m_TT_HitMap_LumiBlocks;
  /// ADC cut for hitmaps
  int m_TT_ADC_HitMap_Thresh;
  /// The maximum number of ADC slices
  int m_SliceNo;
  /// Maximum energy for LUT Et plots
  int m_MaxEnergyRange;
  /// Nominal pedestal value
  int m_TT_ADC_Pedestal;
  /// HAD FADC cut for signal
  int m_HADFADCCut;
  /// EM FADC cut for signal
  int m_EMFADCCut;
  /// Flag to test online code offline
  bool m_onlineTest;
  /// Histograms booked flag
  bool m_histBooked;

  /// Root directory
  std::string m_PathInRootFile;
  /// Root directory for error plots
  std::string m_ErrorPathInRootFile;
     
  /// Tool to retrieve bytestream errors
  ToolHandle<TrigT1CaloMonErrorTool>      m_errorTool;
  /// Histogram helper tool
  ToolHandle<TrigT1CaloLWHistogramTool>   m_histTool;
  /// TT simulation tool for Identifiers
  ToolHandle<LVL1::IL1TriggerTowerTool>   m_ttTool; 

  // ADC hitmaps
  TH2F_LW* m_h_ppm_em_2d_etaPhi_tt_adc_HitMap;                  ///< eta-phi Map of EM FADC > cut for triggered timeslice
  TH2F_LW* m_h_ppm_had_2d_etaPhi_tt_adc_HitMap;                 ///< eta-phi Map of HAD FADC > cut for triggered timeslice
  // ADC maximum timeslice
  TH1F_LW* m_h_ppm_had_1d_tt_adc_MaxTimeslice;                  ///< HAD Distribution of Maximum Timeslice
  TH1F_LW* m_h_ppm_em_1d_tt_adc_MaxTimeslice;                   ///< EM Distribution of Maximum Timeslice

  // ADC profile hitmaps
  TProfile2D_LW* m_h_ppm_em_2d_etaPhi_tt_adc_ProfileHitMap;     ///< eta-phi Profile Map of EM FADC > cut for triggered timeslice
  TProfile2D_LW* m_h_ppm_had_2d_etaPhi_tt_adc_ProfileHitMap;    ///< eta-phi Profile Map of HAD FADC > cut for triggered timeslice

  //timing HitMaps
  TProfile2D_LW* m_h_ppm_em_2d_etaPhi_tt_adc_MaxTimeslice;      ///< Average Maximum TimeSlice for EM Signal (TS:1-15)
  TProfile2D_LW* m_h_ppm_had_2d_etaPhi_tt_adc_MaxTimeslice;     ///< Average Maximum TimeSlice for HAD Signal (TS:1-15)
  
  std::vector<TProfile_LW*> m_v_ppm_1d_tt_adc_SignalProfile;    ///< Signal Shape Profiles for each partition

  //LUT Hitmaps per threshold
  std::vector<TH2F_LW*> m_v_ppm_em_2d_etaPhi_tt_lut_Threshold;  ///< eta-phi Map of EM LUT > each cut
  std::vector<TH2F_LW*> m_v_ppm_had_2d_etaPhi_tt_lut_Threshold; ///< eta-phi Map of HAD LUT > each cut
  TProfile2D_LW* m_h_ppm_em_2d_etaPhi_tt_lut_AverageEt;         ///< EM Average LUT Et for Et > 5
  TProfile2D_LW* m_h_ppm_had_2d_etaPhi_tt_lut_AverageEt;        ///< HAD Average LUT Et for Et > 5

  //distribution of LUT peak per detector region
  TH1F_LW* m_h_ppm_em_1d_tt_lut_Et;                  ///< EM LUT: Distribution of Peak
  TH1F_LW* m_h_ppm_em_1d_tt_lut_Eta;                 ///< EM LUT: Distribution of Peak per eta
  TH1F_LW* m_h_ppm_em_1d_tt_lut_Phi;                 ///< EM LUT: Distribution of Peak per phi

  TH1F_LW* m_h_ppm_had_1d_tt_lut_Et;                 ///< HAD LUT: Distribution of Peak
  TH1F_LW* m_h_ppm_had_1d_tt_lut_Eta;                ///< HAD LUT: Distribution of Peak per eta
  TH1F_LW* m_h_ppm_had_1d_tt_lut_Phi;                ///< HAD LUT: Distribution of Peak per phi

  TH1F_LW* m_h_ppm_1d_tt_lut_LutPerBCN;              ///< Num of LUT > 5 per BC
  TH2F_LW* m_h_ppm_2d_tt_lut_BcidBits;               ///< PPM: Bits of BCID Logic Word Vs. LUT
   
  // error
  TH1F_LW* m_h_ppm_1d_ErrorSummary;                  ///< Summary of SubStatus Errors
  TH2F_LW* m_h_ppm_2d_Status03;                      ///< Errors from TT SubStatus Word (crates 0-3)
  TH2F_LW* m_h_ppm_2d_Status47;                      ///< Errors from TT SubStatus Word (crates 4-7)
  TH2F_LW* m_h_ppm_2d_ErrorField03;                  ///< Errors from ASIC error field (crates 0-3)
  TH2F_LW* m_h_ppm_2d_ErrorField47;                  ///< Errors from ASIC error field (crates 4-7)
  std::vector<TH2F_LW*> m_v_ppm_2d_ASICErrorsDetail; ///< ASIC errors by crate and MCM
  TH2I_LW* m_h_ppm_2d_ErrorEventNumbers;             ///< SubStatus Error Event Numbers
  TH2I_LW* m_h_ppm_2d_ASICErrorEventNumbers;         ///< ASIC Error Field Event Numbers

  // number of triggered slice
  TH1F_LW* m_h_ppm_em_1d_tt_adc_TriggeredSlice;      ///< Number of the EM Triggered Slice
  TH1F_LW* m_h_ppm_had_1d_tt_adc_TriggeredSlice;     ///< Number of the HAD Triggered Slice
   
};

#endif
