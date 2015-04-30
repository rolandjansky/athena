/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     PPrSpareMon.h
// PACKAGE:  TrigT1CaloMonitoring
//
// AUTHOR:   Peter Faulkner
//	     
//
// ********************************************************************
#ifndef PPRSPAREMON_H
#define PPRSPAREMON_H

#include <string>
#include <vector>

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "GaudiKernel/ToolHandle.h"

class TH1F_LW;
class TH2F_LW;
class TH2I_LW;
class TProfile2D_LW;
class StatusCode;

// ============================================================================
namespace LVL1 {
// ============================================================================
// Forward declarations:
// ============================================================================
class ITrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramTool;
// ============================================================================
/** Monitoring of Preprocessor spare channels
 *
 *  <b>ROOT Histogram Directories:</b>
 *
 *  <table>
 *  <tr><th> Directory                                 </th><th> Contents                             </th></tr>
 *  <tr><td> @c L1Calo/PPM/SpareChannels/ADC           </td><td> Triggered timeslice                  <br>
 *                                                               ADC hitmap for ADC counts > ADCHitMap_Thresh <br>
 *                                                               Average ADC for triggered timeslice  </td></tr>
 *  <tr><td> @c L1Calo/PPM/SpareChannels/Errors        </td><td> SubStatus word errors                <br>
 *                                                               ASIC error field errors              </td></tr>
 *  <tr><td> @c L1Calo/PPM/SpareChannels/Errors/Detail </td><td> ASIC error field errors by MCM       </td></tr>
 *  </table>
 *
 *  <!--
 *  <b>Notes on Particular Histograms:</b>
 *
 *  <table>
 *  <tr><th> Histogram                     </th><th> Comment                     </th></tr>
 *  <tr><td>                               </td><td>                             </td></tr>
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
 *  <tr><th> Container                      </th><th> Comment                                  </th></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::TriggerTower>        </td><td> PPM data spare channels only             </td></tr>
 *  <tr><td> @c std::vector<int>            <br>
 *           @c "L1CaloPPMSpareErrorVector" </td><td> Output.
 *                                                    Error summary bits for global histograms <br>
 *                                                    (currently disabled)                     </td></tr>
 *  </table>
 *
 *  <b>Tools Used:</b>
 *
 *  <table>
 *  <tr><th> Tool                         </th><th> Description          </th></tr>
 *  <tr><td> @c TrigT1CaloMonErrorTool    </td><td> @copydoc m_errorTool </td></tr>
 *  <tr><td> @c TrigT1CaloLWHistogramTool </td><td> @copydoc m_histTool  </td></tr>
 *  </table>
 *
 *  <b>JobOption Properties:</b>
 *
 *  <table>
 *  <tr><th> Property                    </th><th> Description                          </th></tr>
 *  <tr><td> @c BS_TriggerTowerContainer </td><td> @copydoc m_TriggerTowerContainerName </td></tr>
 *  <tr><td> @c BS_xAODTriggerTowerContainer </td><td> @copydoc m_xAODTriggerTowerContainerName </td></tr>
 *  <tr><td> @c ADCHitMap_Thresh         </td><td> @copydoc m_TT_ADC_HitMap_Thresh      </td></tr>
 *  <tr><td> @c PathInRootFile           </td><td> @copydoc m_PathInRootFile            </td></tr>
 *  <tr><td> @c ErrorPathInRootFile      </td><td> @copydoc m_ErrorPathInRootFile       </td></tr>
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

class PPrSpareMon: public ManagedMonitorToolBase
{

 public:
  
  PPrSpareMon(const std::string & type, const std::string & name,
 		                        const IInterface* parent);
    
  virtual ~PPrSpareMon();

  virtual StatusCode initialize();
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

private:

  /// TriggerTower Container key
  std::string m_TriggerTowerContainerName;
  std::string m_xAODTriggerTowerContainerName;
  /// ADC cut for hitmaps
  int m_TT_ADC_HitMap_Thresh;
  /// The maximum number of ADC slices
  int m_SliceNo;
  /// Histograms booked flag
  bool m_histBooked;

  /// Root directory
  std::string m_PathInRootFile;
  /// Root directory for error plots
  std::string m_ErrorPathInRootFile;
     
  /// Tool to retrieve bytestream errors
  ToolHandle<ITrigT1CaloMonErrorTool>    m_errorTool;
  /// Histogram helper tool
  ToolHandle<TrigT1CaloLWHistogramTool> m_histTool;

  //ADC Hitmaps for triggered TimeSlice
  TH2F_LW* m_h_ppmspare_2d_tt_adc_HitMap;           ///< Spare Channels Hit Map of FADC > cut for Triggered Timeslice
  TProfile2D_LW* m_h_ppmspare_2d_tt_adc_ProfileMap; ///< Spare Channels Profile Map of FADC for Triggered Timeslice

  // Errors
  TH1F_LW* m_h_ppmspare_1d_ErrorSummary;            ///< Spare Channels Summary of SubStatus Errors
  TH2F_LW* m_h_ppmspare_2d_Status25;                ///< Spare Channels: Errors from TT SubStatus Word (crates 2-5)
  TH2F_LW* m_h_ppmspare_2d_ErrorField25;            ///< Spare Channels: Errors from ASIC error field (crates 2-5)
  std::vector<TH2F_LW*> m_v_ppmspare_2d_ASICErrorsDetail; ///< Spare Channels: ASIC errors by crate and MCM
  TH2I_LW* m_h_ppmspare_2d_ErrorEventNumbers;       ///< Spare Channels SubStatus Error Event Numbers
  TH2I_LW* m_h_ppmspare_2d_ASICErrorEventNumbers;   ///< Spare Channels ASIC Error Field Event Numbers

  // Triggered slice
  TH1F_LW* m_h_ppmspare_1d_tt_adc_TriggeredSlice;   ///< Spare Channels Number of the Triggered Slice
   
};
// ============================================================================
}  // end namespace
// ============================================================================
#endif
