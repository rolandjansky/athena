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
      virtual void getListOfHits(std::vector<const Trk::RIO_OnTrack*>& hits) const = 0;

      /**
       * Return hash built out of associated hits for fast comparison if deviations have same content
       */
      virtual std::size_t getHashOfHits() const = 0;

      /**
       * Set hash built out of associated hits - cannot be calculated in constructor because of dependencies
       */
      virtual void setHashOfHits(std::size_t hash) = 0;

      /**
       * Return if the hash created form associated hits is set
       */
      virtual bool hasValidHashOfHits() const = 0;

      /**
       * Verbose
       */
      virtual void print(std::ostream& out) const = 0;
  };
}
#endif

