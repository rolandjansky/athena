/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _ATHALGORITHM_MCTESTER_H_
#define _ATHALGORITHM_MCTESTER_H_

#include "GaudiKernel/Algorithm.h"

#include "AthenaBaseComps/AthAlgorithm.h"

//external MC-TESTER headers
#ifdef HEPMC3
#include "HepMC3Event.h"
using HepMCEvent=HepMC3Event;
using HepMCParticle=HepMC3Particle;
#else
#include "HepMCEvent.H"
#endif
#include "Generate.h"
#include "Setup.H"

/**
  @class MCTesterAlg
  This Algorithm provides an easy interface to the MC-Tester tool

  @author This algorithm: Nadia Davidson, nadia.davidson@cern.ch
  @author MC-Tester tool: P. Golonka, T. Pierzchala, Z. Was, N. Davidson
*/
class MCTesterAlg: public AthAlgorithm {
public:
   /** Constructor */
   MCTesterAlg (const std::string& name, ISvcLocator* pSvcLocator);
 
   /** Initialization of the MC-Tester tool including setting up the configurable
       quantities set in the jobOption file */
   StatusCode initialize();


   /** Each event in the McEventCollection is passed to MC-Tester and analyzed.
       Some extra checks are performed for 4-momentum conservation, decaying particle
       lifetime and that it has actually decayed*/
   StatusCode execute();


   /** Finalization of the MC-Tester tool. The tool outputs a root file with
       the histograms and branching modes obtained from the analysis. Some
       summary information is given to stdout */
   StatusCode finalize();

  private:
   /** Extra check for decay and particle lifetime. A WARNING is printed if there
    in a failure of the check.*/
   StatusCode MC_Validate_checkDecay(HepMCEvent * e);

   /** Extra check for 4 momentum conservation. A WARNING is printed if there
       in a failure of the check.*/
   StatusCode MC_Validate_check4MomentumSum(HepMCEvent * e);

   // Some storegate variables
   std::string m_key; 
   std::string m_infokey;

   //variables used to configure MC-TESTER
   /** PDG ID of particle to study */
   int m_decay_particle;

   /** Path of root output */
   std::string m_result_path;

   /** Number of bins for the invariant mass histograms. */
   int m_nbins;

   /** Maximum (MeV) for the invariant mass histograms. */
   double m_bin_min;

   /** Minimum (MeV) for the invariant mass histograms. */
   double m_bin_max;

   /** Count vertices such as particle -> particle + gamma? */
   bool m_include_decays_to_self;

   /** List of particle PDG IDs which should be considered stable by MC-TESTER.
    eg. List pi0s as pi0 and not gamma gamma (etc.) */
   std::string m_suppress_decay;

   /** Plot powers of the invariant mass. Default value is 1 and range allowed is
       1-9. Plotting the square of the invariant mass (ie value=2) is
       useful for testing tau spin polarisation. */
   int m_mass_power;

   /** Scale the X-axis to the mass of the particle under study. Ie.
       histograms will have an X-axis range of 0-1. */
   bool m_mass_scale_on;

   /** First description line for front page of MC-TESTER output booklet */
   std::string m_description_l1;

   /** Second description line for front page of MC-TESTER output booklet */
   std::string m_description_l2;

   /** Third and final description line for front page of MC-TESTER output booklet */
   std::string m_description_l3;

   //variables used for extra validation checks
   //setable
   /** px component of collision 4 momentum (for conservation check) */
   double m_collision_px;

   /** py component of collision 4 momentum (for conservation check) */
   double m_collision_py;

   /** pz component of collision 4 momentum (for conservation check) */
   double m_collision_pz;

   /** E component of collision 4 momentum (for conservation check) */
   double m_collision_E;

   /** margin allowed for rounding in 4 momentum conservation check (MeV) */
   double m_collision_check_sum_margin;

   //others

   /** total number of events */
   int m_events_total;

   /** number of events with none of the chosen decaying particle present */
   int m_events_without_particle;

   /** number of chosen decaying particles found */
   int m_particles_of_interest;

   /** number of chosen decaying particles found not to decay */
   int m_non_decaying_particle;

   /** number of events where 4 momentum is not conserved */
   int m_deviation_from_centre_of_mass;

   /** name of particle to study */
   char * m_decay_name;

   /** number of chosen decaying particles with zero lifetime */
   int m_zero_lifetime_particles;
};

#endif

