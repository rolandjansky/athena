/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_STLHELPERS_H
#define FPTRACKER_STLHELPERS_H

#include <cmath>

namespace FPTracker{

  template<class T>
    class SelectBySide{
  public:
  SelectBySide(int side):m_side(side){}
    bool operator()(const T* t){ return t->side() == m_side;}
  private:
    int m_side;
  };

  template <class PtrType>
    bool absZGreater(const PtrType  rhs, const PtrType lhs)
    {
      return lhs->zabspos() > rhs->zabspos();
    }
  
}
#endif
