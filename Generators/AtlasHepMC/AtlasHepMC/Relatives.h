/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/* Author: Andrii Verbytskyi andrii.verbytskyi@mpp.mpg.de */

#ifndef ATLASHEPMC_RELATIVES_H
#define ATLASHEPMC_RELATIVES_H
#ifdef HEPMC3
#include "HepMC3/Relatives.h"
namespace HepMC {
typedef HepMC3::Relatives Relatives;
using HepMC3::children_particles;
using HepMC3::children_vertices;
using HepMC3::grandchildren_particles;
using HepMC3::grandchildren_vertices;
using HepMC3::parent_particles;
using HepMC3::parent_vertices;
using HepMC3::grandparent_particles;
using HepMC3::grandparent_vertices;
using HepMC3::descendant_particles;
using HepMC3::descendant_vertices;
using HepMC3::ancestor_particles;
using HepMC3::ancestor_vertices;
}
#endif
#endif