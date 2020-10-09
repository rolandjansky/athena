/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// CaloCalibrationHit
// 26-Jan-2004 William Seligman

// This class defines a "calibration" hit associated with the Calo
// simulation.

#ifndef CaloSimEvent_CaloCalibrationHit_h
#define CaloSimEvent_CaloCalibrationHit_h

#include "Identifier/Identifier.h"

class CaloCalibrationHit 
/** @brief Class to store calorimeter calibration hit. <br>
    A calibration hit stores for active, inactive and dead material elements the energy loss according to the
    process (EM energy loss, non EM energy loss, Invisible energy and Escaped energy) */

{
 public:

  /** @brief Standard constructor using identifer and energy by type 
      @param[in] id  Cell identifier (calorimeter cell or dead material identifier)
      @param[in] energyEM  energy loss by EM processes
      @param[in] energyNonEM  visible energy loss by non EM processes (pion dEdx for instance)
      @param[in] energyInvisible  invisible energy loss (usually nuclear binding energy)
      @param[in] energyEscaped   energy which escaped from this cell because of production of neutrino (or escaping muon energy)
  */
  CaloCalibrationHit(Identifier id, 
		     double energyEM, 
		     double energyNonEM, 
		     double energyInvisible, 
		     double energyEscaped):
    m_ID(id), 
    m_energy0(energyEM), 
    m_energy1(energyNonEM), 
    m_energy2(energyInvisible), 
    m_energy3(energyEscaped),
    m_particleID(0)
  {}

  /** @brief Standard constructor using identifer, energy by type and primary particle ID
    @param[in] id  Cell identifier (calorimeter cell or dead material identifier)
    @param[in] energyEM  energy loss by EM processes
    @param[in] energyNonEM  visible energy loss by non EM processes (pion dEdx for instance)
    @param[in] energyInvisible  invisible energy loss (usually nuclear binding energy)
    @param[in] energyEscaped   energy which escaped from this cell because of production of neutrino (or escaping muon energy)
    @param[in] particleID barcode of primary particle which caused given hit
  */
  CaloCalibrationHit(Identifier id, 
                       double energyEM, 
                       double energyNonEM, 
                       double energyInvisible, 
                       double energyEscaped, 
                       unsigned int particleID):
  m_ID(id), 
  m_energy0(energyEM), 
  m_energy1(energyNonEM), 
  m_energy2(energyInvisible), 
  m_energy3(energyEscaped),
  m_particleID(particleID)
                                               {}
  /** Default constructor; should never be used, but provided for some
      persistency services. */
  CaloCalibrationHit():
    m_ID(Identifier()),
    m_energy0(0.),
    m_energy1(0.),
    m_energy2(0.),
    m_energy3(0.),
    m_particleID(0)
  {}

  /** Copy constructor **/
  CaloCalibrationHit(const CaloCalibrationHit &cchSource)
    : m_ID (cchSource.m_ID),
      m_energy0 (cchSource.m_energy0),
      m_energy1 (cchSource.m_energy1),
      m_energy2 (cchSource.m_energy2),
      m_energy3 (cchSource.m_energy3),
      m_particleID (cchSource.m_particleID)
  {
  }

  /** Assignment operator **/
  CaloCalibrationHit& operator= (const CaloCalibrationHit &cchSource)
  {
    if (this == &cchSource) return *this;
    m_ID = cchSource.m_ID;
    m_energy0 = cchSource.m_energy0;
    m_energy1 = cchSource.m_energy1;
    m_energy2 = cchSource.m_energy2;
    m_energy3 = cchSource.m_energy3;
    m_particleID = cchSource.m_particleID;
    return *this;
  }

  /** Destructor */
  virtual ~CaloCalibrationHit() {}
  
  /** @return cell identifier of this hit */
  Identifier cellID()      const { return m_ID; }
  
  /** @return EM energy deposits.  Units are MeV. */
  double energyEM()        const { return  m_energy0; }

  /** @return NonEM energy deposits.  Units are MeV. */
  double energyNonEM()     const { return  m_energy1; }

  /** @return invisible energy.  Units are MeV. */
  double energyInvisible() const { return  m_energy2; }

  /** @return escaped energy.  Units are MeV. */
  double energyEscaped()   const { return  m_energy3; }

  /** @return total energy deposits.  Units are MeV. */
  double energyTotal()     const
  {
    return 
       m_energy0 + 
       m_energy1 + 
       m_energy2 + 
       m_energy3;
  }

  /** @return energy deposits by specifying input type.  Units are MeV. */
  double energy( unsigned int i ) const
  {
    switch (i)
      {
      case 0: return  m_energy0;
      case 1: return  m_energy1;
      case 2: return  m_energy2;
      case 3: return  m_energy3;
      default: return 0.;
      }
  }

  /** @return primary particle identifier which caused his hit */
  unsigned int particleID()      const { return m_particleID; }

  /** @return energy deposits by specifying input type, same as above method */
  double operator() (unsigned int i) const { return energy(i); }

  /** Calibration hits are ordered by values of their identifiers */
  bool Less(CaloCalibrationHit* const& h) const 
  {
    if(m_ID != h->m_ID){
      return m_ID < h->m_ID; 
    }else{
      return m_particleID < h->m_particleID; 
    }
  }

  /** Calibration hits are ordered by values of their identifiers */
  /** Calibration hits are ordered by values of their identifiers */
  bool Equals(const CaloCalibrationHit& h) const { 
    return (m_ID == h.m_ID) && (m_particleID == h.m_particleID); 
  };
  bool Equals(const CaloCalibrationHit* h) const {
    return Equals (*h);
  }

  /** Method used for energy accumulation */
  void Add(const CaloCalibrationHit* h)
  {
    m_energy0 += h->m_energy0;
    m_energy1 += h->m_energy1;
    m_energy2 += h->m_energy2;
    m_energy3 += h->m_energy3;
  }

private:

  /** identifier of the cell in which this hit occured. */
  Identifier m_ID;

  /** energies (in MeV) deposited in this hit.  In order, they represent:
   * EM energy deposited
   * non-EM energy deposited
   * "invisible" energy deposited
   * escaped energy
   * Energies are accumulated in double precision and stored as floats */
  double m_energy0;
  double m_energy1;
  double m_energy2;
  double m_energy3;

  /** identifier of Primary Particle which caused this hit */
  unsigned int m_particleID;
};

#endif  // CaloSimEvent_CaloCalibrationHit_h

