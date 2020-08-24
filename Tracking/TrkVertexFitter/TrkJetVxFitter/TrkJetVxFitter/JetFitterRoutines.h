/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 *
 * Februar 2007 (c) Atlas Detector Reconstruction Software
 *
 * @class Trk::JetFitterRoutines
 * 
 * @author Giacinto Piacquadio (University of Freiburg),
 * @author Christian Weiser (University of Freiburg)
 * 
 * This class contains the core functions for the "JetFitter" algorithm.
 * 
 * This is a new algorithm which uses a "Kalman filter" based approach 
 * to implement the "ghost track method" used for the first time by SLD 
 * (reference to be inserted here).
 * 
 * All the tracks inside a jet can be fitted with the constraint of lying on the same 
 * 'flight axis' from the primary vertex towards the jet. This class provides all the methods 
 * which are necessary to perform this fit and to evaluate the probability of two or more 
 * tracks to build one cluster on the jet axis.
 *
 * The real finding algorithm is then implemented in the "SV" finder algorithm. In this 
 * class only the methods for 'fitting' are implemented.
 *
 * There is no clear input and clear output in a similar fit, so this works more like 
 * a continous interaction between the finder and the fitter, which is provided in the 
 * form in a set of routines to use.
 *
 */

#ifndef TrkJetFitter_JetFitterRoutines_H
#define TrkJetFitter_JetFitterRoutines_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "MagFieldConditions/AtlasFieldCacheCondObj.h"

namespace Trk {

  class VxVertexOnJetAxis;
  class VxJetCandidate;
  class JetFitterInitializationHelper;
  class JetFitterHelper;
  class KalmanVertexOnJetAxisUpdator;
  class KalmanVertexOnJetAxisSmoother;
  class TrkDistanceFinderNeutralCharged;
  class TrkDistanceFinderNeutralNeutral;

  static const InterfaceID IID_JetFitterRoutines("Trk::JetFitterRoutines", 1, 0);

  class JetFitterRoutines : public AthAlgTool
  {

  public:

    static const InterfaceID& interfaceID() {
      return IID_JetFitterRoutines;
    }
 

    StatusCode initialize();
    StatusCode finalize();

    /**
     * Constructor
     */

    JetFitterRoutines(const std::string& t, const std::string& n, const IInterface*  p);

    /**
     * Destructor
     */

    ~JetFitterRoutines();

    /**
     * This method provides the initialization of all the tracks in the fit to the position 
     * of minimum distance to the initial flight axis. This should considerably improve the 
     * speed of convergence. It is still not implemented in the first version of the fit.
     */

    void initializeToMinDistancesToJetAxis(VxJetCandidate*) const;

    /**
     * This is the method where the fit is actually done.
     *
     * The VxJetCandidate in input must be properly initialized (initial positions + proper initial covariance matrix for 
     * primary vertex, direction + all the tracks along the jet axis).
     *
     * To initialized the VxJetCandidate properly please use the methods provided by the 
     * JetFitterInitializationHelper class.
     *
     * The parameters to be provided are:
     * - num_maxiterations: the number of maximal iterations (in case of no convergence), 
     * - treat_sign_flip: true if the special procedure for avoding flips to negative flightlengths has to be used
     * - deltachi2_convergence: if the difference in fit chi2 to the previous iteration is smaller 
     *                                 than the value provided convergence is reached
     */

    void performTheFit(VxJetCandidate* myJetCandidate,
		       int num_maxiterations=30,
		       bool treat_sign_flip=true,
		       int num_signflip_maxiterations=30,
		       double deltachi2_convergence=0.001) const;

    /**
     * triggers the smoothing of all vertices 
     * (the tracks in the fit are updated using the constraint provided by the vertices 
     * along the jet axis they were fitted to)
     */

    void smoothAllVertices(VxJetCandidate* myJetCandidate) const;

    /**
     * One iteration of the Kalman Updated of all tracks to the actual fit 
     * is performed.
     * Difference between fitting to primary vertex or to a 
     * vertex along the flight axis is taken into account.
     */

    void updateAllVertices(VxJetCandidate* myJetCandidate) const ;

    //    void updateChi2AllVertices(VxJetCandidate* myJetCandidate) const ;

    /**
     * Calculates in a very fast way the probability two vertices along the jet axis to be 
     * compatible with each other, taking the constraint to be compatible with the previous 
     * fitted flight axis into account.
     *
     * As a result a pair is provided:
     * - first member: probability of the two vertices to be compatible with a single vertex
     * - second vertex: if true, constraining the two vertices to be a single vertex moves 
     *                  the flight axis direction in DeltaR by more than the variable m_maxDRshift, which 
     *                  is set by the jobOption property maxDRshift.
     * (the idea is that the bigger this shift is, the worse the approximation given by the previous 
     * linearization of these two vertices is and so the fast estimation of the compatibility between 
     * the two vertices).
     *
     * If not needed the second member can just be ignored.
     *
     * Description of the algorithmic procedure: a single Kalman Update step is performed on top of the 
     * previous fit, just constraining the two vertices to lie on the same position on the flight axis 
     * (this is done using a measurement equation with zero associated error). This is very fast.
     */

    std::pair<double,bool> fastProbabilityOfMerging(const VxVertexOnJetAxis*,const VxVertexOnJetAxis*,
						    const VxJetCandidate*) const;

    /**
     * Calculates in a complete way the probability two vertices along the jet axis to be 
     * compatible with each other, taking the constraint to be compatible with the previous 
     * fitted flight axis into account.
     *
     * Contrary to the fast estimation method, here the complete fit is repeated and so the flight axis 
     * direction is corrected according to the new constraint taking all non linearities into account (because 
     * relinearization of the tracks partecipating in the fit is performed).
     *
     * The following steps are done:
     * 1) a temporary copy of the full VxJetCandidate has to be created
     * 2) the copies of the vertices to join have to be found
     * 3) the two copied vertices are merged in the new VxJetCandidate, using 
     *    the mergeVerticesInJetCandidate function of the JetFitterHelper class
     * 4) the track Number is updated, using the updateTrackNumbering function of the JetFitterInitializationHelper class
     * 5) the full fit is performed on the new VxJetCandidate object
     *
     * As a result the only full probability of merging is provided as a double.
     *
     */


    double fullProbabilityOfMerging(const VxVertexOnJetAxis* firstVertex,
				    const VxVertexOnJetAxis* secondVertex,
				    const VxJetCandidate* myJetCandidate,
				    int num_maxiterations=20,
				    bool treat_sign_flip=true,
				    int num_signflip_maxiterations=10,
				    double deltachi2_convergence=1e-2) const;
    /**
     * The VxClusteringTable of the VxJetCandidate provided in input is created, 
     * computing all the needed probabilities of merging between all the vertices 
     * two by two using a full probability estimation, with the complete fit 
     * performed with the options provided. 
     *
     * To increase the speed, before using the full estimation of merging probability 
     * a fast merging is done (fastProbabilityOfMerging method) and only if the fastprobability of 
     * merging is higher than the threshold_probability provided, the full probability is computed.
     *
     */

    void fillTableWithFullProbOfMerging(VxJetCandidate* myJetCandidate,
					int num_maxiterations=20,
					bool treat_sign_flip=true,
					int num_signflip_maxiterations=10,
					double deltachi2_convergence=1e-3,
					double threshold_probability=1e-5) const;


    /**
     * The VxClusteringTable of the VxJetCandidate provided in input is created, 
     * computing all the needed probabilities of merging between all the vertices 
     * two by two using the fast probability estimation (using for all combinations 
     * of vertices the fastProbabilityOfMerging method of the same class)
     *
     * This is very fast and gives reasonably good results in all the cases where the 
     * fit is not too much affected by non-linearities.
     *
     */


    void fillTableWithFastProbOfMerging(VxJetCandidate* myJetCandidate) const;
    //please note that in the case of FastProbOfMerging the threshold_probability is always 1e-3
    //(the default one for the FullProb - see three lines above)

    /**
     * Deltes a vertex from the VxJetCandidate, doing everything is needed 
     * to preserve the internal coherence of the VxJetCandidate object (e.g. track numbering, 
     * list of tracks at vertices along the jet axis and so on)
     */

    void deleteVertexFromJetCandidate(VxVertexOnJetAxis* vertexToDelete,
				      VxJetCandidate* myJetCandidate) const;

  private:

    /**
     * Internal method to calculate fast probability of merging, for merging with primary vertex.
     */

    std::pair<double,bool> fastProbabilityOfMergingWithPrimary(const VxVertexOnJetAxis* otherVertex,
							       const VxJetCandidate* myJetCandidate) const;
      
    /**
     * Internal method to calculate fast probability of merging, for merging with non primary vertex.
     */

    std::pair<double,bool> fastProbabilityOfMergingNoPrimary(const VxVertexOnJetAxis*,
							     const VxVertexOnJetAxis*,
							     const VxJetCandidate* myJetCandidate) const;

    /**
     * Internal method to fill the VxClusteringTable of the VxJetCandidate object, 
     * independently on if the fullcomputation is requested or not. 
     */
    
    void fillTableWithProbOfMerging(VxJetCandidate* myJetCandidate,
				    bool fullcomputation,
				    int num_maxiterations=20,
				    bool treat_sign_flip=true,
				    int num_signflip_maxiterations=10,
				    double deltachi2_convergence=1e-2,
				    double threshold_probability=1e-3) const;
    /**
     * Internal method to provide a check if the VxJetCandidate has been initialized 
     * in a consistent way.
     */

    bool checkJetCandidate(VxJetCandidate*) const;

    /**
     * Method to copy new reco positions to linearization positions
     * after checking new positions are inside the ID
     */
    void copyRecoPositionsToLinearizationPositions(VxJetCandidate & myJetCandidate) const;

    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey
       {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};
 
    ToolHandle<JetFitterInitializationHelper> m_initializationHelper;
    ToolHandle<JetFitterHelper> m_helper;
    ToolHandle<KalmanVertexOnJetAxisUpdator> m_updator;
    ToolHandle<KalmanVertexOnJetAxisSmoother> m_smoother;
    ToolHandle<TrkDistanceFinderNeutralCharged> m_minDistanceFinder;
    ToolHandle<TrkDistanceFinderNeutralNeutral> m_minDistanceFinderNeutral;
    

    /*
     * Old option: please do not switch to true.
     * Will be deleted in future releases.
     */

    bool m_fast;

    /*
     * Provides a limit to the DR shifht of the direction of the flight axis during 
     * the fast estimation procedure for the probability of merging between to vertices.
     * If the shift is higher than that, the fastProbabilityOfMerging method informs 
     * the user that the estimation could not be reliable.
     */

    double m_maxDRshift;

    /*
     * If true, the assumption is made that, whenever tracks are merged to the primary vertex, they were already 
     * contained in the primary vertex fit, for which the prior information about the position and covariance matrix 
     * was already used in initializing the fit.
     *
     * This has the problem that if this was not the case and smoothing of the tracks at the primary vertex is requested, 
     * sometimes a negative weight can come out of the removal of the track (is this is particularly affecting the fit).
     *
     * Unless you know what you are doing, put this to false.
     *
     * Also in this case there is a disadvantage: tracks are refit again into the primary vertex, even if they were 
     * already partecipating in the prior information of the vertex. This will be resolved only when the primary and 
     * secondary vertex fit will be related the one to the other.
     */

    bool m_noPrimaryVertexRefit;

    /*
     * Avoid vertex to be initialize outside ID envelope given bu m_maxR and m_maxZ
     */

    double m_maxR;
    double m_maxZ;

  };//end class declaration

}//end namespace Trk


#endif
