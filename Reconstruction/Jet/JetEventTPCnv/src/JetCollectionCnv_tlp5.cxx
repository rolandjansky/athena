/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Top level container converter.
///

#include "JetEventTPCnv/JetCollectionCnv_tlp5.h"
#include "JetEvent/JetCollection.h"

///
/// Register all the converters
///

JetCollectionCnv_tlp5::JetCollectionCnv_tlp5 (void)
{
  addMainTPConverter();
  addTPConverter (&m_jetCnv_p5);
  addTPConverter (&m_jetAssCnv_p1);
}

///
/// Connect up the storage when we are doing the reading/writing.
///
void JetCollectionCnv_tlp5::setPStorage (JetCollection_tlp5 *storage)
{
  setMainCnvPStorage(&storage->m_jetCollectionContainers);
  m_jetCnv_p5.setPStorage(&storage->m_jet_p5);
  m_jetAssCnv_p1.setPStorage(&storage->m_jetAssociationBase_p1);
}

