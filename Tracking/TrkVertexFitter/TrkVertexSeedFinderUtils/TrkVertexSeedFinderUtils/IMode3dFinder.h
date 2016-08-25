/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMode3dFinder.h, (c) ATLAS Detector software 2006
///////////////////////////////////////////////////////////////////

#ifndef TRKSEEDFINDERUTILS_IMODE3DFINDER_H
#define TRKSEEDFINDERUTILS_IMODE3DFINDER_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"

namespace Trk
{

  /**
   @class IMode3dFinder

   Interface class for the algotool which calculates the mode of a 
   threedimensional distribution.

   @author Giacinto.Piacquadio@physik.uni-freiburg.de

   */

  static const InterfaceID IID_IMODE3DFINDER("IMode3dFinder", 1, 1);

  class IMode3dFinder : virtual public IAlgTool {

     public:
       /** Virtual destructor */
       virtual ~IMode3dFinder(){};

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IMODE3DFINDER; };
       
       //obtain the 3d-mode (position) from a list of positions (distribution in space)
       virtual const Amg::Vector3D getMode(const std::vector<Trk::PositionAndWeight> &) const =0;

       //obtain the 3d-mode (position) from a list of positions (distribution in space) - NO WEIGHTS
       virtual const Amg::Vector3D getMode(const std::vector<Amg::Vector3D> &) const=0;       

  };
}

#endif

