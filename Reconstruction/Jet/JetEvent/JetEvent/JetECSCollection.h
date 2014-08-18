/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETECSCOLLECTION_H
#define JETEVENT_JETECSCOLLECTION_H

//********************************************************************
//NAME:     JetECSCollection.h
//********************************************************************

// INCLUDE HEADER FILES:

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

#include "JetEvent/JetECS.h"

class JetECSCollection : public DataVector<JetECS>
{

 public:
  // constructor 
  JetECSCollection() : DataVector<JetECS>() { }; 
  virtual ~JetECSCollection() { };
  //  void print();

};
CLASS_DEF( JetECSCollection , 16121977 , 1 )

#endif
