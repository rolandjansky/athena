/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      SortingClasses.h  -  description
      ----------------------------------------------
created              : Thursday 30th December 2004
author               : atkinson
email                : Tom.Atkinson@cern.ch
description          : Class for sorting components according to largest weight.
*********************************************************************************/

#ifndef TrkSortingClasses_H
#define TrkSortingClasses_H

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

namespace Trk{
  
class SortByLargerComponentWeight {
 public:
  SortByLargerComponentWeight () {};

  bool operator () ( const ComponentParameters firstComponent,
         const ComponentParameters secondComponent ) const

    { return firstComponent.second > secondComponent.second; }

};

class SortByLargerWeight {

 public:
  SortByLargerWeight () {};

  bool operator () ( const double& firstWeight,
         const double& secondWeight ) const
    
    { return firstWeight > secondWeight; }

};

class SortBySmallerWeight {

 public:
  SortBySmallerWeight () {};

  bool operator () ( const double& firstWeight,
                     const double& secondWeight ) const

    { return firstWeight < secondWeight; }

};



}

#endif

