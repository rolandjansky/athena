//  -*- c++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
#include "StoreGate/StoreGateSvc.h"


/**
 * It used to be useful piece of code for replacing actual SG with other store of similar functionality 
 * In fact it used to be more like interface adapter for ARA objects access, nevertheless since the ARA project is gone it is only an unnecessary extra layer.
 * The polimorphic dispatch only created unnecessary overhead. 
 */
namespace HLT {
  typedef StoreGateSvc AccessProxy;
}
