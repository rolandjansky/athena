/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARLJMET_P1_H
#define TTBARLJMET_P1_H

#include "TopEventTPCnv/TTbarLJ_p1.h"
#include "DataModelAthenaPool/DataLink_p1.h"
#include <vector>

// Because 
//
// a) PersistenElementLink does not care about the element type,
// b) We don't need to preserve individual TTbar's type as we only record homogenious containers
//
// We don't need to template persistent TTbarLJMet.
struct TTbarLJMet_p1 : public TTbarLJ_p1 {
  DataLink_p1 m_metLink_p1;
};

#endif/*TTBARLJMET_P1_H*/

