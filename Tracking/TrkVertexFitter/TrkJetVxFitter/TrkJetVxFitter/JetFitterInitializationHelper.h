/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 *
 * Februar 2007 (c) Atlas Detector Reconstruction Software
 *
 * @class Trk::JetFitterInitializationHelper
 *
 * @author Giacinto Piacquadio (University of Freiburg)
 * @author Christian Weiser (University of Freiburg)
 *
 * (In case of problems, please contact: nicola.giacinto.piacquadio@cern.ch)
 *
 * This class is used to initialize a VxJetCandidate, which is the class to be
 * provided to the 'JetFitter' algorithm in order to perform the fit of all the 
 * vertices on the JetAxis.
 * For using it, please look at the doxygen documentation of its methods.
 *
 */     

#ifndef TrkJetFitter_JetFitterInitializationHelper_H
#define TrkJetFitter_JetFitterInitializationHelper_H

#include "GaudiKernel/ToolHandle.h"
#include <utility>
#include <vector>
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkTrackLink/ITrackLink.h"


namespace Trk {
  class TrackParticleBase;
  class Track;
  class VxJetCandidate;
  class RecVertex;
  class IVertexLinearizedTrackFactory;

  static const InterfaceID IID_JetFitterInitializationHelper("Trk::JetFitterInitializationHelper", 1, 0);


  class JetFitterInitializationHelper : public AthAlgTool
  {

  public:

    static const InterfaceID& interfaceID() {
      return IID_JetFitterInitializationHelper;
    }

    virtual StatusCode initialize() override;

    /**
     * Constructor
     */

    JetFitterInitializationHelper(const std::string& t, const std::string& n, const IInterface*  p);

    /**
     * Destructor
     */

    ~JetFitterInitializationHelper();

    /**
     * Initialize the JetCandidate using a vector of Trk::ITrackLink* - needed for example 
     * if you run on ESD (Track*), but you have additional neutral tracks (TrackParticleBase*)
     * IMPORTANT: Ownership of ITrackLink objects is taken OVER
     */

    VxJetCandidate * initializeJetCandidate(const std::vector<const Trk::ITrackLink*> & vectorOfLink,
					    const RecVertex* primaryVertex,
					    const Amg::Vector3D* jetdirection=0,
                                            const Amg::Vector3D* linearizationjetdirection=0) const;


    /**
     * Initialize the Jet Candidate using a vector of Tracks, the primary vertex of the event (origin 
     * of the flight axis along which the tracks will be constrained) and the jet direction.
     */

    VxJetCandidate * initializeJetCandidate(const std::vector<const Trk::Track*> & vectorOfT,
					    const RecVertex* primaryVertex,
					    const Amg::Vector3D* jetdirection=0,
                                            const Amg::Vector3D* linearizationjetdirection=0) const;
    
    /**
     * Initialize the Jet Candidate using a vector of TrackParticles, the primary vertex of the event (origin 
     * of the flight axis along which the tracks will be constrained) and the jet direction.
     */


    VxJetCandidate * initializeJetCandidate(const std::vector<const Trk::TrackParticleBase*> & vectorOfTP,
					    const RecVertex* primaryVertex,
					    const Amg::Vector3D* jetdirection=0,
                                            const Amg::Vector3D* linearizationjetdirection=0) const;
    
    /**
     * Does the update of the ordering of the vertices along the jetaxis.
     * This is needed after you make a change to the number of clusters (initially single tracks) 
     * of the VxJetCandidate, because you need to have for each VxVertexOnJetAxis a number stored 
     * which tells you where the corresponding position along the jet axis is stored in the RecVertexPositions
     * (position + covariance matrix), which is then used to read/store the fit result.
     *
     * The method initializeJetClusters already calls this update during its initialization of the VxJetCandidate.
     *
     */

    void updateTrackNumbering(VxJetCandidate*) const;

    /**
     * Calls the linearization of all the tracks (adds the Linearized Track data member 
     * to every VxTrackAtVertex (VxTrackOnJetAxis) involved in the fit).
     *
     * If the track was already initialized previously, then linearization is done only if the 
     * distance to the previous linearization point is bigger than what specified in mm ( default 
     * is 1 mm ).
     *
     */

    void linearizeAllTracks(VxJetCandidate*,
			    bool signfliptreatment=false,
			    double maxdistance=1.) const;    //CLHEP::mm

  private:

    /**
     * Internal method to initialized a VxJetCandidate
     */

    VxJetCandidate * initializeJetClusters(VxJetCandidate* myJetCandidate,
					   const RecVertex* primaryVertex,
					   const Amg::Vector3D* jetdirection=0,
                                           const Amg::Vector3D* linearizationjetdirection=0) const;
					   

    ToolHandle<IVertexLinearizedTrackFactory> m_linearizedFactory;

    /**
     * Error on phi on the flight direction you want to initialize the fit with
     * (set errphiJetAxis by JobOptions, default is 0.07).
     *
     * This should be substituted with a parameterization of the calorimeter 
     * resolution, which should depend on phi and eta.
     */

    float m_errphiJetAxis;

    /**
     * Error on eta on the flight direction you want to initialize the fit with
     * (set erretaJetAxis by JobOptions, default is 0.065)
     *
     * This should be substituted with a parameterization of the calorimeter 
     * resolution, which should depend on phi and eta.
     */

    float m_erretaJetAxis;

  };//end class declaration

}//end namespace Trk


#endif
