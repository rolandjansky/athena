/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCTMotherTrigMonTool.h
*
* @author Jose E. Garcia <Jose.Enrique.Garcia@cern.ch> @n
* modified s.roe, 2/9/2009
*
****************************************************************************/
#ifndef SCTMotherTrigMonTool_H
#define SCTMotherTrigMonTool_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

//Standard C++
#include <string>
#include <bitset>

//Fwd declarations
class IInterface;
class StatusCode;

/// This tool derives from ManagedMonitorToolBase
class SCTMotherTrigMonTool : public ManagedMonitorToolBase {
public:
  ///Constructor    
  SCTMotherTrigMonTool( const std::string& type, const std::string& name,const IInterface* parent); 

  ///Destructor
  virtual ~SCTMotherTrigMonTool() = default;

  ///Tool initialization
  virtual StatusCode initialize();  

protected:
  ///enumerated constant for the types of level 1 triggers, corresponds to the string m_triggerNames
  enum TriggerTypes {RNDM=0, BPTX, L1CAL, TGC, RPC, MBTS, COSM, CALIB, N_TRIGGER_TYPES };
  ///Fill the m_firedTriggers bitset according to event information
  StatusCode checkTriggers();
  //@deprecated improper camel casing on old method
  StatusCode CheckTriggers(){ return checkTriggers();}
  ///Get the status of a particular trigger bit (trigger bit 0-7)
  bool hasTriggerFired(const unsigned int trigger) const;
  ///Check if the event belongs to Stream = StreamName
  bool isStream(const std::string& StreamName);
  ///Check if the event passes L1 item
  bool isCalibrationNoise(const std::string& L1_Item);
  ///Abbreviations for level 1 trigger types
  static const std::string m_triggerNames[8];
  ///Format and output (INFO msg level) the fired triggers
  StatusCode printTriggers() const;
  //@deprecated improper camel casing on old method
  StatusCode PrintTriggers() const {return printTriggers();}
  ///Unused in this class... why is it here?..it gets picked up in derived class TracksMonTool
  bool m_doTrigger;
  /// Check if the event belongs to the SCTNoise Calibration Stream
  bool m_isStream;
protected:
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "EventInfoKey", "EventInfo", "Key of xAOD::EventInfo"};
private:
  std::bitset<N_TRIGGER_TYPES> m_firedTriggers;

};

#endif
