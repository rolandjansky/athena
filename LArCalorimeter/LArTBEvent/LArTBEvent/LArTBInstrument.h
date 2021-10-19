/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTBINSTRUMENT_H
#define LARTBINSTRUMENT_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

#include<vector>

/** @class LArTBInstrument LArTBInstrument.h "LArTBEvent/LArTBInstrument.h"
  
    For definition for the trigger flag from SPS
    @author A. Abdesselam and P. Ghez
*/
class LArTBInstrument {
 public:

  /** Constructor for EMB TB */
  LArTBInstrument(int ADCscint, int ADCpion, int ADCcerenkov, 
		  short muon, short cedar, short hole, 
		  short beam, short phys, short spill, short burst,
 		  long int clock, long int pretrig, long int w1w2);
  
  /** Second Constructor for EMB TB */
  LArTBInstrument(int ADCscint, int ADCpion, int ADCcerenkov, short muon, 
                  short cedar, short hole, short beam, short phys, short spill, 
                  short burst, short vetowall, short late_pileup, 
		  short early_pileup,
                  long int clock, long int pretrig, long int w1w2);

  /** Constructor for HEC+EMC TB */
  LArTBInstrument(short muon, short cedar, short hole, short beam, short phys, 
		  short spill, short burst, short vetowall, short late_pileup,
		  short early_pileup, short random, short random_hard,
		  short daq_e, short daq_epi, short daq_mu,
 		  long int clock, long int pretrig, long int w1w2);
  
  virtual ~LArTBInstrument();
  
  /* ------------------------------------------------------------  */
  /* ------------------------ G E T T E R ------------------------ */
  /* ------------------------------------------------------------  */  
  /** Get adc counts for trigger scintilator */
  inline int getADCscint() const { return m_ADCscint; }

  /** Get adc counts for pion counter */
  inline int getADCpion() const { return m_ADCpion; }

  /** Get adc counts for cerenkov counter */
  inline int getADCcerenkov() const { return m_ADCcerenkov; }

  /** Get flag for muon counter */
  inline short getMuon() const { return m_muon; }

  /** Get flag for CEDAR cerenkov counter */
  inline short getCEDAR() const { return m_CEDAR; }

  /** Get flag for Hole counter.
   *  \if 1 either side of hole counters(upper and lower) has signal */
  inline short getHole() const { return m_hole; }

  /** Get flag for Beam signal (from SPS) */
  inline short getBeam() const { return m_beam; }

  /** Get flag for Physics trigger (from SPS) @see LArTBInstrument */  
  inline short getPhys() const { return m_phys; }

  /** Get flag for Spill signal (from SPS) @see LArTBInstrument */  
  inline short getSpill() const { return m_spill; }
 
  /** Get flag from Burst signal (from SPS) @see LArTBInstrument */ 
  inline short getBurst() const { return m_burst; }

  /** Get flag for the vetowall counter
   *  /if 1 any vetowall counter is on */
  inline short getVetowall() const { return m_vetowall; }

  /** Get flag for late pileup */
  inline short getLate_pileup() const { return m_late_pileup; }
  
  /** Get flag for early pileup */
  inline short getEarly_pileup() const { return m_early_pileup; }

  /** Get flag for software random signal */
  inline short getRandom() const { return m_random; }

  /** Get flag for hardware random clock */
  inline short getRandom_hard() const { return m_random; }

  /** Get DAQ trigger flag for electron(low energy) */
  inline short getDAQe() const { return m_daq_e; }

  /** Get DAQ trigger flag for electron/pion */
  inline short getDAQepi() const { return m_daq_epi; }

  /** Get DAQ trigger flag for muon */
  inline short getDAQmu() const { return m_daq_mu; }
  
  /** Get scaler data of 1MHz clock */
  inline long int getClock() const { return m_clock; }

  /** Get scaler data of pretrigger */
  inline long int getPretrig() const { return m_pretrig; }

  /** Get scaler data of conincidence of W1 and W2 counters */
  inline long int getW1W2() const { return m_w1w2; }

  /** Get a flag of pattern unit for a given pattern number and bit(<16) */
  inline bool getPatternUnit(int ipu, int i) 
    const { return m_pattern_unit[ipu][i]; }

  /* ------------------------------------------------------------  */
  /* ------------------------ S E T T E R ------------------------ */
  /* ------------------------------------------------------------  */  

  /** Set Pattern unit for a given pattern number (16bits)*/
  void setPattern_unit(int ipu, const bool * pu) {
    for(int i=0; i<16; i++) m_pattern_unit[ipu][i] = *(pu++);
  }
  
 private:  
  
  LArTBInstrument();
  
  int m_ADCscint;        /**<  scintillator */
  int m_ADCpion;         /**<  pion signal amplitude */
  int m_ADCcerenkov;     /**<  cerenkov signal amplitude */
  short m_muon;          /**<  =1 / 0 */
  short m_CEDAR;         /**<  Signal in CEDAR counter (1/0) */
  short m_hole;          /**<  =1 / 0 */
  short m_beam;          /**<  =1 / 0 */
  short m_phys;          /**<  =1 / 0 */
  short m_spill;         /**<  =1 / 0 */
  short m_burst;         /**<  =1 / 0 */
  short m_vetowall;      /**<  veto walls before the calorimeter (1/0) */
  
  /** Veto signal of 4th PU in 6us gate after pretrigger (1/0) */
  short m_late_pileup;   

  /** Veto signal of W1*W2 in 5us gate before pretrigger (1/0)*/
  short m_early_pileup;

  short m_random;             /**< random clock trigger(software) */
  short m_random_hard;        /**< random clock trigger(hardware) */
  short m_daq_e;              /**< DAQ flag for electron(low energy) */
  short m_daq_epi;            /**< DAQ flag for electron/pion */
  short m_daq_mu;             /**< DAQ flag for muon */
  long int m_clock;           /**< Rate of 1MHz clock */
  long int m_pretrig;         /**< Signal Rate of pretrigger counters */
  long int m_w1w2;            /**< Signal Rate of W1*W2 counters */
  bool m_pattern_unit[4][16]{}; /**< Pattern units */
}; 
CLASS_DEF(LArTBInstrument,2785,0)
#endif //LARTBINSTRUMENT_H

