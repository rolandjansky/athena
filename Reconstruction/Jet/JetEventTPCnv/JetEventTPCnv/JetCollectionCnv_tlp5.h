/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Top level converter for jet collections
///
#ifndef JETEVENTTPCNV_JETCOLLECTIONCNV_TLP5
#define JETEVENTTPCNV_JETCOLLECTIONCNV_TLP5

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "JetEventTPCnv/JetCollection_tlp5.h"

#include "JetEventTPCnv/JetCnv_p5.h"
#include "JetEventTPCnv/JetAssociationBaseCnv_p1.h"

#include "JetEventTPCnv/JetCollectionCnv_p5.h"

class JetCollectionCnv_tlp5 : public AthenaPoolTopLevelTPConverter<JetCollectionCnv_p5, JetCollection_tlp5>
{
 public:
  JetCollectionCnv_tlp5 (void);
  virtual inline ~JetCollectionCnv_tlp5 (void) {}

  void setPStorage (JetCollection_tlp5 *storage);

  virtual TPCnvTokenList_p1 *getTokenListVar()
    {
      return &(getTLPersObject()->m_tokenList);
    }

 protected:
  /// All the various converters.
  JetCnv_p5 m_jetCnv_p5;
  JetAssociationBaseCnv_p1 m_jetAssCnv_p1;
};

#endif
