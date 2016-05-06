/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWFIRSTINT_H
#define EFLOWFIRSTINT_H

/********************************************************************

NAME:     eflowFirstInt.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson, R Duxfield (based on R.Duxfields Root package)
CREATED:  18th Aug, 2005

********************************************************************/

#include <iostream>

class eflowFirstInt {

 public:

  eflowFirstInt() :  m_fudgeMean(0.0), m_fudgeStdDev(0.0)  {}
  eflowFirstInt(double mean, double stdDev) :  m_fudgeMean(mean), m_fudgeStdDev(stdDev)  {}
  ~eflowFirstInt()  {}

  double fudgeMean() const    {return m_fudgeMean;}
  double fudgeStdDev() const  {return m_fudgeStdDev;}

  void setFudgeMean(double fudgeMean)      {m_fudgeMean = fudgeMean;}
  void setFudgeStdDev(double fudgeStdDev)  {m_fudgeStdDev = fudgeStdDev;}
  
 protected:

  double m_fudgeMean;
  double m_fudgeStdDev;

};


#endif
