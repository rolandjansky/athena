/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:        CMMMon.h
// PACKAGE:     TrigT1CaloMonitoring  
//
// AUTHOR:      Johanna Fleckner (Johanna.Fleckner@uni-mainz.de)
//           
// DESCRIPTION: Monitoring of the JEP on CMM level
//
// ********************************************************************

#ifndef CMMMon_H
#define CMMMon_H

#include <string>

#include "GaudiKernel/ToolHandle.h"

#include "AthContainers/DataVector.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

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
class CMMJetHits;
class CMMEtSums;
class ITrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramToolV1;
// ============================================================================
/** Monitoring of the JEP on CMM level.
 *
 *  Produces histograms of CMM input data, output data and hardware errors.
 *
 *  <b>ROOT Histogram Directories:</b>
 *
 *  <table>
 *  <tr><th> Directory                          </th><th> Contents                              </th></tr>
 *  <tr><td> @c L1Calo/JEM_CMM/Input/Thresholds </td><td> Multiplicities per threshold          </td></tr>
 *  <tr><td> @c L1Calo/JEM_CMM/Input/EnergySums </td><td> Ex, Ey, Et sum distributions          </td></tr>
 *  <tr><td> @c L1Calo/JEM_CMM/Output/Jet       </td><td> Multiplicities per threshold          </td></tr>
 *  <tr><td> @c L1Calo/JEM_CMM/Output/Energy    </td><td> Multiplicities per threshold          <br>
 *                                                        Ex, Ey, Et sum distributions          <br>
 *                                                        Ex, Ey, Et overflow rates             </td></tr>
 *  <tr><td> @c L1Calo/JEM_CMM/Output/RoI       </td><td> Multiplicities per threshold          <br>
 *                                                        Ex, Ey, Et sum distributions          </td></tr>
 *  <tr><td> @c L1Calo/JEM_CMM/Errors/Hardware  </td><td> Error summary                         <br>
 *                                                        SubStatus errors                      <br>
 *                                                        Parity errors                         <br>
 *                                                        Energy-Jet triggered slice difference <br>
 *                                                        Error event numbers                   </td></tr>
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
 *  <tr><th> Merge                                    </th><th> Used For                 </th></tr>
 *  <tr><td> @ref MergesUsedsection "@c eventSample " </td><td> Error event number plots </td></tr>
 *  </table>
 *
 *  <b>StoreGate Containers Used:</b>
 *
 *  <table>
 *  <tr><th> Container                    </th><th> Comment                                  </th></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CMMJetHits>        </td><td> CMM Jet hits data                        </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CMMEtSums>         </td><td> CMM Et sums data                         </td></tr>
 *  <tr><td> @c LVL1::CMMRoI              </td><td> CMM RoI data                             </td></tr>
 *  <tr><td> @c std::vector<int>          <br>
 *           @c "L1CaloJEMCMMErrorVector" </td><td> Output.
 *                                                  Error summary bits for global histograms </td></tr>
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
 *  <tr><th> Property               </th><th> Description                    </th></tr>
 *  <tr><td> @c CMMJetHitsLocation  </td><td> @copydoc m_CMMJetHitsLocation  </td></tr>
 *  <tr><td> @c CMMEtSumsLocation   </td><td> @copydoc m_CMMEtSumsLocation   </td></tr>
 *  <tr><td> @c CMMRoILocation      </td><td> @copydoc m_CMMRoILocation      </td></tr>
 *  <tr><td> @c PathInRootFile      </td><td> @copydoc m_PathInRootFile      </td></tr>
 *  <tr><td> @c ErrorPathInRootFile </td><td> @copydoc m_ErrorPathInRootFile </td></tr>
 *  </table>
 *
 *  <b>Related Documentation:</b>
 *
 *  <a href="http://hepwww.rl.ac.uk/Atlas-L1/Modules/ROD/ROD-spec-version1_2_2.pdf">
 *  ATLAS Level-1 Calorimeter Trigger - Read-out Driver</a>
 *
 *  @authors Johanna Fleckner, Andrea Neusiedl, Peter Faulkner
 *
 */

class CMMMon : public ManagedMonitorToolBase
{
public:

   typedef DataVector<LVL1::CMMJetHits> CMMJetHitsCollection;
   typedef DataVector<LVL1::CMMEtSums>  CMMEtSumsCollection;
  
   CMMMon( const std::string & type, const std::string & name,
	                             const IInterface* parent ); 

   virtual ~CMMMon();

   virtual StatusCode initialize();
   virtual StatusCode bookHistogramsRecurrent();
   virtual StatusCode fillHistograms();
   virtual StatusCode procHistograms();

private:

   /// CMM error summary bins
   enum SummaryErrors { JetStatus, EnergyStatus, JetParity, EnergyParity,
                        RoIParity, NumberOfSummaryBins };
   /// CMM RoI parity error bins
   enum RoIParityErrors { ExParity, EyParity, EtParity, JetEtParity,
                          NumberOfRoIParityBins };

   /// Tool to retrieve bytestream errors
   ToolHandle<ITrigT1CaloMonErrorTool>    m_errorTool;
   /// Histogram helper tool
   ToolHandle<TrigT1CaloLWHistogramToolV1> m_histTool;

   /** location of data */
   /// CMMJetHits collection StoreGate key
   std::string m_CMMJetHitsLocation;
   /// CMMEtSums collection StoreGate key
   std::string m_CMMEtSumsLocation;
   /// CMMRoI collection StoreGate key
   std::string m_CMMRoILocation;

   /// Root directory
   std::string m_PathInRootFile;   
   /// Root directory for error plots
   std::string m_ErrorPathInRootFile;
   /// Histograms booked flag
   bool m_histBooked;

   /** Histos */   
   // CMM Jet Hits
   TH1F_LW* m_h_cmm_1d_thresh_TotalMainHits;     ///< Main Jet Multiplicity per Threshold  --  CMM DAQ
   TH1F_LW* m_h_cmm_1d_thresh_TotalFwdHitsRight; ///< Forward Right Jet Multiplicity per Threshold  --  CMM DAQ
   TH1F_LW* m_h_cmm_1d_thresh_TotalFwdHitsLeft;  ///< Forward Left Jet Multiplicity per Threshold  --  CMM DAQ
   TH1F_LW* m_h_cmm_1d_thresh_JetEtHits;         ///< JetEt Multiplicity per Threshold  --  CMM DAQ
   // JEM Hits
   TH1F_LW* m_h_cmm_1d_thresh_MainHits;          ///< Main Jet Multiplicity per Threshold  --  CMM input
   TH1F_LW* m_h_cmm_1d_thresh_FwdHitsRight;      ///< Forward Right Jet Multiplicity per Threshold  --  CMM input
   TH1F_LW* m_h_cmm_1d_thresh_FwdHitsLeft;       ///< Forward Left Jet Multiplicity per Threshold  --  CMM input

   // CMM Et Sums
   TH1F_LW* m_h_cmm_1d_energy_TotalEx;           ///< Ex  --  CMM DAQ
   TH1F_LW* m_h_cmm_1d_energy_TotalEy;           ///< Ey  --  CMM DAQ
   TH1F_LW* m_h_cmm_1d_energy_TotalEt;           ///< Et  --  CMM DAQ
   TH1F_LW* m_h_cmm_1d_energy_MissingEtHits;     ///< MissingEt Multiplicity per Threshold  --  CMM DAQ
   TH1F_LW* m_h_cmm_1d_energy_SumEtHits;         ///< SumEt Multiplicity per Threshold  --  CMM DAQ
   TH1F_LW* m_h_cmm_1d_energy_MissingEtSigHits;  ///< MissingEtSig Multiplicity per Threshold  --  CMM DAQ
   TProfile2D_LW* m_h_cmm_2d_energy_Overflow;    ///< CMM Energy Overflow Rates
   // JEM Et Sums
   TH1F_LW*  m_h_cmm_1d_energy_SubSumsEx;        ///< JEM Ex  --  CMM input
   TH1F_LW*  m_h_cmm_1d_energy_SubSumsEy;        ///< JEM Ey  --  CMM input
   TH1F_LW*  m_h_cmm_1d_energy_SubSumsEt;        ///< JEM Et  --  CMM input

   // CMM RoI
   TH1F_LW* m_h_cmm_1d_roi_JetEtHits;            ///< JetEt Multiplicity per Threshold  --  CMM RoI
   TH1F_LW* m_h_cmm_1d_roi_SumEtHits;            ///< SumEt Multiplicity per Threshold  --  CMM RoI
   TH1F_LW* m_h_cmm_1d_roi_MissingEtHits;        ///< MissingEt Multiplicity per Threshold  --  CMM RoI
   TH1F_LW* m_h_cmm_1d_roi_MissingEtSigHits;     ///< MissingEtSig Multiplicity per Threshold  --  CMM RoI

   TH1F_LW* m_h_cmm_1d_roi_Ex;                   ///< Ex  --  CMM RoI
   TH1F_LW* m_h_cmm_1d_roi_Ey;                   ///< Ey  --  CMM RoI
   TH1F_LW* m_h_cmm_1d_roi_Et;                   ///< Et  --  CMM RoI

   //errors
   TH2F_LW* m_h_cmm_2d_thresh_Status;            ///< Errors from CMM Jet SubStatus Word
   TH2F_LW* m_h_cmm_2d_energy_Status;            ///< Errors from CMM Energy SubStatus Word
   TH2F_LW* m_h_cmm_2d_thresh_Parity;            ///< CMM Jet Parity Errors
   TH2F_LW* m_h_cmm_2d_energy_Parity;            ///< CMM Energy Parity Errors
   TH1F_LW* m_h_cmm_1d_roi_Parity;               ///< CMM RoI Parity Errors
   TH1F_LW* m_h_cmm_1d_ErrorSummary;             ///< Error Summary of CMM Jet, Energy and RoI path
   TH1F_LW* m_h_cmm_1d_TriggeredSlices;	         ///< Comparison of CMM Jet and Energy triggered slice numbers
   TH2I_LW* m_h_cmm_2d_ErrorEventNumbers;        ///< JEM-CMM Error Event Numbers
};
// ============================================================================
} // end namespace
// ============================================================================

#endif
