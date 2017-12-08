/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/MM_RawData.h"
#include "GaudiKernel/MsgStream.h"

MsgStream& operator << ( MsgStream& sl, const MM_RawData& data) 
{
    sl << "MM_RawData ("<< &data <<") "
    << ", Strip ID=" << data.identify();
    return sl;
}


std::ostream& operator << ( std::ostream& sl, const MM_RawData& data) 
{
    sl << "MM_RawData "
    << ", Strip ID=" << data.identify();
    return sl;
}
