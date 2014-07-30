/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/particleInheritance.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Declare inheritance relationships for particle-related classes.
 *
 * Eventually, these should be moved to the EDM classes.
 */


#include "EventKernel/INavigable4Momentum.h"
#include "EventKernel/IParticle.h"
#include "SGTools/BaseInfo.h"


SG_ADD_BASE (IParticle, SG_VIRTUAL(INavigable4Momentum));
SG_ADD_BASE (IParticle, SG_VIRTUAL(IAthenaBarCode));
SG_ADD_BASE (IParticle, SG_VIRTUAL(INavigable));
SG_ADD_BASE (IParticle, SG_VIRTUAL(I4Momentum));
SG_ADD_BASE (INavigable4Momentum, SG_VIRTUAL(IAthenaBarCode));
SG_ADD_BASE (INavigable4Momentum, SG_VIRTUAL(INavigable));
SG_ADD_BASE (INavigable4Momentum, SG_VIRTUAL(I4Momentum));
