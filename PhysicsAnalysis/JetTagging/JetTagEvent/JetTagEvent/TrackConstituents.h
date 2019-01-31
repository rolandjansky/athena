/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           TrackConstituent.h  -  Description
                             -------------------
    begin   : January, 2005
    authors : Kyle Cranmer, Andreas Wildauer
    email   : cranmer@cern.ch, andreas.wildauer@cern.ch

    changes : 

***************************************************************************/

#ifndef ANALYSIS_TRACKCONSTITUENT_H
#define ANALYSIS_TRACKCONSTITUENT_H

#include<vector>
#include<string>

#include "Navigation/Navigable.h"
#include "Particle/TrackParticleContainer.h"
#include "ParticleEvent/IConstituent.h"
//class INavigable4MomentumCollection;
//class INavigable4Momentum;

namespace Analysis
{
  /** Class TrackConstituent: This is the implemntation of the
      TrackConstituents class for ParticleJet

      @author cranmer@cern.ch
  */

  class TrackConstituents : public IConstituent,
                            public Navigable<Rec::TrackParticleContainer, double>
    {
      typedef Navigable<Rec::TrackParticleContainer,double>::external_index_type index_type;
      typedef Navigable<Rec::TrackParticleContainer,double>::constituent_iter  iter_type;
      //      typedef object_iter  iterator;
      
    public:
      TrackConstituents() ;      //!< constructor
      TrackConstituents(NameType& name) ;      //!< constructor
      ~TrackConstituents() = default;

      typedef Navigable<Rec::TrackParticleContainer,double>::object_iter  iterator;


      virtual NameType name() const;
      //!< returns the infoType of the info objects. This has to be set by the tagger.

      virtual IConstituent* clone() const; 
      //!< a clone method for the proper workings of the copy constructor

      //iterator begin(); available via Navigable with (*iterator) giving TrackParticle*
      //iterator end(); available via Navigable

      /// get a vector of trackparticles.  user must delete the vector, but not constituents
      std::vector<const Rec::TrackParticle*>* tracks() const;
      
      unsigned int nTracks() const { return size() ;    }

      double getTrackWeight(const Rec::TrackParticle* ) const;
      
      double getTrackWeight(const Rec::TrackParticleContainer*, index_type& ) const;
      
      //////////////////////
      // Set Methods
      /////////////////////

      virtual void setName(NameType& name);
      //!< to set the name

      /**set method for tracks.  By default, navigable weight = 0 which
	 reflects that the tracks do not contribute to the kinematics.*/
      void set_track(const Rec::TrackParticleContainer*,const Rec::TrackParticle*, 
		     double weight=0.);
      /**set method for tracks.  By default, navigable weight = 0 which
	 reflects that the tracks do not contribute to the kinematics.*/
      void set_track(const Rec::TrackParticleContainer*, index_type&, double weight=0.);

      // LACK OF KNOWLEDGE HACK HERE
      // use also INavigable4MomentumCollection and INavigable4Momentum to add constituents
      // would be nice to have a addConstituents method in the IConstituent interface (dont know if that is possible)
      /**set method for tracks.  By default, navigable weight = 0 which
	 reflects that the tracks do not contribute to the kinematics.*/
//       void set_track(const INavigable4MomentumCollection*,const INavigable4Momentum*, 
// 		     double weight=0.);

      //compromise for above... use concrete container, but common name for templated methods
      void set_constituent(const Rec::TrackParticleContainer*,const Rec::TrackParticle*, 
		     double weight=0.);
      void set_constituent(const Rec::TrackParticleContainer*, index_type&, double weight=0.);
      


    private:
      std::string m_name;      

    }
  ;
  
}


#endif
