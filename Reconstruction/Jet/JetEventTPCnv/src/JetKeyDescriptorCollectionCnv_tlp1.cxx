/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Top level container converter.
///

#include "JetEventTPCnv/JetKeyDescriptorCollectionCnv_tlp1.h"
#include "JetEvent/JetKeyDescriptorCollection.h"

///
/// Register all the converters
///

JetKeyDescriptorCollectionCnv_tlp1::JetKeyDescriptorCollectionCnv_tlp1 (void)
{
  addMainTPConverter();
  addTPConverter (&m_jetKeyDescriptorCnv_p1);
}

///
/// Connect up the storage when we are doing the reading/writing.
///
void JetKeyDescriptorCollectionCnv_tlp1::setPStorage (JetKeyDescriptorCollection_tlp1 *storage)
{
  setMainCnvPStorage(&storage->m_jetKeyDescriptorCollectionContainers);
  m_jetKeyDescriptorCnv_p1.setPStorage(&storage->m_jetKeyDescriptor_p1);
}

