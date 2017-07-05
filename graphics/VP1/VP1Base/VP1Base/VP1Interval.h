/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1Interval                         //
//                                                            //
//  Description: Convenience class describing interval.       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: May 2008                                 //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1INTERVAL_H
#define VP1INTERVAL_H

#include <QString>
#include <limits>

class VP1Interval {
public:
  static double inf() { return std::numeric_limits<double>::infinity(); }

  VP1Interval(const double& lower,const double& upper, bool openLower=true, bool openUpper=true, bool excludeRange=false);
  VP1Interval();//]0,0[
  ~VP1Interval(){};

  //Set:
  void setOpenLower(bool openLower);
  void setOpenUpper(bool openUpper);
  void setOpen(bool openLower,bool openUpper);
  void setLower(const double& lower);
  void setUpper(const double& upper);
  void setExcludeInterval(bool excludeInterval); // TODO: so far used only by contains(double). If needed, extend it to other 'contains' methods.
  void set(const double& lower,const double& upper, bool openLower=true, bool openUpper=true, bool excludeRange=false);
  void translate(const double&);

  //Get:
  double lower() const;
  double upper() const;
  double openLower() const;
  double openUpper() const;
  bool excludeInterval() const;
  QString toString() const;
  double length() const;

  //Query:
  bool contains(const double& x) const;
  bool contains(const VP1Interval& other) const;
  bool hasOverlap(const VP1Interval& other) const;//If at least one point is in common.
  // VP1Interval intersection(const VP1Interval& other) const;//mathematical intersection.

  //Periodic variables (e.g. angles with period=2pi):
  bool contains(const double& x, const double& period ) const;
  bool hasOverlap(const VP1Interval& other, const double& period ) const;

  bool isSane() const;
  bool isEmpty() const;
  bool isAllR() const;

  bool noLowerBound() const;
  bool noUpperBound() const;
  bool excludedByLower(const double&x) const;
  bool excludedByUpper(const double&x) const;

  //Comparisons:
  bool operator == ( const VP1Interval & other ) const;

  //Fixme: Add intersections with other intervals, and comparison operators!
  //Fixme: also add stuff for angular intervals (i.e. those periodic in 2pi)

private:

  void testSanity() const;

  double m_lower;
  double m_upper;
  bool m_openLower;
  bool m_openUpper;
  bool m_excludeInterval;

};

#include "VP1Base/VP1Interval.icc"

#endif
