/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Andrei Gaponenko, 2008

#ifndef TTBARLL_P1_H
#define TTBARLL_P1_H

#include "DataModelAthenaPool/ElementLink_p3.h"

// Because 
//
// a) PersistenElementLink does not care about the element type,
// b) We don't need to preserve individual TTbar's type as we only record homogenious containers
//
// We don't need to template persistent TTbarLL.
struct TTbarLL_p1 {
   ElementLinkInt_p3 m_lepton1;
   ElementLinkInt_p3 m_lepton2;
};

#endif/*TTBARLL_P1_H*/
