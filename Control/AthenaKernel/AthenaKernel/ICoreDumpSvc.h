/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_ICOREDUMPSVC_H
#define ATHENAKERNEL_ICOREDUMPSVC_H

/**
 * @file   ICoreDumpSvc.h
 * @brief  Interface of a core dump service
 * @author Frank Winklmeier
 *
 */

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/EventContext.h"
#include <string>

/**
 * @class  ICoreDumpSvc
 * @brief  Interface of a core dump service
 * @author Frank Winklmeier
 *
 * This interface allows to set and print core dump records.
 */

class ICoreDumpSvc: virtual public IInterface {
public:
  DeclareInterfaceID(ICoreDumpSvc, 1, 0);

  /// Virtualize D'tor
  virtual ~ICoreDumpSvc() {}

  /// Set a name/value pair in the core dump record
  virtual void setCoreDumpInfo( const std::string& name, const std::string& value ) = 0;

  /// Set a name/value pair in the core dump record for given EventContext
  virtual void setCoreDumpInfo( const EventContext& ctx, const std::string& name, const std::string& value ) = 0;
  
  /// Print all core dump records
  virtual std::string dump() const = 0;
};

#endif // ATHENAKERNEL_ICOREDUMPSVC_H
