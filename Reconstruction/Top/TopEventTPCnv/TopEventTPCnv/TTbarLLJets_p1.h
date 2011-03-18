/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Andrei Gaponenko, 2008

#ifndef TTBARLLJETS_P1_H
#define TTBARLLJETS_P1_H

#include "TopEventTPCnv/TTbarJetsBase_p1.h"

#include <vector>

#include "DataModelAthenaPool/ElementLink_p3.h"

// Because 
//
// a) PersistenElementLink does not care about the element type,
// b) We don't need to preserve individual TTbar's type as we only record homogenious containers
//
// We don't need to template persistent TTbarLL.
struct TTbarLLJets_p1 : public TTbarJetsBase_p1 {
   ElementLinkInt_p3 m_lepton1;
   ElementLinkInt_p3 m_lepton2;
};

#endif/*TTBARLLJETS_P1_H*/
