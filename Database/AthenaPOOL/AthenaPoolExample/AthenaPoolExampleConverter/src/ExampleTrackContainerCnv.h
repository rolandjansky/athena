/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXAMPLETRACKCONTAINERCNV_H
#define EXAMPLETRACKCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "ExampleTrackContainerCnv_p1.h"
#define private public
#define protected public
#include "AthenaPoolExampleConverter/ExampleTrackContainer_p1.h"
#include "AthenaPoolExampleData/ExampleTrackContainer.h"
#undef private
#undef protected

#include <memory>

/** 
 ** Create derived converter to customize the saving of ExampleTrackContainer
 **/
class ExampleTrackContainerCnv : public T_AthenaPoolCustomCnv<ExampleTrackContainer, ExampleTrackContainer_p1> {
   friend class CnvFactory<ExampleTrackContainerCnv>;
public:
   ExampleTrackContainerCnv(ISvcLocator* svcloc) : T_AthenaPoolCustomCnv<ExampleTrackContainer, ExampleTrackContainer_p1>::T_AthenaPoolCustomCnv(svcloc) {}

   /// Extend base-class conversion methods
   virtual ExampleTrackContainer_p1* createPersistent(ExampleTrackContainer* transObj) {
      MsgStream log(msgSvc(), "ExampleTrackContainerCnv");
      ExampleTrackContainer_p1* persObj = m_TPconverter.createPersistent(transObj, log);
      return(persObj);
   }

   virtual ExampleTrackContainer* createTransient() {
      static pool::Guid p1_guid("FF777FDA-721C-4756-BBF3-4CE28C2A3AF5");
      if (compareClassGuid(p1_guid)) {
         MsgStream log(msgSvc(), "ExampleTrackContainer");
         std::auto_ptr<ExampleTrackContainer_p1> cont_p1(this->poolReadObject<ExampleTrackContainer_p1>());
         return(m_TPconverter.createTransient(cont_p1.get(), log));
      }
      return(0);
   }

private:
   ExampleTrackContainerCnv_p1 m_TPconverter;
};

#endif
