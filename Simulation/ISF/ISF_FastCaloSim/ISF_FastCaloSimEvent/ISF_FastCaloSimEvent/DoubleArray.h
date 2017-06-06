/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DoubleArray_h
#define DoubleArray_h

#include "TArrayD.h"
#include "TObject.h"

class DoubleArray : public TObject, public TArrayD {

public:
   DoubleArray();
   DoubleArray( int );
   ~DoubleArray();

private:
//   TArrayD* m_array;

   ClassDef( DoubleArray, 1 )
};

#if defined(__MAKECINT__)
#pragma link C++ class DoubleArray+;
#endif

#endif
