/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Top level converter for jet collections
///
#ifndef JETEVENTTPCNV_JETKEYDESCRIPTORCOLLECTIONCNV_TLP1
#define JETEVENTTPCNV_JETKEYDESCRIPTORCOLLECTIONCNV_TLP1

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "JetEventTPCnv/JetKeyDescriptorCollection_tlp1.h"

#include "JetEventTPCnv/JetKeyDescriptorCnv_p1.h"
#include "JetEventTPCnv/JetKeyDescriptorCollectionCnv_p1.h"

class JetKeyDescriptorCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter<JetKeyDescriptorCollectionCnv_p1, JetKeyDescriptorCollection_tlp1>
{
 public:
  JetKeyDescriptorCollectionCnv_tlp1 (void);
  virtual inline ~JetKeyDescriptorCollectionCnv_tlp1 (void) {}

  void setPStorage (JetKeyDescriptorCollection_tlp1 *storage);

  virtual TPCnvTokenList_p1 *getTokenListVar()
    {
      return &(getTLPersObject()->m_tokenList);
    }

 protected:
  /// All the various converters.
  JetKeyDescriptorCnv_p1 m_jetKeyDescriptorCnv_p1;
};

/// The converter for the container itself (ARA)...
template<>
class T_TPCnv<JetKeyDescriptorCollection, JetKeyDescriptorCollection_tlp1>
: public JetKeyDescriptorCollectionCnv_tlp1
{
public:
  T_TPCnv ();
  void persToTrans (const JetKeyDescriptorCollection_tlp1 *pers,
		    JetKeyDescriptorCollection *trans,
		    MsgStream &msg);
  void transToPers (const JetKeyDescriptorCollection *trans,
                    JetKeyDescriptorCollection_tlp1 *pers, 
                    MsgStream &msg);
};

#endif
