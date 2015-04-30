/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigT1CaloGlobalMonTool.h
// PACKAGE:  TrigT1CaloMonitoring
//
// AUTHOR:   Peter Faulkner
//	     
//
// ********************************************************************
#ifndef TRIGT1CALOGLOBALMONTOOL_H
#define TRIGT1CALOGLOBALMONTOOL_H

#include <string>
#include <vector>

#include "GaudiKernel/ToolHandle.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

class TH1F;
class TH2F;
class StatusCode;


// ============================================================================
namespace LVL1 {
// ============================================================================
class ITrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramToolV1;
// ============================================================================
/** Summary error plots across all L1Calo sub-detectors.
 *
 *  Error info is passed from other monitoring tools via StoreGate.
 *
 *  <b>ROOT Histogram Directories:</b>
 *
 *  <table>
 *  <tr><th> Directory                 </th><th> Contents                             </th></tr>
 *  <tr><td> @c L1Calo/Overview        </td><td> Number of processed events           </td></tr>
 *  <tr><td> @c L1Calo/Overview/Errors </td><td> Global error overview                <br>
 *                                               Global error overview last few
 *                                                           lumiblocks (online only) <br>
 *                                               Errors by lumiblock and time         <br>
 *                                               Rejected events by lumiblock         </td></tr>
 *  </table>
 *
 *  <b>Notes on Particular Histograms:</b>
 *
 *  <table>
 *  <tr><th> Histogram                      </th><th> Comment                               </th></tr>
 *  <tr><td> @c L1Calo/Overview/            <br>
 *           @c l1calo_1d_NumberOfEvents    </td><td> Also includes number of rejected events
 *                                                                   if rejection turned on </td></tr>
 *  <tr><td> @c L1Calo/Overview/Errors/     <br>
 *           @c l1calo_2d_GlobalOverview    </td><td> On Tier0 produced for every lumiblock </td></tr>
 *  <tr><td> @c l1calo_1d_ErrorsByLumiblock </td><td> On Tier0 only produced if non-empty   </td></tr>
 *  <tr><td> @c l1calo_1d_ErrorsByTime      </td><td> On Tier0 only produced if non-empty   <br>
 *                                                    Number of errors by time on Tier0     <br>
 *                                                    Time of errors by lumiblock online    </td></tr>
 *  <tr><td> @c l1calo_1d_RejectedEvents    </td><td> Only produced if rejection turned on  <br>
 *                                                    On Tier0 only produced if non-empty   </td></tr>
 *  </table>
 *
 *  <b>More Information on the Global Error Overview Plot:</b>
 *
 *  <table>
 *  <tr><th> Bin
 *    </th><th> Description
 *      </th><th> More Detailed Plots In </th></tr> 
 *  <tr><td> PPMDataStatus
 *    </td><td> Errors in first five bits of PPM ASIC error field <br>
 *              (MCM channels disabled/absent)
 *      </td><td> @c PPM/Errors </td></tr>
 *  <tr><td> PPMDataError
 *    </td><td> Errors in remaining bits of PPM ASIC error field
 *      </td><td> @c PPM/Errors </td></tr>
 *  <tr><td> SubStatus
 *    </td><td> Errors from sub-status word error field <br>
 *              (PPM/CPM/JEM)
 *      </td><td> @c PPM/Errors          <br>
 *                @c CPM/Errors/Hardware <br>
 *                @c JEM/Errors/Hardware </td></tr>
 *  <tr><td> Parity
 *    </td><td> Parity errors from CPM tower data or JEM jet element data
 *      </td><td> @c CPM/Errors/Hardware <br>
 *                @c JEM/Errors/Hardware </td></tr>
 *  <tr><td> LinkDown
 *    </td><td> Link down errors from CPM tower data or JEM jet element data
 *      </td><td> @c CPM/Errors/Hardware <br> 
 *                @c JEM/Errors/Hardware </td></tr>
 *  <tr><td> RoIParity
 *    </td><td> Parity errors from RoI words
 *      </td><td> @c CPM/Errors/Hardware <br> 
 *                @c JEM/Errors/Hardware </td></tr>
 *  <tr><td> Transmission
 *    </td><td> Data mismatches   <br>
 *              (PPM->CPM towers) <br>
 *              Not checked if there are ROB Status or unpacking errors
 *      </td><td> @c CPM/Errors/Transmission_Simulation </td></tr>
 *  <tr><td> Simulation
 *    </td><td> Simulation/data mismatches                                 <br>
 *              (PPM LUT/CPM RoI, Hits/JEM JetElements, RoI, Hits, EtSums) <br>
 *              CPM/JEM data not checked if there are ROB Status or unpacking errors
 *      </td><td> @c PPM/Errors/Data_Simulation         <br>
 *                @c CPM/Errors/Transmission_Simulation <br>
 *                @c JEM/Errors/Transmission_Simulation </td></tr>
 *  <tr><td> CMMSubStatus
 *    </td><td> Errors from sub-status word error field <br>
 *              (CMMs)
 *      </td><td> @c CPM_CMM/Errors/Hardware <br>
 *                @c JEM_CMM/Errors/Hardware </td></tr>
 *  <tr><td> CMMParity
 *    </td><td> Cable or backplane parity errors from CMMs
 *      </td><td> @c CPM_CMM/Errors/Hardware <br>
 *                @c JEM_CMM/Errors/Hardware </td></tr>
 *  <tr><td> CMMTransmission
 *    </td><td> Data mismatches                             <br>
 *              (CPM->CMM Hits, JEM->CMM Hits, JEM->CMM EtSums,
 *               Local->Remote Sums, CMM->RoIs)             <br>
 *              Not checked if there are ROB Status or unpacking errors
 *      </td><td> @c CPM_CMM/Errors/Transmission_Simulation <br>
 *                @c JEM_CMM/Errors/Transmission_Simulation </td></tr>
 *  <tr><td> CMMSimulation
 *    </td><td> Simulation/data mismatches                  <br>
 *              (Local Sums, Total Sums, Et Maps)           <br>
 *              Not checked if there are ROB Status or unpacking errors
 *      </td><td> @c CPM_CMM/Errors/Transmission_Simulation <br>
 *                @c JEM_CMM/Errors/Transmission_Simulation </td></tr>
 *  <tr><td> RODStatus
 *    </td><td> Errors from ROD Status element block
 *      </td><td> @c ROD </td></tr>
 *  <tr><td> RODMissing
 *    </td><td> No ROD fragment or ROD fragment empty <br>
 *              Not checked if there are ROB Status errors in the same crate/slink
 *      </td><td> @c ROD </td></tr>
 *  <tr><td> ROBStatus
 *    </td><td> Errors from ROB Status element block
 *      </td><td> @c ROD/ROBStatus </td></tr>
 *  <tr><td> Unpacking
 *    </td><td> Errors in bytestream unpacking
 *      </td><td> @c ROD/Unpacking </td></tr>
 *  </table>
 *
 *  <b>Custom Merges Used (Tier0):</b>
 *
 *  <table>
 *  <tr><th> Merge                                      </th><th> Used For           </th></tr>
 *  <tr><td> @ref MergesUsedsection "@c mergeRebinned " </td><td> By lumiblock plots </td></tr>
 *  </table>
 *
 *  <b>StoreGate Containers Used:</b>
 *
 *  <table>
 *  <tr><th> Container                      </th><th> Comment                             </th></tr>
 *  <tr><td> @c EventInfo                   </td><td> For lumiblock number                </td></tr>
 *  <tr><td> @c std::vector<int>            <br>
 *           @c "L1CaloPPMErrorVector"      </td><td> Error summary bits from PPrMon      </td></tr>
 *  <tr><td> @c std::vector<int>            <br>
 *           @c "L1CaloPPMSpareErrorVector" </td><td> Error summary bits from PPrSpareMon </td></tr>
 *  <tr><td> @c std::vector<int>            <br>
 *           @c "L1CaloCPMErrorVector"      </td><td> Error summary bits from TrigT1CaloCpmMonTool
 *                                                                                        </td></tr>
 *  <tr><td> @c std::vector<int>            <br>
 *           @c "L1CaloJEMErrorVector"      </td><td> Error summary bits from JEMMon      </td></tr>
 *  <tr><td> @c std::vector<int>            <br>
 *           @c "L1CaloJEMCMMErrorVector"   </td><td> Error summary bits from CMMMon      </td></tr>
 *  <tr><td> @c std::vector<int>            <br>
 *           @c "L1CaloRODErrorVector"      </td><td> Error summary bits from TrigT1CaloRodMonTool
 *                                                                                        </td></tr>
 *  <tr><td> @c std::vector<int>            <br>
 *           @c "L1CaloPPMMismatchVector"   </td><td> Error summary bits from PPMSimBSMon </td></tr>
 *  <tr><td> @c std::vector<int>            <br>
 *           @c "L1CaloCPMMismatchVector"   </td><td> Error summary bits from CPMSimBSMon </td></tr>
 *  <tr><td> @c std::vector<int>            <br>
 *           @c "L1CaloJEMMismatchVector"   </td><td> Error summary bits from JEPSimBSMon </td></tr>
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
 *  <tr><th> Property            </th><th> Description           </th></tr>
 *  <tr><td> @c RootDirectory    </td><td> @copydoc m_rootDir    </td></tr>
 *  <tr><td> @c RecentLumiBlocks </td><td> @copydoc m_recentLumi </td></tr>
 *  <tr><td> @c OnlineTest       </td><td> @copydoc m_onlineTest </td></tr>
 *  </table>
 *
 *  <!--
 *  <b>Related Documentation:</b>
 *  -->
 *
 *  @author Peter Faulkner
 *
 */

class TrigT1CaloGlobalMonTool: public ManagedMonitorToolBase
{

public:
  
  TrigT1CaloGlobalMonTool(const std::string & type, const std::string & name,
  		          const IInterface* parent);
    

  virtual ~TrigT1CaloGlobalMonTool();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
    
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

private:

  // Enums for global summary plot

  // Hardware errors
  /// Error bits in PPM error data
  enum PPMErrors { DataStatus, DataError, PPMSubStatus };
  /// Error bits in CPM error data
  enum CPMErrors { CPMEMParity, CPMEMLink, CPMHadParity, CPMHadLink, CPMStatus,
                   CPMRoIParity, CMMCPParity, CMMCPStatus };
  /// Error bits in JEM error data
  enum JEMErrors { JEMEMParity, JEMHadParity, JEMEMLink, JEMHadLink, JEMStatus,
                   JEMRoIParity };
  /// Error bits in JEM CMM error data
  enum CMMErrors { JEMCMMJetStatus, JEMCMMEnergyStatus, JEMCMMJetParity,
                   JEMCMMEnergyParity, JEMCMMRoIParity};
  /// Error bits in ROD error data
  enum RODErrors { GLink, /*CMMParity,*/ LVDSLink, FIFOOverflow, DataTransport,
                   Timeout, BCNMismatch, TriggerType, NoPayload, NoFragment,
	           ROBStatusError, UnpackingError };

  // Transmission/Comparison with simulation errors
  /// Error bits in PPM simulation error data
  enum PPMMismatch { LUTMismatch };
  /// Error bits in CPM simulation error data
  enum CPMMismatch { EMTowerMismatch, HadTowerMismatch, CPMRoIMismatch,
                     CPMHitsMismatch, CMMHitsMismatch, LocalSumMismatch,
		     RemoteSumMismatch, TotalSumMismatch };
  /// Error bits in JEM simulation error data
  enum JEMMismatch { EMElementMismatch, HadElementMismatch, JEMRoIMismatch,
                     JEMHitsMismatch, CMMJetHitsMismatch, LocalJetMismatch,
		     RemoteJetMismatch, TotalJetMismatch, JetEtMismatch,
		     JetEtRoIMismatch, JEMEtSumsMismatch, CMMEtSumsMismatch,
		     LocalEnergyMismatch, RemoteEnergyMismatch,
		     TotalEnergyMismatch, SumEtMismatch, MissingEtMismatch,
		     MissingEtSigMismatch, EnergyRoIMismatch };

  /// Bins for global error plots
  enum GlobalErrors { PPMDataStatus, PPMDataError, SubStatus, Parity, LinkDown,
                      RoIParity, Transmission, Simulation, CMMSubStatus,
		      GbCMMParity, CMMTransmission, CMMSimulation,
		      RODStatus, RODMissing, ROBStatus, Unpacking,
		      NumberOfGlobalErrors };

  typedef std::vector<int> ErrorVector;

  /// Book and label global overview plot
  TH2F* bookOverview(const std::string& name, const std::string& title);

  /// Tool to retrieve bytestream errors
  ToolHandle<ITrigT1CaloMonErrorTool>    m_errorTool;
  /// Histogram helper tool
  ToolHandle<TrigT1CaloLWHistogramToolV1> m_histTool;

  /// Root directory
  std::string m_rootDir;

  /// Number of lumiblocks in recent lumiblocks plot
  int m_recentLumi;
  /// Flag to test online code when running offline
  bool m_onlineTest;
  /// The current lumiblock number
  unsigned int m_lumiNo;
  /// Position of current lumiblock in vector of recent lumiblock plots
  int m_lumipos;
  /// Lumiblock numbers corresponding to recent lumiblock plots
  std::vector<unsigned int> m_luminumbers;

  //========================
  //   Global Overview plots
  //========================

  TH2F* m_h_l1calo_2d_GlobalOverview;       ///< L1Calo Global Error Overview
  TH2F* m_h_l1calo_2d_CurrentEventOverview; ///< L1Calo Current Event Error Overview (temporary)
  TH2F* m_h_l1calo_2d_GlobalOverviewRecent; ///< L1Calo Global Error Overview Last m_recentLumi Lumiblocks
  TH1F* m_h_l1calo_1d_ErrorsByLumiblock;    ///< Events with Errors by Lumiblock
  TH1F* m_h_l1calo_1d_ErrorsByTime;         ///< Time of First Event in Lumiblock with Error/Events with Errors by Time
  TH1F* m_h_l1calo_1d_RejectedEvents;       ///< Rejected Events by Lumiblock
  TH1F* m_h_l1calo_1d_NumberOfEvents;       ///< Number of processed events
  /// L1Calo Global Error Overview Block 0-m_recentLumi (temporary)
  std::vector<TH2F*> m_v_l1calo_2d_GlobalOverviewBlock;

};
// ============================================================================
}  // end namespace
// ============================================================================
#endif
