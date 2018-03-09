/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainersRoot/src/getDynamicAuxID.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2016
 * @brief Find the auxid for a dynamic branch.
 */


#include "AthContainersRoot/getDynamicAuxID.h"
#include "AthContainersRoot/RootAuxVectorFactory.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "TClass.h"
#include "TROOT.h"


namespace SG {


/**
 * @brief Look up a @c TClass given a name.  Try to avoid autoparsing.
 * @param cname Name of the class to find.
 */
TClass* getClassIfDictionaryExists (const std::string& cname)
{
  if (TClass* cl = (TClass*)gROOT->GetListOfClasses()->FindObject(cname.c_str())) {
    if (cl->IsLoaded() && cl->HasDictionary()) return cl;
    return nullptr;
  }

  if (gInterpreter->GetClassSharedLibs (cname.c_str())) {
    TClass* cl = TClass::GetClass (cname.c_str());
    if (cl->HasDictionary())
      return cl;
  }
  return nullptr;
}


/**
 * @brief Find the auxid for a dynamic branch.
 * @param ti Type of the auxiliary variable.
 *           Usually the type of the vector payload, but if @c standalone
 *           is true, then this is the type of the stored object.
 * @param name Auxiliary variable name.
 * @param elementTypeName Name of the type for one aux data element.
 *                        Should be the samr as @c branchTypeName
 *                        if @c standalone is true.
 * @param branchTypeName Name of the type for this branch.
 * @param standalone True if this is a standalone object.
 */
SG::auxid_t getDynamicAuxID (const std::type_info& ti,
                             const std::string& name,
                             const std::string& elementTypeName,
                             const std::string& branchTypeName,
                             bool standalone)
{
  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  SG::auxid_t auxid = SG::null_auxid;

  auxid = r.getAuxID (ti, name);
  if (auxid != SG::null_auxid) return auxid;

  std::string fac_class_name = "SG::AuxTypeVectorFactory<" +
    elementTypeName;
  if (fac_class_name[fac_class_name.size()-1] == '>')
    fac_class_name += ' ';
  fac_class_name += '>';
  TClass* fac_class = getClassIfDictionaryExists (fac_class_name);
  if (fac_class)
  {
    TClass* base_class = getClassIfDictionaryExists ("SG::IAuxTypeVectorFactory");
    if (base_class) {
      int offs = fac_class->GetBaseClassOffset (base_class);
      if (offs >= 0) {
        void* fac_vp = fac_class->New();
        if (fac_vp) {
          SG::IAuxTypeVectorFactory* fac = reinterpret_cast<SG::IAuxTypeVectorFactory*> (reinterpret_cast<unsigned long>(fac_vp) + offs);
          r.addFactory (ti, fac);
          auxid = r.getAuxID(ti, name);
        }
      }
    }
  }

  if (auxid == SG::null_auxid) {
    std::string vec_name = branchTypeName;
    if (standalone) {
      vec_name = "std::vector<" + branchTypeName;
      if (vec_name[vec_name.size()-1] == '>')
        vec_name += " ";
      vec_name += ">";
    }
    TClass* vec_class = TClass::GetClass (vec_name.c_str());

    if (vec_class) {
      SG::IAuxTypeVectorFactory* fac = new SG::RootAuxVectorFactory (vec_class);
      r.addFactory (ti, fac);
      auxid = r.getAuxID(ti, name);
    }
  }

  return auxid;
}


} // namespace SG
