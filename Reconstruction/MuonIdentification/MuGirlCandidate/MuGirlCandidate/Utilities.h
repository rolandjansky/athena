/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRL_UTILITIES_H
#define MUGIRL_UTILITIES_H

#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "GaudiKernel/MsgStream.h"

#include "MuGirlCandidate/MuGirlNS.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include <iostream>

inline MsgStream& operator<<(MsgStream& log, const Amg::Vector3D& pt)
{
    //    return log << "eta=" << pt.eta() << ",phi=" << pt.phi() << ",perp=" << pt.perp() << ",z=" << pt.z();
    char buf[200];
    sprintf(buf, "eta=%7.4f,phi=%7.4f,perp=%9.3f,z=%9.3f", pt.eta(), pt.phi(), pt.perp(), pt.z());
    return log << buf;
}

inline std::ostream& operator<<(std::ostream& os, const Amg::Vector3D& pt)
{
    //    return os << "eta=" << pt.eta() << ",phi=" << pt.phi() << ",perp=" << pt.perp() << ",z=" << pt.z();
    char buf[200];
    sprintf(buf, "eta=%7.4f,phi=%7.4f,perp=%9.3f,z=%9.3f", pt.eta(), pt.phi(), pt.perp(), pt.z());
    return os << buf;
}
/*
inline MsgStream& operator<<(MsgStream& log, const Amg::Vector3D& dir)
{
    //    return log << "eta=" << dir.eta() << ",phi=" << dir.phi() << ",perp=" << dir.perp() << ",z=" << dir.z();
    char buf[200];
    sprintf(buf, "eta=%7.4f,phi=%7.4f,perp=%9.3f,z=%9.3f", dir.eta(), dir.phi(), dir.perp(), dir.z());
    return log << buf;
}

inline std::ostream& operator<<(std::ostream& os, const Amg::Vector3D& dir)
{
    //    return os << "eta=" << dir.eta() << ",phi=" << dir.phi() << ",perp=" << dir.perp() << ",z=" << dir.z();
    char buf[200];
    sprintf(buf, "eta=%7.4f,phi=%7.4f,perp=%9.3f,z=%9.3f", dir.eta(), dir.phi(), dir.perp(), dir.z());
    return os << buf;
}
*/
inline MsgStream& operator<<(MsgStream& log, const Trk::TrackSurfaceIntersection* pIsect)
{
    return log << "pos: " << pIsect->position() << " dir: " << pIsect->direction() << " sn=" << pIsect->serialNumber();
}

inline std::ostream& operator<<(std::ostream& os, const Trk::TrackSurfaceIntersection* pIsect)
{
    return os << "pos: " << pIsect->position() << " dir: " << pIsect->direction() << " sn=" << pIsect->serialNumber();
}

namespace Trk
{
class TrkDetElementBase;
class PrepRawData;
}
namespace MuGirlNS
{
class BBox;

inline double canonicPhi(double phi)
{
    while (phi >=  M_PI) phi -= M_TWOPI;
    while (phi <  -M_PI) phi += M_TWOPI;
    return phi;
}

inline void correctEtaforPhi(Amg::Vector3D& pt, double phi)
{
    double deltaPhi = canonicPhi(pt.phi() - phi);
    Amg::setPerp(pt, pt.perp()/cos(deltaPhi) );
    Amg::setPhi (pt, phi);
}

void DetElement2Bbox(const Trk::TrkDetElementBase* pElement, BBox& bbox);
void PrepData2Bbox(const Trk::PrepRawData* pPrepData, BBox& bbox);

inline size_t count_clusters(const Clusters& clusters)
{
    size_t c = 0;
    for (Clusters::const_iterator it = clusters.begin(); it != clusters.end(); ++it)
        c += it->size();
    return c;
}

} // namespace MuGirlNS

#endif //MUGIRL_UTILITIES_H
