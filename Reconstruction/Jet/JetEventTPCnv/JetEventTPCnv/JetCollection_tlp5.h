/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Top level container that will store all the
/// various jet objects that are getting stored.
///
#ifndef JETEVENTTPCNV_JETCOLLECTION_TLP5
#define JETEVENTTPCNV_JETCOLLECTION_TLP5

#include "JetEventTPCnv/Jet_p5.h"
#include "JetEventTPCnv/JetCollection_p5.h"
#include "JetEventTPCnv/JetAssociationBase_p1.h"
#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"

#include <vector>

class JetCollection_tlp5
{
 public:
  std::vector<JetCollection_p5> m_jetCollectionContainers;
  std::vector<Jet_p5> m_jet_p5;
  std::vector<JetAssociationBase_p1> m_jetAssociationBase_p1;

  TPCnvTokenList_p1 m_tokenList;
};


#endif
