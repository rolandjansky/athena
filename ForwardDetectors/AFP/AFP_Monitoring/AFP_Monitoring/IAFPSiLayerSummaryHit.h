/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
 

#ifndef IAFPSILAYERSUMMARYHIT_H
#define IAFPSILAYERSUMMARYHIT_H

#include <xAODForward/AFPSiHit.h>

#include <string>

class IAFPSiLayerSummaryHit
{
public:
  virtual void fillHit (const std::string& binName, const xAOD::AFPSiHit& hit) = 0;
};

#endif
