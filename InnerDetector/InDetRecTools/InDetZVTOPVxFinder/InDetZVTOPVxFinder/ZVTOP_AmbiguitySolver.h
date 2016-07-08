/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ZVTOP_AmbiguitySolver.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETZVTOP_AMBIGUITYSOLVER_H
#define INDETZVTOP_AMBIGUITYSOLVER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetZVTOPVxFinder/IZVTOP_AmbiguitySolver.h"
namespace Trk
{
  //class VxCandidate; --David S.
  class Track;
}
namespace InDet 
{

  class IZVTOP_TrkProbTubeCalc;
  class IZVTOP_SimpleVtxProbCalc;

  /** @class ZVTOP_AmbiguitySolver 

      This is for the Doxygen-Documentation.  
      Please delete these lines and fill in information about
      the Algorithm!
      Please precede every member function declaration with a
      short Doxygen comment stating the purpose of this function.
      
      @author  Tatjana Lenz <tatjana.lenz@cern.ch>
  */  

  class ZVTOP_AmbiguitySolver : virtual public IZVTOP_AmbiguitySolver, public AthAlgTool
    {
    public:
      ZVTOP_AmbiguitySolver(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~ZVTOP_AmbiguitySolver ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();
      //std::vector<Trk::VxCandidate*> solveAmbiguities(std::vector<Trk::VxCandidate*> VxContainer); --David S.
      std::vector< xAOD::Vertex* > solveAmbiguities(std::vector< xAOD::Vertex* > VertexContainer);
      
    private:

      ToolHandle <InDet::IZVTOP_TrkProbTubeCalc>	m_TrkProbTubeCalc;
      ToolHandle <InDet::IZVTOP_SimpleVtxProbCalc>	m_VtxProbCalc;

      
    }; 
} // end of namespace

#endif 
