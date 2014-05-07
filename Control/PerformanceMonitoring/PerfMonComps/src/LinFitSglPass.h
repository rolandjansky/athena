/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class LinFitSglPass                       //
//                                                            //
//  Description: Little helper class for single-pass linear   //
//               fits of data points.                         //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2011                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef LINFITSGLPASS_H
#define LINFITSGLPASS_H

namespace PerfMon {

class LinFitSglPass {
public:

  LinFitSglPass();
  ~LinFitSglPass(){}

  void addPoint(const double&,const double&);
  void getFit(double&offset,double&slope) const;//Fitted line parametrised as: y(x) = offset + x*slope
  void reset();
  unsigned nPoints() const { return m_n; }
  double slope() const;
private:
  unsigned m_n;
  double m_sumx;
  double m_sumy;
  double m_sumxy;
  double m_sumxx;
};

////////////////////
// Implementation //
////////////////////

inline LinFitSglPass::LinFitSglPass()
  :  m_n(0),m_sumx(0),m_sumy(0),m_sumxy(0),m_sumxx(0)
{
}

inline void LinFitSglPass::reset()
{
  m_n=0;
  m_sumx=m_sumy=m_sumxy=m_sumxx=0;
}

inline void LinFitSglPass::addPoint(const double&x,const double&y)
{
  ++m_n;
  m_sumx  +=   x;
  m_sumy  +=   y;
  m_sumxy += x*y;
  m_sumxx += x*x;
}

inline void LinFitSglPass::getFit(double&offset,double&slope) const
{
  if (m_n<2) {
    offset=slope=0;
  } else {
    slope  = (m_sumxy*m_n-m_sumx*m_sumy)/(m_sumxx*m_n-m_sumx*m_sumx);
    offset = (m_sumy-slope*m_sumx)/m_n;
  }
}

inline double LinFitSglPass::slope() const
{
  double offset,slope;
  getFit(offset,slope);
  return slope;
}

} //> namespace PerfMon

#endif
