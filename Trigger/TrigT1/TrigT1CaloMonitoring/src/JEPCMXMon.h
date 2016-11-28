/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:        JEPCMXMon.h
// PACKAGE:     TrigT1CaloMonitoring  
//
// DESCRIPTION: Monitoring of the JEP on CMX level
//
// ********************************************************************

#ifndef TRIGT1CALOMONITORING_JEPCMXMON_H
#define TRIGT1CALOMONITORING_JEPCMXMON_H

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
class ITrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramTool;
// ============================================================================
/** Monitoring of the JEP on CMX level.
 *
 *  Produces histograms of CMX input data, output data and hardware errors.
 *
 *  <b>ROOT Histogram Directories:</b>
 *
 *  <table>
 *  <tr><th> Directory                         </th><th> Contents                              </th></tr>
 *  <tr><td> @c L1Calo/JEM_CMX/Input/Jet       </td><td> TOB energy distributions              <br>
 *                                                       TOB hitmap                            <br>
 *                                                       TOBs per JEM                          <br>
 *                                                       TOBs per CMX                          </td></tr>
 *  <tr><td> @c L1Calo/JEM_CMX/Input/Energy    </td><td> Ex, Ey, Et sum distributions          </td></tr>
 *  <tr><td> @c L1Calo/JEM_CMX/Output/Jet      </td><td> Multiplicities per threshold          <br>
 *                                                       RoI overflow                          <br>
 *                                                       Topo data checksum, occupancy map,
 *                                                       occupancy counts                      <br>
 *                                                       Topo TOBs per JEM                     <br>
 *                                                       Topo TOBs per CMX                     </td></tr>
 *  <tr><td> @c L1Calo/JEM_CMX/Output/Energy   </td><td> Multiplicities per threshold          <br>
 *                                                       Ex, Ey, Et sum distributions          <br>
 *                                                       Ex, Ey, Et overflow rates             </td></tr>
 *  <tr><td> @c L1Calo/JEM_CMX/Output/RoI      </td><td> Multiplicities per threshold          <br>
 *                                                       Ex, Ey, Et sum distributions          </td></tr>
 *  <tr><td> @c L1Calo/JEM_CMX/Errors/Hardware </td><td> Error summary                         <br>
 *                                                       SubStatus errors                      <br>
 *                                                       Parity errors                         <br>
 *                                                       Energy-Jet triggered slice difference <br>
 *                                                       Error event numbers                   </td></tr>
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
 *           @c <LVL1::CMXJetTob>         </td><td> CMX Jet TOBs data                        </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CMXJetHits>        </td><td> CMX Jet hits data and Topo info          </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::CMXEtSums>         </td><td> CMX Et sums data                         </td></tr>
 *  <tr><td> @c LVL1::CMXRoI              </td><td> CMX RoI data                             </td></tr>
 *  <tr><td> @c std::vector<int>          <br>
 *           @c "L1CaloJEMCMXErrorVector" </td><td> Output.
 *                                                  Error summary bits for global histograms </td></tr>
 *  </table>
 *
 *  <b>Tools Used:</b>
 *
 *  <table>
 *  <tr><th> Tool                               </th><th> Description          </th></tr>
 *  <tr><td> @c LVL1::ITrigT1CaloMonErrorTool   </td><td> @copydoc m_errorTool </td></tr>
 *  <tr><td> @c LVL1::TrigT1CaloLWHistogramTool </td><td> @copydoc m_histTool  </td></tr>
 *  </table>
 *
 *  <b>JobOption Properties:</b>
 *
 *  <table>
 *  <tr><th> Property               </th><th> Description                    </th></tr>
 *  <tr><td> @c ErrorTool           </td><td> @copydoc m_errorTool           </td></tr>
 *  <tr><td> @c HistogramTool       </td><td> @copydoc m_histTool            </td></tr>
 *  <tr><td> @c CMXJetHitsLocation  </td><td> @copydoc m_CMXJetHitsLocation  </td></tr>
 *  <tr><td> @c CMXEtSumsLocation   </td><td> @copydoc m_CMXEtSumsLocation   </td></tr>
 *  <tr><td> @c CMXRoILocation      </td><td> @copydoc m_CMXRoILocation      </td></tr>
 *  <tr><td> @c ErrorLocation       </td><td> @copydoc m_errorLocation       </td></tr>
 *  <tr><td> @c RootDirectory       </td><td> @copydoc m_rootDir             </td></tr>
 *  <tr><td> @c MaxTOBsPerCMX       </td><td> @copydoc m_maxTobsPerCmx       </td></tr>
 *  </table>
 *
 *  <b>Related Documentation:</b>                                  <!-- UPDATE!! -->
 *
 *  <a href="http://hepwww.rl.ac.uk/Atlas-L1/Modules/ROD/ROD-spec-version1_2_2.pdf">
 *  ATLAS Level-1 Calorimeter Trigger - Read-out Driver</a>
 *
 *  @authors Johanna Fleckner, Andrea Neusiedl, Peter Faulkner
 *
 */

class JEPCMXMon : public ManagedMonitorToolBase
{
public:
  
   JEPCMXMon( const std::string & type, const std::string & name,
	                                const IInterface* parent ); 

   static const InterfaceID& interfaceID();

   virtual ~JEPCMXMon();

   virtual StatusCode initialize();
   virtual StatusCode bookHistogramsRecurrent();
   virtual StatusCode fillHistograms();
   virtual StatusCode procHistograms();
 
private:

   /// CMX error summary bins
   enum SummaryErrors { JetStatus, EnergyStatus, JetParity, EnergyParity,
                        NumberOfSummaryBins };


   static const int s_crates     = 2;   ///< Number of JEM crates
   static const int s_modules    = 16;  ///< Number of modules per crate
   static const int s_tobsPerJEM = 4;   ///< Maximum number of TOBs per JEM sent to CMX

   /// Tool to retrieve bytestream errors
   ToolHandle<ITrigT1CaloMonErrorTool>   m_errorTool;
   /// Histogram helper tool
   ToolHandle<TrigT1CaloLWHistogramTool> m_histTool;

   /** location of data */
   /// CMXJetTob collection StoreGate key
   std::string m_CMXJetTobLocation;
   /// CMXJetHits collection StoreGate key
   std::string m_CMXJetHitsLocation;
   /// CMXEtSums collection StoreGate key
   std::string m_CMXEtSumsLocation;
   /// CMXRoI collection StoreGate key
   std::string m_CMXRoILocation;
   /// Error vector StoreGate key
   std::string m_errorLocation;

   /// Root directory
   std::string m_rootDir;
   /// Maximum number of TOBs per CMX plotted
   int m_maxTobsPerCmx;
   /// Histograms booked flag
   bool m_histBooked;

   /** Histos */   
   // CMX Jet TOBs
   TH1F_LW* m_h_cmx_1d_tob_EnergyLg;             ///< CMX-Jet TOB Energy Large Window Size
   TH1F_LW* m_h_cmx_1d_tob_EnergySm;             ///< CMX-Jet TOB Energy Small Window Size
   TH1F_LW* m_h_cmx_1d_tob_TOBsPerJEM;           ///< CMX-Jet TOBs per JEM
   TH1F_LW* m_h_cmx_1d_tob_TOBsPerCMX;           ///< CMX-Jet TOBs per CMX
   TH2F_LW* m_h_cmx_2d_tob_Hitmap;               ///< CMX-Jet TOBs Hit Map
   // CMX Jet Hits
   TH1F_LW* m_h_cmx_1d_thresh_TotalMainHits;     ///< Main Jet Multiplicity per Threshold
   //TH1F_LW* m_h_cmx_1d_thresh_TotalFwdHits;      ///< Forward Jet Multiplicity per Threshold
   TH1F_LW* m_h_cmx_1d_thresh_RoIOverflow;       ///< CMX-Jet Hits RoI Overflow
   /// CMX Jet Topo
   TH1F_LW* m_h_cmx_1d_topo_DataChecksum;        ///< CMX-Jet Topo Output Data Checksum Non-zero
   TH2F_LW* m_h_cmx_2d_topo_JEMOccupancyMap;     ///< CMX-Jet Topo JEM Occupancy Maps
   TH2F_LW* m_h_cmx_2d_topo_JEMOccupancyCounts;  ///< CMX-Jet Topo JEM Occupancy Counts Weighted
   TH1F_LW* m_h_cmx_1d_topo_TOBsPerJEM;          ///< CMX-Jet Topo TOBs per JEM
   TH1F_LW* m_h_cmx_1d_topo_TOBsPerCMX;          ///< CMX-Jet Topo TOBs per CMX

   // JEM Et Sums
   TH1F_LW* m_h_cmx_1d_energy_SubSumsEx;         ///< CMX Ex JEM
   TH1F_LW* m_h_cmx_1d_energy_SubSumsEy;         ///< CMX Ey JEM
   TH1F_LW* m_h_cmx_1d_energy_SubSumsEt;         ///< CMX Et JEM
   // CMX Et Sums
   TH1F_LW* m_h_cmx_1d_energy_TotalEx;           ///< Ex CMX Standard
   TH1F_LW* m_h_cmx_1d_energy_TotalEy;           ///< Ey CMX Standard
   TH1F_LW* m_h_cmx_1d_energy_TotalEt;           ///< Et CMX Standard
   TH1F_LW* m_h_cmx_1d_energy_MissingEtHits;     ///< MissingEt Multiplicity per Threshold Standard
   TH1F_LW* m_h_cmx_1d_energy_SumEtHits;         ///< SumEt Multiplicity per Threshold Standard
   TH1F_LW* m_h_cmx_1d_energy_MissingEtSigHits;  ///< MissingEtSig Multiplicity per Threshold
   TProfile2D_LW* m_h_cmx_2d_energy_Overflow;    ///< CMX Energy Overflow Rates Standard
   TH1F_LW* m_h_cmx_1d_energy_TotalExRes;        ///< Ex CMX Restricted Eta Range
   TH1F_LW* m_h_cmx_1d_energy_TotalEyRes;        ///< Ey CMX Restricted Eta Range
   TH1F_LW* m_h_cmx_1d_energy_TotalEtRes;        ///< Et CMX Restricted Eta Range
   TH1F_LW* m_h_cmx_1d_energy_MissingEtHitsRes;  ///< MissingEt Multiplicity per Threshold Restricted
   TH1F_LW* m_h_cmx_1d_energy_SumEtHitsWt;       ///< SumEt Multiplicity per Threshold Weighted
   TProfile2D_LW* m_h_cmx_2d_energy_OverflowRes; ///< CMX Energy Overflow Rates Restricted Eta Range

   // CMX RoI
   TH1F_LW* m_h_cmx_1d_roi_SumEtHits;            ///< SumEt Multiplicity per Threshold Normal
   TH1F_LW* m_h_cmx_1d_roi_MissingEtHits;        ///< MissingEt Multiplicity per Threshold Normal
   TH1F_LW* m_h_cmx_1d_roi_MissingEtSigHits;     ///< MissingEtSig Multiplicity per Threshold
   TH1F_LW* m_h_cmx_1d_roi_SumEtHitsMask;        ///< SumEt Multiplicity per Threshold Masked
   TH1F_LW* m_h_cmx_1d_roi_MissingEtHitsMask;    ///< MissingEt Multiplicity per Threshold Masked

   TH1F_LW* m_h_cmx_1d_roi_Ex;                   ///< Ex CMX RoI Normal
   TH1F_LW* m_h_cmx_1d_roi_Ey;                   ///< Ey CMX RoI Normal
   TH1F_LW* m_h_cmx_1d_roi_Et;                   ///< Et CMX RoI Normal
   TH1F_LW* m_h_cmx_1d_roi_ExMask;               ///< Ex CMX RoI Masked
   TH1F_LW* m_h_cmx_1d_roi_EyMask;               ///< Ey CMX RoI Masked
   TH1F_LW* m_h_cmx_1d_roi_EtMask;               ///< Et CMX RoI Masked

   //errors
   TH2F_LW* m_h_cmx_2d_thresh_Status;            ///< Errors from CMX Jet SubStatus Word
   TH2F_LW* m_h_cmx_2d_energy_Status;            ///< Errors from CMX Energy SubStatus Word
   TH2F_LW* m_h_cmx_2d_thresh_ParityTob;         ///< CMX Jet TOB Parity Errors
   TH1F_LW* m_h_cmx_1d_thresh_ParityRem;         ///< CMX Jet Remote Hit Sums Parity Errors
   TH2F_LW* m_h_cmx_2d_energy_ParityMod;         ///< CMX Energy Module Sums Parity Errors
   TH1F_LW* m_h_cmx_1d_energy_ParityRem;         ///< CMX Energy Remote Crate Sums Parity Errors
   TH1F_LW* m_h_cmx_1d_ErrorSummary;             ///< Error Summary of CMX Jet, Energy and RoI path
   TH1F_LW* m_h_cmx_1d_TriggeredSlices;	         ///< Comparison of CMX Jet and Energy triggered slice numbers
   TH2I_LW* m_h_cmx_2d_ErrorEventNumbers;        ///< JEM-CMX Error Event Numbers
};

} // end namespace

#endif
