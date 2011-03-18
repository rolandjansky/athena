/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Andrei Gaponenko, 2009

#ifndef TTBARLJTOP_P1_H
#define TTBARLJTOP_P1_H

#include "TopEventTPCnv/TTbarLJ_p1.h"

#include <vector>

// Because 
//
// a) PersistenElementLink does not care about the element type,
// b) We don't need to preserve individual TTbar's type as we only record homogenious containers
//
// We don't need to template persistent TTbarLJTop.
struct TTbarLJTop_p1 : public TTbarLJ_p1 {
  std::vector<unsigned> m_htop_indexes;
};

#endif/*TTBARLJTOP_P1_H*/
