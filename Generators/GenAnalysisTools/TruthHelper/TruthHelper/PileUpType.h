/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*************************************************************************
//*                                                                       *
//*  class  PileUpType                                                   **
//*                                                                       *
//*  Returns iterators on the pileup type particles:
//*  - the  signal particles
//*  - the  in-time Minimum bias particles
//*  - the signal+in-time Minimum bias particles
//*  - the out-of-time Minimum bias particles
//*  - the cavern background particles
//*  - the Selector function MUST have the operator() implemented - for example: isGenStable
//*
// Author: Ketevi A. Assamagan <ketevi@bnl.gov>
// Date: February 2008                                                    *
//*************************************************************************
#ifndef TRUTHHELPER_PILEUPTYPE_H
#define TRUTHHELPER_PILEUPTYPE_H

#include "GeneratorObjects/McEventCollection.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"

#include <vector>

/// For details on how the MC particles are defined and organized see
///  https://twiki.cern.ch/twiki/bin/view/Atlas/PileupPerformance#MC_Truth_Task_Force_Recommendati
///
/// @todo Move this elsewhere -- it does not implement the GenAccessIO API.
class PileUpType {
public:

    /**  need to pass the McEvntCollection
	this class does not own this collection and will NOT delete it at the end */
    PileUpType(const McEventCollection * collection);



    const HepMC::GenEvent * signal_mc_event() const;

    template<class Selector>
    void signal_particles( std::vector<HepMC::ConstGenParticlePtr>& particleList,
                           const Selector& select ) const;

    /** extract the in-time minimum bias McEvent Particles from the McEventCollection
        A pair of iterators is returned on all the McEventCollection */
    McEventCollection::const_iterator in_time_minimum_bias_event_begin() const;
    McEventCollection::const_iterator in_time_minimum_bias_event_end() const;

    /** extract the signal and the in-time minimum bias McEvent Particles from the McEventCollection
        A pair of iterators is returned on all the McEventCollection
        The signal is the first in the list
    */
    McEventCollection::const_iterator signal_and_in_time_minimum_bias_event_begin() const;
    McEventCollection::const_iterator signal_and_in_time_minimum_bias_event_end() const;

    template<class Selector>
    void in_time_minimum_bias_particles( std::vector<HepMC::ConstGenParticlePtr>& particleList,
                                         const Selector& select ) const;

    /** the in-time particles - this includes in the signal */
    McEventCollection::const_iterator in_time_event_begin() const;
    McEventCollection::const_iterator in_time_event_end() const;

    template<class Selector>
    void in_time_particles( std::vector<HepMC::ConstGenParticlePtr>& particleList,
                            const Selector& select ) const;

    /** return the MC events in [-2BC, +2BC], including the signal McEvent at BC=0 */
    template<class Selector>
    void particles_in_two_bunch_crossings( std::vector<HepMC::ConstGenParticlePtr>& particleList,
                                           const Selector& select ) const;

    /** return the MC events in [-inf,-2BC] and [2BC,+inf], excluding the cavern background and the beam halo */
    template<class Selector>
    void particles_outside_two_bunch_crossings( std::vector<HepMC::ConstGenParticlePtr>& particleList,
                                                const Selector& select ) const;

    /** return the MC events of the cavern background */
    template<class Selector>
    void cavern_background_particles( std::vector<HepMC::ConstGenParticlePtr>& particleList,
                                      const Selector& select ) const;

private:
    const McEventCollection * m_particleList;
};


#include "TruthHelper/PileUpType.icc"


#endif
