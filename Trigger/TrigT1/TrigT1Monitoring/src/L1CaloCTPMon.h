/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:        L1CaloCTPMon.h
// PACKAGE:     TrigT1CaloMonitoring  
//
// AUTHOR:      Johanna Fleckner (Johanna.Fleckner@uni-mainz.de)
//              Kate Whalen (Run 2 version) <kate.whalen@cern.ch>
//
// DESCRIPTION: Monitoring of L1Calo --> CTP transmission
//
// ********************************************************************

#ifndef L1CaloCTPMon_H
#define L1CaloCTPMon_H

#include <string>
#include <vector>

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "DataModel/DataVector.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

class LWHist;
class TH1F_LW;
class TH2I_LW;
class StatusCode;

namespace TrigConf {
  class ILVL1ConfigSvc;
}

class CTP_BC;

// ============================================================================
namespace LVL1 {
// ============================================================================
// Forward declarations:
// ============================================================================
class ITrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramTool;
// ============================================================================

/** Monitoring of L1Calo --> CTP transmission
 *
 *  Compares L1Calo data with CTP TIP data.
 *
 *  <b>ROOT Histogram Directories:</b>
 *
 *  <table>
 *  <tr><th> Directory              </th><th> Contents                              </th></tr>
 *  <tr><td> @c LVL1_Interfaces/CTP </td><td> L1Calo/CTP TIP matches and mismatches <br>
 *                                            Mismatch event numbers                </td></tr>
 *  </table>
 *
 *  <!--
 *  <b>Notes on Particular Histograms:</b>
 *
 *  <table>
 *  <tr><th> Histogram               </th><th> Comment                     </th></tr>
 *  <tr><td>                         </td><td>                             </td></tr>
 *  </table>
 *  -->
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
 *  <tr><th> Container             </th><th> Comment           </th></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CMMJetHits> </td><td> CMM Jet hits data </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CMMEtSums>  </td><td> CMM Et sums data  </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CMMCPHits>  </td><td> CMM CP hits data  </td></tr>
 *  <tr><td> @c CTP_RDO            </td><td> CTP raw data      </td></tr>
 *  </table>
 *
 *  <b>Services and Tools Used:</b>
 *
 *  <table>
 *  <tr><th> Service or Tool              </th><th> Description          </th></tr>
 *  <tr><td> @c TrigConf::ILVL1ConfigSvc  </td><td> @copydoc m_configSvc </td></tr>
 *  <tr><td> @c TrigT1CaloMonErrorTool    </td><td> @copydoc m_errorTool </td></tr>
 *  <tr><td> @c TrigT1CaloLWHistogramTool </td><td> @copydoc m_histTool  </td></tr>
 *  </table>
 *
 *  <b>JobOption Properties:</b>
 *
 *  <table>
 *  <tr><th> Property              </th><th> Description                   </th></tr>
 *  <tr><td> @c LVL1ConfigSvc      </td><td> @copydoc m_configSvc          </td></tr>
 *  <tr><td> @c CMMJetHitsLocation </td><td> @copydoc m_CMMJetHitsLocation </td></tr>
 *  <tr><td> @c CMMEtSumsLocation  </td><td> @copydoc m_CMMEtSumsLocation  </td></tr>
 *  <tr><td> @c CMMCPHitsLocation  </td><td> @copydoc m_CMMCPHitsLocation  </td></tr>
 *  <tr><td> @c CTP_RDOLocation    </td><td> @copydoc m_CTPRDOLocation     </td></tr>
 *  <tr><td> @c PathInRootFile     </td><td> @copydoc m_PathInRootFile     </td></tr>
 *  </table>
 *
 *  <!--
 *  <b>Related Documentation:</b>
 *  -->
 *
 *  @authors Johanna Fleckner, Andrea Neusiedl, Peter Faulkner
 *
 */

class L1CaloCTPMon : public ManagedMonitorToolBase
{
public:

   L1CaloCTPMon( const std::string & type, const std::string & name,
   	                                   const IInterface* parent ); 

   virtual ~L1CaloCTPMon();

   virtual StatusCode initialize();
   virtual StatusCode bookHistogramsRecurrent();
   virtual StatusCode fillHistograms();
   virtual StatusCode procHistograms();
     
private:

   /// Hit types for binning
   enum L1CaloCTPHitTypes { EM1Type, EM2Type,                                              // EM1, EM2 cables 
                            Tau1Type, Tau2Type,                                            // TAU1, TAU2 cables
                            Jet3BitType, Jet2BitType,                                      // JET1, JET2 cables
                            TEFullEtaType, XEFullEtaType, XSType,                          // EN1 cable
                            TERestrictedEtaType, XERestrictedEtaType, NumberOfHitTypes };  // EN2 cable

   /// Compare L1Calo hits with corresponding TIP hits
   void compare(const CTP_BC& bunch, int hits, int totalBits, int offset,
                                               L1CaloCTPHitTypes type);
   /// Bin labels for summary plots
   void setLabels(LWHist* hist, bool xAxis = true);

   /// Trigger configuration service
   ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
   /// Corrupt events tool
   ToolHandle<ITrigT1CaloMonErrorTool>      m_errorTool;
   /// Histogram utilities tool
   ToolHandle<TrigT1CaloLWHistogramTool>   m_histTool;

   // location of data
   std::string m_cmxJetHitsLocation;
   /// CMXEtSums container StoreGate key
   std::string m_cmxEtSumsLocation;         
   /// CMX-CP hits container StoreGate key
   std::string m_cmxCpHitsLocation;
   /// CTP_RDO StoreGate key
   std::string m_CTPRDOLocation;
 
   /// Root directory
   std::string m_PathInRootFile;   
   /// Debug printout flag
   bool m_debug;
   /// Histograms booked flag
   bool m_histBooked;

   /// TIP map
   std::vector<std::pair<std::string, int>> m_tipMap;

   /// Number of TIP bits (CTP input)
   const int m_nTIP = 512; 

   // Number of jet thresholds 
   const int max_JET_2bit_Threshold_Number = 15; 
   const int max_JET_3bit_Threshold_Number = 10; 

   /** Histos */   
   // Data transmission checks
   TH1F_LW* m_h_ctp_1d_L1CaloNeCTPSummary; ///< Transmission Errors between L1Calo and CTP
   TH1F_LW* m_h_ctp_1d_L1CaloEqCTPSummary; ///< Transmission Matches between L1Calo and CTP
   TH1F_LW* m_h_ctp_1d_TIPMatches;         ///< CTP/L1Calo TIP Matches
   TH1F_LW* m_h_ctp_1d_HitNoTIPMismatch;   ///< L1Calo Hit but no CTP TIP Mismatches
   TH1F_LW* m_h_ctp_1d_TIPNoHitMismatch;   ///< CTP TIP but no L1Calo Hit Mismatches
   TH2I_LW* m_h_ctp_2d_MismatchEvents;     ///< Transmission Errors between L1Calo and CTP Event Numbers
   TH1F_LW* m_h_ctp_1d_EM_HitNoTIPMismatch;  ///< L1Calo hit but no CTP TIP mismatches (EM thresholds
   TH1F_LW* m_h_ctp_1d_TAU_HitNoTIPMismatch; ///< L1Calo hit but no CTP TIP mismatches (TAU thresholds)
   TH1F_LW* m_h_ctp_1d_JET_HitNoTIPMismatch; ///< L1Calo hit but no CTP TIP mismatches (JET thresholds)
   TH1F_LW* m_h_ctp_1d_TE_HitNoTIPMismatch;  ///< L1Calo hit but no CTP TIP mismatches (TE thresholds)
   TH1F_LW* m_h_ctp_1d_XE_HitNoTIPMismatch;  ///< L1Calo hit but no CTP TIP mismatches (XE thresholds)
   TH1F_LW* m_h_ctp_1d_XS_HitNoTIPMismatch;  ///< L1Calo hit but no CTP TIP mismatches (XS thresholds)
   
   
};
// ============================================================================
}  // end namespace
// ============================================================================

#endif
