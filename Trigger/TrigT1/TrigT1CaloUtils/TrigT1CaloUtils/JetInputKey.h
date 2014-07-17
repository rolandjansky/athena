/***************************************************************************
                          JetInputKey.h  -  description
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

#ifndef JetInputKey_H
#define JetInputKey_H

#ifndef  TRIGGERSPACE

// running in Athena
#include "TrigT1CaloUtils/JetElementKeyBase.h"

#else
// running in TRIGGERSPACE

#include "JetElementKeyBase.h"

#endif


namespace LVL1 {

/**
  The JetInputKey object provides the key for each JetElement
  depending on its eta,phi coords (JetElementKey differs from 
  JetInputKey in that ElementKey has 0.4 FCAL granularity, while 
  InputKey has 0.2 FCAL granularity). <p>
  
  As 2 implementations are required for the Jet and Energy triggers,
  differing only in the FCAL granularity, we solve this problem by 
  inheritance. Thus JetInputKey now inherits from JEKey (which is
  essentially the original JetElementKey, & itself inherits from KeyUtilities)
  
  */
class JetInputKey : public JetElementKeyBase {
 
public:
  
  JetInputKey(double phi, double eta);
  JetInputKey(const Coordinate coord);
  JetInputKey();
};
}
#endif
