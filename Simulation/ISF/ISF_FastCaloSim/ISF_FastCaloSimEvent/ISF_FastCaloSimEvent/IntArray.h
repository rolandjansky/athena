/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IntArray_h
#define IntArray_h

#include "TArrayI.h"
#include "TObject.h"

class IntArray : public TObject, public TArrayI {

public:
   IntArray();
   IntArray( int );
   ~IntArray();

private:
   TArrayI* m_array;

   ClassDef( IntArray, 0 )
};

#endif
