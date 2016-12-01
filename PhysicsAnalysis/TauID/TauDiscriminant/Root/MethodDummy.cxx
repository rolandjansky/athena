/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * file: MethodDummy.cxx
 *
 * Author: Noel Dawe (end1@sfu.ca)
 */

#include "TauDiscriminant/MethodDummy.h"

using namespace TauID;
using namespace std;

float MethodDummy::response(xAOD::TauJet& /*tau*/)
{    
    return 0.;
}

bool MethodDummy::build(const string&, bool)
{    
    return true;
}
