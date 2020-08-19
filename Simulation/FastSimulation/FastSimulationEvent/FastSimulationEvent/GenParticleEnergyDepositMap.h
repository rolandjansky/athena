/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenParticleEnergyDepositMap.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef FASTSIMULATIONEVENT_GENPARTICLEENERGYDEPOSIT_H
#define FASTSIMULATIONEVENT_GENPARTICLEENERGYDEPOSIT_H

// Trk
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "GeoPrimitives/GeoPrimitives.h"
//
#include "AthenaKernel/CLASS_DEF.h"
// STL
#include <map>
#include <utility>

#include "AtlasHepMC/GenParticle_fwd.h"

namespace FSim {

  struct EnergyDeposit {
       
       int                          sample;
       Trk::MaterialEffectsOnTrack* materialEffects;
       Amg::Vector3D                position;

       EnergyDeposit() :
        sample(0),
        materialEffects(0),
        position(0.,0.,0.)
       {}

       EnergyDeposit(const EnergyDeposit& ed) :
        sample(ed.sample),
        materialEffects(ed.materialEffects ? ed.materialEffects->clone() : 0),
        position(ed.position)
       {}
                  
       EnergyDeposit(int sam, Trk::MaterialEffectsOnTrack* matE, const Amg::Vector3D& pos) :
        sample(sam),
        materialEffects(matE),
        position(pos)
       {}

       EnergyDeposit& operator=(const EnergyDeposit& rhs) {
         if (&rhs==this) return *this;
         sample = rhs.sample;
         materialEffects = rhs.materialEffects ? rhs.materialEffects->clone() : 0;
         position = rhs.position;
         return *this;
       }
       
       ~EnergyDeposit()
       { delete materialEffects; }
       
 };

}
 
typedef std::multimap<const HepMC::GenParticlePtr , FSim::EnergyDeposit> GenParticleEnergyDepositMap;
typedef std::multimap<int, FSim::EnergyDeposit>                       BarcodeEnergyDepositMap;

CLASS_DEF( GenParticleEnergyDepositMap, 97004131, 1)
CLASS_DEF( BarcodeEnergyDepositMap , 229446511 , 1 )

#endif
