/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     OverviewMon.h
// PACKAGE:  TrigT1CaloMonitoring
//
// AUTHOR:   Peter Faulkner
//
//
// ********************************************************************
#ifndef TRIGT1CALOMONITORING_OVERVIEWMON_H
#define TRIGT1CALOMONITORING_OVERVIEWMON_H

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
// Forward declarations:
// ============================================================================
class ITrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramTool;
// ============================================================================
/** Summary error plots across all L1Calo sub-detectors.
 *
 *  Error info is passed from other monitoring tools via StoreGate.
 *
 *  <b>ROOT Histogram Directories:</b>
 *
 *  <table>
 *  <tr><th> Directory                 </th><th> Contents </th></tr>
 *  <tr><td> @c L1Calo/Overview        </td><td> Number of processed events
 *</td></tr>
 *  <tr><td> @c L1Calo/Overview/Errors </td><td> Global error overview <br>
 *                                               Global error overview last few
 *                                                           lumiblocks (online
 *only) <br>
 *                                               Errors by lumiblock and time
 *<br>
 *                                               Rejected events by lumiblock
 *</td></tr>
 *  </table>
 *
 *  <b>Notes on Particular Histograms:</b>
 *
 *  <table>
 *  <tr><th> Histogram                      </th><th> Comment </th></tr>
 *  <tr><td> @c L1Calo/Overview/            <br>
 *           @c l1calo_1d_NumberOfEvents    </td><td> Also includes number of
 *rejected events
 *                                                                   if
 *rejection turned on </td></tr>
 *  <tr><td> @c L1Calo/Overview/Errors/     <br>
 *           @c l1calo_2d_GlobalOverview    </td><td> On Tier0 produced for
 *every lumiblock </td></tr>
 *  <tr><td> @c l1calo_1d_ErrorsByLumiblock </td><td> On Tier0 only produced if
 *non-empty   </td></tr>
 *  <tr><td> @c l1calo_1d_ErrorsByTime      </td><td> On Tier0 only produced if
 *non-empty   <br>
 *                                                    Number of errors by time
 *on Tier0     <br>
 *                                                    Time of errors by
 *lumiblock online    </td></tr>
 *  <tr><td> @c l1calo_1d_RejectedEvents    </td><td> Only produced if rejection
 *turned on  <br>
 *                                                    On Tier0 only produced if
 *non-empty   </td></tr>
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
 *  <tr><td> Transmission
 *    </td><td> Data mismatches   <br>
 *              (PPM->CPM towers) <br>
 *              Not checked if there are ROB Status or unpacking errors
 *      </td><td> @c CPM/Errors/Transmission_Simulation </td></tr>
 *  <tr><td> Simulation
 *    </td><td> Simulation/data mismatches                     <br>
 *              (PPM LUT/CPM RoI/JEM JetElements, RoI, EtSums) <br>
 *              CPM/JEM data not checked if there are ROB Status or unpacking
 *errors
 *      </td><td> @c PPM/Errors/Data_Simulation         <br>
 *                @c CPM/Errors/Transmission_Simulation <br>
 *                @c JEM/Errors/Transmission_Simulation </td></tr>
 *  <tr><td> CMXSubStatus
 *    </td><td> Errors from sub-status word error field <br>
 *              (CMXs)
 *      </td><td> @c CPM_CMX/Errors/Hardware <br>
 *                @c JEM_CMX/Errors/Hardware </td></tr>
 *  <tr><td> CMXParity
 *    </td><td> Cable or backplane parity errors from CMXs
 *      </td><td> @c CPM_CMX/Errors/Hardware <br>
 *                @c JEM_CMX/Errors/Hardware </td></tr>
 *  <tr><td> CMXTransmission
 *    </td><td> Data mismatches                                  <br>
 *              (JEM->CMX EtSums, Local->Remote Sums, CMX->RoIs) <br>
 *              Not checked if there are ROB Status or unpacking errors
 *      </td><td> @c CPM_CMX/Errors/Transmission_Simulation      <br>
 *                @c JEM_CMX/Errors/Transmission_Simulation      </td></tr>
 *  <tr><td> CMXSimulation
 *    </td><td> Simulation/data mismatches                       <br>
 *              (RoI->CMX TOBs, Local Sums, Total Sums, Et Maps, Topo Info) <br>
 *              Not checked if there are ROB Status or unpacking errors
 *      </td><td> @c CPM_CMX/Errors/Transmission_Simulation <br>
 *                @c JEM_CMX/Errors/Transmission_Simulation </td></tr>
 *  <tr><td> RODStatus
 *    </td><td> Errors from ROD Status element block
 *      </td><td> @c ROD </td></tr>
 *  <tr><td> RODMissing
 *    </td><td> No ROD fragment or ROD fragment empty <br>
 *              Not checked if there are ROB Status errors in the same
 *crate/slink
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
 *  <tr><th> Merge                                      </th><th> Used For
 *</th></tr>
 *  <tr><td> @ref MergesUsedsection "@c mergeRebinned " </td><td> By lumiblock
 *plots </td></tr>
 *  </table>
 *
 *  <b>StoreGate Containers Used:</b>
 *
 *  <table>
 *  <tr><th> Container                      </th><th> Comment </th></tr>
 *  <tr><td> @c EventInfo                   </td><td> For lumiblock number
 *</td></tr>
 *  <tr><td> @c std::vector<int>            <br>
 *           @c "L1CaloPPMErrorVector"      </td><td> Error summary bits from
 *PPMon      </td></tr>
 *  <tr><td> @c std::vector<int>            <br>
 *           @c "L1CaloPPMSpareErrorVector" </td><td> Error summary bits from
 *PPSpareMon </td></tr>
 *  <tr><td> @c std::vector<int>            <br>
 *           @c "L1CaloCPMErrorVector"      </td><td> Error summary bits from
 *CPMon      </td></tr>
 *  <tr><td> @c std::vector<int>            <br>
 *           @c "L1CaloJEMErrorVector"      </td><td> Error summary bits from
 *JEPJEMMon  </td></tr>
 *  <tr><td> @c std::vector<int>            <br>
 *           @c "L1CaloJEMCMXErrorVector"   </td><td> Error summary bits from
 *JEPCMXMon  </td></tr>
 *  <tr><td> @c std::vector<int>            <br>
 *           @c "L1CaloRODErrorVector"      </td><td> Error summary bits from
 *RODMon     </td></tr>
 *  <tr><td> @c std::vector<int>            <br>
 *           @c "L1CaloPPMMismatchVector"   </td><td> Error summary bits from
 *PPSimMon   </td></tr>
 *  <tr><td> @c std::vector<int>            <br>
 *           @c "L1CaloCPMMismatchVector"   </td><td> Error summary bits from
 *CPSimMon   </td></tr>
 *  <tr><td> @c std::vector<int>            <br>
 *           @c "L1CaloJEMMismatchVector"   </td><td> Error summary bits from
 *JEPSimMon  </td></tr>
 *  </table>
 *
 *  <b>Tools Used:</b>
 *
 *  <table>
 *  <tr><th> Tool                               </th><th> Description </th></tr>
 *  <tr><td> @c LVL1::ITrigT1CaloMonErrorTool   </td><td> @copydoc m_errorTool
 *</td></tr>
 *  <tr><td> @c LVL1::TrigT1CaloLWHistogramTool </td><td> @copydoc m_histTool
 *</td></tr>
 *  </table>
 *
 *  <b>JobOption Properties:</b>
 *
 *  <table>
 *  <tr><th> Property                 </th><th> Description </th></tr>
 *  <tr><td> @c ErrorTool             </td><td> @copydoc m_errorTool </td></tr>
 *  <tr><td> @c HistogramTool         </td><td> @copydoc m_histTool </td></tr>
 *  <tr><td> @c PPMErrorLocation      </td><td> @copydoc m_ppmErrorLocation
 *</td></tr>
 *  <tr><td> @c PPMSpareErrorLocation </td><td> @copydoc m_ppmSpareErrorLocation
 *</td></tr>
 *  <tr><td> @c CPMErrorLocation      </td><td> @copydoc m_cpmErrorLocation
 *</td></tr>
 *  <tr><td> @c JEMErrorLocation      </td><td> @copydoc m_jemErrorLocation
 *</td></tr>
 *  <tr><td> @c JEMCMXErrorLocation   </td><td> @copydoc m_jemCmxErrorLocation
 *</td></tr>
 *  <tr><td> @c RODErrorLocation      </td><td> @copydoc m_rodErrorLocation
 *</td></tr>
 *  <tr><td> @c PPMMismatchLocation   </td><td> @copydoc m_ppmMismatchLocation
 *</td></tr>
 *  <tr><td> @c CPMMismatchLocation   </td><td> @copydoc m_cpmMismatchLocation
 *</td></tr>
 *  <tr><td> @c JEMMismatchLocation   </td><td> @copydoc m_jemMismatchLocation
 *</td></tr>
 *  <tr><td> @c RootDirectory         </td><td> @copydoc m_rootDir </td></tr>
 *  <tr><td> @c RecentLumiBlocks      </td><td> @copydoc m_recentLumi </td></tr>
 *  <tr><td> @c OnlineTest            </td><td> @copydoc m_onlineTest </td></tr>
 *  </table>
 *
 *  <!--
 *  <b>Related Documentation:</b>
 *  -->
 *
 *  @author Peter Faulkner
 *
 */

class OverviewMon : public ManagedMonitorToolBase {
 public:
  OverviewMon(const std::string& type, const std::string& name,
              const IInterface* parent);

  virtual ~OverviewMon();

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
  enum CPMErrors {
    CPMEMParity,
    CPMEMLink,
    CPMHadParity,
    CPMHadLink,
    CPMStatus,
    CMXCPTobParity,
    CMXCPSumParity,
    CMXCPStatus
  };
  /// Error bits in JEM error data
  enum JEMErrors {
    JEMEMParity,
    JEMHadParity,
    JEMEMLink,
    JEMHadLink,
    JEMStatus
  };
  /// Error bits in JEM CMX error data
  enum CMXErrors {
    JEMCMXJetStatus,
    JEMCMXEnergyStatus,
    JEMCMXJetParity,
    JEMCMXEnergyParity
  };
  /// Error bits in ROD error data
  enum RODErrors {
    GLink,
    /*CMMParity,*/ LVDSLink,
    FIFOOverflow,
    DataTransport,
    Timeout,
    BCNMismatch,
    TriggerType,
    NoPayload,
    NoFragment,
    ROBStatusError,
    UnpackingError
  };

  // Transmission/Comparison with simulation errors
  /// Error bits in PPM simulation error data
  enum PPMMismatch { LUTMismatch };
  /// Error bits in CPM simulation error data
  enum CPMMismatch {
    EMTowerMismatch,
    HadTowerMismatch,
    EMRoIMismatch,
    TauRoIMismatch,
    LeftCMXTobMismatch,
    RightCMXTobMismatch,
    LocalSumMismatch,
    RemoteSumMismatch,
    TotalSumMismatch,
    TopoMismatch
  };
  /// Error bits in JEM simulation error data
  enum JEMMismatch {
    EMElementMismatch,
    HadElementMismatch,
    JEMRoIMismatch,
    CMXJetTobMismatch,
    LocalJetMismatch,
    RemoteJetMismatch,
    TotalJetMismatch,
    CMXJetTopoMismatch,
    JEMEtSumsMismatch,
    CMXEtSumsMismatch,
    LocalEnergyMismatch,
    RemoteEnergyMismatch,
    TotalEnergyMismatch,
    SumEtMismatch,
    MissingEtMismatch,
    MissingEtSigMismatch,
    EnergyRoIMismatch
  };

  /// Bins for global error plots
  enum GlobalErrors {
    PPMDataStatus,
    PPMDataError,
    SubStatus,
    Parity,
    LinkDown,
    Transmission,
    Simulation,
    CMXSubStatus,
    GbCMXParity,
    CMXTransmission,
    CMXSimulation,
    RODStatus,
    RODMissing,
    ROBStatus,
    Unpacking,
    NumberOfGlobalErrors
  };

  typedef std::vector<int> ErrorVector;

  /// Book and label global overview plot
  TH2F* bookOverview(const std::string& name, const std::string& title);

  /// Tool to retrieve bytestream errors
  ToolHandle<ITrigT1CaloMonErrorTool> m_errorTool;
  /// Histogram helper tool
  ToolHandle<TrigT1CaloLWHistogramTool> m_histTool;

  /// PPM error vector location in StoreGate
  std::string m_ppmErrorLocation;
  /// PPM spare channels error vector location in StoreGate
  std::string m_ppmSpareErrorLocation;
  /// CPM error vector location in StoreGate
  std::string m_cpmErrorLocation;
  /// JEM error vector location in StoreGate
  std::string m_jemErrorLocation;
  /// JEM-CMX error vector location in StoreGate
  std::string m_jemCmxErrorLocation;
  /// ROD error vector location in StoreGate
  std::string m_rodErrorLocation;
  /// PPM mismatch vector location in StoreGate
  std::string m_ppmMismatchLocation;
  /// CPM mismatch vector location in StoreGate
  std::string m_cpmMismatchLocation;
  /// JEM mismatch vector location in StoreGate
  std::string m_jemMismatchLocation;

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

  TH2F* m_h_l1calo_2d_GlobalOverview;  ///< L1Calo Global Error Overview

  TH2F* m_h_l1calo_2d_CurrentEventOverview;  ///< L1Calo Current Event Error
  /// Overview (temporary)
  TH2F* m_h_l1calo_2d_GlobalOverviewRecent;  ///< L1Calo Global Error Overview
  /// Last m_recentLumi Lumiblocks
  TH1F* m_h_l1calo_1d_ErrorsByLumiblock;  ///< Events with Errors by Lumiblock

  TH1F* m_h_l1calo_1d_ErrorsByTime;  ///< Time of First Event in Lumiblock
  /// with Error/Events with Errors by
  /// Time

  TH1F* m_h_l1calo_1d_RejectedEvents;  ///< Rejected Events by Lumiblock

  TH1F* m_h_l1calo_1d_NumberOfEvents;  ///< Number of processed events
  /// L1Calo Global Error Overview Block 0-m_recentLumi (temporary)
  std::vector<TH2F*> m_v_l1calo_2d_GlobalOverviewBlock;

  bool m_registred_h_l1calo_1d_ErrorsByLumiblock;
  bool m_registred_h_l1calo_1d_ErrorsByTime;
  bool m_registred_h_l1calo_1d_RejectedEvents;
};

}  // end namespace

#endif
