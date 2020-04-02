#ifndef ATLASHEPMC_FLOW_H
#define ATLASHEPMC_FLOW_H
#include "HepMC/Flow.h"
namespace HepMC
{
template <class T> int flow(T a,int i=0){return a->flow(i);}
}
#endif
