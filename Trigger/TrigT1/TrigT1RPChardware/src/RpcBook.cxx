/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPChardware/RpcBook.h"

rpcbook::rpcbook() {}
  
void 
rpcbook::book(const int /*id*/,  const char */*title*/, 
              const int /*nch*/, const float /*xl*/, const float /*xh*/) {}
void 
rpcbook::fill(const int /*id*/, const float /*x*/) {}
  
void 
rpcbook::fill(const int /*id*/, const float /*x*/, const float /*w*/) {} 
  
void 
rpcbook::write() {}
  
void 
rpcbook::giveName(const int /*id*/, char */*str*/, const int /*sizeofstr*/) {} 
