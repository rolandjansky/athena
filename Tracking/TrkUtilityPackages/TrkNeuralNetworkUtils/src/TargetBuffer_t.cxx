/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifdef NDEBUG
#else

#include "TargetBuffer_t.h"
#include <sstream>

void TTN::Buffer_t::throwRangeError(std::vector<double>::size_type idx) const {
  std::stringstream msg;
  msg << "TTN::Buffer_t index out of range ( " << idx << " !< " << m_maxIndex << " )";
  throw std::runtime_error(msg.str());
}

#endif
