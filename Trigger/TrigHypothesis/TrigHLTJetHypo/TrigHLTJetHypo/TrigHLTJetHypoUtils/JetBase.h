/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYO_JETBASE_H
#define TRIGHLTJETHYO_JETBASE_H

#include "./IJet.h"
#include <sstream>

namespace HypoJet{
  class JetBase: virtual public HypoJet::IJet {
    std::string toString() const {
      std::stringstream ss;
      ss << "Jet-  pt: " << pt() << " eta: " 
	 << eta() << " phi: " << phi() << '\n';
      return ss.str();
    }
  };
}
#endif
