/* Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
   Author: Andrii Verbytskyi andrii.verbytskyi@mpp.mpg.de
*/
#ifndef ATLASHEPMC_POLARIZATION_H
#define ATLASHEPMC_POLARIZATION_H
#include "HepMC/Polarization.h"
namespace HepMC
{
template<class T> void set_polarization( T a,  Polarization b) {
    a->set_polarization(b);
}
template<class T>  Polarization polarization(T a) {
    return a->polarization();
}
}
#endif
