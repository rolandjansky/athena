/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "./JetStreamer.h"


std::ostream& operator << (std::ostream& os, const JetStreamer& js){
  os << "JetStreamer\n" <<  js.m_stream->dump() << '\n';
  return os;
}

