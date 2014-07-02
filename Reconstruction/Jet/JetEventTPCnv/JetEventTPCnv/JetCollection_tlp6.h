/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Top level container that will store all the
/// various jet objects that are getting stored.
///
#ifndef JETEVENTTPCNV_JETCOLLECTION_TLP6
#define JETEVENTTPCNV_JETCOLLECTION_TLP6

#include "JetEventTPCnv/Jet_p6.h"
#include "JetEventTPCnv/JetCollection_p6.h"
#include "JetEventTPCnv/JetAssociationBase_p1.h"
#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"

#include <vector>

class JetCollection_tlp6
{
 public:
  std::vector<JetCollection_p6> m_jetCollectionContainers;
  std::vector<Jet_p6> m_jet_p6;
  std::vector<JetAssociationBase_p1> m_jetAssociationBase_p1;

  TPCnvTokenList_p1 m_tokenList;
};


#endif
