/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// ********************************************************************
//
// NAME:     L1CaloPMTScoresMon.h
// PACKAGE:  TrigT1Monitoring
//
// AUTHOR:   Peter Faulkner
//
// ********************************************************************
#ifndef TRIGT1MONITORING_L1CALOPMTSCORESMON_H
#define TRIGT1MONITORING_L1CALOPMTSCORESMON_H

#include <string>
#include <vector>

#include "GaudiKernel/ToolHandle.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloMatchCell2Tower.h"

class StatusCode;
class TH2F_LW;

class CaloCell;
class ITileBadChanTool;
class TileHWID;
// ============================================================================
namespace LVL1 {
// ============================================================================
// Forward declarations:
// ============================================================================
class IL1CaloTTIdTools;
class IL1TriggerTowerTool;
class ITrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramTool;
// ============================================================================

/** Monitoring of mean Tile PMT gains by TriggerTower
 *
 *  Only runs on the first good event.
 *
 *  <b>ROOT Histogram Directories:</b>
 *
 *  <table>
 *  <tr><th> Directory
 *    </th><th> Contents                                   </th></tr>
 *  <tr><td> @c LVL1_Interfaces/Calorimeter/PMTGainScores
 *    </td><td> Disabled towers in Tile                    </td></tr>
 *  <tr><td> @c LVL1_Interfaces/Calorimeter/PMTGainScores/PMTCurrent
 *    </td><td> Current PMT gains by partition and overall </td></tr>
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
 *  <tr><td> @c CaloCellContainer    </td><td> For Tile calo data </td></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::TriggerTower> </td><td> PPM data          </td></tr>
 *  </table>
 *
 *  <b>Tools Used:</b>
 *
 *  <table>
 *  <tr><th> Tool                                </th><th> Description                </th></tr>
 *  <tr><td> @c LVL1::IL1TriggerTowerTool        </td><td> @copydoc m_ttTool          </td></tr>
 *  <tr><td> @c TrigT1CaloMonErrorTool           </td><td> @copydoc m_errorTool       </td></tr>
 *  <tr><td> @c TrigT1CaloLWHistogramTool        </td><td> @copydoc m_histTool        </td></tr>
 *  <tr><td> @c LVL1::IL1CaloMatchCell2Tower     </td><td> @copydoc m_cellMatch     </td></tr>
 *  <tr><td> @c LVL1::IL1CaloTTIdTools           </td><td> @copydoc m_ttIdTools       </td></tr>
 *  <tr><td> @c ITileBadChanTool                 </td><td> @copydoc m_tileBadChanTool </td></tr>
 *  </table>
 *
 *  <b>JobOption Properties:</b>
 *
 *  <table>
 *  <tr><th> Property                 </th><th> Description                          </th></tr>
 *  <tr><td> @c TileBadChanTool       </td><td> @copydoc m_tileBadChanTool           </td></tr>
 *  <tr><td> @c CaloCellContainer     </td><td> @copydoc m_caloCellContainerName     </td></tr>
 *  <tr><td> @c TriggerTowerContainer </td><td> @copydoc m_TriggerTowerContainerName </td></tr>
 *  <tr><td> @c PathInRootFile        </td><td> @copydoc m_PathInRootFile            </td></tr>
 *  </table>
 *
 *  <!--
 *  <b>Related Documentation:</b>
 *  -->
 *
 *  @author Peter Faulkner
 *
 */

class L1CaloPMTScoresMon: public ManagedMonitorToolBase
{

 public:
  
  L1CaloPMTScoresMon(const std::string & type, const std::string & name,
		     const IInterface* parent);
    

  virtual ~L1CaloPMTScoresMon();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

private:

  /// Return Tile PMT Gain score for a cell
  double tileNonNominal(const CaloCell* cell);

  /// Tool for identifiers and disabled channels
  ToolHandle<LVL1::IL1TriggerTowerTool> m_ttTool;
  /// Event veto tool
  ToolHandle<ITrigT1CaloMonErrorTool>    m_errorTool;
  /// Histogram helper tool
  ToolHandle<TrigT1CaloLWHistogramTool> m_histTool;
  /// CaloCell to TriggerTower matching tool
  ToolHandle<LVL1::IL1CaloMatchCell2Tower> m_cellMatch
    { this, "L1CaloMatchCell2Tower",  "LVL1::L1CaloMatchCell2Tower",  "L1CaloMatchCell2Tower" };
  /// Tool for Identifier to eta/phi mappings
  ToolHandle<LVL1::IL1CaloTTIdTools> m_ttIdTools;
  /// Tool for PMT gain status
  ToolHandle<ITileBadChanTool> m_tileBadChanTool;
  /// Tile identifier helper
  const TileHWID* m_tileHWID;

  /// CaloCellContainer StoreGate key
  std::string m_caloCellContainerName;
  /// xAODTriggerTower collection StoreGate key
  std::string m_xAODTriggerTowerContainerName;

  /// Root histograms directory
  std::string m_PathInRootFile;

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey
    { this, "CablingKey", "LArOnOffIdMap", "SG Key of LArOnOffIdMapping object" };

  /// Event counter
  int  m_events;
  /// Histograms booked flag
  bool m_histBooked;

  TH2F_LW* m_h_hadPMTScore;               ///< Tile PMT Gain Scores
  std::vector<TH2F_LW*> m_v_hadPMTScores; ///< Tile PMT Gain Scores by layer
  TH2F_LW* m_h_hadDisabled;               ///< Tile Disabled Towers
  
  static const int s_numPMTPlots   = 3;   ///< Number of layers
};
// ============================================================================
}  // end namespace
// ============================================================================
#endif
