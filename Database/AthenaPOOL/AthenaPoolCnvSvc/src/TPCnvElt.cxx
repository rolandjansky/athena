/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaPoolCnvSvc/src/TPCnvElt.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Helper for calling a TP converter from an Athena converter.
 */


#include "AthenaPoolCnvSvc/TPCnvElt.h"
#include "AthenaPoolCnvSvc/exceptions.h"
#include "StorageSvc/DbReflex.h"

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/System.h"


namespace AthenaPoolCnvSvc {


/**
 * @brief Given a @c type_info, get the corresponding pool guid.
 * @param ti @c type_info to look for.
 *
 * Throws an exception on errors.
 */
Guid guidFromTypeinfo (const std::type_info& ti)
{
  pool::TypeH typ = pool::DbReflex::forTypeInfo (ti);
  if (!typ) {
    // Print additional diagnostic information here to try to get a better
    // handle on why this is happening.
    // See ATEAM-697.
    std::string clname = System::typeinfoName (ti);
    MsgStream msg (Athena::getMessageSvc(), "TPCnvElt");
    msg << MSG::ERROR << "Cannot find full dictionary information for class " <<
      clname << endmsg;
    TClass* cls = static_cast<TClass*>(typ);
    if (!cls) {
      msg << MSG::ERROR <<  "No TClass; trying to retrieve again." << endmsg;
      cls = TClass::GetClass (ti);
      if (!cls) {
        msg << MSG::ERROR << "Couldn't retrieve via type_info; try by name." << endmsg;
        cls = TClass::GetClass (clname.c_str());
      }
    }
    if (cls) {
      msg << MSG::ERROR << "Got TClass " << cls->GetName() << " " <<
        cls->HasInterpreterInfoInMemory() << " " <<
        cls->HasInterpreterInfo() << " " <<
        cls->GetState() << " " <<
        cls->HasDictionary() << " " <<
        cls->IsLoaded() << " " <<
        cls->IsForeign() << endmsg;

      TClass* cls2 = TClass::GetClass (ti);
      if (cls2) {
        msg << MSG::ERROR << "Got TClass by ti " << cls2->GetName() << " " <<
          (cls == cls2) << " " <<
          cls2->HasInterpreterInfoInMemory() << " " <<
          cls2->HasInterpreterInfo() << " " <<
          cls2->GetState() << " " <<
          cls2->HasDictionary() << " " <<
          cls2->IsLoaded() << " " <<
          cls2->IsForeign() << endmsg;
      }
      else {
        msg << MSG::ERROR << "Could not re-get class by ti" << endmsg;
      }

      cls2 = TClass::GetClass (clname.c_str());
      if (cls2) {
        msg << MSG::ERROR << "Got TClass by name " << cls2->GetName() << " " <<
          (cls == cls2) << " " <<
          cls2->HasInterpreterInfoInMemory() << " " <<
          cls2->HasInterpreterInfo() << " " <<
          cls2->GetState() << " " <<
          cls2->HasDictionary() << " " <<
          cls2->IsLoaded() << " " <<
          cls2->IsForeign() << endmsg;
      }
      else {
        msg << MSG::ERROR << "Could not re-get class by name" << endmsg;
      }

      sleep (10);

      cls2 = TClass::GetClass (ti);
      if (cls2) {
        msg << MSG::ERROR << "Got TClass by ti " << cls2->GetName() << " " <<
          (cls == cls2) << " " <<
          cls2->HasInterpreterInfoInMemory() << " " <<
          cls2->HasInterpreterInfo() << " " <<
          cls2->GetState() << " " <<
          cls2->HasDictionary() << " " <<
          cls2->IsLoaded() << " " <<
          cls2->IsForeign() << endmsg;
      }
      else {
        msg << MSG::ERROR << "Could not re-get class by ti" << endmsg;
      }

      cls2 = TClass::GetClass (clname.c_str());
      if (cls2) {
        msg << MSG::ERROR << "Got TClass by name " << cls2->GetName() << " " <<
          (cls == cls2) << " " <<
          cls2->HasInterpreterInfoInMemory() << " " <<
          cls2->HasInterpreterInfo() << " " <<
          cls2->GetState() << " " <<
          cls2->HasDictionary() << " " <<
          cls2->IsLoaded() << " " <<
          cls2->IsForeign() << endmsg;
      }
      else {
        msg << MSG::ERROR << "Could not re-get class by name" << endmsg;
      }
    }
    throwExcNoDictForClass (ti);
  }
  return pool::DbReflex::guid (typ);
}


} // namespace AthenaPoolCnvSvc
