/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARLLJETSMET_P1_H
#define TTBARLLJETSMET_P1_H

#include "TopEventTPCnv/TTbarLLJets_p1.h"
#include "DataModelAthenaPool/DataLink_p1.h"
#include <vector>

// Because 
//
// a) PersistenElementLink does not care about the element type,
// b) We don't need to preserve individual TTbar's type as we only record homogenious containers
//
// We don't need to template persistent TTbarLLJetsMet.
struct TTbarLLJetsMet_p1 : public TTbarLLJets_p1 {
  DataLink_p1 m_metLink_p1;
};

#endif/*TTBARLLJETSMET_P1_H*/

