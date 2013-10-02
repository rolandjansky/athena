/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file
 * This file defines the Trk::InverseTruthMap template class.
 */

#ifndef INVERSETRUTHMAP_H
#define INVERSETRUTHMAP_H

#include <map>

namespace HepMC { class GenParticle; }

namespace Trk {
  /**
   * @class InverseTruthMap
   *
   * An InverseTruthMap object allows to quickly find a reconstructed
   * object matching a given GenParticle.
   *
   * A map for lookup in the other direction is given by TrackTruthCollection 
   * (or by SpacePointTruthCollection, or by PrepRawDataTruthCollection).
   * Note that a true "inverse" of a TrackTruthCollection would not be very useful,
   * because TrackTruthCollection maps 
   *
   *  Trk::Track*  ==>  (an object containing GenParticle* AND SOME OTHER STUFF)
   *
   * therefore to use a true inverse map you would need to have that
   * SOME OTHER STUFF to perform a lookup, in addition to the
   * GenParticle you are interested in.  (The actual code uses
   * persistifiable pointers, I use the bare pointer notation for the
   * sake of clarity.)
   *
   * The map inversion discards SOME OTHER STUFF data.  If needed, they
   * can be accessed through the original map.  That is, after a
   * GenParticle* ==> Trk::Track* lookup, use the found Trk::Track* to
   * extract its corresponding TrackTruth object that contains the
   * matching probability.
   *
   * Similar for SpacePointTruthCollection and PrepRawDataTruthCollection.
   * 
   * Example of use:
   * @code
   *     #include "TrkEventUtils/InverseTruthMap.h"
   *     #include "TrkEventUtils/TruthCollectionInverter.h"
   *
   *     ...
   *
   *     TrackTruthCollection *origMap = ...; // retrieve from SG
   *
   *     Trk::InverseTruthMap<TrackTruthCollection> invMap;
   *
   *     Trk::buildInverseTruthMap(*origMap, &invMap);
   * @endcode
   *
   * InverseTruthMap requires that the TruthCollection template argument
   * defines a key_type, like std::map does.
   *
   * @see TruthInverters
   *
   * @author Andrei Gaponenko <agaponenko@lbl.gov>, 2005
   */
  template<class TruthCollection> class InverseTruthMap : 
    public std::map<const HepMC::GenParticle*, typename TruthCollection::key_type> {};

}

#endif/*INVERSETRUTHMAP_H*/
