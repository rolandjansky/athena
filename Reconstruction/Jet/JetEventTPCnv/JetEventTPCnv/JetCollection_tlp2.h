/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Top level container that will store all the
/// various jet objects that are getting stored.
///
#ifndef JETEVENTTPCNV_JETCOLLECTION_TLP2
#define JETEVENTTPCNV_JETCOLLECTION_TLP2

#include "JetEventTPCnv/Jet_p4.h"
#include "JetEventTPCnv/JetCollection_p4.h"
#include "JetEventTPCnv/JetAssociationBase_p1.h"
#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"

#include <vector>

class JetCollection_tlp2
{
 public:
  std::vector<JetCollection_p4> m_jetCollectionContainers;
  std::vector<Jet_p4> m_jet_p4;
  std::vector<JetAssociationBase_p1> m_jetAssociationBase_p1;

  TPCnvTokenList_p1 m_tokenList;
};


#endif
