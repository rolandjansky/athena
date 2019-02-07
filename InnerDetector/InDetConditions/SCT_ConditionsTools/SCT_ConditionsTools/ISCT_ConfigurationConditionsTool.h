/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_ConfigurationConditionsTool.h
 * interface file for service that keeps track of configuration conditions
 * @author gwilliam@mail.cern.ch
 **/

#ifndef ISCT_ConfigurationConditionsTool_h
#define ISCT_ConfigurationConditionsTool_h

#include <set>
#include <map>

#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsTools/ISCT_ConditionsTool.h"

class Identifier;
class IdentifierHash;

/**
 * @class SCT_ConfigurationConditionsTool
 * Service that keeps track of configuration conditions
 **/

class ISCT_ConfigurationConditionsTool: virtual public ISCT_ConditionsTool {

 public:
  //@name Service methods
  //@{

  virtual ~ISCT_ConfigurationConditionsTool() = default;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISCT_ConfigurationConditionsTool, 1, 0);

  //@}

  /** Set of bad module Identifiers */
  virtual const std::set<Identifier>* badModules(const EventContext& ctx) const =0;
  virtual const std::set<Identifier>* badModules() const =0;

  /** Map of link status for all modules with at least one bad link (true = good, false = bad)*/
  virtual const std::map<IdentifierHash, std::pair<bool, bool> >* badLinks(const EventContext& ctx) const =0;
  virtual const std::map<IdentifierHash, std::pair<bool, bool> >* badLinks() const =0;
  /** Link status for a particluar module (true = good, false = bad)*/
  virtual std::pair<bool, bool> badLinks(const IdentifierHash& hash, const EventContext& ctx) const =0;
  virtual std::pair<bool, bool> badLinks(const IdentifierHash& hash) const =0;

  /** Map of chips status for all modules with at least one bad chip (packed as 1st 12 bits of unsigned int)*/
  virtual const std::map<Identifier, unsigned int>* badChips(const EventContext& ctx) const =0;
  virtual const std::map<Identifier, unsigned int>* badChips() const =0;
  /** Chips status for a particular module (packed as 1st 12 bits of unsigned int)*/ 
  virtual unsigned int badChips(const Identifier& moduleId, const EventContext& ctx) const =0;
  virtual unsigned int badChips(const Identifier& moduleId) const =0;

  /** Set of bad strip Identifiers */ 
  virtual void badStrips(std::set<Identifier>& strips, const EventContext& ctx, bool ignoreBadModules=false, bool ignoreBadChips=false) const =0;
  virtual void badStrips(std::set<Identifier>& strips, bool ignoreBadModules=false, bool ignoreBadChips=false) const =0;
  /** Set of bad strip Identifiers for a particular module*/
  virtual void badStrips(const Identifier& moduleId, std::set<Identifier>& strips, const EventContext& ctx, bool ignoreBadModules=false, bool ignoreBadChips=false) const =0;
  virtual void badStrips(const Identifier& moduleId, std::set<Identifier>& strips, bool ignoreBadModules=false, bool ignoreBadChips=false) const =0;

  /** Get the chip number containing a particular strip*/
  virtual int getChip(const Identifier& stripId, const EventContext& ctx) const =0;
  virtual int getChip(const Identifier& stripId) const =0;

 private:
};

#endif // ISCT_ConfigurationConditionsTool_h
