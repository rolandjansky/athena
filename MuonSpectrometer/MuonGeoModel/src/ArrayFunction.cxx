/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/ArrayFunction.h"

namespace Genfun {

FUNCTION_OBJECT_IMP(ArrayFunction)
  
// Constructor
ArrayFunction::ArrayFunction(const double *begin, const double *end) :
  _values(begin,end) {}
  
// Destructor
ArrayFunction::~ArrayFunction(){}
  
// Copy constructor
ArrayFunction::ArrayFunction(const ArrayFunction &right):
  AbsFunction(),
  _values(right._values) {}
  
  // Retreive function value
double ArrayFunction::operator ()(double argument) const {
  int i =  int (argument+0.5);
  if (i<0 || i>=int(_values.size())) return 0;
  else return _values[i];
}

}
