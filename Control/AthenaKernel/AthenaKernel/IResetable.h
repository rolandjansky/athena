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
  virtual void reset() = 0;    
  virtual bool isSet() const = 0;
  virtual const std::string& key() const = 0;
};

#endif
