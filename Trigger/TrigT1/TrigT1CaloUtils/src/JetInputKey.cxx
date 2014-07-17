/***************************************************************************
                          JetInputKey.cpp  -  description
                             -------------------
    begin                : Fri Jan 21 2006
    copyright            : (C) 2006 by Alan Watson
    email                : Alan.Watson@cern.ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
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

