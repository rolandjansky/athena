/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigMbD3PDMaker/src/TrigMbInheritance.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2010
 * @brief Declare inheritance relationships for TrigMb classes.
 *
 * Eventually, these should be moved to the EDM classes.
 */

#include "InDetBCM_RawData/BCM_RDO_Container.h"
#include "SGTools/BaseInfo.h"

SG_ADD_BASE (BCM_RDO_Container, DataVector<BCM_RDO_Collection>);

