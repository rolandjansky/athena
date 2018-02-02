/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RINGERRINGS_H
#define RINGERRINGS_H


#include <vector>

#include "AthenaKernel/CLASS_DEF.h"
#include "GaudiKernel/MsgStream.h"

/**
 * This class carries the generated Rings.
 */

class RingerRings {

private:
  std::vector<float> m_rings; //< Holds the generated rings.
    
public:

  /**
   * Class default constructor
   */
  RingerRings();
    
  /**
   * Class constructor
   * 
   * @param rings The rings to be saved.
   */
  RingerRings(const std::vector<float> &rings);


  /**
   * Class constructor
   * 
   * @param nRings Number of rings.
   * @param rings  Ring array.
   */	
  RingerRings(const unsigned int nRings, const float *rings);
    
  /** Acessor methods **/
  std::vector<float> &rings();
  const std::vector<float> &rings() const;
  void rings(const std::vector<float> &r);
  float &at(const unsigned int i);
  const float &at(const unsigned int i) const;
  unsigned int size() const;
  const float &operator [](const unsigned int i) const;
  float &operator [](const unsigned int i);
  
  /**
   * Prints some debug message.
   * @param log The msg stream to print the info to.
   */
  void print ( MsgStream& log ) const;
};

CLASS_DEF( RingerRings , 169208235 , 1 )

#endif

  
