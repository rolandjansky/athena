/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigT1CaloBSMon.h
// PACKAGE:  TrigT1CaloMonitoring
//
// AUTHOR:   Peter Faulkner
//	     
//           Force read of all L1Calo containers for cpu accounting
//
// ********************************************************************
#ifndef TRIGT1CALOBSMON_H
#define TRIGT1CALOBSMON_H

#include "GaudiKernel/ToolHandle.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

class StatusCode;

// ============================================================================
namespace LVL1 {
// ============================================================================
class IL1CaloMonitoringCaloTool;

/** Force load of L1Calo containers and/or CaloCell container.
 *
 *  Allows separate accounting of cpu time for data unpacking.
 *  For testing purposes only, not used in normal running.
 *
 *  <b>Tools Used:</b>
 *
 *  <table>
 *  <tr><th> Tool                               </th><th> Description          </th></tr>
 *  <tr><td> @c LVL1::IL1CaloMonitoringCaloTool </td><td> @copydoc m_caloTool  </td></tr>
 *  </table>
 *
 *  <b>JobOption Properties:</b>
 *
 *  <table>
 *  <tr><th> Property         </th><th> Description          </th></tr>
 *  <tr><td> @c LoadL1Calo    </td><td> @copydoc m_l1calo    </td></tr>
 *  <tr><td> @c LoadL1CaloPPM </td><td> @copydoc m_l1caloPPM </td></tr>
 *  <tr><td> @c LoadL1CaloCPM </td><td> @copydoc m_l1caloCPM </td></tr>
 *  <tr><td> @c LoadL1CaloJEM </td><td> @copydoc m_l1caloJEM </td></tr>
 *  <tr><td> @c LoadL1CaloROD </td><td> @copydoc m_l1caloROD </td></tr>
 *  <tr><td> @c LoadCaloCells </td><td> @copydoc m_caloCells </td></tr>
 *  </table>
 *
 *  @author Peter Faulkner
 *
 */

class TrigT1CaloBSMon: public ManagedMonitorToolBase
{

public:
  
  TrigT1CaloBSMon(const std::string & type, const std::string & name,
		                            const IInterface* parent);
    

  virtual ~TrigT1CaloBSMon();

  virtual StatusCode initialize();
  virtual StatusCode fillHistograms();

private:

  /// CaloCell info by TT tool (forces CaloCell read)
  ToolHandle<LVL1::IL1CaloMonitoringCaloTool> m_caloTool;

  /// Switch for all L1Calo
  bool m_l1calo;
  /// Switch for L1Calo PPM only
  bool m_l1caloPPM;
  /// Switch for L1Calo CPM only
  bool m_l1caloCPM;
  /// Switch for L1Calo JEM only
  bool m_l1caloJEM;
  /// Switch for L1Calo ROD only
  bool m_l1caloROD;
  /// Switch for CaloCells
  bool m_caloCells;

  /// Trigger Tower container StoreGate key
  std::string m_triggerTowerLocation;
  /// CPM core tower container StoreGate key
  std::string m_cpmTowerLocation;
  /// CPM overlap tower container StoreGate key
  std::string m_cpmTowerLocationOverlap;
  /// CPM hits container StoreGate key
  std::string m_cpmHitsLocation;
  /// CMM-CP hits container StoreGate key
  std::string m_cmmCpHitsLocation;
  /// CPM RoI container StoreGate key
  std::string m_cpmRoiLocation;
  /// Core Jet Element container StoreGate key
  std::string m_jetElementLocation;
  /// Overlap Jet Element container StoreGate key
  std::string m_jetElementLocationOverlap;
  /// JEM hits container StoreGate key
  std::string m_jemHitsLocation;
  /// CMM-Jet hits container StoreGate key
  std::string m_cmmJetHitsLocation;
  /// JEM RoI container StoreGate key
  std::string m_jemRoiLocation;
  /// CMM RoI container StoreGate key
  std::string m_cmmRoiLocation;
  /// JEM Et sums container StoreGate key
  std::string m_jemEtSumsLocation;
  /// CMM Et sums container StoreGate key
  std::string m_cmmEtSumsLocation;
  /// ROD header container StoreGate key
  std::string m_rodHeaderLocation;
  /// CP RoIB ROD header container StoreGate key
  std::string m_cpRoibRodHeaderLocation;
  /// JEP RoIB ROD header container StoreGate key
  std::string m_jepRoibRodHeaderLocation;
  /// ROB and Unpacking Error vector StoreGate key
  std::string m_robErrorVectorLocation;

};

// ============================================================================
}  // end namespace
// ============================================================================
#endif
