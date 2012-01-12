/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGTAUL2BUGFIX_H
#define _TRIGTAUL2BUGFIX_H

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauL2BugFix.h
/// Package : TrigTauPerformAnalysis
/// Author  : Eric Torrence (University of Oregon)
/// Created : February 2007
///
/// DESCRIPTION:
///           Hack to fix up the L2 objects to replace track+calo eta/phi
///           with calo-only eta/phi.  This should be a temporary fix.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TObject.h"

class TrigTauEvent;

class TrigTauL2BugFix : public TObject {

 public:
  TrigTauL2BugFix();
  virtual ~TrigTauL2BugFix();

  void fix(TrigTauEvent* event);

private:

  ClassDef(TrigTauL2BugFix, 1) ;

};

#endif
