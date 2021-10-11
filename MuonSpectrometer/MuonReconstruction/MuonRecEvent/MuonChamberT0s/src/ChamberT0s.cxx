/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonChamberT0s/ChamberT0s.h"

namespace Muon {
    
ChamberT0s::ChamberT0s( ) 
{}

ChamberT0s::ChamberT0s( const std::vector< std::pair < Identifier, float > >& t0s ) 
    :
    m_t0s (t0s) 
{}

ChamberT0s::ChamberT0s( std::vector< std::pair < Identifier, float > >&& t0s ) 
    :
    m_t0s (std::move(t0s)) 
{}

ChamberT0s::~ChamberT0s( )
{} 



}
