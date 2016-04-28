/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* author : Johann Collot                                                           */
/* date of creation : 07/01/2001                                                    */
/* date of last modification : 13/09/2002  CLID removed                             */

// 03-Dec-2003 Bill Seligman: Substantially revised.  This class no
// longer inherits from any base class.  

// 09-Dec-2006 RD Schaffer: moved energy to double for accumulation
// precision - float in persistent version

#ifndef LArSimEvent_LArHit_h
#define LArSimEvent_LArHit_h

#include "Identifier/Identifier.h"
#include "AthenaKernel/Units.h"

class LArHit 

/** @brief  Class to store hit energy and time in LAr cell from G4 simulation */

{
public:

    /** LArHit Constructor. 
      @param[in] l_cell   Identifier of the cell in which the hit occured.
      @param[in] l_energy Energy (in MeV) deposited in the cell.
      @param[in] l_time   Time   (in ns) of the hit. <br>
     energy and time should be double in transient memory to have proper rounding accuracy in accumulating sum(E) and sum(E.t) from G4 steps
     */
    LArHit(Identifier l_cell, double l_energy, double l_time);

    /** Default constructor for persistency.
       Should never be used
    */
    LArHit();

    /** Destructor. */
    ~LArHit();
  
    /** @return  cell identifier of this hit */
    Identifier cellID() const;
  
    /** @return  energy in MeV of this hit */
    double     energy() const;
  
    /** @return time in ns of this hit <br>
      Time is defined as the g4 time minus the time of flight at speed of light from the center of Atlas
    */
    double     time() const;

    /** Operator Less than this for hit ordering */
    bool       Less(LArHit* const& h) const;

    /** Operator Equals this */
    bool       Equals(LArHit* const& h) const;

    /** Add another hit ot this hit
     @param[in] h  hit to add */
    void       Add(LArHit* const& h);

    /** The method to be called at the end of event by SD.
         Finalize time computation time = Sum(E.t)/Sum(E)  */
    void       finalize();
    

private:
  
    /** identifier of the cell in which this hit occured. */
    Identifier m_ID;
  
    /** energy (in MeV) deposited in this hit */
    double m_energy;
  
    /** time (in ns) */
    double m_time;

};


/// inlines
inline
LArHit::LArHit(Identifier l_cell, double l_energy, double l_time) 
	:
	m_ID(l_cell), 
	m_energy(l_energy),
        m_time( (l_time*l_energy) )
{}


inline
LArHit::LArHit() 
	:
	m_ID(Identifier()),
	m_energy(0.),
        m_time(0.)
{}

inline
LArHit::~LArHit() 
{ }

inline
Identifier
LArHit::cellID() const 
{ return m_ID; }

inline
double
LArHit::energy() const 
{ return (double) m_energy; }
  
inline
double
LArHit::time() const
{ return m_time; }

inline 
bool
LArHit::Less(LArHit* const& h) const 
{ return m_ID < h->m_ID; } 

inline
bool
LArHit::Equals(LArHit* const& h) const 
{ return m_ID == h->m_ID; }

inline
void
LArHit::Add(LArHit* const& h)
{
    m_energy += h->m_energy;
    m_time   += h->m_time;
}

// The method to be called at the end of event by SD
// For backwards compatibility
inline
void
LArHit::finalize()
{
  m_time = m_energy==0 ? 0. : (double)(m_time/m_energy)/Athena::Units::ns;
  m_energy = m_energy/Athena::Units::MeV;
}

#endif  // LArSimEvent_LArHit_h

