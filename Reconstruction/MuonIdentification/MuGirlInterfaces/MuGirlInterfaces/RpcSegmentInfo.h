/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLNSRPCSEGMENTINFO_H
#define MUGIRLNSRPCSEGMENTINFO_H
#include "Identifier/Identifier.h"

namespace Trk
{
    class RIO_OnTrack;
}

namespace MuGirlNS
{
    typedef std::vector<const Trk::RIO_OnTrack*> RIO_OnTrackList;
    typedef std::vector<RIO_OnTrackList*> RIO_OnTrackLists;
 
    struct StauRpcHit
    {
        Identifier id;
        double measuredTime;
	double propagationTime;
	double muonToF;
	double distance;
        double x;
        double y;
        double z;
        double error;
        bool isEta;
    };
    typedef std::vector< StauRpcHit* > StauRpcHitsList;
    typedef std::vector< StauRpcHitsList* > StauRpcHitsLists;

}

#endif // MUGIRLNSRPCSEGMENTINFO_H
