/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
     

      //The below four functions are dummy functions so that this compiles. The functions are needed in the interface IMode3dFinder.h for Mode3dFromFsmw1dFinder (the seed finder for the Inclusive Secondary Vertex Finder)
      virtual void setPriVtxPosition( double, double );
      virtual unsigned int Modes1d(std::vector<float> &, std::vector<float> &, 
				   std::vector<float> &, std::vector<float> &);

      virtual const std::vector<int> & AcceptedCrossingPointsIndices() const;
      virtual void getCorrelationDistance( double &cXY, double &cZ );
      
  private:
      
      ToolHandle< IMode1dFinder > m_mode1dfinder;

      //this variable is needed for the above dummy functions
      mutable std::vector<int>  m_acceptedCrossingPoint;
  };
}
#endif
