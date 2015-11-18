/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGRINGERHELPER_H
#define TRIGRINGERHELPER_H

#include <vector>

typedef float Feature;
typedef std::vector<Feature> Pattern;


// Helper class used to build ringsets
class RingSet {
  
  public: //interface

    enum Normalisation {
      EVENT       = 0, ///< normalises the ring energies with the total event energy
      SECTION     = 1, ///< normalises the ring energies with the section energy
      SET         = 2, ///< normalises the ring energies with the set energy
      SEQUENTIAL  = 3, ///< normalises the ring energies with a special scheme
      NONE        = 4 ///< no normalisation is applied
    };
    enum Section {
      EM          = 0, ///< electromagnetic section of the ATLAS calorimetry
      HADRONIC    = 1 ///< hadronic section of the ATLAS calorimetry
    };
    


    RingSet (unsigned int max, const Normalisation &norm, const Section &section) 
    : m_max(max),
      m_norm(norm),
      m_section(section),
      m_val(max)
    {;}
 
    ~RingSet()
    {;}
    
    inline unsigned int    max(void) const { return m_max; }
    inline Normalisation  normalisation(void) const { return m_norm; }
    inline Section        section(void) const { return m_section; }
    
    ///Returns the (current) ring values.
    inline Pattern& pattern (void) { return m_val; }
    
    ///Resets all current values
    inline void reset (void) { m_val.clear(); }

    ///Appends ring to ringset
    inline void add ( double ring) { m_val.push_back(ring); }

  private: //representation
    
    unsigned int  m_max;
    Normalisation m_norm;     ///< the normalisation scheme for this ring set
    Section       m_section;  ///< the section this ring set belongs to
    Pattern       m_val;      ///< my current values
};
#endif /* TrigCaloRingerHelper */
