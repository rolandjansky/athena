/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Top level container that will store all the
/// various jet objects that are getting stored.
///
#ifndef JETEVENTTPCNV_JETKEYDESCRIPTORCOLLECTION_TLP1
#define JETEVENTTPCNV_JETKEYDESCRIPTORCOLLECTION_TLP1

#include "JetEventTPCnv/JetKeyDescriptor_p1.h"
#include "JetEventTPCnv/JetKeyDescriptorCollection_p1.h"
#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"

#include <vector>

class JetKeyDescriptorCollection_tlp1
{
 public:
  std::vector<JetKeyDescriptorCollection_p1> m_jetKeyDescriptorCollectionContainers;
  std::vector<JetKeyDescriptor_p1>           m_jetKeyDescriptor_p1;

  TPCnvTokenList_p1 m_tokenList;
};


#endif
