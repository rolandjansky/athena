// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODCALORINGS_UTILS_GETCALORINGSDECORATOR_H
#define XAODCALORINGS_UTILS_GETCALORINGSDECORATOR_H

// Local includes:
#include "xAODCaloRings/CaloRingsAuxTypes.h"

// System include(s).
#include <string>

namespace xAOD {

inline
std::string caloRingsLinksDecorKey(){
  return "caloRingsLinks";
}

/**
 * @brief Get CaloRings accessor with read only permissions.
 **/
const caloRingsReader_t& getCaloRingsReader();

} // namespace xAOD

#endif // XAODCALORINGS_TOOLS_GETCALORINGSDECORATOR_H
