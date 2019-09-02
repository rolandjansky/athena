/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IZVTOP_AmbiguitySolver.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IINDETZVTOP_AMBIGUITYSOLVER_H
#define IINDETZVTOP_AMBIGUITYSOLVER_H

#include "GaudiKernel/IAlgTool.h"
//xAOD includes --David S.
#include "xAODTracking/Vertex.h" //typedef
#include <vector>


namespace InDet 
{

  class IZVTOP_AmbiguitySolver : virtual public IAlgTool {
  public:
    DeclareInterfaceID (IZVTOP_AmbiguitySolver, 1, 0);

    // enter declaration of your interface-defining member functions here
    virtual std::vector< xAOD::Vertex* > solveAmbiguities(std::vector< xAOD::Vertex* > VertexContainer) const = 0;
  };

} // end of namespace

#endif 
