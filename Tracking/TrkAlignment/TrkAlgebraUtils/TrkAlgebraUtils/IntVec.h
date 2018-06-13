/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALGS_INTVEC_H
#define TRKALGS_INTVEC_H

namespace Trk {

class IntVec {
 public:
  IntVec();
  IntVec(int);
  IntVec(int, int);
  IntVec(const IntVec&);
  ~IntVec();

  int& operator[](int);
  const int& operator[](int) const;
  IntVec& operator=(const IntVec&);
  IntVec  operator+(const IntVec&);
  IntVec& operator+=(const IntVec&);
  IntVec  operator-(const IntVec&);
  IntVec& operator-=(const IntVec&);

  void reSize(int);
  int n_elem() const  { return m_Nele; }

 private:
  int m_Nele;
  int* m_ptr_data;
};

} // end namespace Trk

#endif // TRKALGS_INTVEC_H
