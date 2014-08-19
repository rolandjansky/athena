/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRL_UTILITIES_H
#define MUGIRL_UTILITIES_H

#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "GaudiKernel/MsgStream.h"

#include "MuGirl/MuGirlNS.h"
#include <iostream>

inline MsgStream& operator<<(MsgStream& log, const HepGeom::Point3D<double>& pt)
{
//    return log << "eta=" << pt.eta() << ",phi=" << pt.phi() << ",perp=" << pt.perp() << ",z=" << pt.z();
    char buf[200];
    sprintf(buf, "eta=%7.4f,phi=%7.4f,perp=%9.2f,z=%9.2f", pt.eta(), pt.phi(), pt.perp(), pt.z());
    return log << buf;
}

inline std::ostream& operator<<(std::ostream& os, const HepGeom::Point3D<double>& pt)
{
//    return os << "eta=" << pt.eta() << ",phi=" << pt.phi() << ",perp=" << pt.perp() << ",z=" << pt.z();
    char buf[200];
    sprintf(buf, "eta=%7.4f,phi=%7.4f,perp=%9.2f,z=%9.2f", pt.eta(), pt.phi(), pt.perp(), pt.z());
    return os << buf;
}

inline MsgStream& operator<<(MsgStream& log, const HepGeom::Vector3D<double>& dir)
{
//    return log << "eta=" << dir.eta() << ",phi=" << dir.phi() << ",perp=" << dir.perp() << ",z=" << dir.z();
    char buf[200];
    sprintf(buf, "eta=%7.4f,phi=%7.4f,perp=%9.2f,z=%9.2f", dir.eta(), dir.phi(), dir.perp(), dir.z());
    return log << buf;
}

inline std::ostream& operator<<(std::ostream& os, const HepGeom::Vector3D<double>& dir)
{
//    return os << "eta=" << dir.eta() << ",phi=" << dir.phi() << ",perp=" << dir.perp() << ",z=" << dir.z();
    char buf[200];
    sprintf(buf, "eta=%7.4f,phi=%7.4f,perp=%9.2f,z=%9.2f", dir.eta(), dir.phi(), dir.perp(), dir.z());
    return os << buf;
}

inline MsgStream& operator<<(MsgStream& log, const Trk::TrackSurfaceIntersection* pIsect)
{
    return log << "pos: " << pIsect->position() << " dir: " << pIsect->direction();
}

inline std::ostream& operator<<(std::ostream& os, const Trk::TrackSurfaceIntersection* pIsect)
{
    return os << "pos: " << pIsect->position() << " dir: " << pIsect->direction();
}

namespace MuGirlNS
{

    inline double canonicPhi(double phi)
    {
        while (phi >=  M_PI) phi -= M_TWOPI;
        while (phi <  -M_PI) phi += M_TWOPI;
        return phi;
    }

    inline void correctEtaforPhi(HepGeom::Point3D<double>& pt, double phi)
    {
        double deltaPhi = canonicPhi(pt.phi() - phi);
        pt.setPerp(pt.perp() / cos(deltaPhi));
        pt.setPhi(phi);
    }

}

#endif //MUGIRL_UTILITIES_H
