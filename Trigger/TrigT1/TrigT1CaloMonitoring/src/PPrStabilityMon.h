/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:        PPrStabilityMon.h
// PACKAGE:     TrigT1CaloMonitoring
//
// Author:      Rohin T Narayan (narayan@physik.uni-heidelberg.de)
//              Universitaet Heidelberg
//              Patrick Rieck - rieck@physik.hu-berlin.de
//
// ********************************************************************

#ifndef TRIGT1CALOMONITORING_PPRSTABILITYMON_H
#define TRIGT1CALOMONITORING_PPRSTABILITYMON_H

#include <string>
#include <vector>

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

class StatusCode;
class EventInfo;


class L1CaloPprFineTimePlotManager;
class L1CaloPprPedestalPlotManager;
class L1CaloPprPedestalCorrectionPlotManager;
class L1CaloPprEtCorrelationPlotManager;

namespace Trig {
class IBunchCrossingTool;
}
// ============================================================================
namespace LVL1 {
// ============================================================================
// Forward declarations:
// ============================================================================
class IL1TriggerTowerTool;
class ITrigT1CaloMonErrorTool;
class TrigT1CaloLWHistogramTool;

/** This class does stability monitoring by lumiblock
 *  of entities to be defined by the properties declared.
 *
 *  The monitoring is done for each TriggerTower except on Tier0
 *  which has summary plots only.
 * 
 *  The class uses L1CaloPprEtCorrelationPlotManager, L1CaloPprFineTimePlotManager
 *  and L1CaloPprPedestalPlotManager, sub-classes of L1CaloPprPlotManager,
 *  from the package TrigT1CaloCalibTools to generate the monitoring histograms.
 *
 *  <b>ROOT Histogram Directories (Tier0):</b>
 *
 *  <table>
 *  <tr><th> Directory                             </th><th> Contents                        </th></tr>
 *  <tr><td> @c L1Calo/PPM/Stability/EtCorrelation </td><td> L1Calo/Calo Et correlation by lumiblock <br>
 *                                                           Ditto for each partition                <br>
 *                                                           Eta/phi profile of correlation by
 *                                                           TriggerTower                            <br>
 *                                                           Eta/phi RMS of correlation by
 *                                                           TriggerTower                    </td></tr>
 *  <tr><td> @c L1Calo/PPM/Stability/FineTime      </td><td> Ditto for fine time difference
 *                                                           from reference                  </td></tr>
 *  <tr><td> @c L1Calo/PPM/Stability/Pedestal      </td><td> Ditto for pedestal difference
 *                                                           from mean                       </td></tr>
 *  <tr><td> @c L1Calo/PPM/Stability/PedestalCorrection  </td><td> Ditto for pedestalCorrection
 *                                                                                 </td></tr>
 *  </table>
 *
 *  <b>ROOT Histogram Directories (online):</b>
 *
 *  <table>
 *  <tr><th> Directory                                         </th><th> Contents            </th></tr>
 *  <tr><td> @c L1Calo/PPrStabilityMon/EtCorrelation           </td><td> As Tier0            </td></tr>
 *  <tr><td> @c L1Calo/PPrStabilityMon/EtCorrelation/Partition <br>
 *           where @c Partition is @c LArEMBA, @c TileEBA etc. </td><td> L1Calo/Calo Et correlation by lumiblock
 *                                                                       for each tower in the partition
 *                                                                                           </td></tr>
 *  <tr><td> @c L1Calo/PPrStabilityMon/FineTime                </td><td> As Tier0            </td></tr>
 *  <tr><td> @c L1Calo/PPrStabilityMon/FineTime/Partition      </td><td> Ditto for fine time difference
 *                                                                       from reference      </td></tr>
 *  <tr><td> @c L1Calo/PPrStabilityMon/Pedestal                </td><td> As Tier0            </td></tr>
 *  <tr><td> @c L1Calo/PPrStabilityMon/Pedestal/Partition      </td><td> Ditto for pedestal difference
 *                                                                       from mean           </td></tr>
 *  <tr><td> @c L1Calo/PPrStabilityMon/PedestalCorrection      </td><td> As Tier0            </td></tr>
 *  <tr><td> @c L1Calo/PPrStabilityMon/PedestalCorrection/Partition  </td><td> Ditto for pedestalCorrection
 *                                                                                           </td></tr>
 *  </table>
 *
 *  <!--
 *  <b>Notes on Particular Histograms:</b>
 *
 *  <table>
 *  <tr><th> Histogram                     </th><th> Comment                     </th></tr>
 *  <tr><td>                               </td><td>                             </td></tr>
 *  </table>
 *  -->
 *
 *  <b>Custom Merges Used (Tier0):</b>
 *
 *  <table>
 *  <tr><th> Merge                                           </th><th> Used For           </th></tr>
 *  <tr><td> @ref MergesUsedsection     "@c mergeRebinned "  </td><td> By lumiblock plots </td></tr>
 *  <tr><td> @ref PostProcessingsection "@c PostProcessing " </td><td> RMS plots          </td></tr>
 *  </table>
 *
 *  <b>StoreGate Containers Used:</b>
 *
 *  <table>
 *  <tr><th> Container               </th><th> Comment                                      </th></tr>
 *  <tr><td> @c DataVector
 *           @c <LVL1::TriggerTower> </td><td> PPM data                                     </td></tr>
 *  <tr><td> @c CaloCellContainer    </td><td> Calo data. EtCorrelation and FineTime only   </td></tr>
 *  <tr><td> @c EventInfo            </td><td> For run and lumiblock numbers via @c EventID </td></tr>
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
 *  <tr><th> Property                     </th><th> Description                          </th></tr>
 *  <tr><td> @c BS_TriggerTowerContainer  </td><td> @copydoc m_TriggerTowerContainerName </td></tr>
 *  <tr><td> @c BS_xAODTriggerTowerContainer  </td><td> @copydoc m_xAODTriggerTowerContainerName </td></tr>
 *  <tr><td> @c ppmADCMinValue            </td><td> @copydoc m_ppmADCMinValue            </td></tr>
 *  <tr><td> @c ppmADCMaxValue            </td><td> @copydoc m_ppmADCMaxValue            </td></tr>
 *  <tr><td> @c PathInRootFile            </td><td> @copydoc m_PathInRootFile            </td></tr>
 *  <tr><td> @c doFineTimeMonitoring      </td><td> @copydoc m_doFineTimeMonitoring      </td></tr>
 *  <tr><td> @c doPedestalMonitoring      </td><td> @copydoc m_doPedestalMonitoring      </td></tr>
 *  <tr><td> @c doPedestalCorrectionMonitoring </td><td> @copydoc m_doPedestalCorrectionMonitoring </td></tr>
 *  <tr><td> @c doEtCorrelationMonitoring </td><td> @copydoc m_doEtCorrelationMonitoring </td></tr>
 *  <tr><td> @c lumiMax                   </td><td> @copydoc m_lumiBlockMax              </td></tr>
 *  <tr><td> @c fineTimeCut               </td><td> @copydoc m_fineTimeCut               </td></tr>
 *  <tr><td> @c pedestalMaxWidth          </td><td> @copydoc m_pedestalMaxWidth          </td></tr>
 *  <tr><td> @c EtMinForEtCorrelation     </td><td> @copydoc m_EtMinForEtCorrelation     </td></tr>
 *  <tr><td> @c doCaloQualCut             </td><td> @copydoc m_doCaloQualCut             </td></tr>
 *  </table>
 *
 *  <!--
 *  <b>Related Documentation:</b>
 *  -->
 *
 *  @authors Rohin T Narayan, Patrick Rieck, Julia I. Hofmann
 *
 * */

class PPrStabilityMon: public ManagedMonitorToolBase, public virtual IIncidentListener
{

 public:
  
  PPrStabilityMon(const std::string & type, const std::string & name,const IInterface* parent);
  virtual ~PPrStabilityMon();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();
  virtual StatusCode bookHistogramsRecurrent();
  
  virtual void handle(const Incident& I);

private:
  
  void parseBeamIntensityPattern();
  std::vector<std::pair<bool, int16_t>> m_distanceFromHeadOfTrain;
  
  /// Cut on ADC minimum value, used for fine time monitoring
  unsigned int m_ppmADCMinValue;
  /// Cut on ADC maximum value, used for fine time monitoring
  unsigned int m_ppmADCMaxValue;
  /// Maximum number of lumiblocks in stability plots
  unsigned int m_lumiBlockMax;

  /// Tool to retrieve bytestream errors
  ToolHandle<ITrigT1CaloMonErrorTool>    m_errorTool;
  /// Histogram helper tool
  ToolHandle<TrigT1CaloLWHistogramTool> m_histTool;
  /// Tool for identifiers and database info
  ToolHandle<IL1TriggerTowerTool> m_ttTool;
  /// Tool to retrieve bunch structure
  ToolHandle<Trig::IBunchCrossingTool> m_bunchCrossingTool;
  /// Manager for fine time plots
  L1CaloPprFineTimePlotManager*         m_fineTimePlotManager;
  /// Manager for pedestal plots
  L1CaloPprPedestalPlotManager*         m_pedestalPlotManager;
  /// Manager for pedestalCorrection plots
  L1CaloPprPedestalCorrectionPlotManager*	m_pedestalCorrectionPlotManager;
  /// Manager for Et correlation plots
  L1CaloPprEtCorrelationPlotManager*    m_etCorrelationPlotManager;
  
  /// Flag for fine time monitoring
  bool m_doFineTimeMonitoring;
  /// Flag for pedestal monitoring
  bool m_doPedestalMonitoring;
  /// Flag for pedestalCorrection monitoring
  bool m_doPedestalCorrectionMonitoring;
  /// Flag for Et correlation monitoring
  bool m_doEtCorrelationMonitoring;

  /// TriggerTower Container key
  std::string m_TriggerTowerContainerName;
  std::string m_xAODTriggerTowerContainerName;
  /// Root directory
  std::string m_PathInRootFile;

  /// Event information container
  const EventInfo* m_evtInfo;
  /// Maximum acceptable fine time value
  double m_fineTimeCut;
  /// Maximum acceptable pedestal deviation from mean
  double m_pedestalMaxWidth;
  /// Minimum Et cut for Et correlation
  double m_EtMinForEtCorrelation;
  /// Switch for calo quality cut via job options in fine time 
  bool m_doCaloQualCut;
  
};

// ============================================================================
}  // end namespace
// ============================================================================

#endif
