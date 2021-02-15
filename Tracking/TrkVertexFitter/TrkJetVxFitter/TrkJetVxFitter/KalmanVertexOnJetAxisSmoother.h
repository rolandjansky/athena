/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 *
 * ATLAS Reconstruction Software - (C) 2005 - 2007
 *
 * @class Trk::KalmanVertexOnJetAxisSmoother
 *
 * @author Giacinto Piacquadio, (giacinto.piacquadio@physik.uni-freiburg.de)
 * @author Christian Weiser, (christian.weiser@physik.uni-freiburg.de)
 * 
 * A concrete implementation of the Vertex Smoother 
 * for smoothing the vertexing along the Jet Axis.
 *
 * This is a special implementation (part of the JetFitter(TM) algorithm) 
 * which is able to smooth a vertex correcting the jet axis parameters 
 * to find the best overall agreement.
 *
 * Basic approach is based on R. Fruhwirth et al. Comp. Phys. Comm 96(1996) 189 
 *
 * First version: December 2006
 * Updated version for athena: Februar 2007
 *
 * Alberts-Ludwig-Universita"t Freiburg
 */


#ifndef TrkJetFitter_KalmanVertexOnJetAxisSmoother_H
#define TrkJetFitter_KalmanVertexOnJetAxisSmoother_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "EventPrimitives/EventPrimitives.h"

namespace Trk
{
  class VxVertexOnJetAxis;
  class VxJetCandidate;
  class KalmanVertexOnJetAxisUpdator;
  class VertexPositions;
  class RecVertexPositions;
 
 static const InterfaceID IID_KalmanVertexOnJetAxisSmoother("Trk::KalmanVertexOnJetAxisSmoother", 1, 0);


 class KalmanVertexOnJetAxisSmoother : public AthAlgTool
 {
  public: 
  
   virtual StatusCode initialize() override;

   /**
    * Constructor
    */

   KalmanVertexOnJetAxisSmoother(const std::string& t, const std::string& n, const IInterface*  p);

   /**
    * Destructor
    */
 
   ~KalmanVertexOnJetAxisSmoother();

   static const InterfaceID& interfaceID()
     {
       return IID_KalmanVertexOnJetAxisSmoother;
     };

   /**
    * Update the VxVertexOnJetAxis (chi2 and ndf) with the knowledge coming from the 
    * fitted VxJetCandidate, updating (or not) also all the related track parameters 
    * of the tracks at the vertex (smoothing).
    */ 

   void update(VxVertexOnJetAxis* vertexToSmooth,const VxJetCandidate* candidateToUpdate,
	       bool updateTrack=true) const;

   void fastUpdate(VxVertexOnJetAxis* vertexToSmooth,const VxJetCandidate* candidateToUpdate,
               bool updateTrack=true) const;
   /**
    * Update the VxVertexOnJetAxis (chi2 and ndf) with the knowledge coming from the 
    * fitted RecVertexPositions, taking into account the linearization positions, updating 
    * (or not) all the related track parameters of the tracks at the vertex (smoothing).
    */
   
   void update(VxVertexOnJetAxis* vertexToSmooth,bool isPrimary,
	       const RecVertexPositions & myFinalPosition,
	       const VertexPositions & linearizationPositions,
	       bool updateTrack=false,
               bool doFastUpdate=false) const;
    
  private:

   ToolHandle<KalmanVertexOnJetAxisUpdator> m_Updator;
   
   /**
    * Meaningless in case no adaptive fitting is used.
    * Adaptive finding will be implemented probably later also in the fit along the flight axis.
    */

   double m_maxWeight;
   double m_initialMomentumError;
   AmgSymMatrix(3) m_initial_cov_momentum;
   
 }; //end of class definitions

} //end of the namespace definitions

#endif
