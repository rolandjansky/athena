/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IRESETABLE_H
#define ATHENAKERNEL_IRESETABLE_H

#include <string>

/** @class IResetable
 *  @brief a resetable object (e.g. a SG DataHandle)
 *  @author ATLAS Collaboration
 *  $Id: IResetable.h,v 1.2 2007-03-13 17:14:11 ssnyder Exp $
 **/
class IResetable {
public:
  virtual ~IResetable() {}

  /// Clear cached data from this object.
  /// If HARD is true, then also clear any data that depends on the identity
  /// of the current event store.  HARD will be set if the handle could
  /// potentially be looking at a different store the next time
  /// it is used.  (This happens in Hive.)
  ///
  /// For example, a VarHandle caches both a pointer to the referenced
  /// object and a pointer to the DataProxy used to reference it.
  /// If HARD is false, then only the object pointer need be cleared; but
  /// if it is true, then the DataProxy pointer should be cleared as well
  /// (and the object deregistered from the store).
  virtual void reset (bool hard) = 0;

  // Backwards compatibility.
  // FIXME: Needed by TrigDecisionTool.
  // Look into getting rid of this once TDT is more up-to-date.
  virtual void reset() { reset(false); }

  ///optional special action on final reset call (e.g. in caller destructor)
  virtual void finalReset() {};
  virtual bool isSet() const = 0;
  virtual const std::string& key() const = 0;
};

#endif
