///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McUtils.h 
// Header file for various Mc utils
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEUTILS_MCUTILS_H 
#define MCPARTICLEUTILS_MCUTILS_H 

// STL includes

// HepMC includes

// Forward declaration
namespace HepPDT  { class ParticleDataTable; }

namespace McUtils {

  /** @brief Helper function to get the charge of a particle given its PDG Id.
   *         Also handles (some) particles which are not in the PDGTABLE.MeV file
   *         This function internally uses HepPDT.
   */
  double chargeFromPdgId( const int pdgId, 
			  const HepPDT::ParticleDataTable* pdt );

} //> end namespace McUtils

#endif //> MCPARTICLEUTILS_MCUTILS_H 
