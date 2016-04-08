/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArTBEvent/LArTBInstrument.h"



LArTBInstrument::LArTBInstrument(int ADCscint, int ADCpion, int ADCcerenkov, 
				 short muon, short cedar, short hole, 
				 short beam, short phys, short spill, 
				 short burst,
				 long int clock, 
				 long int pretrig, long int w1w2)
{
  m_ADCscint    = ADCscint;
  m_ADCpion     = ADCpion;
  m_ADCcerenkov = ADCcerenkov;
  m_muon  = muon;
  m_CEDAR = cedar;
  m_hole  = hole;
  m_beam  = beam;
  m_phys  = phys;
  m_spill = spill;
  m_burst = burst;
  m_vetowall     = -1;
  m_late_pileup  = -1;
  m_early_pileup = -1;
  m_random = -1;
  m_random_hard = -1;
  m_daq_e   = -1;
  m_daq_epi = -1;
  m_daq_mu  = -1;
  m_clock   = clock;
  m_pretrig = pretrig;
  m_w1w2    = w1w2;
}

LArTBInstrument::LArTBInstrument(int ADCscint, int ADCpion, int ADCcerenkov, 
                                 short muon, short cedar, short hole, short beam, 
                                 short phys, short spill, short burst, 
                                 short vetowall, short late_pileup, 
                                 short early_pileup, long int clock, 
                                 long int pretrig, long int w1w2)
  : m_random(0),
    m_random_hard(0),
    m_daq_e(0),
    m_daq_epi(0),
    m_daq_mu(0)
{
  m_ADCscint    = ADCscint;
  m_ADCpion     = ADCpion;
  m_ADCcerenkov = ADCcerenkov;
  m_muon  = muon;
  m_CEDAR = cedar;
  m_hole  = hole;
  m_beam  = beam;
  m_phys  = phys;
  m_spill = spill;
  m_burst = burst;
  m_vetowall     = vetowall;
  m_late_pileup  = late_pileup;
  m_early_pileup = early_pileup;
  m_clock   = clock;
  m_pretrig = pretrig;
  m_w1w2    = w1w2;
}

LArTBInstrument::LArTBInstrument(short muon, short cedar, short hole, 
				 short beam, short phys, short spill, 
				 short burst, short vetowall, 
				 short late_pileup, 
				 short early_pileup, short random,
				 short random_hard,
				 short daq_e, short daq_epi, short daq_mu,
				 long int clock, 
				 long int pretrig, long int w1w2)
{
  m_ADCscint    = -1;
  m_ADCpion     = -1;
  m_ADCcerenkov = -1;
  m_muon  = muon;
  m_CEDAR = cedar;
  m_hole  = hole;
  m_beam  = beam;
  m_phys  = phys;
  m_spill = spill;
  m_burst = burst;
  m_vetowall     = vetowall;
  m_late_pileup  = late_pileup;
  m_early_pileup = early_pileup;
  m_random = random;
  m_random_hard = random_hard;
  m_daq_e = daq_e;
  m_daq_epi = daq_epi;
  m_daq_mu  = daq_mu;
  m_clock   = clock;
  m_pretrig = pretrig;
  m_w1w2    = w1w2;
}


LArTBInstrument::~LArTBInstrument() { }









