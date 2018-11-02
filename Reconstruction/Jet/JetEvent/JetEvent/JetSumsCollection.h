/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETSUMSCOLLECTION_H
#define JETEVENT_JETSUMSCOLLECTION_H

//********************************************************************
//NAME:     JetCollection.h
//********************************************************************

// INCLUDE HEADER FILES:

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

#include "JetEvent/JetSums.h"

class JetSumsCollection : public DataVector<JetSums>
{

 public:
  // constructor 
  JetSumsCollection() : DataVector<JetSums>() { }; 
  virtual ~JetSumsCollection() { };
  //  void print();

};
CLASS_DEF( JetSumsCollection , 1263722752 , 1 )

#endif
