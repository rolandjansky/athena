/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

/**
 * @file 
 *
 * Extrapolation for HepMC particles.
 *
 * @author Andrei Gaponenko <agaponenko@lbl.gov>
 */

#ifndef ITRKTRUTHTOTRACK_H
#define ITRKTRUTHTOTRACK_H

#include "GaudiKernel/IAlgTool.h"

// FIXME: namespace Trk { TrackParameters; }
#include "TrkParameters/TrackParameters.h"

#include "AtlasHepMC/GenParticle_fwd.h"

#include "xAODTruth/TruthParticle.h"
//namespace xAOD { class TruthParticle; }

namespace Trk {

  /**
   * ITruthToTrack is an interface to create
   * Trk::TrackParameters from a HepMC::GenParticle.  The motivation is
   * to use a Trk::IExtrapolator for extrapolation of "true" track
   * parameters.
   */

  class ITruthToTrack : virtual public IAlgTool {
  public:

    static const InterfaceID& interfaceID() { 
      static const InterfaceID IID_ITruthToTrack("Trk::ITruthToTrack",1,0);
      return IID_ITruthToTrack;
    }

    virtual ~ITruthToTrack() {}

    /** This function produces a Trk::TrackParameters object
     *  corresponding to the HepMC::GenParticle at the production
     *  vertex.  The object is allocated using operator new and returned
     *  by pointer.  The caller is responsible for deletion of the
     *  object. (Wrapping it into a smart pointer may be the most
     *  convenient way to make sure the memory is freed.)
     */
    virtual const Trk::TrackParameters* makeProdVertexParameters(const HepMC::GenParticle* part) const = 0;
    virtual const Trk::TrackParameters* makeProdVertexParameters(const xAOD::TruthParticle* part) const = 0;

    /** This function extrapolates track to the perigee, and returns
     * perigee parameters.
     *
     * The return value points to an object allocated with the
     * operator new.  The caller is responsible for deletion of the
     * object.
     */
    virtual const Trk::TrackParameters* makePerigeeParameters(const HepMC::GenParticle* part) const = 0;
    virtual const Trk::TrackParameters* makePerigeeParameters(const xAOD::TruthParticle* part) const = 0;

  };
  
} // namespace Trk

#endif/*ITRKTRUTHTOTRACK_H*/
