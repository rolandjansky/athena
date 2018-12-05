/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETSAMPLINGCOLLECTION_H
#define JETEVENT_JETSAMPLINGCOLLECTION_H

//********************************************************************
//NAME:     JetSamplingCollection.h
//********************************************************************

// INCLUDE HEADER FILES:

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "JetEvent/JetSampling.h"

class JetSamplingCollection: public DataVector<JetSampling> 
{
 public:
  // constructor 
  //JetSamplingCollection() : DataVector<JetSampling>() { }; 
  virtual ~JetSamplingCollection() { };
  
////DRQ: Commented out since this function isn't implemented
////  void print();
};

CLASS_DEF(JetSamplingCollection, 3025, 1 )

#endif
