/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 *
 * Februar 2007 (c) Atlas Detector Reconstruction Software
 *
 * @class Trk::JetFitterHelper
 *
 * @author Giacinto Piacquadio (University of Freiburg)
 *
 * (In case of problems, please contact: nicola.giacinto.piacquadio@cern.ch)
 *
 *  This class provides utilities for the JetFitter algorithm, 
 *  mainly to manipulate the JetVxCandidate object during the fit, 
 *  but also to perform linearization and similar tasks...
 *
 *  One very interesting feature is the application of *exact* constraints 
 *  through the Kalman Filter formalism, a possibility illustrated for 
 *  the first time in Babar with TreeFitter by Wouter Hulsbergen. 
 *  This helps to have a fast estimation of the probability of forming a common cluster 
 *  between two vertices. (reference will be put here)
 *
 */

#ifndef TrkJetFitter_JetFitterHelper_H
#define TrkJetFitter_JetFitterHelper_H

#include <utility>
#include <vector>
#include "AthenaBaseComps/AthAlgTool.h"


namespace Trk {

  class VxJetCandidate;
  class VxVertexOnJetAxis;
  class RecVertexPositions;

  static const InterfaceID IID_JetFitterHelper("JetFitterHelper", 1, 0);

  class JetFitterHelper : public AthAlgTool
  {

  public:

    static const InterfaceID& interfaceID() {
      return IID_JetFitterHelper;
    }

    /**
     * Constructor
     */

    JetFitterHelper(const std::string& t, const std::string& n, const IInterface*  p);

    /**
     * Destructor
     */

    ~JetFitterHelper();

    /**
     * Adds tracks from the second VxVertexOnJetAxis to the first one
     */
    
    void addTracksOfFirstVertexToSecondVertex(const VxVertexOnJetAxis & first,VxVertexOnJetAxis & second) const;
    
    /**
     * Performs a Kalman Update constraining the VxVertexOnJetAxis specified to be merged with the primary vertex.
     * You need to provide a copy of the RecVertexPositions, because this will be updated with the new constraint
     * directly.
     */

    void performKalmanConstraintToBePrimaryVertex(RecVertexPositions & positionToUpdate,
						  const VxVertexOnJetAxis & vertexToConstraint) const;

    /**
     * Performs a Kalman Update constraining the two VxVertexOnJetAxis specified to be merged.
     * You need to provide a copy of the RecVertexPositions, because this will be updated with the new constraint
     * directly.
     */

    void performKalmanConstraintToMergeVertices(RecVertexPositions & positionToUpdate,
						const VxVertexOnJetAxis & vertexToConstraint1,
						const VxVertexOnJetAxis & vertexToConstraint2) const;

    /**
     * Modifies the VxJetCandidate provided, merging the two provided VxVertexOnJetAxis into a single 
     * vertex along the jet axis.
     *
     * Use this method in order to perform the merging in a coherent way (RecVertexPositions, list of 
     * VxVertexOnJetAxis, tracks in the merged VxVertexOnJetAxis, numbering of clusters, everything 
     * is updated in a coherent way).
     * Afterwards the modified VxJetCandidate is ready for a new full fit.
     */

    VxVertexOnJetAxis & mergeVerticesInJetCandidate(VxVertexOnJetAxis & vertex1,
						    VxVertexOnJetAxis & vertex2,
						    VxJetCandidate & myJetCandidate) const;
    /**
     * Modifies the VxJetCandidate provided, merging the provided VxVertexOnJetAxis into 
     * the primary vertex.
     *
     * Use this method in order to perform the merging in a coherent way (RecVertexPositions, list of 
     * VxVertexOnJetAxis, tracks in the VxVertexOnJetAxis representing the primary vertex, 
     * numbering of clusters, everything is updated in a coherent way).
     * Afterwards the modified VxJetCandidate is ready for a new full fit.
     */

    void mergeVertexToPrimaryInJetCandidate(VxVertexOnJetAxis & vertex,
					    VxJetCandidate & myJetCandidate) const;


  private:

    

  };//end class declaration

}//end namespace Trk


#endif
