#ifndef ATLASHEPMC_POLARIZATION_H
#define ATLASHEPMC_POLARIZATION_H
#include "HepMC/Polarization.h"
namespace HepMC
{
void template<class T> set_polarization( T a,  Polarization b) {
    a->set_polarization(b);
}
template<class T> Polarization polarization(T a) {
    return a->polarization();
}
}
#endif
