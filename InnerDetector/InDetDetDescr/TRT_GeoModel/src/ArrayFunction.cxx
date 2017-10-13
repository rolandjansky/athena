/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ArrayFunction.h"

namespace Genfun {

FUNCTION_OBJECT_IMP(ArrayFunction)
  
// Constructor
ArrayFunction::ArrayFunction(const double *begin, const double *end) :
  m_values(begin,end) {}
  
// Destructor
ArrayFunction::~ArrayFunction(){}
  
// Copy constructor
ArrayFunction::ArrayFunction(const ArrayFunction &right):
  Genfun::AbsFunction(), m_values(right.m_values) {}
  
  // Retrieve function value
double ArrayFunction::operator ()(double argument) const {
  int i =  int (argument+0.5);
  if (i<0 || i>=int(m_values.size())) return 0;
  else return m_values[i];
}

}
