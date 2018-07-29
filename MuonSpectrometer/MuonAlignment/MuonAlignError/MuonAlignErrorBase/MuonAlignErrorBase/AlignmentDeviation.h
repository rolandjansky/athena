/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNERRORBASE_ALIGNMENTDEVIATION_H
#define MUONALIGNERRORBASE_ALIGNMENTDEVIATION_H

#include "GeoPrimitives/GeoPrimitives.h"
#include <iosfwd>
#include <vector>

namespace Trk {

  class RIO_OnTrack;

  /**
   * An object decorating a track and holding degrees of freedom reflecting alignment accuracy
   */
  class AlignmentDeviation {
    public:

      virtual ~AlignmentDeviation() {}

      /**
       * The number of free parameters
       */
      virtual int nPar() const = 0;

      /**
       * The error matrix on the free parameters. Track fitters should use this
       * to compute a constraint on the free parameters.
       */
      virtual double getCovariance(int ipar, int jpar) const = 0;

      /**
       * Return a Transform in the global coordinate system, given a list of
       * parameters.
       */
      virtual Amg::Transform3D getTransform(const std::vector<double>& parameters) const = 0;

      /**
       * Return the list of hits the transform should be applied to.
       */
      void getListOfHits(std::vector<const Trk::RIO_OnTrack*>& hits) const;

      /**
       * Return hash built out of associated hits for fast comparison if deviations have same content
       */
      std::size_t getHashOfHits() const;

      /**
       * Set hash built out of associated hits - cannot be calculated in constructor because of dependencies
       */
      void setHashOfHits(std::size_t hash);

      /**
       * Return if the hash created form associated hits is set
       */
      bool hasValidHashOfHits() const;

      /**
       * Verbose
       */
      virtual void print(std::ostream& out) const = 0;

    protected:
      std::vector<const Trk::RIO_OnTrack*> m_hits; // not owned
      std::size_t m_hitshash;
      bool m_hitshashdone;
  };

  inline void AlignmentDeviation::getListOfHits (std::vector<const Trk::RIO_OnTrack*>& hits) const {
    hits = m_hits;
  }

  inline std::size_t AlignmentDeviation::getHashOfHits () const {
    return m_hitshash;
  }

  inline void AlignmentDeviation::setHashOfHits (std::size_t hash) {
    m_hitshash = hash;
    m_hitshashdone = true;
  }

  inline bool AlignmentDeviation::hasValidHashOfHits () const {
    return m_hitshashdone;
  }

}
#endif

