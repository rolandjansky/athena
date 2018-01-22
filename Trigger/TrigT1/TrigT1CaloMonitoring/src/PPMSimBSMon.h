/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     PPMSimBSMon.h
// PACKAGE:  TrigT1CaloMonitoring
//
// AUTHOR:   Peter Faulkner
//           Sky French
//	     
//
// ********************************************************************
#ifndef PPMSIMBSMON_H
#define PPMSIMBSMON_H

#include <string>
#include <vector>

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

class TH2F_LW;
class TH2I_LW;

class L1CaloCondSvc;
class L1CaloRunParametersContainer;

// ============================================================================
namespace LVL1 {
// ============================================================================
// Forward declarations:
// ============================================================================
class TriggerTower;
class IL1TriggerTowerTool;
class ITrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramTool;



// ============================================================================

/** Cross-check of PPM LUT data with simulation.
 *
 *  Compares LUT from data with LUT simulated from FADC counts.
 *
 *  <b>ROOT Histogram Directories:</b>
 *
 *  <table>
 *  <tr><th> Directory                                                 </th><th> Contents                                               </th></tr>
 *  <tr><td> @c L1Calo/PPM/Errors/Data_Simulation/PPMLUTSim            </td><td> Eta-phi maps of data/simulation matches and mismatches </td></tr>
 *  <tr><td> @c L1Calo/PPM/Errors/Data_Simulation/MismatchEventNumbers </td><td> Event numbers of mismatches                            </td></tr>
 *  </table>
 *
 *  <b>Notes on Particular Histograms:</b>
 *
 *  <table>
 *  <tr><th> Histogram                                       </th><th> Comment                                                  </th></tr>
 *  <tr><td> @c L1Calo/PPM/Errors/Data_Simulation/PPMLUTSim/ <br>
 *           @c ppm_{em|had}_2d_etaPhi_tt_lut_SimNoData      </td><td> Will always be empty if there are less than 7 ADC slices </td></tr>
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
 *  <tr><td> @c std::vector<int>          <br>
 *           @c "L1CaloPPMMismatchVector" </td><td> Output.
 *                                                  Error summary bits for global histograms </td></tr>
 *  </table>
 *
 *  <b>Tools Used:</b>
 *
 *  <table>
 *  <tr><th> Tool                         </th><th> Description          </th></tr>
 *  <tr><td> @c LVL1::IL1TriggerTowerTool </td><td> @copydoc m_ttTool    </td></tr>
 *  <tr><td> @c TrigT1CaloMonErrorTool    </td><td> @copydoc m_errorTool </td></tr>
 *  <tr><td> @c TrigT1CaloLWHistogramTool </td><td> @copydoc m_histTool  </td></tr>
 *  </table>
 *
 *  <b>JobOption Properties:</b>
 *
 *  <table>
 *  <tr><th> Property                </th><th> Description                     </th></tr>
 *  <tr><td> @c TriggerTowerLocation </td><td> @copydoc m_triggerTowerLocation </td></tr>
 *  <tr><td> @c RootDirectory        </td><td> @copydoc m_rootDir              </td></tr>
 *  <tr><td> @c SimulationADCCut     </td><td> @copydoc m_simulationADCCut     </td></tr>
 *  </table>
 *
 *  <b>Related Documentation:</b>
 *
 *  <a href="http://hepwww.rl.ac.uk/Atlas-L1/Modules/PPr/PPMod_Wrup.pdf">
 *  The Pre-Processor Module (PPM) for the ATLAS Level-1 Calorimeter Trigger</a><br>
 *  <a href="http://hepwww.rl.ac.uk/Atlas-L1/Modules/ROD/ROD-spec-version1_2_2.pdf">
 *  ATLAS Level-1 Calorimeter Trigger - Read-out Driver</a>
 *
 *  @authors Peter Faulkner, Sky French
 *
 */

class PPMSimBSMon: public ManagedMonitorToolBase
{

public:
  
  PPMSimBSMon(const std::string & type, const std::string & name,
		       const IInterface* parent);
    

  virtual ~PPMSimBSMon();

  virtual StatusCode initialize();
  virtual StatusCode finalize();  
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

private:

  typedef std::vector<int> ErrorVector;

  StatusCode retrieveConditions();
  
  /// Fill error event number histogram
  void  fillEventSample(int crate, int module);

  /// Simulate LUT data from FADC data
  void simulateAndCompare(const xAOD::TriggerTowerContainer* ttIn);

  /// LUT simulation tool
  ToolHandle<LVL1::IL1TriggerTowerTool> m_ttTool;
  /// Corrupt event veto tool
  ToolHandle<ITrigT1CaloMonErrorTool>    m_errorTool;
  /// Histogram helper tool
  ToolHandle<TrigT1CaloLWHistogramTool> m_histTool;

  /// L1Calo conditions                                                                               
  ServiceHandle<L1CaloCondSvc> m_l1CondSvc;

  /// Database container
  L1CaloRunParametersContainer* m_runParametersContainer;

  /// Root directory name
  std::string m_rootDir;

  /// Trigger Tower container StoreGate key
  std::string m_triggerTowerLocation;

  /// Cut on ADC digits for re-simulation
  int m_simulationADCCut;
  /// Histograms booked flag
  bool m_histBooked;

  //=======================
  //   Match/Mismatch plots
  //=======================

  // LUT-CP
  TH2F_LW* m_h_ppm_em_2d_etaPhi_tt_lutCp_SimEqData;  ///< PPM LUT EM Data/Simulation Non-zero Matches
  TH2F_LW* m_h_ppm_em_2d_etaPhi_tt_lutCp_SimNeData;  ///< PPM LUT EM Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_ppm_em_2d_etaPhi_tt_lutCp_SimNoData;  ///< PPM LUT EM Simulation but no Data
  TH2F_LW* m_h_ppm_em_2d_etaPhi_tt_lutCp_DataNoSim;  ///< PPM LUT EM Data but no Simulation
  TH2F_LW* m_h_ppm_had_2d_etaPhi_tt_lutCp_SimEqData; ///< PPM LUT HAD Data/Simulation Non-zero Matches
  TH2F_LW* m_h_ppm_had_2d_etaPhi_tt_lutCp_SimNeData; ///< PPM LUT HAD Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_ppm_had_2d_etaPhi_tt_lutCp_SimNoData; ///< PPM LUT HAD Simulation but no Data
  TH2F_LW* m_h_ppm_had_2d_etaPhi_tt_lutCp_DataNoSim; ///< PPM LUT HAD Data but no Simulation
  
  // LUT-JEP
  TH2F_LW* m_h_ppm_em_2d_etaPhi_tt_lutJep_SimEqData;  ///< PPM LUT EM Data/Simulation Non-zero Matches
  TH2F_LW* m_h_ppm_em_2d_etaPhi_tt_lutJep_SimNeData;  ///< PPM LUT EM Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_ppm_em_2d_etaPhi_tt_lutJep_SimNoData;  ///< PPM LUT EM Simulation but no Data
  TH2F_LW* m_h_ppm_em_2d_etaPhi_tt_lutJep_DataNoSim;  ///< PPM LUT EM Data but no Simulation
  TH2F_LW* m_h_ppm_had_2d_etaPhi_tt_lutJep_SimEqData; ///< PPM LUT HAD Data/Simulation Non-zero Matches
  TH2F_LW* m_h_ppm_had_2d_etaPhi_tt_lutJep_SimNeData; ///< PPM LUT HAD Data/Simulation Non-zero Mismatches
  TH2F_LW* m_h_ppm_had_2d_etaPhi_tt_lutJep_SimNoData; ///< PPM LUT HAD Simulation but no Data
  TH2F_LW* m_h_ppm_had_2d_etaPhi_tt_lutJep_DataNoSim; ///< PPM LUT HAD Data but no Simulation

  // BCID
  TH2F_LW* m_h_ppm_2d_etaPhi_tt_peakf_NonZeroMatches;   ///< PPM PEAKFINDER BCID Data/Simulation Non-zero Matches
  TH2F_LW* m_h_ppm_2d_etaPhi_tt_peakf_ZeroMatches;      ///< PPM PEAKFINDER BCID Data/Simulation Zero Matches
  TH2F_LW* m_h_ppm_2d_etaPhi_tt_peakf_SimNoData;        ///< PPM PEAKFINDER BCID Data/Simulation Simulation but no Data
  TH2F_LW* m_h_ppm_2d_etaPhi_tt_peakf_DataNoSim;        ///< PPM PEAKFINDER BCID Data/Simulation Data but no Simulation
  TH2F_LW* m_h_ppm_2d_etaPhi_tt_satBcid_NonZeroMatches; ///< PPM SATURATED BCID Data/Simulation Non-zero Matches
  TH2F_LW* m_h_ppm_2d_etaPhi_tt_satBcid_ZeroMatches;    ///< PPM SATURATED BCID Data/Simulation Zero Matches
  TH2F_LW* m_h_ppm_2d_etaPhi_tt_satBcid_SimNoData;      ///< PPM SATURATED BCID Data/Simulation Simulation but no Data
  TH2F_LW* m_h_ppm_2d_etaPhi_tt_satBcid_DataNoSim;      ///< PPM SATURATED BCID Data/Simulation Data but no Simulation
  
  // Mismatch Event Number Histograms
  TH2I_LW* m_h_ppm_2d_LUT_MismatchEvents_cr0cr1;   ///< PPM LUT Mismatch Event Numbers Crates 0 and 1
  TH2I_LW* m_h_ppm_2d_LUT_MismatchEvents_cr2cr3;   ///< PPM LUT Mismatch Event Numbers Crates 2 and 3
  TH2I_LW* m_h_ppm_2d_LUT_MismatchEvents_cr4cr5;   ///< PPM LUT Mismatch Event Numbers Crates 4 and 5
  TH2I_LW* m_h_ppm_2d_LUT_MismatchEvents_cr6cr7;   ///< PPM LUT Mismatch Event Numbers Crates 6 and 7
  
  // Mismatch Event Number Histograms for LUT-JEP
  //TH2I_LW* m_h_ppm_2d_LUTJEP_MismatchEvents_cr0cr1;   ///< PPM LUT-JEP Mismatch Event Numbers Crates 0 and 1
  //TH2I_LW* m_h_ppm_2d_LUTJEP_MismatchEvents_cr2cr3;   ///< PPM LUT-JEP Mismatch Event Numbers Crates 2 and 3
  //TH2I_LW* m_h_ppm_2d_LUTJEP_MismatchEvents_cr4cr5;   ///< PPM LUT-JEP Mismatch Event Numbers Crates 4 and 5
  //TH2I_LW* m_h_ppm_2d_LUTJEP_MismatchEvents_cr6cr7;   ///< PPM LUT-JEP Mismatch Event Numbers Crates 6 and 7
  
};

// ============================================================================
}  // end namespace
// ============================================================================

#endif
