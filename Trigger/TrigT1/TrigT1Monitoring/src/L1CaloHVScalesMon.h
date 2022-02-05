/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// ********************************************************************
//
// NAME:     L1CaloHVScalesMon.h
// PACKAGE:  TrigT1Monitoring
//
// AUTHOR:   Peter Faulkner
//
// ********************************************************************
#ifndef TRIGT1MONITORING_L1CALOHVSCALESMON_H
#define TRIGT1MONITORING_L1CALOHVSCALESMON_H

#include <string>
#include <vector>

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloMatchCell2Tower.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

class StatusCode;
class TH2F_LW;


class CaloCell;
class CaloLVL1_ID;
class L1CaloCondSvc;
class CaloDetDescrElement;
class CaloTriggerTowerService;
class L1CaloCoolChannelId;
// ============================================================================
namespace LVL1 {
// ============================================================================
// Forward declarations:
// ============================================================================
class IL1CaloMatchCell2Tower;
class IL1CaloLArTowerEnergy;
class IL1CaloTTIdTools;
class IL1TriggerTowerTool;
class ITrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramTool;
// ============================================================================

/** Monitoring of mean LAr HV corrections by TriggerTower
 *
 *  Compares the current corrections from the database with a reference.
 *  (The values at the last gain calibration.)
 *  Only runs on the first good event.
 *
 *  <b>ROOT Histogram Directories:</b>
 *
 *  <table>
 *  <tr><th> Directory
 *    </th><th> Contents                                       </th></tr>
 *  <tr><td> @c LVL1_Interfaces/Calorimeter/HVCorrections
 *    </td><td> Relative differences by partition and overall  <br>
 *              Disabled towers and towers with missing FEB    </td></tr>
 *  <tr><td> @c LVL1_Interfaces/Calorimeter/HVCorrections/HVCurrent
 *    </td><td> Current corrections by partition and overall   </td></tr>
 *  <tr><td> @c LVL1_Interfaces/Calorimeter/HVCorrections/HVLastCalibration
 *    </td><td> Reference corrections by partition and overall </td></tr>
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
 *  <tr><th> Merge                                </th><th> Used For  </th></tr>
 *  <tr><td> @ref MergesUsedsection "@c lowerLB " </td><td> All plots </td></tr>
 *  </table>
 *
 *  <b>StoreGate Containers Used:</b>
 *
 *  <table>
 *  <tr><th> Container               </th><th> Comment           </th></tr>
 *  <tr><td> @c CaloCellContainer    </td><td> For LAr calo data </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::TriggerTower> </td><td> PPM data          </td></tr>
 *  </table>
 *
 *  <b>Services and Tools Used:</b>
 *
 *  <table>
 *  <tr><th> Service or Tool                     </th><th> Description              </th></tr>
 *  <tr><td> @c LVL1::IL1TriggerTowerTool        </td><td> @copydoc m_ttTool        </td></tr>
 *  <tr><td> @c TrigT1CaloMonErrorTool           </td><td> @copydoc m_errorTool     </td></tr>
 *  <tr><td> @c TrigT1CaloLWHistogramTool        </td><td> @copydoc m_histTool      </td></tr>
 *  <tr><td> @c LVL1::IL1CaloMatchCell2Tower     </td><td> @copydoc m_cellMatch     </td></tr>
 *  <tr><td> @c LVL1::IL1CaloLArTowerEnergy      </td><td> @copydoc m_larEnergy     </td></tr>
 *  <tr><td> @c LVL1::IL1CaloTTIdTools           </td><td> @copydoc m_ttIdTools     </td></tr>
 *  <tr><td> @c L1CaloCondSvc                    </td><td> @copydoc m_l1CondSvc     </td></tr>
 *  <tr><td> @c CaloTriggerTowerService          </td><td> @copydoc m_ttSvc         </td></tr>
 *  </table>
 *
 *  <b>JobOption Properties:</b>
 *
 *  <table>
 *  <tr><th> Property                 </th><th> Description                          </th></tr>
 *  <tr><td> @c CaloCellContainer     </td><td> @copydoc m_caloCellContainerName     </td></tr>
 *  <tr><td> @c TriggerTowerContainer </td><td> @copydoc m_TriggerTowerContainerName </td></tr>
 *  <tr><td> @c PathInRootFile        </td><td> @copydoc m_PathInRootFile            </td></tr>
 *  <tr><td> @c DoHVDifference        </td><td> @copydoc m_hvDifference              </td></tr>
 *  </table>
 *
 *  <!--
 *  <b>Related Documentation:</b>
 *  -->
 *
 *  @author Peter Faulkner
 *
 */

class L1CaloHVScalesMon: public ManagedMonitorToolBase
{

 public:
  
  L1CaloHVScalesMon(const std::string & type, const std::string & name,
		    const IInterface* parent);
    

  virtual ~L1CaloHVScalesMon();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

private:

  /// Return receiver (0/1) FCAL2/3 cell belongs to
  unsigned int fcalReceiver(const CaloDetDescrElement* caloDDE, double eta);
  /// Return receiver ID(s) for given EM coolID
  std::vector<unsigned int> emRxId(const L1CaloCoolChannelId& coolId);
  /// Return receiver ID(s) for given Had coolID
  std::vector<unsigned int> hadRxId(const L1CaloCoolChannelId& coolId);

  /// Tool for identifiers and disabled channels
  ToolHandle<LVL1::IL1TriggerTowerTool>        m_ttTool;
  /// Event veto tool
  ToolHandle<ITrigT1CaloMonErrorTool>           m_errorTool;
  /// Histogram helper tool
  ToolHandle<TrigT1CaloLWHistogramTool>        m_histTool;
  /// CaloCell to TriggerTower matching tool
  ToolHandle<LVL1::IL1CaloMatchCell2Tower> m_cellMatch
    { this, "L1CaloMatchCell2Tower",  "LVL1::L1CaloMatchCell2Tower",  "L1CaloMatchCell2Tower" };
  /// Tool for missing FEBs
  ToolHandle<LVL1::IL1CaloLArTowerEnergy>      m_larEnergy;
  /// Tool for Identifier to eta/phi mappings
  ToolHandle<LVL1::IL1CaloTTIdTools>           m_ttIdTools;
  /// Current CaloCell HV corrections
  SG::ReadCondHandleKey<ILArHVScaleCorr> m_scaleCorrKey
  { this, "LArHVScaleCorr", "LArHVScaleCorrRecomputed", "" };
  SG::ReadCondHandleKey<ILArHVScaleCorr> m_onlineScaleCorrKey
  { this, "OnlineLArHVScaleCorr", "LArHVScaleCorr", "" };
  /// LVL1 ID helper1
  const CaloLVL1_ID*                           m_lvl1Helper;
  /// L1Calo conditions service
  ServiceHandle<L1CaloCondSvc>                 m_l1CondSvc;
  /// Cabling & tt service for ID to receiver mappings
  ToolHandle<CaloTriggerTowerService>          m_ttSvc;

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey
  { this, "CablingKey", "LArOnOffIdMap", "SG Key of LArOnOffIdMapping object" };

  /// CaloCellContainer StoreGate key
  std::string m_caloCellContainerName;
  /// TriggerTower collection StoreGate key
  std::string m_xAODTriggerTowerContainerName;

  /// Root histograms directory
  std::string m_PathInRootFile;

  /// Event counter
  int  m_events;
  /// Histograms booked flag
  bool m_histBooked;
  /// Compare with reference HVs flag
  bool m_hvDifference;

  TH2F_LW* m_h_emHVScale;                   ///< EM Mean HV Scale
  TH2F_LW* m_h_hadHVScale;                  ///< Had Mean HV Scale
  TH2F_LW* m_h_emHVEntry;                   ///< EM Mean HV Scale number of entries (temporary)
  TH2F_LW* m_h_hadHVEntry;                  ///< Had Mean HV Scale number of entries (temporary)
  TH2F_LW* m_h_emHVScaleRef;                ///< EM Mean HV Scale Last Calibration
  TH2F_LW* m_h_hadHVScaleRef;               ///< Had Mean HV Scale Last Calibration
  TH2F_LW* m_h_emHVScaleDif;                ///< EM Mean HV Scale Difference ((Current-Last)/Last)
  TH2F_LW* m_h_hadHVScaleDif;               ///< Had Mean HV Scale Difference ((Current-Last)/Last)
  std::vector<TH2F_LW*> m_v_emHVScales;     ///< EM Mean HV Scale by partition and layer
  std::vector<TH2F_LW*> m_v_hadHVScales;    ///< Had Mean HV Scale by partition and layer
  std::vector<TH2F_LW*> m_v_emHVEntries;    ///< EM Mean HV Scale number of entries by partition and layer (temporary)
  std::vector<TH2F_LW*> m_v_hadHVEntries;   ///< Had Mean HV Scale number of entries by partition and layer (temporary)
  std::vector<TH2F_LW*> m_v_emHVScalesRef;  ///< EM Mean HV Scale Last Calibration by partition and layer
  std::vector<TH2F_LW*> m_v_hadHVScalesRef; ///< Had Mean HV Scale Last Calibration by partition and layer
  std::vector<TH2F_LW*> m_v_emHVScalesDif;  ///< EM Mean HV Scale Difference ((Current-Last)/Last) by partition and layer
  std::vector<TH2F_LW*> m_v_hadHVScalesDif; ///< Had Mean HV Scale Difference ((Current-Last)/Last) by partition and layer
  TH2F_LW* m_h_emFEB;                       ///< EM Towers with Missing FEB
  TH2F_LW* m_h_hadFEB;                      ///< Had Towers with Missing FEB
  TH2F_LW* m_h_emDisabled;                  ///< EM Disabled Towers
  TH2F_LW* m_h_hadDisabled;                 ///< LAr Had Disabled Towers
  
  static const int s_numEmHVPlots  = 8;     ///< Number of EM partitions/layers
  static const int s_numHadHVPlots = 5;     ///< Number of Had partitions/layers
};
// ============================================================================
}  // end namespace
// ============================================================================
#endif
