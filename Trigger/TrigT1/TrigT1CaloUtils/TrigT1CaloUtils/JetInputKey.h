/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          JetInputKey.h  -  description
                             -------------------
    begin                : Fri Jan 21 2006
    email                : Alan.Watson@cern.ch
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
