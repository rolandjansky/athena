/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_INEXTPASSFILTER_H
# define ATHENAKERNEL_INEXTPASSFILTER_H
//<<<<<< CLASS DECLARATIONS                                             >>>>>>
/** @class INextPassFilter
 * @brief  interface to a tool (typically) that decides whether
 *         the event loop mgr (typically) need to do another pass over the evts
 * @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration
 *$Id: INextPassFilter.h,v 1.1 2005-06-10 21:31:23 calaf Exp $	
 */

class INextPassFilter {
public:
  /// decide whether we need another iteration
  virtual bool doNextPass() = 0;
  virtual ~INextPassFilter() {}
};

#endif // ATHENAKERNEL_INEXTPASSFILTER_H
