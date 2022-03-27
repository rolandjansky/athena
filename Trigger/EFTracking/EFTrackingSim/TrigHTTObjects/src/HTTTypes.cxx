// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#include "TrigHTTObjects/HTTTypes.h"



std::ostream& operator<<(std::ostream& os, SiliconTech t)
{
    switch (t)
    {
    case SiliconTech::strip: return (os << "strip");
    case SiliconTech::pixel: return (os << "pixel");
    default: return (os << "undefined");
    }
}

std::ostream& operator<<(std::ostream& os, DetectorZone t)
{
    switch (t)
    {
    case DetectorZone::barrel: return (os << "barrel");
    case DetectorZone::posEndcap: return (os << "posEndcap");
    case DetectorZone::negEndcap: return (os << "negEndcap");
    default: return (os << "undefined");
    }
}