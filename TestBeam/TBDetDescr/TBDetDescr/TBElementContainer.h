/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//==============================================
//  File TBElementContainer.h
//  define the container which holds TBElement
//=============================================
#ifndef TBElementContainer_H
#define TBElementContainer_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "TBDetDescr/TBElement.h"

class TBElementContainer : public DataVector<TBElement> 
{
 public:
   TBElementContainer() : DataVector<TBElement>() { };
   virtual ~TBElementContainer() { };
 };

CLASS_DEF(TBElementContainer, 2891, 1)

#endif

