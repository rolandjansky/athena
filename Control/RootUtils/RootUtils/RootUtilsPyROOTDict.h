// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RootUtilsPyROOTDict.h,v 1.5 2008-04-23 19:48:34 ssnyder Exp $

/**
 * @file  RootUtils/RootUtilsPyROOTDict.h
 * @author scott snyder
 * @date Dec 2007
 * @brief Dictionary header for RootUtils,
 *        for modules with PyROOT dependencies.
 */

#include "RootUtils/PyROOTTTreePatch.h"
#include "RootUtils/PyLogger.h"
#include "RootUtils/PyROOTPickle.h"
#include "RootUtils/PyROOTTFilePythonize.h"
#include "RootUtils/PyROOTInspector.h"
#include "RootUtils/PyROOTTypePatch.h"



// Work around a problem sometimes seen with cling in which `struct timespec'
// appears to be predeclared without the include guard being defined.
// This can cause problems, for example, with headers that include Python.h.
// As a workaroud, force the include guard to be defined when this
// dictionary is loaded.
#include "TInterpreter.h"
class RootUtilsInit
{
public:
  RootUtilsInit();
};
RootUtilsInit::RootUtilsInit()
{
  gInterpreter->ProcessLine ("#define _STRUCT_TIMESPEC 1");
}
RootUtilsInit rootUtilsInit;

