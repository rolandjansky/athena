/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/Cluster.h"


MsgStream& operator<< (MsgStream& os, const TrkDriftCircleMath::Cluster& cl)
{
    os << cl.position() << " err " << cl.err();
    if (cl.id().measuresPhi()) os << " phi";
    else os << " eta";
    return os;
}

std::ostream& operator<<( std::ostream& os, const TrkDriftCircleMath::Cluster& cl )
{
    os << cl.position() << " err " << cl.err();
    if (cl.id().measuresPhi()) os << " phi";
    else os << " eta";
    return os;
}
