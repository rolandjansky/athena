///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Categories.h 
// Enum of categories of particles (based on their PDG id)
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ANALYSISUTILS_CATEGORIES_H 
#define ANALYSISUTILS_CATEGORIES_H 

/** This is an enum to encode various categories of particles.
 *  It is used by the CategoryIdFilter class.
 *  You can combine different categories to your analysis filtering needs.
 */

namespace PDGCat {

  enum category {
    Untagged = 0,
    Lepton,
    Hadron,
    Meson,
    Baryon,
    DiQuark,
    BMeson,
    BBaryon,
    DFlavour,     //> contain a quark down
    UFlavour,     //> contain a quark up
    SFlavour,     //> contain a quark strange
    CFlavour,     //> contain a quark charm
    BFlavour,     //> contain a quark bottom
    TFlavour,     //> contain a quark top
    LightFlavour, // u,d,s,c
    HeavyFlavour, // b,t
    Boson         // gauge + Higgs bosons
  }; 

} //> end namespace PDGCat

#endif //> ANALYSISUTILS_CATEGORIES_H
