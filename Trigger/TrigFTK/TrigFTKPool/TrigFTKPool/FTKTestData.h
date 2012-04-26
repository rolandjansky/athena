/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKTestData_h
#define FTKTestData_h

class FTKTestData {
private:
  int m_ival;

public:
  FTKTestData() : m_ival(0) {;}
  FTKTestData(int ival) : m_ival(ival) {;}

  void setValue(int);
  int getValue() const;
};

#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(FTKTestData,827647585,1)
#endif // FTKTestData_h
