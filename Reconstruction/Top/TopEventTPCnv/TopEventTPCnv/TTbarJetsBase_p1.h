/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Andrei Gaponenko, 2008

#ifndef TTBARJETSBASE_P1_H
#define TTBARJETSBASE_P1_H

#include <vector>

#include "DataModelAthenaPool/VectorElementLink_p1.h"
#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"

struct TTbarJetsBase_p1 {
  GeneratePersVectorELinkType_p1<std::vector<ElementLink<JetCollection> > >::type  m_jets;
};

#endif/*TTBARJETSBASE_P1_H*/
