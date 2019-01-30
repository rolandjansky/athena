// emacs , this -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENTTPCNV_JETMOMENTMAP_p6_H
#define JETEVENTTPCNV_JETMOMENTMAP_p6_H

#include <vector>

class JetMomentMap_p6
{
  template <class T>
  friend class JetMomentMapConverterBase;
public:

  JetMomentMap_p6() { };
  virtual ~JetMomentMap_p6() = default;
  
  // this limit the number of persified moments to 256
  typedef unsigned char mom_num_t;
  static const mom_num_t s_maxNumMoment = -1; // as mom_num_t is unsigned, this will be the max value

 protected:
  
  std::vector<float> m_moments;
  std::vector<bool>  m_momentStatus;
  std::vector<mom_num_t> m_momentNum;
  std::vector<unsigned int> m_jetIds;
};

#endif
