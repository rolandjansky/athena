/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Top level converter for jet collections
///
#ifndef JETEVENTTPCNV_JETCOLLECTIONCNV_TLP2
#define JETEVENTTPCNV_JETCOLLECTIONCNV_TLP2

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "JetEventTPCnv/JetCollection_tlp2.h"

#include "JetEventTPCnv/JetCnv_p4.h"
#include "JetEventTPCnv/JetAssociationBaseCnv_p1.h"

#include "JetEventTPCnv/JetCollectionCnv_p4.h"

class JetCollectionCnv_tlp2 : public AthenaPoolTopLevelTPConverter<JetCollectionCnv_p4, JetCollection_tlp2>
{
 public:
  JetCollectionCnv_tlp2 (void);
  virtual inline ~JetCollectionCnv_tlp2 (void) {}

  void setPStorage (JetCollection_tlp2 *storage);

  virtual TPCnvTokenList_p1 *getTokenListVar()
    {
      return &(getTLPersObject()->m_tokenList);
    }

 protected:
  /// All the various converters.
  JetCnv_p4 m_jetCnv_p4;
  JetAssociationBaseCnv_p1 m_jetAssCnv_p1;
};

#endif
