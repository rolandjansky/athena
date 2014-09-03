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
 * $Id: ICoreDumpSvc.h,v 1.2 2008-05-28 10:55:04 fwinkl Exp $
 */

#ifndef GAUDIKERNEL_IINTERFACE_H
 #include "GaudiKernel/IInterface.h"
#endif
#ifndef _CPP_STRING
 #include <string>
#endif

/**
 * @class  ICoreDumpSvc
 * @brief  Interface of a core dump service
 * @author Frank Winklmeier
 *
 * This interface allows to set and print core dump records.
 */

class ICoreDumpSvc: virtual public IInterface {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID();

  /// Virtualize D'tor
  virtual ~ICoreDumpSvc() {}

  /// Set a name/value pair in the core dump record
  virtual void setCoreDumpInfo( const std::string& name, const std::string& value ) = 0;
  
  /// Print all core dump records
  virtual std::string dump() const = 0;
};


inline const InterfaceID& ICoreDumpSvc::interfaceID()
{
  static const InterfaceID IID_ICoreDumpSvc("ICoreDumpSvc", 1, 0);
  return IID_ICoreDumpSvc;
}

#endif // ATHENAKERNEL_ICOREDUMPSVC_H
