/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/src/MissingETInheritance.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2012
 * @brief Declare inheritance relationships for missingET classes.
 *
 * Eventually, these should be moved to the EDM classes.
 */


#include "MissingETEvent/MissingEtCalo.h"
#include "SGTools/BaseInfo.h"


SG_ADD_BASE (MissingEtCalo, MissingET);
