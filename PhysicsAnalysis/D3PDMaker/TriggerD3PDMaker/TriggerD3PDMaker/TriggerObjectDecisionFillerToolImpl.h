// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TriggerD3PDMaker/src/TriggerObjectDecisionFillerToolImpl.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2010
 * @brief Record which triggers are passed by a given trigger object:
 *         non-template dependent portion.
 */


#ifndef TRIGGERD3PDMAKER_TRIGGEROBJECTDECISIONFILLERTOOLIMPL_H
#define TRIGGERD3PDMAKER_TRIGGEROBJECTDECISIONFILLERTOOLIMPL_H


#include "TriggerD3PDMaker/TriggerKeyTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/AlgTool.h"
#include <vector>
#include <string>
#include <set>
#include <map>
namespace Trig {
  class TrigDecisionTool;
}
class IIncidentSvc;
class TrigMatchTool;


namespace D3PD {


class BlockFillerToolImpl;


/**
 * @brief Record which triggers are passed by a given trigger object.
 *
 * This is a helper class for @c TriggerObjectDecisionFillerTool.
 * It's used to factor out the pieces that don't depend on the template arg.
 */
class TriggerObjectDecisionFillerToolImpl
{
public:
  /**
   * @brief Constructor.
   * @param tool The tool holding this object.
   * @param name The tool name.
   */
  TriggerObjectDecisionFillerToolImpl (BlockFillerToolImpl& tool,
                                       const std::string& name);


  /// Initialize the object.
  StatusCode initialize();


  /// Book variables for this block.
  StatusCode book();


  /**
   * @brief Fill variables for one object.
   * @param chains List of chain names which the object passed.
   */
  StatusCode fill (const std::vector<std::string>& chains);


  /// Return the matching tool.
  ToolHandle<TrigMatchTool>& matchTool();


  /// Function called when a relevant incident happens.
  void handle (const Incident& inc);


private:
  /// Property: Input list of trigger chains.  May be regexps.
  std::vector<std::string> m_chains;

  /// Property: Trigger decision tool instance.
  ToolHandle<Trig::TrigDecisionTool> m_trigDec;

  /// Property: The trigger matching tool.
  ToolHandle<TrigMatchTool> m_matchTool;

  /// Property: Incident service instance.
  ServiceHandle<IIncidentSvc> m_incidentSvc;

  /// Property: TriggerKeyTool instance.
  ToolHandle<TriggerKeyTool> m_triggerKeyTool;

  /// Property: Allow adding to the list of bits if the menu changes?
  bool m_allowExtend;


  /// Name/variable pointer pair for one trigger bit.
  struct TrigBit
  {
    TrigBit (const std::string& the_name) : name (the_name) {}
    std::string name;
    bool* value;
  };

  /// Set of trigger configuration keys we've seen.
  std::set<std::string> m_keysSeen;

  /// Set of trigger bits we're filling.
  typedef std::map<std::string, bool*> TrigNameMap;
  TrigNameMap m_trigNameMap;

  /// The parent tool.
  BlockFillerToolImpl& m_tool;
};


} // namespace D3PD


#endif // not TRIGGERD3PDMAKER_TRIGGEROBJECTDECISIONFILLERTOOLIMPL_H
