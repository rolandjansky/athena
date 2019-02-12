/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSIS_TRACKASSOCIATION_H
#define ANALYSIS_TRACKASSOCIATION_H

#include<vector>
#include<string>

#include "JetEvent/JetAssociationBase.h"
#include "Navigation/Navigable.h"
#include "Particle/TrackParticleContainer.h"

namespace Analysis
{
  class TrackAssociation : public JetAssociationBase,
    public Navigable<Rec::TrackParticleContainer, double>
    {
      typedef Navigable<Rec::TrackParticleContainer,double>::external_index_type index_type;
      typedef Navigable<Rec::TrackParticleContainer,double>::constituent_iter  iter_type;
      //      typedef object_iter  iterator;
      typedef std::string  NameType;
      
    public:
      TrackAssociation() ;      //!< constructor
      TrackAssociation(const NameType& name);      //!< constructor
      ~TrackAssociation() = default;
      
      typedef Navigable<Rec::TrackParticleContainer,double>::object_iter  iterator;
      
      /// get a vector of trackparticles.  user must delete the vector, but not constituents
      std::vector<const Rec::TrackParticle*>* tracks() const;
      
      JetAssociationBase* clone() const;
      
      unsigned int nTracks() const { return size() ;    }
      
      double getTrackWeight(const Rec::TrackParticle* ) const;
      
      double getTrackWeight(const Rec::TrackParticleContainer*, const index_type& ) const;
      
      //////////////////////
      // Set Methods
      /////////////////////
      
      
      /** set method for tracks.  By default, navigable weight = 0 which
	  reflects that the tracks do not contribute to the kinematics.   */
      void set_track(const Rec::TrackParticleContainer*,
		     const Rec::TrackParticle*, 
		     double weight=0.);
      
      /** set method for tracks.  By default, navigable weight = 0 which
	  reflects that the tracks do not contribute to the kinematics.   */
      void set_track(const Rec::TrackParticleContainer*,
		     const index_type&,
		     double weight=0.);
      
      //compromise for above... use concrete container, but common name for templated methods
      void set_association(const Rec::TrackParticleContainer*,
			   const Rec::TrackParticle*, 
			   double weight=0.);
      void set_association(const Rec::TrackParticleContainer*,
			   const index_type&,
			   double weight=0.);
            
    };
}

#endif
