/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _CHECK_CAST_H_
#define _CHECK_CAST_H_

#include <sstream>
#include <stdexcept>
#include <typeinfo>

template<class T_dest, class T_src>
const T_dest *check_cast(const T_src *src) {
  const T_dest *dest=dynamic_cast<const T_dest *>(src);
  if (!dest) {
    std::stringstream message;
    message << "failed to cast "  << typeid(src).name() << " to " << typeid(dest).name();
    throw std::runtime_error(message.str());
  }
  return dest;
}

#endif
