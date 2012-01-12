/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGTAUL1BUGFIX_H
#define _TRIGTAUL1BUGFIX_H

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauL1BugFix.h
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           Hack to fix up the L1 objects to replace track+calo eta/phi
///           with calo-only eta/phi.  This should be a temporary fix.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TObject.h"

class TrigTauEvent;

class TrigTauL1BugFix : public TObject {

 public:
  TrigTauL1BugFix();
  virtual ~TrigTauL1BugFix();

  void fix(TrigTauEvent* event);

private:

  ClassDef(TrigTauL1BugFix, 1) ;

};

#endif
