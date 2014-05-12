/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerIndexCalculator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkDetDescrUtils/LayerIndex.h"

bool Trk::operator< ( const Trk::LayerIndex& one, const Trk::LayerIndex& two )
  { return (one.value() < two.value());  }

bool Trk::operator<=( const Trk::LayerIndex& one, const Trk::LayerIndex& two )
  { return (one.value() <= two.value()); }

bool Trk::operator> ( const Trk::LayerIndex& one, const Trk::LayerIndex& two )
  { return (one.value() > two.value());  }  

bool Trk::operator>=( const Trk::LayerIndex& one, const Trk::LayerIndex& two )
  { return (one.value() >= two.value()); }

MsgStream& Trk::operator << ( MsgStream& sl, const Trk::LayerIndex& layx)
{
    sl << layx.value();
    return sl;
}

std::ostream& Trk::operator << ( std::ostream& sl, const Trk::LayerIndex& layx)
{ 
    sl << layx.value();
    return sl;
}


