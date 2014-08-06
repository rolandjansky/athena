// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TriggerD3PDMaker/src/TriggerBitFillerTool.h
 * @author Haifeng Li <Haifeng.Li@cern.ch>   
 * @date Sep 03, 2009
 * @brief Block filler tool for trigger decisions.
 */


#ifndef TRIGGERD3PDMAKER_TRIGGERBITFILLERTOOL_H
#define TRIGGERD3PDMAKER_TRIGGERBITFILLERTOOL_H

#include "D3PDMakerUtils/VoidBlockFillerTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include <vector>
#include <string>
#include <set>
#include <map>
namespace Trig {
  class TrigDecisionTool;
}
class IIncidentSvc;


namespace D3PD {


class TriggerKeyTool;


/**
 * @brief Block filler tool for trigger decisions.
 */
class TriggerBitFillerTool
  : public BlockFillerTool<void>,
    public IIncidentListener
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TriggerBitFillerTool (const std::string& type,
                        const std::string& name,
                        const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();


  /// Book variables for this block.
  virtual StatusCode book();


  /**
   * @brief Fill one block.
   *
   * This is called once per event.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill ();


  /// Function called when a relevant incident happens.
  virtual void handle (const Incident& inc);


private:
  /// Property: Input list of trigger names.  May be regexps.
  std::vector<std::string> m_triggers;

  /// Property: Trigger decision tool instance.
  ToolHandle<Trig::TrigDecisionTool> m_trigDec;

  /// Property: Incident service instance.
  ServiceHandle<IIncidentSvc> m_incidentSvc;

  /// Property: TriggerKeyTool instance.
  ToolHandle<TriggerKeyTool> m_triggerKeyTool;

  /// Property: Allow adding to the list of bits if the menu changes?
  bool m_allowExtend;


  /// Set of trigger bits we're filling.
  typedef std::map<std::string, bool*> TrigNameMap;
  TrigNameMap m_trigNameMap;


  /// Set of trigger configuration keys we've seen.
  std::set<std::string> m_keysSeen;
};


} // namespace D3PD


#endif // not TRIGGERD3PDMAKER_TRIGGERBITFILLERTOOL_H
