/* Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
   Author: Andrii Verbytskyi andrii.verbytskyi@mpp.mpg.de
*/
#ifndef ATLASHEPMC_FLOW_H
#define ATLASHEPMC_FLOW_H
#include "HepMC/Flow.h"
namespace HepMC
{
template <class T> int flow(T a,int i){return a->flow(i);}
template <class T> Flow flow(T a){return a->flow();}
}
#endif
