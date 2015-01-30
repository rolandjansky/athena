/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Top level converter for jet collections
///
#ifndef JETEVENTTPCNV_JETCOLLECTIONCNV_TLP6
#define JETEVENTTPCNV_JETCOLLECTIONCNV_TLP6

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "JetEventTPCnv/JetCollection_tlp6.h"

#include "JetEventTPCnv/JetCnv_p6.h"
#include "JetEventTPCnv/JetAssociationBaseCnv_p1.h"

#include "JetEventTPCnv/JetCollectionCnv_p6.h"

class JetCollectionCnv_tlp6 : public AthenaPoolTopLevelTPConverter<JetCollectionCnv_p6, JetCollection_tlp6>
{
 public:
  JetCollectionCnv_tlp6 (void);
  virtual inline ~JetCollectionCnv_tlp6 (void) {}

  void setPStorage (JetCollection_tlp6 *storage);

  virtual TPCnvTokenList_p1 *getTokenListVar()
    {
      return &(getTLPersObject()->m_tokenList);
    }

 protected:
  /// All the various converters.
  JetCnv_p6 m_jetCnv_p6;
  JetAssociationBaseCnv_p1 m_jetAssCnv_p1;
};

#endif
