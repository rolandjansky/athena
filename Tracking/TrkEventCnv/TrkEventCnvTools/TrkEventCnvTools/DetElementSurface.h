/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKEVENTCNVTOOLS_DETELEMENTSURFACE_H
#define TRKEVENTCNVTOOLS_DETELEMENTSURFACE_H
#include "TrkSurfaces/Surface.h"
#include "TrkDetDescrUtils/Intersection.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/DistanceSolution.h"
#include "TrkParametersBase/ParametersBase.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include <cstdlib>//for "abort"

namespace Trk
{

/** Dummy class used to allow special convertors to be called for surfaces owned by a detector element.
Only intended for use within the persistency framework.*/
// template <class SURFACE>
// class DetElementSurface : public SURFACE
// {
//     public:
//     DetElementSurface( const DetElementSurface& rhs) : SURFACE(rhs) {}
// };


template<int DIM, class T, class S> class ParametersT;

class DetElementSurface : public Surface
{
  public:
    DetElementSurface() : Surface() {}
    DetElementSurface(Identifier id) : Surface(), m_id(id) {}
    DetElementSurface( const Surface& rhs) : Surface(rhs), m_id(rhs.associatedDetectorElementIdentifier()) {}
// Normally the Surface copy ctor sets the Identifier to be invalid. Store it, to get around this.
    bool                        operator==(const  Trk::Surface&) const { return false;}
    Surface *                   clone () const { return nullptr; }
    bool                        insideBounds(const Amg::Vector2D &, double, double) const {return false;}
    bool insideBoundsCheck(const Amg::Vector2D& /*locpos*/, const BoundaryCheck& /*bchk*/) const { return false;}
    const Amg::Vector3D *       localToGlobal (const Amg::Vector2D &) const { return nullptr; }
    const Amg::Vector3D *       localToGlobal (const LocalParameters &) const { return nullptr; }
    const Amg::Vector2D *       globalToLocal (const Amg::Vector3D &, const double) const { return nullptr; }
    virtual DistanceSolution    straightLineDistanceEstimate (const Amg::Vector3D &, const Amg::Vector3D &) const { return DistanceSolution(); }
    const SurfaceBounds &       bounds () const { std::abort();/*should never be called*/ }
    MsgStream &                 dump (MsgStream &sl) const { return sl;}
    std::ostream &              dump (std::ostream &sl) const { return sl;};
    const Identifier            associatedDetectorElementIdentifier() const {return m_id;}
    bool                        isFree() {return true;}// To avoid memory leaks
    Surface::SurfaceType        type() const {return Surface::Other;}
    void                        localToGlobal(const Amg::Vector2D&, const Amg::Vector3D&, Amg::Vector3D&) const {}
    bool                        globalToLocal(const Amg::Vector3D&, const Amg::Vector3D&, Amg::Vector2D&) const {return false;}
    std::string                 name() const { return "Trk::DetElementSurface";}
    // A dummy method required for consistency of the package and
    virtual std::unique_ptr<ParametersBase<5, Trk::Charged>>
    createUniqueTrackParameters(double,
                                double,
                                double,
                                double,
                                double,
                                AmgSymMatrix(5) *) const
    {
      return nullptr;
    }
    virtual std::unique_ptr<ParametersBase<5, Trk::Charged>>
    createUniqueTrackParameters(const Amg::Vector3D&,
                                const Amg::Vector3D&,
                                double,
                                AmgSymMatrix(5) *) const
    {
      return nullptr;
    }

    virtual std::unique_ptr<ParametersBase<5, Trk::Neutral>>
    createUniqueNeutralParameters(double,
                                  double,
                                  double,
                                  double,
                                  double,
                                  AmgSymMatrix(5) *) const
    {
      return nullptr;
    }
    virtual std::unique_ptr<ParametersBase<5, Trk::Neutral>>
    createUniqueNeutralParameters(const Amg::Vector3D&,
                                  const Amg::Vector3D&,
                                  double,
                                  AmgSymMatrix(5) *) const
    {
      return nullptr;
    }

    virtual Intersection straightLineIntersection(const Amg::Vector3D&,
                                                  const Amg::Vector3D&,
                                                  bool, Trk::BoundaryCheck) const { return Trk::Intersection(Amg::Vector3D(0.,0.,0.),0.,false); }


// persistency libraries..
    virtual Trk::DistanceSolution straightLineDistanceEstimate(const Amg::Vector3D&,
                                           const Amg::Vector3D&, bool) const {return 0;}

    virtual bool isOnSurface(const Amg::Vector3D&, Trk::BoundaryCheck, double, double) const {return false;}

private:
    Identifier  m_id;
};


}

#endif
