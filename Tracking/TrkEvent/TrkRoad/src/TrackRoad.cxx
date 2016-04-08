/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkRoad/TrackRoad.h"
#include "GaudiKernel/MsgStream.h"
#include <sstream>

namespace Trk
{

    std::string TrackRoad::toString() const
    {
        std::ostringstream oss;
        oss << "pos"
        //<< " eta=" << m_globalPos.eta()
        << " phi=" << m_globalPos.phi()
        << " perp=" << m_globalPos.perp()
        << " z=" << m_globalPos.z()
        << " dir"
        //<< " eta=" << m_globalDir.eta()
        << " phi=" << m_globalDir.phi()
        << " z=" << m_globalDir.z()
        << " deta=" << m_dDeltaEta
        << " dphi=" << m_dDeltaPhi
        << " type=" << m_type
        << " weight=" << m_weight;
        return oss.str();
    }

    MsgStream& TrackRoad::dump(MsgStream& stream) const
    {
        stream << MSG::DEBUG << "TrackRoad " << toString() << endreq;
        return stream;
    }

    std::ostream& TrackRoad::dump(std::ostream& stream) const
    {
        stream << MSG::DEBUG << "TrackRoad " << toString() << std::endl;
        return stream;
    }

    MsgStream& operator <<(MsgStream& stream, const TrackRoad& tr)
    {
        return tr.dump(stream);
    }

    std::ostream& operator <<(std::ostream& stream, const TrackRoad& tr)
    {
        return tr.dump(stream);
    }

}
