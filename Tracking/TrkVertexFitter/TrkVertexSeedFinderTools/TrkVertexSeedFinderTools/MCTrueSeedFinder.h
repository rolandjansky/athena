/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERTOOLS_MCTRUESEEDFINDER_H
#define TRKVERTEXSEEDFINDERTOOLS_MCTRUESEEDFINDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"
#include "xAODEventInfo/EventInfo.h"
#include "GeneratorObjects/McEventCollection.h"

class IPartPropSvc;

namespace HepMC {
class GenEvent;
class GenParticle;
}

namespace Trk
{
  // @author N. Giacinto Piacquadio (Albert-Ludwigs-Universitaet Freiburg - Germany)
  //
  // @ATLAS software
  //
  // This class implements a dummy seed finder
  //
  // -----------------------------------------
  // Changes:
  //
  // David Shope <david.richard.shope@cern.ch> (2016-04-19)
  //
  // EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex, 
  //                         from Trk::RecVertex   to xAOD::Vertex,
  //                         from Trk::Vertex      to Amg::Vector3D

  class MCTrueSeedFinder : public AthAlgTool, virtual public IVertexSeedFinder
  {
  public:
    StatusCode initialize();
    StatusCode finalize();

    //default constructor due to Athena interface
    MCTrueSeedFinder(const std::string& t, const std::string& n, const IInterface*  p);
    
    //destructor
    virtual ~MCTrueSeedFinder();

    // Interface for Tracks with starting seed/linearization point
    virtual Amg::Vector3D findSeed(const std::vector<const Trk::Track*> & vectorTrk,const xAOD::Vertex * constraint=0);
    
    /** Interface for MeasuredPerigee with starting point */
    virtual Amg::Vector3D findSeed(const std::vector<const Trk::TrackParameters*> & perigeeList,const xAOD::Vertex * constraint=0);

    // Interface for finding vector of seeds from tracks
    virtual std::vector<Amg::Vector3D> findMultiSeeds(const std::vector<const Trk::Track*>& vectorTrk,const xAOD::Vertex * constraint=0);

    // Interface for finding vector of seeds from track parameters
    virtual std::vector<Amg::Vector3D> findMultiSeeds(const std::vector<const Trk::TrackParameters*>& perigeeList,const xAOD::Vertex * constraint=0);

    //The below four functions are dummy functions so that this compiles. The functions are needed in the interface IMode3dFinder.h for Mode3dFromFsmw1dFinder (the seed finder for the Inclusive Secondary Vertex Finder)

    virtual void setPriVtxPosition( double vx, double vy );

    virtual int perigeesAtSeed( std::vector<const Trk::TrackParameters*> * a,
                              const std::vector<const Trk::TrackParameters*> & b ) const;

    virtual int getModes1d(std::vector<float>& a, std::vector<float>& b, 
			   std::vector<float>& c, std::vector<float>& d  ) const;
    virtual void getCorrelationDistance( double &cXY, double &cZ );


  private:
    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey { this, "EventInfo", "EventInfo", "key to retrieve EventInfo" };
   
    SG::ReadHandleKey<McEventCollection> m_mcEventCollectionKey { this, "McTruthCollection", "G4Truth", "MC Event Collection Name" };

    /// Get particle properties
    ServiceHandle< IPartPropSvc > m_partPropSvc;

    //Implement directly pass methods from GenObjectsFilterTool until better sol'n is found
    bool m_removeInTimePileUp; ///< Flag to consider in-time pile-up interactions
    bool m_removeHardScattering; ///< Flag to consider hard-scattering interaction

    /// Function selecting GenEvent objects
    bool pass( const HepMC::GenEvent* evt,
	       const McEventCollection* coll = 0 ) const;
    /// Function selecting GenParticle objects
    bool pass( const HepMC::GenParticle* part,
	       const McEventCollection* coll = 0 ) const;

    /// Store collection of interactions' position sorted by "intensity" (sumpt2)
    std::vector<Amg::Vector3D> m_interactions;

    StatusCode retrieveInteractionsInfo();

    //cache control variables
    unsigned int m_cacheRunNumber; ///< cached results for given run/event number
    unsigned int m_cacheEventNumber; ///< cached results for given run/event number
    unsigned int m_currentInteractionIdx; ///< keep track of what interactions we've given already
    
  };
}
#endif
