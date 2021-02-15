/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrkVertexFitters_SequentialVertexSmoother_H
#define TrkVertexFitters_SequentialVertexSmoother_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/IVertexSmoother.h"


/**
 *
 * @class Trk::SequentialVertexSmoother
 *
 * A concrete implementation of the Vertex smoother
 * Takes the tracks from the  VxCandidates and updates 
 * them with the knowledge of the reconstructed vertex 
 * position. Can be used with any VertexTrackUpdator.
 * The default is KalmanVertexTrackUpdator
 *
 * @author Kirill Prokofiev, November 2005
 *
 * Modifications due to EDM changes +  additional
 * protection against empty pointers. August 2006
 * <Kirill.Prokofiev@cern.ch>
 *
 * ---------------------------------------------------
 * Changes:
 *
 * David Shope <david.richard.shope@cern.ch> (2016-04-19)
 *  EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex
 *
 */

namespace Trk
{
 class IVertexTrackUpdator;
 
 class SequentialVertexSmoother : public AthAlgTool, virtual public IVertexSmoother
 {

  public:
  
/**
 * default AlgTools methods  
 */
   virtual StatusCode initialize() override;
  
/**
 *constructor and destructor  
 */
    SequentialVertexSmoother(const std::string& t, const std::string& n, const IInterface*  p);
    virtual ~SequentialVertexSmoother();
 
 
 /**
  * Smooth method itself: updates the tracks
  * belonging to the VxCandidate with the knowledge
  * of the vertex position
  */
   virtual void smooth(xAOD::Vertex& vtx) const override;
  
  private:

   ToolHandle< IVertexTrackUpdator > m_vertexTrackUpdator; 
    
 }; //end of class definition

}//end of namespace definition
#endif
