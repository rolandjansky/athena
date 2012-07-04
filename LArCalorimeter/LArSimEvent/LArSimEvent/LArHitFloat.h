/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// 12-march-2006 G.Unal:  alternative LArHit object with float energy and time
//  to be used in digitization /pileup to optimize memory usage

#ifndef LArSimEvent_LArHitFloat_h
#define LArSimEvent_LArHitFloat_h

#include "Identifier/Identifier.h"
#include "CLHEP/Units/SystemOfUnits.h"

class LArHitFloat

/** @brief  Class to store hit energy and time in LAr cell from G4 simulation */

{
public:

    /** LArHitFloat Constructor. 
      @param[in] l_cell   Identifier of the cell in which the hit occured.
      @param[in] l_energy Energy (in MeV) deposited in the cell.
      @param[in] l_time   Time   (in ns) of the hit. <br>
     energy and time should be double in transient memory to have proper rounding accuracy in accumulating sum(E) and sum(E.t) from G4 steps
     */
    LArHitFloat(Identifier l_cell, float l_energy, float l_time);

    /** Default constructor for persistency.
       Should never be used
    */
    LArHitFloat();

    /** Destructor. */
    ~LArHitFloat();
  
    /** @return  cell identifier of this hit */
    Identifier cellID() const;
  
    /** @return  energy in MeV of this hit */
    float     energy() const;
  
    /** @return time in ns of this hit <br>
      Time is defined as the g4 time minus the time of flight at speed of light from the center of Atlas
    */
    float     time() const;

private:
  
    /** identifier of the cell in which this hit occured. */
    Identifier m_ID;
  
    /** energy (in MeV) deposited in this hit */
    float m_energy;
  
    /** time (in ns) */
    float m_time;

};


/// inlines
inline
LArHitFloat::LArHitFloat(Identifier l_cell, float l_energy, float l_time) 
	:
	m_ID(l_cell), 
	m_energy(l_energy),
        m_time(l_time )
{}


inline
LArHitFloat::LArHitFloat() 
	:
	m_ID(Identifier()),
	m_energy(0.),
        m_time(0.)
{}

inline
LArHitFloat::~LArHitFloat() 
{ }

inline
Identifier
LArHitFloat::cellID() const 
{ return m_ID; }

inline
float
LArHitFloat::energy() const 
{ return m_energy; }
  
inline
float
LArHitFloat::time() const
{ return m_time; }

#endif  // LArSimEvent_LArHitFloat_h
