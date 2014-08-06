// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TriggerD3PDMaker/TriggerKeyTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2011
 * @brief Logic to fetch the current trigger key.
 */


#ifndef TRIGGERD3PDMAKER_TRIGGERKEYTOOL_H
#define TRIGGERD3PDMAKER_TRIGGERKEYTOOL_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include <utility>
#include <string>
namespace TrigConf {
  class ITrigConfigSvc;
}


namespace D3PD {


/// Gaudi interface definition.
static const InterfaceID
IID_TriggerKeyTool ("D3PD::TriggerKeyTool", 1, 0);



/**
 * @brief Logic to fetch the current trigger key.
 *
 * Generally, a trigger configuration is defined by a supermaster key (SMK)
 * and two sets of prescale keys, for L1 and HLT.  These keys can be
 * retrieved from the trigger configuration service, and when
 * they exist, that's the end of the story.
 *
 * However, MC files do not necessarily have a SMK, and generally won't
 * have prescale keys either.  In such a case, we can fake a SMK
 * by retrieving the name of the script used to define the trigger
 * and hashing it.
 *
 * This tool factors out this process.
 *
 * It returns the trigger configuration in one of two forms:
 *
 *   - pair<SMK, pair<L1PRES, HLTPRES> >
 *     If the SMK doesn't exist, a hashed version of the script will be used
 *     instead.  The prescale keys will generally be 0 for MC.
 *
 *   - string
 *     This is either the SMK encoded into a string or the script name.
 */
class TriggerKeyTool
  : public AthAlgTool
{
public:
  /// Type to hold SMK and prescale keys.
  typedef std::pair<int, std::pair<int, int> > Keys;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TriggerKeyTool (const std::string& type,
                  const std::string& name,
                  const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();


  /**
   * @brief Return the current list of trigger keys.
   *
   * Failure is indicated by first==-1.
   */
  Keys getKeys() const;


  /**
   * @brief Return a string describing the trigger configuration
   * (excluding prescales).
   *
   * Failure is indicated by an empty string.
   */
  std::string getString() const;


  /// Gaudi interface definition.
  static const InterfaceID& interfaceID(); 


private:
  /// Property: Trigger configuration service instance.
  ServiceHandle<TrigConf::ITrigConfigSvc> m_trigConfigSvc;

  /// Property: DS configuration instance (sometimes used when reading MC).
  ServiceHandle< TrigConf::ITrigConfigSvc > m_dsSvc;
};


/// Gaudi interface definition.
inline const InterfaceID& TriggerKeyTool::interfaceID()
{
  return IID_TriggerKeyTool;
}


} // namespace D3PD



#endif // not TRIGGERD3PDMAKER_TRIGGERKEYTOOL_H
