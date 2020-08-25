/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXFITTERS_DUMMYVERTEXSMOOTHER_H
#define TRKVERTEXFITTERS_DUMMYVERTEXSMOOTHER_H

#include "TrkVertexFitterInterfaces/IVertexSmoother.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkParametersBase/ParametersBase.h"
#include "CxxUtils/checker_macros.h"
namespace Trk
{
  class IExtrapolator;
  
  /**
   * @author N. Giacinto Piacquadio (University of Freiburg)
   * @author Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
   * 
   * @class Trk::DummyVertexSmoother
   *
   * First real implementation: August 2006
   *
   * Dummy vertex smoother. Does not refit trajectories 
   * with the knowledge of the vertex. Instead it takes
   * trajectories, extrapolate them to the transverse point 
   * of the closest approach to the reconstructed vertex
   * and calculates the perigee parameters of trajectories
   * with respect to that vertex.
   *
   * The extrapolated perigee parameters w.r.t the reconstructed vertex
   * are then stored in m_perigeeAtVertex datamembers of VxTrackAtVertex's.
   *
   * ---------------------------------------------------------------------
   * Changes:
   *
   * David Shope <david.richard.shope@cern.ch> (2016-04-19)
   * EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex
   *
   */ 

  //Not THREAD_SAFE due to const_cast 
  class ATLAS_NOT_THREAD_SAFE DummyVertexSmoother : public AthAlgTool, virtual public IVertexSmoother
  {
  public:
  
  /**
   * Athena default methods
   */
    StatusCode initialize();
    StatusCode finalize();
    
  /** 
   * Constructor and destructor 
   */
    DummyVertexSmoother(const std::string& t, const std::string& n, const IInterface*  p);
    
    ~DummyVertexSmoother();
    
  /**
   *Actual smooth method 
   */  
    void smooth(xAOD::Vertex & vtx) const;

  private:
  
    Trk::IExtrapolator*	      m_extrapolator;
    std::string  m_extrapolatorName;
    std::string  m_extrapolatorInstance;
  
  };//end of class definitions
}//end of namespace definitions
#endif
