/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file InDetD3PDMaker/src/indetInheritance.cxx
 * @author Serhan Mete, scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Declare inheritance relationships for tracking classes.
 *
 * Eventually, these should be moved to the EDM classes.
 */

#include "TrkV0Vertex/V0Container.h"
#include "AthenaKernel/BaseInfo.h"


SG_ADD_BASE (V0Container, DataVector<Trk::V0Candidate>);

