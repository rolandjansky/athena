/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERUTILS_MODE3DTO1DFINDER_H
#define TRKVERTEXSEEDFINDERUTILS_MODE3DTO1DFINDER_H


#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexSeedFinderUtils/IMode3dFinder.h"
#include "AthenaBaseComps/AthAlgTool.h"


namespace Trk
{

  /**
   @class Mode3dFinder

   Algotool which calculates the mode of a 
   threedimensional distribution splitting it into three 
   monodimensional distribution and calculating the mode of these 
   distribution through the 1dModeFinder that you specify in the 
   job option file.

   @author Giacinto.Piacquadio@physik.uni-freiburg.de

   */

  class IMode1dFinder;
  
  class Mode3dTo1dFinder : public AthAlgTool, virtual public IMode3dFinder
  {
  public:
    StatusCode initialize();
    StatusCode finalize();
    
    //default constructor due to Athena interface
      Mode3dTo1dFinder(const std::string& t, const std::string& n, const IInterface*  p);
      
      //destructor
      virtual ~Mode3dTo1dFinder();
      
      
      //obtain the 3d-mode (position) from a list of positions (distribution in space)
      virtual const Amg::Vector3D getMode(const std::vector<Trk::PositionAndWeight> &) const;

      //obtain the 3d-mode (position) from a list of positions (distribution in space) - NO WEIGHTS
      virtual const Amg::Vector3D getMode(const std::vector<Amg::Vector3D> &) const;
      
      
  private:
      
      ToolHandle< IMode1dFinder > m_mode1dfinder;
  };
}
#endif
