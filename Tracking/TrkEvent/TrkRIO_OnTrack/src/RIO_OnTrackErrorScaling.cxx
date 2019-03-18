/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "TrkRIO_OnTrack/RIO_OnTrackErrorScaling.h"

RIO_OnTrackErrorScaling::RIO_OnTrackErrorScaling() {}

RIO_OnTrackErrorScaling::~RIO_OnTrackErrorScaling() {}

CLID RIO_OnTrackErrorScaling::clid() const {
  return ClassID_traits<RIO_OnTrackErrorScaling>::ID();
}

void RIO_OnTrackErrorScaling::checkParameters(const char *label,
                                              unsigned int n_paramter_sets,
                                              const char* const* param_names,
                                              unsigned int n_paramters) const
{
  if (params().size() != n_paramter_sets ) {
      std::stringstream message;
      message << label << ": Expecting parameters for 2 parameters for";
      for (unsigned int idx=0; idx < n_paramter_sets; ++idx) {
        message << " " << param_names[idx];
      }
      message << " But got " << params().size() << ".";
      throw std::runtime_error( message.str() );
  }
  for (unsigned int idx=0; idx<params().size(); ++idx) {
    if (params()[idx].size() != n_paramters) {
      assert( idx < n_paramter_sets );
      std::stringstream message;
      message << label << ": Expected 2 parameters for "  << param_names[idx] << " but got " << params()[idx].size()
              << ".";
      throw std::runtime_error( message.str() );
    }
  }
}

