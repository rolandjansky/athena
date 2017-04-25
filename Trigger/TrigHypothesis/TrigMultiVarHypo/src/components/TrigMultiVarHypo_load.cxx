/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
//Dear emacs, this is -*- c++ -*-

/**
 * @file TrigMultiVarHypo_load.cxx
 * @author <a href="mailto:Andre.dos.Anjos@cern.ch">Andre DOS ANJOS</a> 
 * $Author: rabello $
 * $Revision: 1.1 $
 * $Date: 2006-08-23 19:19:34 $
 *
 * Loads all entries on DLL load.The argument to the LOAD_FACTORY_ENTRIES() is
 * the name of the component library (libXXX.so).
 */

#include "GaudiKernel/LoadFactoryEntries.h"

LOAD_FACTORY_ENTRIES(TrigMultiVarHypo)



