/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Top level container converter.
///

#include "JetEventTPCnv/JetCollectionCnv_tlp6.h"
#include "JetEvent/JetCollection.h"
#include "JetEventTPCnv/JetConverterBase.h"


///
/// Register all the converters
///

JetCollectionCnv_tlp6::JetCollectionCnv_tlp6 (void)
{
  addMainTPConverter();
  addTPConverter (&m_jetCnv_p6);
  addTPConverter (&m_jetAssCnv_p1);
}

///
/// Connect up the storage when we are doing the reading/writing.
///
void JetCollectionCnv_tlp6::setPStorage (JetCollection_tlp6 *storage)
{
  setMainCnvPStorage(&storage->m_jetCollectionContainers);
  m_jetCnv_p6.setPStorage(&storage->m_jet_p6);
  m_jetAssCnv_p1.setPStorage(&storage->m_jetAssociationBase_p1);
}

