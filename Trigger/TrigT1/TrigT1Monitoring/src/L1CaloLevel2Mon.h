/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:        L1CaloLevel2Mon.h
// PACKAGE:     TrigT1CaloMonitoring  
//
// AUTHOR:      Johanna Fleckner (Johanna.Fleckner@uni-mainz.de)
//           
// DESCRIPTION: Monitoring of transmission L1Calo-->Level2
//
// ********************************************************************

#ifndef L1CaloLevel2Mon_H
#define L1CaloLevel2Mon_H

#include <string>
#include <vector>

#include "GaudiKernel/ToolHandle.h"

#include "DataModel/DataVector.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

class StatusCode;
class LWHist;
class TH1F_LW;
class TH2F_LW;
class TH2I_LW;


namespace ROIB {
  class EMTauResult;
  class EMTauRoI;
  class JetEnergyResult;
  class JetEnergyRoI;
}
// ============================================================================
namespace LVL1 {
// ============================================================================
// Forward declarations:
// ============================================================================
class CPMRoI;
class JEMRoI;
class ITrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramToolV1;
// ============================================================================
/** Monitoring of L1Calo --> Level2 transmission
 *
 *  Compares L1Calo data with L2 data from RoIBResult.
 *
 *  <b>ROOT Histogram Directories:</b>
 *
 *  <table>
 *  <tr><th> Directory                 </th><th> Contents                         </th></tr>
 *  <tr><td> @c LVL1_Interfaces/Level2 </td><td> L1Calo/L2 matches and mismatches <br>
 *                                               Mismatch event numbers           </td></tr>
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
 *  <tr><th> Container           </th><th> Comment       </th></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CPMRoI>   </td><td> CPM RoI data  </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::JEMRoI>   </td><td> JEM RoI data  </td></tr>
 *  <tr><td> @c LVL1::CMMRoI     </td><td> CMM RoI data  </td></tr>
 *  <tr><td> @c ROIB::RoIBResult </td><td> RoIB raw data </td></tr>
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
 *  <tr><th> Property              </th><th> Description                      </th></tr>
 *  <tr><td> @c JEMRoILocation     </td><td> @copydoc m_L1Calo_JEMRoILocation </td></tr>
 *  <tr><td> @c CMMRoILocation     </td><td> @copydoc m_L1Calo_CMMRoILocation </td></tr>
 *  <tr><td> @c CPMRoILocation     </td><td> @copydoc m_L1Calo_CPMRoILocation </td></tr>
 *  <tr><td> @c RoIBResultLocation </td><td> @copydoc m_L2_RoIBResultLocation </td></tr>
 *  <tr><td> @c PathInRootFile     </td><td> @copydoc m_PathInRootFile        </td></tr>
 *  </table>
 *
 *  <!--
 *  <b>Related Documentation:</b>
 *  -->
 *
 *  @authors Johanna Fleckner, Andrea Neusiedl, Peter Faulkner
 *
 */

class L1CaloLevel2Mon : public ManagedMonitorToolBase
{
public:

   L1CaloLevel2Mon( const std::string & type, const std::string & name,
	                                      const IInterface* parent ); 

   virtual ~L1CaloLevel2Mon();

   virtual StatusCode initialize();
   virtual StatusCode bookHistogramsRecurrent();
   virtual StatusCode fillHistograms();
   virtual StatusCode procHistograms();

private:

   /// Threshold types for binning
   enum L1CaloL2RoiTypes { EMType, EMTauType, JetType, JetEtType, EnergyType,
                           MissingEtType, SumEtType, MissingEtSigType,
			   NumberOfRoiTypes };

   typedef DataVector<LVL1::CPMRoI> CpmRoiCollection;
   typedef DataVector<LVL1::JEMRoI> JemRoiCollection;
   typedef std::vector<ROIB::EMTauResult>     EMTauResultCollection;
   typedef std::vector<ROIB::EMTauRoI>        EMTauRoICollection;
   typedef std::vector<ROIB::JetEnergyResult> JetEnergyResultCollection;
   typedef std::vector<ROIB::JetEnergyRoI>    JetEnergyRoICollection;

   /// Set histogram bin labels for summary plots
   void setLabels(LWHist* hist, bool xAxis = true);

   /// Corrupt events tool
   ToolHandle<ITrigT1CaloMonErrorTool>    m_errorTool;
   /// Histogramming utilities
   ToolHandle<TrigT1CaloLWHistogramToolV1> m_histTool;

   // Location of data
   /// JEMRoI collection StoreGate key
   std::string m_L1Calo_JEMRoILocation;
   /// CMMRoI StoreGate key
   std::string m_L1Calo_CMMRoILocation;
   /// CPMRoI collection StoreGate key
   std::string m_L1Calo_CPMRoILocation;
   /// RoIBResult StoreGate key
   std::string m_L2_RoIBResultLocation;

   /// Root histogram directory
   std::string m_PathInRootFile;   
   /// Histograms booked flag
   bool m_histBooked;

   /** Histos */   
   // Data transmission checks
   TH1F_LW* m_h_l2_1d_L1NeL2Summary;     ///< Transmission Errors between L1Calo and Level2
   TH1F_LW* m_h_l2_1d_L1EqL2Summary;     ///< Transmission Matches between L1Calo and Level2
   TH2F_LW* m_h_l2_2d_JEMRoIErrors;      ///< Transmission Errors between JEMs and Level2
   TH2F_LW* m_h_l2_2d_CPMRoIErrorsEM;    ///< Transmission Errors between CPMs and Level2 EM
   TH2F_LW* m_h_l2_2d_CPMRoIErrorsEMTau; ///< Transmission Errors between CPMs and Level2 EMTau
   TH2I_LW* m_h_l2_2d_MismatchEvents;    ///< Transmission Errors between L1Calo and Level2 Event Numbers
};
// ============================================================================
}  // end namespace
// ============================================================================

#endif
