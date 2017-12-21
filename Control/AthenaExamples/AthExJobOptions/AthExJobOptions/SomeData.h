// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXJOBOPTIONS_SOMEDATA_H
#define ATHEXJOBOPTIONS_SOMEDATA_H 1

#include "AthenaKernel/CLASS_DEF.h"


/////////////////////////////////////////////////////////////////////////////

class SomeData {
public:
   SomeData( double value ) : m_value( value ) {}

   double getValue() { return m_value; }
   void setValue( double value ) { m_value = value; }

private:
   double m_value;
};


// CLASS_DEF macro's can be obtained for your class by running the clid script
// with your class name, like so (in an ATLAS environment):
//
//  $ clid -m SomeData
//
CLASS_DEF( SomeData, 86850056, 1 )

#endif // !ATHEXJOBOPTIONS_SOMEDATA_H
