/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrkVertexFitterUtils_FULLLINEARIZEDTRACKFACTORY_H
#define TrkVertexFitterUtils_FULLLINEARIZEDTRACKFACTORY_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/IVertexLinearizedTrackFactory.h"

#include "TrkParametersBase/ParametersT.h"
#include "TrkParametersBase/Charged.h"
#include "TrkParametersBase/Neutral.h"
#include "TrkParameters/TrackParameters.h" 

#include "MagFieldConditions/AtlasFieldCacheCondObj.h"

 /**
   * @class Trk::FullLinearizedTrackFactory 
   *
   * A concrete implementation of the linearized track factory.
   * This factory linearizes the measurement equation in the
   * vicinity of a given point (see ILinearizedTrackFactory class,
   * [1] Billoir and Qian et  al. NIM A311 (1992) 139-150 and
   * [2] R. Fruehwirth et al. Comp. Phys. Comm 96(1996) 189-208 
   * for more info).
   * In the current implementation the  ATLAS perigee parametrization
   * of helix (d_0,z_0,phi_p,theta_p,q/p ) and _full_ equations of
   * motion ae used without any approximations or expansion. 
   * For the method of the calculation see: (to be published,
   * so far contact the authors)
   *
   * The result of a calculation is stored in the Trk::LinearizedTrack
   * object, which is a private member of Trk::VxTrackAtVertex.
   *
   * @author Kirill Prokofiev (University of Sheffield)
   * @author N. Giacinto Piacquadio (for the Freiburg Group)
   *
   * -----------------------------------------
   * Changes:
   *
   * David Shope <david.richard.shope@cern.ch> (2016-06-01)
   *
   *   EDM Migration to xAOD - replace Trk::Vertex with Amg::Vector3D
   *
   */

namespace Trk
{
  class IExtrapolator;
  class Track;

   
  class FullLinearizedTrackFactory : public AthAlgTool, virtual public IVertexLinearizedTrackFactory
  {
  public:
  
  /**
   * Standard AlgToolMethods
   */
    StatusCode initialize();
  
    StatusCode finalize();
    
  /**
   * Default constructor due to Athena interface
   */
    FullLinearizedTrackFactory(const std::string& t, const std::string& n, const IInterface*  p);
    
   /**
    *  Destructor
    */
    ~FullLinearizedTrackFactory();
 
   /**
    *  Interface for VxTrackAtVertex:
    *  Takes a MeasuredPerigee from VxTrackAtVertex and a Lineariztion point.
    *  Produces a new PerigeeLinearizedState and stores its pointer as a private  
    *  member of VxTrackAtVertex
    */
    void linearize(VxTrackAtVertex & theTrack,const Amg::Vector3D & linPoint) const;
    
    /**
     * Linearization method:
     * Takes a MeasuredPerigee and a LinearizationPoint.
     * Creates a new PerigeeLinearizedTrackState and returns its pointer.
     */
    LinearizedTrack * linearizedTrack(const TrackParameters *param, 
			              const Amg::Vector3D & linPoint) const;

    /**
      * Linearization method:
      * Takes a MeasuredPerigee and a LinearizationPoint.
      * Creates a new PerigeeLinearizedTrackState and returns its pointer.
      */
    LinearizedTrack * linearizedTrack(const NeutralParameters *param,
                                      const Amg::Vector3D & linPoint) const;
    
    
  private:
 
    ToolHandle< Trk::IExtrapolator >          m_extrapolator;
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey 
          {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};

  };
}
#endif
