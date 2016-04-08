/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrkVertexFitterInterfaces_IVertexKinematicFitter_h 
#define TrkVertexFitterInterfaces_IVertexKinematicFitter_h 

//#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVertexFitterInterfaces/IKinematicConstraint.h"
#include "GaudiKernel/IAlgTool.h"

/**
 * @class IVertexKinematicFitter
 *
 * A simple abstract interface for vertex fitters with
 * arbitrary additional constraints. It is assumed that the 
 * constraints equations are added to the fit using the 
 * Lagrange multipliers method.
 *
 * @author Kirill.Prokofiev@cern.ch
 * March 2008
 */

static const InterfaceID IID_IVertexKinematicFitter("Trk::IVertexKinematicFitter", 1, 0);
namespace Trk
{

 
 class KinematicParticle;
 class KinematicVertex;
 class Vertex;
 
 class IVertexKinematicFitter: virtual public IAlgTool
 {

  public:

   static const InterfaceID& interfaceID(){return IID_IVertexKinematicFitter;}
  /** 
   * Virtual destructor 
   */
   virtual ~IVertexKinematicFitter(){};


  /**
   *  A vertex fit from a set of kinematic particles, no  constraint
   */
   virtual KinematicVertex * fit(const std::vector<const Trk::KinematicParticle*>& particleList) = 0;
  
   
  /**
   * A vertex fit from a set of kinematic particles with additional constraints
   */
   virtual KinematicVertex * fit(const std::vector<const Trk::KinematicParticle*>& particleList, 
                                 const std::vector<const Trk::IKinematicConstraint *>& constraintList) = 0;	 
							 
							 
  /**
   * A vertex fit from a set of kinematic particles with additional constraints and
   * starting point.
   */							 
   virtual KinematicVertex * fit(const std::vector<const Trk::KinematicParticle*>& particleList, 
                                 const std::vector<const Trk::IKinematicConstraint *>& constraintList,
				 const Vertex& startingPoint) = 0;
				 
   /** validation/monitoring method: tell history of chi2 during iterations, size gives # of iterations */
   virtual const std::vector<double>&      chisquaredConvergenceOfLastFit() const=0;
   /** validation/monitoring method: tell history of old-to-new position distances during iterations */
   virtual const std::vector<double>&      positionConvergenceOfLastFit() const=0;
				 
  private:


 }; //end of class definitions
}//end of namespace definitions

#endif //TrkVertexFitterInterfaces_IVertexKinematicFitter
