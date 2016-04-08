/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : RingerRings_p1
 *
 * @brief persistent partner for RingerRings
 *
 * @author Danilo Enoque Ferreira de Lima <dferreir@mail.cern.ch> - UFRJ
 *
 * File and Version Information:
 * $Id: RingerRings_p1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_RINGERRINGS_P1_H
#define TRIGCALOEVENTTPCNV_RINGERRINGS_P1_H

#include <vector>

class RingerRings_p1 {
  friend class RingerRingsCnv;

 public:
    
  // default constructor
  RingerRings_p1() : m_numberOfRings(0) { }

  // default destructor
  ~RingerRings_p1() { }

  friend class RingerRingsCnv_p1;

 private:

  unsigned int m_numberOfRings;
  std::vector<float> m_rings;

};

#endif
