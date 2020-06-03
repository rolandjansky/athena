/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          JetInputKey.cpp  -  description
                             -------------------
    begin                : Fri Jan 21 2006
    email                : Alan.Watson@cern.ch
 ***************************************************************************/

#ifndef  TRIGGERSPACE
// running in Athena
#include "TrigT1CaloUtils/JetInputKey.h"

#else
// running in TRIGGERSPACE
#include "JetInputKey.h"

#endif

namespace LVL1 {

/** takes phi and eta and constructs a JetInputKey object
which can return the key for this coord
*/
JetInputKey::JetInputKey(double phi, double eta) : JetElementKeyBase(phi,eta)
{
  setupJetRegionData();
  setupThisKeyValues();
}

/** constructs a JetInputKey object*/
JetInputKey::JetInputKey() : JetElementKeyBase()
{
  setupJetRegionData();
  setupThisKeyValues();
}

/** constructs a JetInputKey object*/
JetInputKey::JetInputKey(const Coordinate coord) : JetElementKeyBase(coord.phi(),coord.eta())
{
  setupJetRegionData();
  setupThisKeyValues();
}

} // end LVL1 namespace

