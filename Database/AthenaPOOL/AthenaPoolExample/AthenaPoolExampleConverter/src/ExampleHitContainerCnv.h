/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXAMPLEHITCONTAINERCNV_H
#define EXAMPLEHITCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "ExampleHitContainerCnv_p1.h"
#define private public
#define protected public
#include "AthenaPoolExampleConverter/ExampleHitContainer_p1.h"
#include "AthenaPoolExampleData/ExampleHitContainer.h"
#undef private
#undef protected

#include <memory>

/** 
 ** Create derived converter to customize the saving of ExampleHitContainer
 **/
class ExampleHitContainerCnv : public T_AthenaPoolCustomCnv<ExampleHitContainer, ExampleHitContainer_p1> {
   friend class CnvFactory<ExampleHitContainerCnv>;
public:
   ExampleHitContainerCnv(ISvcLocator* svcloc) : T_AthenaPoolCustomCnv<ExampleHitContainer, ExampleHitContainer_p1>::T_AthenaPoolCustomCnv(svcloc) {}

   /// Extend base-class conversion methods
   virtual ExampleHitContainer_p1* createPersistent(ExampleHitContainer* transObj){
      MsgStream log(msgSvc(), "ExampleHitContainerCnv");
      ExampleHitContainer_p1* persObj = m_TPconverter.createPersistent(transObj, log);
      return(persObj);
   }

   virtual ExampleHitContainer* createTransient() {
      static pool::Guid p1_guid("6BB89FA1-EB62-4641-97CA-3F8DB6588053");
      if (compareClassGuid(p1_guid)) {
         MsgStream log(msgSvc(), "ExampleHitContainer");
         std::auto_ptr<ExampleHitContainer_p1> cont_p1(this->poolReadObject<ExampleHitContainer_p1>());
         return(m_TPconverter.createTransient(cont_p1.get(), log));
      }
      return(0);
   }

private:
   ExampleHitContainerCnv_p1 m_TPconverter;
};

#endif
