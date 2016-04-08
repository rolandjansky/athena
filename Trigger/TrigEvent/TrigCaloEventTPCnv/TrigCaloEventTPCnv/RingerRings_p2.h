/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : RingerRings_p2
 *
 * @brief persistent partner for RingerRings
 *
 * @author Danilo Enoque Ferreira de Lima <dferreir@mail.cern.ch> - UFRJ
 *
 * File and Version Information:
 * $Id: RingerRings_p2.h,v 1.2 2011-05-31 22:01:37 damazio Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_RINGERRINGS_P2_H
#define TRIGCALOEVENTTPCNV_RINGERRINGS_P2_H

#include <vector>

class RingerRings_p2 {
  friend class RingerRingsCnv_p2;

 public:
    
  // default constructor
  RingerRings_p2() : m_numberOfRings(0) { }

  // default destructor
  ~RingerRings_p2() { }

  //friend class RingerRingsCnv_p1;

 private:

  unsigned int m_numberOfRings;
  std::vector<float> m_rings;

};

#endif
