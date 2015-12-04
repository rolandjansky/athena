/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1Interval                       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Base/VP1Interval.h"
#include "VP1Base/VP1Msg.h"
#include <cmath>

//____________________________________________________________________
VP1Interval::VP1Interval(const double& lower,const double& upper, bool openLower, bool openUpper, bool excludeInterval )
  : m_lower(lower), m_upper(upper), m_openLower(openLower), m_openUpper(openUpper), m_excludeInterval(excludeInterval)
{
  testSanity();
}

//____________________________________________________________________
void VP1Interval::set(const double& lower,const double& upper, bool openLower, bool openUpper, bool excludeInterval)
{
  m_lower = lower;
  m_upper = upper;
  m_openLower = openLower;
  m_openUpper = openUpper;
  m_excludeInterval = excludeInterval;
  testSanity();
}

//____________________________________________________________________
void VP1Interval::testSanity() const
{
  if (!isSane())
    VP1Msg::messageDebug("WARNING: VP1Interval is not sane: "+toString());
}

//____________________________________________________________________
bool VP1Interval::hasOverlap(const VP1Interval& other) const
{
  if (isEmpty()||other.isEmpty())
    return false;
  if (isAllR()||other.isAllR())
    return true;
  if (m_upper<other.m_lower||other.m_upper<m_lower)
    return false;
  if (m_upper==other.m_lower)
    return !other.openLower() && !openUpper();
  if (other.m_upper==m_lower)
    return !openLower() && !other.openUpper();
  return true;
}

//____________________________________________________________________
bool VP1Interval::contains(const VP1Interval& other) const
{
  if (!noLowerBound()) {
    if (other.m_lower<m_lower)
      return false;
    if (other.m_lower==m_lower&&!other.m_openLower&&m_openLower)
      return false;
  }
  if (!noUpperBound()) {
    if (other.m_upper>m_upper)
      return false;
    if (other.m_upper==m_upper&&!other.m_openUpper&&m_openUpper)
      return false;
  }
  return true;
}


/*bool VP1Interval::contains(const double& x) const
{
 VP1Msg::messageDebug("m_excludeInterval: "+QString::number(m_excludeInterval));
  if (m_excludeInterval) {
  	return ! (!excludedByLower(x) && !excludedByUpper(x));
  }
  return !excludedByLower(x) && !excludedByUpper(x);
}
*/

//____________________________________________________________________
bool VP1Interval::contains(const double& x, const double& period ) const
{
  if (isEmpty())
    return false;

  if (period<=0)
    return period==0;

  if (length()>=period)
    return true;

  //Translate x a number of periods, so that x is in
  //[lower,lower+period[, and compare:
  return contains(x+period*ceil((m_lower-x)/period));
}


//____________________________________________________________________
bool VP1Interval::hasOverlap(const VP1Interval& other, const double& period ) const
{
  if (isEmpty()||other.isEmpty())
    return false;
  if (period<=0)
    return period==0;
  if (length()>=period||other.length()>=period)
    return true;
  //Translate the other interval so that it's lower value is in
  //[lower,lower+period[, and then compare (both the interval and the
  //interval translated -period are necessary):
  const double t(period*ceil((m_lower-other.m_lower)/period));
  const double a(other.m_lower+t), b(other.m_upper+t);
  if (hasOverlap(VP1Interval(a,b,other.m_openLower,other.m_openUpper)))
    return true;
  return hasOverlap(VP1Interval(a-period,b-period,other.m_openLower,other.m_openUpper));
}
