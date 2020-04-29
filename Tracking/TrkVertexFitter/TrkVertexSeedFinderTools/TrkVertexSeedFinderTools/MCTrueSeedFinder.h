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

#include "AtlasHepMC/GenParticle_fwd.h"
#include "AtlasHepMC/GenEvent_fwd.h"

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

  class MCTrueSeedFinder : public extends<AthAlgTool, IVertexSeedFinder>
  {
  public:
    // Standard Gaudi constructor.
    MCTrueSeedFinder (const std::string& t,
                      const std::string& n,
                      const IInterface*  p);


    virtual ~MCTrueSeedFinder();


    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;


    using IVertexSeedFinder::findSeed;


    /**
     *  Finds a linearization point out of a vector of tracks
     *  and returns it as an Amg::Vector3D object. If you want an 
     *  additional constraint can be taken into account.
     */
    virtual Amg::Vector3D
    findSeed (const std::vector<const Trk::Track*> & vectorTrk,
              const xAOD::Vertex * constraint=0) const override;
    

    /** 
     * Finds a linearization point out of a vector of TrackParameters
     *  and returns it as an Amg::Vector3D object. If you want an 
     * additional constraint can be taken into account.
     */
    virtual Amg::Vector3D
    findSeed (const std::vector<const Trk::TrackParameters*> & perigeeList,
              const xAOD::Vertex * constraint=0) const override;


    /**
     * Finds full vector of linearization points from a vector of tracks
     *  and returns it as an Amg::Vector3D object.  Intended for seed finders that produce all at once.
     *  If you want an additional constraint can be taken into account.
     */
    virtual std::vector<Amg::Vector3D>
    findMultiSeeds(const std::vector<const Trk::Track*>& vectorTrk,
                   const xAOD::Vertex * constraint=0) const override;


    /**
     * Finds full vector of linearization points from a vector
     * of TrackParameters and returns it as an Amg::Vector3D object.
     * Intended for seed finders that produce all at once.
     * If you want an additional constraint can be taken into account.
     */
    virtual std::vector<Amg::Vector3D>
    findMultiSeeds(const std::vector<const Trk::TrackParameters*>& perigeeList,
                   const xAOD::Vertex * constraint=0) const override;


  private:
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

    StatusCode retrieveInteractionsInfo (std::vector<Amg::Vector3D>& interactions) const;
  };
}
#endif
