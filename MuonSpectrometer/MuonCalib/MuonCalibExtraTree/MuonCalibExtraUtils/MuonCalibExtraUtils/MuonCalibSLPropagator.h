/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBSLPROPAGATOR_H
#define MUONCALIBSLPROPAGATOR_H

#include "GeoPrimitives/GeoPrimitives.h"

namespace MuonCalib {

    class MuonCalibSurface;
    class MuonCalibDisk;
    class MuonCalibCylinder;

    /** straight line propagator */
    class MuonCalibSLPropagator {
    public:
        /** Propagator track parameters to a plane
            @param pos position on the parameters
            @param dir direction of the parameters
            @param planePos position of the plane
            @param normal of the plane
            @return position on the plane
        */
        Amg::Vector3D propagateToPlane(const Amg::Vector3D& pos, const Amg::Vector3D& dir, const Amg::Vector3D& planePos,
                                       const Amg::Vector3D& planeNormal) const;

        /** Propagator track parameters to a plane paralel to the xy plane
            @param pos position on the parameters
            @param dir direction of the parameters
            @param zPosPlane z position of the plane
            @return position on the plane
        */
        Amg::Vector3D propagateToPlane(const Amg::Vector3D& pos, const Amg::Vector3D& dir, double zPosPlane) const;

        /** Propagator track parameters to a cylinder paralel to the z-axis, centered at (0.,0.) in the xy plane
            @param pos position on the parameters
            @param dir direction of the parameters
            @param radius of the cylinder
            @return position on the cylinder
        */
        Amg::Vector3D propagateToCylinder(const Amg::Vector3D& pos, const Amg::Vector3D& dir, double radiusCylinder) const;

        /** Propagator track parameters to a straight line
            @param pos position on the parameters
            @param dir direction of the parameters
            @param posLine position on the line
            @param dirLine direction of the line
            @return point of closest approach to line
        */
        Amg::Vector3D propagateToStraightLine(const Amg::Vector3D& pos, const Amg::Vector3D& dir, const Amg::Vector3D& posLine,
                                              const Amg::Vector3D& dirLine) const;

        /** Propagator track parameters to the perigee
            @param pos position on the parameters
            @param dir direction of the parameters
            @return position at perigee
        */
        Amg::Vector3D propagateToPerigee(const Amg::Vector3D& pos, const Amg::Vector3D& dir) const;

        /** Propagator track parameters to a plane paralel to the xy plane
            @param pos position on the parameters
            @param dir direction of the parameters
            @param disk destination disk
            @return position on the surface
        */
        Amg::Vector3D propagate(const Amg::Vector3D& pos, const Amg::Vector3D& dir, const MuonCalibSurface& surf) const;

        /** Propagator track parameters to a plane paralel to the xy plane
            @param pos position on the parameters
            @param dir direction of the parameters
            @param disk destination disk
            @return position on the plane
        */
        Amg::Vector3D propagate(const Amg::Vector3D& pos, const Amg::Vector3D& dir, const MuonCalibDisk& disk) const;

        /** Propagator track parameters to a plane paralel to the xy plane
            @param pos position on the parameters
            @param dir direction of the parameters
            @param cylinder destination cylinder
            @return position on the cylinder
        */
        Amg::Vector3D propagate(const Amg::Vector3D& pos, const Amg::Vector3D& dir, const MuonCalibCylinder& cylinder) const;

    private:
    };

}  // namespace MuonCalib

#endif
