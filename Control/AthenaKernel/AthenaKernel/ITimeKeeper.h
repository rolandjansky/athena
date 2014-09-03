/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_ITIMEKEEPER_H
# define ATHENAKERNEL_ITIMEKEEPER_H

//<<<<<< INCLUDES                                                       >>>>>>
#include <ctime> /* time_t */

#ifndef GAUDIKERNEL_ISERVICE_H
 #include "GaudiKernel/IService.h"
#endif

//<<<<<< CLASS DECLARATIONS                                             >>>>>>
/** @class ITimeKeeper
  * @brief access cpu time info. In particular allows to know when job time
  *        is over
  *        N.B. times are in 1/100 CPU sec units (POSIX standard)
  * 
  * @author Paolo Calafiura <pcalafiura@lbl.gov> 
  * $Id: ITimeKeeper.h,v 1.3 2007-02-08 17:48:38 binet Exp $
  */

class ITimeKeeper : virtual public IService {
public:
  /// main user call: inform time keeper that next job iteration is about
  /// to start. Returns false to signal time is over (not enough time for
  /// next iter)
  virtual bool nextIter() = 0; 
  virtual time_t timeL() const = 0;    ///< time remaining before limit
  virtual time_t timeX() const = 0;    ///< time used so far
  virtual bool timeOver() const = 0;   ///< is there time for another iter?

  /// Gaudi boilerplate
  static const InterfaceID& interfaceID();
};

#endif // ATHENAKERNEL_ITIMEKEEPER_H
