/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          InDetPriVxFinderTool.h  -  Description
                             -------------------
    begin   : 27-01-2004
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
     changes : 14-02-2006 added inheritance from abstract class IPriVxFinderTool
                          changed const VxCandidate* to VxCandidate*
                          (necessary to compile with gcc starting from 3.3.x)
                          by GP (Freiburg University)

    changes: 18-11-2010  Kirill Prokofiev   
             Development never stops ;). 
	     Now adding the split vertices for the HI run

 ***************************************************************************/

#ifndef INDETPRIVXFINDERTOOL_INDETPRIVXFINDERTOOL_H
#define INDETPRIVXFINDERTOOL_INDETPRIVXFINDERTOOL_H

#include "InDetRecToolInterfaces/IVertexFinder.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
//cannot be forward declared because of typedef
#include "TrkTrack/TrackCollection.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrkParameters/TrackParameters.h"

#include <vector>

/** Primary Vertex Finder Tool.
  InDetPriVxFinderTool uses the Billoir method to fit a primary vertex in the event. The
  method includes taking all tracks passing the preselection and to fit them.
  In a second iteration tracks with a 'too big' chi-squared contribution are
  left out and the remaining tracks are refitted. The 'too big' contribution
  (maximum chi-square) can be set by jobOptions. Preselection cuts can be
  given via jobOptions for d0, z0 and minimum Pt. It is also possible to set
  a beam constraint to be considered in the fit. this is also done in the
  jobOptions.

    - \c TracksName: The name of the StoreGate input container from
      which the tracks are read. The default is "TrkTracks", the container
      from the legacy converters/ambiguity processor.
    - \c VxCandidatesOutputName: The name of the StoreGate container where the fit
      results are put. default is "VxPrimaryCandidate".
    - \c FitRoutine: The routine which should be used for the fitting. The
      default is "FastVertexFitter".

    - \c chi2 cut method: Specifies how to chi2 cut on tracks. There are two possibilities:
      Version 1: (chi2CutMethod = 1)
          - get rid of all tracks with chi2 > m_maxChi2PerTrack in one go
          - no refit after removing one track
          - refit after all tracks with too high chi2 were removed
      Version 2: (chi2CutMethod = 2)
          - get rid of tracks one by one starting with the one with highest chi2 > m_maxChi2PerTrack
          - refit after this track has been removed
            and do a chi2 cut again until all chi2 < m_maxChi2PerTrack
    - \c maxChi2PerTrack: The maximum chi-squared of a track to be considered
         in the second iteration of the vertex fitting. default is 10.

    - \c useBeamConstraint: Use beam spot constraint. default is no (0).
    - \c BeamConstraint_x: Constrained x-position of beam spot. default is 0.
    - \c BeamConstraint_x: Constrained y-position of beam spot. default is 0.
    - \c BeamConstraint_x: Constrained z-position of beam spot. default is 0.
    - \c BeamConstraint_err_x: Error on constrained x-position. default is 0.
    - \c BeamConstraint_err_y: Error on constrained y-position. default is 0.
    - \c BeamConstraint_err_z: Error on constrained z-position. default is 5.6 cm.

    - \c Track selection cuts:
          minPt = 1000. CLHEP::MeV: the minimum pt(CLHEP::MeV) of a track.
          maxZ0 = 150. CLHEP::mm: the maximal z0(CLHEP::mm) of a track.
          maxD0 =   1. CLHEP::mm; the maximal d0(CLHEP::mm) of a track.

*/

/* Forward declarations */
class VxContainer;
class IBeamCondSvc;
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace Trk
{
class IVertexFitter;
class VxCandidate;
class Track;
class TrackParticleBase;
class IVxCandidateXAODVertex;
}

namespace InDet
{
  class IMultiPVSeedFinder;
  class IInDetTrackSelectionTool;
  
  class InDetPriVxFinderTool : public AthAlgTool, virtual public IVertexFinder
{
public:
    InDetPriVxFinderTool(const std::string& t, const std::string& n, const IInterface*  p);
    virtual ~InDetPriVxFinderTool();
    StatusCode initialize();
    VxContainer* findVertex(const TrackCollection* trackTES);
    VxContainer* findVertex(const Trk::TrackParticleBaseCollection* trackTES);
    std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(const xAOD::TrackParticleContainer* trackParticles);
    StatusCode finalize();
private:
    ToolHandle < InDet::IMultiPVSeedFinder > m_iPriVxSeedFinder;
    ToolHandle < Trk::IVertexFitter > m_iVertexFitter;
    ToolHandle< InDet::IInDetTrackSelectionTool > m_trkFilter;
    ToolHandle< Trk::IVxCandidateXAODVertex > m_VertexEdmFactory;

    ServiceHandle<IBeamCondSvc> m_iBeamCondSvc; //!< pointer to the beam condition service
    
    /** Define a beam constraint for the fit */
    bool m_useBeamConstraint; //!<  Use a vertex/beam constraint

    /** choose the chi2 cut method */
    int    m_chi2CutMethod;
    bool   m_enableMultipleVertices;
    double m_clusterLength;
    /** The maximum chi-squared per track which is allowed in the fit. */
    double m_maxChi2PerTrack;

    /** the common finding code (regardless of Track or TrackParticle(Base) is here */
    VxContainer* m_findVertex(std::vector< std::vector<const Trk::TrackParameters*> >& origParameters);
    
    virtual void m_sortTracksInChi2(std::vector<int> &indexOfSortedChi2, Trk::VxCandidate * myVxCandidate);
    virtual void m_sortTracksInZ0(std::vector<const Trk::TrackParameters*> tv,std::vector<int>& indexOfSortedZ0);
    
    
    bool m_createSplitVertices;
    int m_splitVerticesTrkInvFraction; ///< Integer: 1./fraction of tracks to be assigned to the tag split vertex 
 };
}
#endif
