/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 MissingET package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: MissingETEventDict.h,v 1.7 2009-04-06 13:53:48 resconi Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef MISSINGETEVENT_MISSINGETDICT_H
# define MISSINGETEVENT_MISSINGETDICT_H

#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtCalo.h"
#include "MissingETEvent/MissingEtRegions.h"
#include "MissingETEvent/MissingEtTruth.h"
#include "MissingETEvent/MissingETComposition.h"

struct MissingETEventDict {
  std::pair<ElementLink<INavigable4MomentumCollection>, MissingETHandlers::Weight > pdum;
};

#endif // MISSINGETEVENT_MISSINGETDICT_H
