/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CalibrationHit.h
// Definition of a calibration hit in the LArG4 simulation.
// 08-Jan-2004 Bill Seligman

// Note: A calibration hit has to record a few different types of
// energies.  As far as the code below is concerned, the
// LArG4::CalibrationHit does not know what these energies are.  That
// is, there are four energy fields, and maybe the first one is E-M
// energy, the second is hadronic energy, etc., but that's irrelevant
// to this code.  It's other routines that have to interpret the
// meaning of these fields.

#ifndef LArG4_CalibrationHit_H
#define LArG4_CalibrationHit_H

#include "LArG4Code/LArG4Identifier.h"

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

namespace LArG4 {

  class CalibrationHit : public G4VHit
  {
  public:

    // Constructors and destructors.  Note that since hits get created
    // and destroyed all the time, I've inlined everything that I can.

    CalibrationHit() :
      m_identifier(LArG4Identifier()) ,
      m_energy0(0.) ,
      m_energy1(0.) ,
      m_energy2(0.) ,
      m_energy3(0.)
    {}

    CalibrationHit(LArG4Identifier ident, 
		   G4double energy0 = 0.,
		   G4double energy1 = 0.,
		   G4double energy2 = 0.,
		   G4double energy3 = 0.) :
      m_identifier(ident) ,
      m_energy0(energy0) ,
      m_energy1(energy1) ,
      m_energy2(energy2) ,
      m_energy3(energy3)
    {}

    virtual ~CalibrationHit() {;}

    inline void* operator new(size_t);
    inline void operator delete(void *aHit);

    // These two methods are never called in LArG4; there's no point
    // in making them inline.

    virtual void Draw();
    virtual void Print();

  private:

    LArG4Identifier m_identifier;

    // Implementation note: we don't define the energies as a vector
    // or a map, because there are so many hits that we want to avoid
    // the memory overhead.  We could get away with an array, but in
    // this case we'd save so little code that it's not worth it.

    G4double m_energy0;
    G4double m_energy1;
    G4double m_energy2;
    G4double m_energy3;

  public:

    // Accessor methods.  Note the lack of "Set" methods.  Once you've
    // defined the position of a hit, you can't change it.

    inline LArG4Identifier identifier() const { return m_identifier; }

    inline G4double energy(unsigned int i) const
    {
           if ( i == 0 ) return m_energy0;
      else if ( i == 1 ) return m_energy1;
      else if ( i == 2 ) return m_energy2;
      else if ( i == 3 ) return m_energy3;
	   else return 0.;
    }

    // The following line lets you execute "aHit(2)" to get the 3rd
    // energy value.
    inline G4double operator() (unsigned int i) { return energy(i); }

    // The following methods make life much easier for the class
    // LArG4::CalibrationSensitiveDetector.  They define comparisons
    // between CalibrationHit pointers that reference the underlying
    // objects.  For example given that p and q are CalibrationHit*,
    // then p<q if the identifier of (*p) is less than the identifier
    // of (*q).

    bool Less(CalibrationHit* const& h) const { return m_identifier < h->m_identifier; } 
    bool Equals(CalibrationHit* const& h) const { return m_identifier == h->m_identifier; };
    void Add(CalibrationHit* const& h)
    {
      m_energy0 += h->m_energy0;
      m_energy1 += h->m_energy1;
      m_energy2 += h->m_energy2;
      m_energy3 += h->m_energy3;
    }

  };


  // The following is copied from the Geant4 example; I'm not entirely
  // sure why we need to define "new" and "delete". 

  typedef G4THitsCollection<CalibrationHit> CalibrationHitsCollection;

  extern G4Allocator<CalibrationHit> CalibrationHitAllocator;

  inline void* CalibrationHit::operator new(size_t)
  {
    void *aHit;
    aHit = (void *) CalibrationHitAllocator.MallocSingle();
    return aHit;
  }

  inline void CalibrationHit::operator delete(void *aHit)
  {
    CalibrationHitAllocator.FreeSingle((CalibrationHit*) aHit);
  }

} // namespace LArG4

#endif // LArG4_CalibrationHit_H
