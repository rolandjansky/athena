/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
 

#ifndef IAFPSILAYERSUMMARYHIT_H
#define IAFPSILAYERSUMMARYHIT_H

#include <xAODForward/AFPSiHit.h>

#include <string>

/// Class providing interface to summary object which can be filled for every hit.
class IAFPSiLayerSummaryHit
{
public:
  /// @brief Fills the distribution using data passed in arguments.
  /// 
  /// @param binName name of the bin (layer) in summary distribution for which entry is to be added
  /// @param hit pointer to the xAOD::AFPSiHit object which provides value used to fill the distribution
  virtual void fillHit (const std::string& binName, const xAOD::AFPSiHit& hit) = 0;
};

#endif
