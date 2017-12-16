/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ISCT_ConfigurationConditionsSvc.h
 * interface file for service that keeps track of configuration conditions
 * @author gwilliam@mail.cern.ch
**/

#ifndef ISCT_ConfigurationConditionsSvc_h
#define ISCT_ConfigurationConditionsSvc_h

#include <set>
#include <list>
#include <map>

#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SCT_ConditionsServices/ISCT_ConditionsSvc.h"

class Identifier;
class IdentifierHash;
class StatusCode;


/**
 * @class SCT_ConfigurationConditionsSvc
 * Service that keeps track of configuration conditions
**/

class ISCT_ConfigurationConditionsSvc: virtual public ISCT_ConditionsSvc {

public:
  //@name Service methods
  //@{

  virtual ~ISCT_ConfigurationConditionsSvc(){}

  //  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID & interfaceID();

  //@}

  /** Set of bad module Identifiers */
  virtual std::set<Identifier>*                          badModules()                                                                                        = 0;  

  /** Map of link status for all modules with at least one bad link (true = good, false = bad)*/
  virtual std::map<Identifier, std::pair<bool, bool> >*  badLinks()                                                                                          = 0;
  /** Link status for a particluar module (true = good, false = bad)*/
  virtual std::pair<bool, bool>                          badLinks(const Identifier & moduleId)                                                                       = 0;

  /** Map of chips status for all modules with at least one bad chip (packed as 1st 12 bits of unsigned int)*/
  virtual std::map<Identifier, unsigned int>*            badChips()                                                                                          = 0;  
  /** Chips status for a particular module (packed as 1st 12 bits of unsigned int)*/ 
  virtual unsigned int                                   badChips(const Identifier & moduleId) const                                                                      = 0;

  /** Set of bad strip Identifiers */ 
  virtual void                                           badStrips(std::set<Identifier>& strips, bool ignoreBadModules = false, bool ignoreBadChips = false) = 0;  
  /** Set of bad strip Identifiers for a particular module*/
  virtual void                                           badStrips(const Identifier & moduleId, std::set<Identifier>& strips, bool ignoreBadModules = false, bool ignoreBadChips = false) = 0;

  /** Get the chip number containing a particular strip*/
  virtual int                                            getChip(const Identifier & stripId) const = 0;

private:

};

inline const InterfaceID & ISCT_ConfigurationConditionsSvc::interfaceID(){
  static const InterfaceID IID_SCT_ConfigurationConditionsSvc("SCT_ConfigurationConditionsSvc",1,0);
  return IID_SCT_ConfigurationConditionsSvc;
}

#endif
