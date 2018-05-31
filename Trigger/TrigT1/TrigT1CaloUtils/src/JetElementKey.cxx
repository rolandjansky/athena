/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          JetElementKey.cpp  -  description
                             -------------------
    begin                : Tue Sep 26 2000
    email                : e.moyse@qmw.ac.uk
 ***************************************************************************/

#ifndef  TRIGGERSPACE
// running in Athena
#include "TrigT1CaloUtils/JetElementKey.h"

#else
// running in TRIGGERSPACE
#include "JetElementKey.h"

#endif

namespace LVL1 {

/** takes phi and eta and constructs a JetElementKey object
which can return the key for this coord
*/
JetElementKey::JetElementKey(double phi, double eta) : JetElementKeyBase(phi,eta)
{
  setupRegionData();
  setupThisKeyValues();
}

/** constructs a JetElementKey object*/
JetElementKey::JetElementKey() : JetElementKeyBase()
{
  setupRegionData();
  setupThisKeyValues();
}

/** constructs a JetElementKey object*/
JetElementKey::JetElementKey(const Coordinate coord) : JetElementKeyBase(coord.phi(),coord.eta())
{
  setupRegionData();
  setupThisKeyValues();
}

} // end LVL1 namespace

