/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_QUADFOCUSERNULL_H
#define FPTRACKER_QUADFOCUSERNULL_H
#include "IQuadFocuser.h"
namespace FPTracker{
  class TransversePoint;
 class QuadFocuserNull:public IQuadFocuser{
  public:
    void focus(double, double, double, const TransversePoint& direction, const Point& displacment) ;
    double xe()  const;
    double xae() const;
    double ye()  const;
    double yae() const;
    std::string label() const;

  private:

    const static std::string s_label;
    double m_xe;
    double m_xae;
    double m_ye;
    double m_yae;
  };
}
#endif
