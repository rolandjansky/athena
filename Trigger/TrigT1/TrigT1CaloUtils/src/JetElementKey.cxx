/***************************************************************************
                          JetElementKey.cpp  -  description
                             -------------------
    begin                : Tue Sep 26 2000
    copyright            : (C) 2000 by Edward Moyse
    email                : e.moyse@qmw.ac.uk
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

