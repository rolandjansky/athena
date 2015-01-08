/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNERRORTOOL_ALIGNMENTROTATIONDEVIATION_H
#define MUONALIGNERRORTOOL_ALIGNMENTROTATIONDEVIATION_H

#include "MuonAlignErrorTool/AlignmentDeviation.h"

namespace MuonAlign {
  class AlignmentRotationDeviation : public Trk::AlignmentDeviation {
    public:

      AlignmentRotationDeviation(Amg::Vector3D center, Amg::Vector3D axis, double sigma, const std::vector<const Trk::RIO_OnTrack*>& hits);

      virtual ~AlignmentRotationDeviation();

      /**
       * The number of free parameters
       */
      virtual int nPar() const;

      /**
       * The error matrix on the free parameters. Track fitters should use this
       * to compute a constraint on the free parameters.
       */
      virtual double getCovariance(int ipar, int jpar) const;

      /**
       * Return a Transform in the global coordinate system, given a list of
       * parameters.
       */
      virtual Amg::Transform3D getTransform(const std::vector<double>& parameters) const;

      /**
       * Return the list of hits the transform should be applied to.
       */
      virtual void getListOfHits(std::vector<const Trk::RIO_OnTrack*>& hits) const;

      /**
       * Verbose
       */
      virtual void print(std::ostream& out) const;

    private:
      Amg::Vector3D m_center;
      Amg::Vector3D m_axis;
      double m_sigma;
      std::vector<const Trk::RIO_OnTrack*> m_hits; // not owned
  };
}

#endif

