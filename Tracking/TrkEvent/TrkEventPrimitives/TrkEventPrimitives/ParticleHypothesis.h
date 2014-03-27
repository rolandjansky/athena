/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParticleHypothesis.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXUTILS_PARTICLEHYPOTHESIS_H
#define TRKEXUTILS_PARTICLEHYPOTHESIS_H

// STL
#include <vector>
// Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// define the particle hypotheses
#define PARTICLEHYPOTHESES 11

namespace Trk {

  /** @enum ParticleHypothesis
   Enumeration for Particle hypothesis respecting the
   interaction with material

   @author Andreas.Salzburger@cern.ch
   */
   enum ParticleHypothesis { nonInteracting  = 0,
                             geantino        = 0,
                             electron        = 1,
                             muon            = 2,                            
                             pion            = 3,                             
                             kaon            = 4,
                             proton          = 5,
                             photon          = 6,  // for Fatras usage
                             neutron         = 7,  // for Fatras usage 
                             pi0             = 8, // for Fatras usage 
                             k0              = 9, // for Fatras usage 
                             nonInteractingMuon = 10, // For material collection
                             noHypothesis    = 99,
                             undefined       = 99};
  
  /** @struct ParticleMasses
   Mass declaration of particles covered 
   in the ParticleHypothesis.
   Masses are given in MeV and taken from:

   Review of Particle Physics (2010)
   K. Nakamura et al. (Particle Data Group), J. Phys. G 37, 075021 (2010)

   @author Andreas.Salzburger@cern.ch
   */
    
   struct ParticleMasses {
      /** the vector of masses */
      std::vector<double> mass;   
   
      /**Default constructor*/
      ParticleMasses()
        {
         mass.reserve(PARTICLEHYPOTHESES);

         mass.push_back(0.*Gaudi::Units::MeV);         // non interacting mass
         mass.push_back(0.51099891*Gaudi::Units::MeV); // electron mass
         mass.push_back(105.658367*Gaudi::Units::MeV); // muon mass
         mass.push_back(139.57018*Gaudi::Units::MeV);  // charged pion mass
         mass.push_back(493.677*Gaudi::Units::MeV);    // kaon mass
         mass.push_back(938.272013*Gaudi::Units::MeV); // proton mass
         mass.push_back(0.*Gaudi::Units::MeV);         // photon rest mass
         mass.push_back(939.565346*Gaudi::Units::MeV); // neutron rest mass
         mass.push_back(134.9766*Gaudi::Units::MeV);   // pi0 rest mass
         mass.push_back(497.614*Gaudi::Units::MeV);    // K0 rest mass
         mass.push_back(105.658367*Gaudi::Units::MeV); // muon mass
    
         }
      
   };
   
  /** @struct ParticleSwitcher
   Simple struct to translate an integer in the ParticleHypothesis type
     
   @author Andreas.Salzburger@cern.ch
   */
    
   struct ParticleSwitcher {
      /** the vector of masses */
      std::vector<ParticleHypothesis> particle;   
   
      /**Default constructor*/
      ParticleSwitcher()
        {
         particle.reserve(PARTICLEHYPOTHESES);

         particle.push_back(Trk::nonInteracting); 
         particle.push_back(Trk::electron);
         particle.push_back(Trk::muon);           
         particle.push_back(Trk::pion);              
         particle.push_back(Trk::kaon);           
         particle.push_back(Trk::proton);         
         particle.push_back(Trk::photon);         
         particle.push_back(Trk::neutron);         
         particle.push_back(Trk::pi0);         
         particle.push_back(Trk::k0);         
         particle.push_back(Trk::nonInteractingMuon);           
        }
      
   };
    
   
}

#endif
