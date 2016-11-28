/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     RODMonV1.h
// PACKAGE:  TrigT1CaloMonitoring
//
// AUTHOR:   alexander.mazurov@cern.ch
//	     
//
// ********************************************************************
#ifndef TRIGT1CALOMONITORING_RODMONV1_H
#define TRIGT1CALOMONITORING_RODMONV1_H

#include <string>
#include <vector>

#include "GaudiKernel/ToolHandle.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "AthContainers/DataVector.h"

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
class RODHeader;
class ITrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramToolV1;
// ============================================================================
/** Monitoring of ROD errors.
 *
 *  Also includes unpacking, ROB and Full Event Status errors
 *
 *  <b>ROOT Histogram Directories:</b>
 *
 *  <table>
 *  <tr><th> Directory               </th><th> Contents                               </th></tr>
 *  <tr><td> @c L1Calo/ROD           </td><td> Average payload sizes                  <br>
 *                                             ROD status errors                      <br>
 *                                             ROD, ROB and unpacking error summaries <br>
 *                                             ROD Error event numbers                </td></tr>
 *  <tr><td> @c L1Calo/ROD/ROBStatus </td><td> ROB status errors                      <br>
 *                                             Full event status errors               <br>
 *                                             ROB error event numbers                <br>
 *                                             Full event error and rejected event
 *                                                                      event numbers </td></tr>
 *  <tr><td> @c L1Calo/ROD/Unpacking </td><td> ByteStream unpacking errors            <br>
 *                                             Unpacking error event numbers          </td></tr>
 *  </table>
 *
 *  <!--
 *  <b>Notes on Particular Histograms:</b>
 *
 *  <table>
 *  <tr><th> Histogram                      </th><th> Comment                         </th></tr>
 *  <tr><td>                                </td><td>                                 </td></tr>
 *  </table>
 *  -->
 *
 *  <b>Meaning of ByteStream Unpacking Errors:</b>
 *
 *  <table>
 *  <tr><th> Error           </th><th> Meaning                                              </th></tr>
 *  <tr><td> DuplicateROB    </td><td> Duplicate ROB fragment detected                      </td></tr>
 *  <tr><td> RODSourceID     </td><td> Invalid ROD source identifier                        </td></tr>
 *  <tr><td> RODnstatus      </td><td> Number of ROD status words > 2                       </td></tr>
 *  <tr><td> UserHeader      </td><td> Missing or invalid User Header                       </td></tr>
 *  <tr><td> MissingHeader   </td><td> Missing sub-block header                             </td></tr>
 *  <tr><td> MissingSubBlock </td><td> Missing sub-block(s)                                 </td></tr>
 *  <tr><td> CrateNumber     </td><td> Inconsistent crate numbers in ROD
 *                                     source identifier and/or sub-blocks                  </td></tr>
 *  <tr><td> ModuleNumber    </td><td> Inconsistent module number in sub-blocks             </td></tr>
 *  <tr><td> Slices          </td><td> Triggered slice or slice number 
 *                                     inconsistent with total number of slices             </td></tr>
 *  <tr><td> DuplicateData   </td><td> Duplicate data detected                              </td></tr>
 *  <tr><td> RoIType         </td><td> Invalid RoI Type in RoI word                         </td></tr>
 *  <tr><td> Version         </td><td> Unsupported Data Version in sub-block header         </td></tr>
 *  <tr><td> Format          </td><td> Unsupported Data Format in sub-block header          </td></tr>
 *  <tr><td> ComprVersion    </td><td> Unsupported Compression Version in sub-block header  </td></tr>
 *  <tr><td> ComprSlices     </td><td> Unsupported Number of slices for Compression Version </td></tr>
 *  <tr><td> DataTruncated   </td><td> Premature end of sub-block data                      </td></tr>
 *  <tr><td> ExcessData      </td><td> Excess data in sub-block                             </td></tr>
 *  <tr><td> DataSourceID    </td><td> Invalid Source ID in sub-block data                  </td></tr>
 *  <tr><td> Unknown         </td><td> None of the above, shouldn't happen                  </td></tr>
 *  </table>
 *
 *  <b>Custom Merges Used (Tier0):</b>
 *
 *  <table>
 *  <tr><th> Merge                                        </th><th> Used For                 </th></tr>
 *  <tr><td> @ref MergesUsedsection "@c eventSample "     </td><td> Error event number plots </td></tr>
 *  <tr><td> @ref MergesUsedsection "@c weightedAverage " </td><td> Payload averages         </td></tr>
 *  </table>
 *
 *  <b>StoreGate Containers Used:</b>
 *
 *  <table>
 *  <tr><th> Container                 </th><th> Comment                                  </th></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::RODHeader>      </td><td> L1Calo ROD data                          </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::RODHeader>      </td><td> RoIB CP ROD data                         </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::RODHeader>      </td><td> RoIB JEP ROD data                        </td></tr>
 *  <tr><td> @c EventInfo              </td><td> For Full event status error bits         </td></tr>
 *  <tr><td> @c std::vector<int>       <br>
 *           @c "L1CaloRODErrorVector" </td><td> Output.
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
 *  <tr><th> Property               </th><th> Description                       </th></tr>
 *  <tr><td> @c RodHeaderLocation   </td><td> @copydoc m_rodHeaderLocation      </td></tr>
 *  <tr><td> @c RootDirectory       </td><td> @copydoc m_rootDir                </td></tr>
 *  <tr><td> @c OnlineTest          </td><td> @copydoc m_onlineTest             </td></tr>
 *  </table>
 *
 *  <b>Related Documentation:</b>
 *
 *  <a href="http://hepwww.rl.ac.uk/Atlas-L1/Modules/ROD/ROD-spec-version1_2_2.pdf">
 *  ATLAS Level-1 Calorimeter Trigger - Read-out Driver</a><br>
 *  <a href="https://edms.cern.ch/document/445840/4.0e/eformat.pdf">
 *  The raw event format in the ATLAS Trigger & DAQ, ATL-D-ES-0019</a><br>
 *  <a href="https://twiki.cern.ch/twiki/bin/viewauth/Atlas/ROBINFragmentErrors">
 *  Definition of the status words in a ROB fragment header</a>
 *
 *  @author Peter Faulkner
 *
 */

class RODMonV1: public ManagedMonitorToolBase
{

public:
  
  RODMonV1(const std::string & type, const std::string & name,
		       const IInterface* parent);
    

  virtual ~RODMonV1();

  virtual StatusCode initialize();
    
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

private:

  /// Summary error bins
  enum StatusBits { GLink, /*CMMParity,*/ LVDSLink, FIFOOverflow, DataTransport,
                    Timeout, BCNMismatch, TriggerType, LimitedRoI, NoFragment,
		    NumberOfStatusBins, NoPayload = LimitedRoI,
		    ROBStatusError = NumberOfStatusBins, UnpackingError };

  typedef DataVector<LVL1::RODHeader> RodHeaderCollection;
  typedef std::vector<unsigned int>   ROBErrorCollection;
  typedef std::vector<int>            ErrorVector;
  
  /// Label ROD error status bins
  void setLabelsStatus(LWHist* hist, bool xAxis = true);
  /// Label ROB status Generic bins
  void setLabelsROBStatusGen(LWHist* hist, bool xAxis = true);
  /// Label ROB status Specific bins
  void setLabelsROBStatusSpec(LWHist* hist, bool xAxis = true);
  /// Label Full Event status Specific bins
  void setLabelsEvtStatusSpec(LWHist* hist, bool xAxis = true);
  /// Label unpacking error bins
  void setLabelsUnpacking(LWHist* hist, bool xAxis = true);

  /// Tool to retrieve bytestream errors.
  ToolHandle<ITrigT1CaloMonErrorTool>    m_errorTool;
  /// Histogram helper tool
  ToolHandle<TrigT1CaloLWHistogramToolV1> m_histTool;

  /// DAQ ROD header container StoreGate key
  std::string m_rodHeaderLocation;
  /// CP RoIB ROD header container StoreGate key
  std::string m_cpRoibRodHeaderLocation;
  /// JEP RoIB ROD header container StoreGate key
  std::string m_jepRoibRodHeaderLocation;
  
  /// Root directory
  std::string m_rootDir;

  /// Accumulated payload sizes all events
  std::vector<double> m_sumPayloads1;
  /// Accumulated payload sizes recent events
  std::vector<double> m_sumPayloads2;

  /// Number of events
  int m_events;
  /// Test online code flag
  bool m_onlineTest;
  /// Histograms booked flag
  bool m_histBooked;

  //=======================
  //   Payload plots
  //=======================

  TH1F_LW* m_h_rod_1d_PpPayload;                 ///< ROD PP Average Payload Size
  TH1F_LW* m_h_rod_1d_CpPayload;                 ///< ROD CP Average Payload Size
  TH1F_LW* m_h_rod_1d_JepPayload;                ///< ROD JEP Average Payload Size
  TH1F_LW* m_h_rod_1d_CpJepRoiPayload;           ///< ROD CP and JEP RoI Average Payload Size

  //=======================
  //   Status bit plots
  //=======================

  TH2F_LW* m_h_rod_2d_PpStatus;                  ///< PP ROD Status Bits and Payload Check
  TH2F_LW* m_h_rod_2d_CpJepStatus;               ///< CP and JEP ROD Status Bits and Payload Check
  TH2F_LW* m_h_rod_2d_CpJepRoiStatus;            ///< CP and JEP RoI ROD Status Bits
  TH2F_LW* m_h_rod_2d_PpRobStatusGeneric;        ///< PP ROB Status Bits Generic Field
  TH2F_LW* m_h_rod_2d_CpJepRobStatusGeneric;     ///< CP and JEP ROB Status Bits Generic Field
  TH2F_LW* m_h_rod_2d_CpJepRoiRobStatusGeneric;  ///< CP and JEP RoI ROB Status Bits Generic Field
  TH2F_LW* m_h_rod_2d_PpRobStatusSpecific;       ///< PP ROB Status Bits Specific Field
  TH2F_LW* m_h_rod_2d_CpJepRobStatusSpecific;    ///< CP and JEP ROB Status Bits Specific Field
  TH2F_LW* m_h_rod_2d_CpJepRoiRobStatusSpecific; ///< CP and JEP RoI ROB Status Bits Specific Field
  TH1F_LW* m_h_rod_1d_EvtStatusGeneric;          ///< Full Event Status Bits Generic Field
  TH1F_LW* m_h_rod_1d_EvtStatusSpecific;         ///< Full Event Status Bits Specific Field
  TH2F_LW* m_h_rod_2d_PpUnpack;                  ///< PP Bytestream Unpacking Errors
  TH2F_LW* m_h_rod_2d_CpJepUnpack;               ///< CP and JEP Bytestream Unpacking Errors
  TH2F_LW* m_h_rod_2d_CpJepRoiUnpack;            ///< CP and JEP RoI Bytestream Unpacking Errors

  //=======================
  //   Summary plots
  //=======================

  TH1F_LW* m_h_rod_1d_ErrorSummary;              ///< ROD Error Summary
  TH1F_LW* m_h_rod_1d_RobErrorSummary;           ///< ROB Status Error Summary
  TH1F_LW* m_h_rod_1d_UnpackErrorSummary;        ///< Bytestream Unpacking Error Summary
  TH2I_LW* m_h_rod_2d_ErrorEventNumbers;         ///< ROD Error Event Numbers
  TH2I_LW* m_h_rod_2d_RobErrorEventNumbers;      ///< ROB Status Error Event Numbers
  TH2I_LW* m_h_rod_2d_EvtErrorEventNumbers;      ///< Full Event Status Error Event Numbers
  TH2I_LW* m_h_rod_2d_UnpackErrorEventNumbers;   ///< Bytestream Unpacking Error Event Numbers

};

// ============================================================================
}  // end namespace
// ============================================================================

#endif
