/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IZVTOP_AmbiguitySolver.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IINDETZVTOP_AMBIGUITYSOLVER_H
#define IINDETZVTOP_AMBIGUITYSOLVER_H

#include "GaudiKernel/IAlgTool.h"

//xAOD includes --David S.
#include "xAODTracking/Vertex.h"

//namespace Trk --David S.
//{
// class VxCandidate;
//}

namespace InDet 
{


  static const InterfaceID IID_IZVTOP_AmbiguitySolver("InDet::IZVTOP_AmbiguitySolver", 1, 0);

  class IZVTOP_AmbiguitySolver : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here
    //virtual  std::vector<Trk::VxCandidate*> solveAmbiguities(std::vector<Trk::VxCandidate*> VxContainer) = 0; --David S.
    virtual std::vector< xAOD::Vertex* > solveAmbiguities(std::vector< xAOD::Vertex* > VertexContainer) = 0;
  };

  inline const InterfaceID& InDet::IZVTOP_AmbiguitySolver::interfaceID()
    { 
      return IID_IZVTOP_AmbiguitySolver; 
    }

} // end of namespace

#endif 
