/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: StdHackGenerator.cxx,v 1.4 2008-03-20 15:30:00 ssnyder Exp $
/**
 * @file  RootUtils/src/StdHackGenerator.cxx
 * @author scott snyder
 * @date Oct 2007
 * @brief Work around inconsistent use of @c std:: and spaces.
 */

#include "RootUtils/StdHackGenerator.h"
#include "TROOT.h"
#include "TClassEdit.h"
#include "TClassTable.h"
#include "TInterpreter.h"
#include "TClass.h"
#include <cstring>
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
#include "TCint.h"
#include "Api.h"
#include "Property.h"
#endif

namespace {


/**
 * @brief Make a new class name, inserting any missing @c std:: qualifications.
 * @param in The name to edit.
 * @return The new name (may be the same as @a in).
 */
std::string addstd (const std::string& in)
{
  // string -> std::string
  if (in == "string")
    return "std::string";

  // Otherwise, give up if it's not a template name.
  if (strchr (in.c_str(), '<') == 0)
    return in;

  // Split apart template arguments.
  std::vector<std::string> vec;
  int loc;
  TClassEdit::GetSplit (in.c_str(), vec, loc);
  if (vec.empty())
    return "";
  else {
    std::string out;

    // If the template name is a STL name that does not start with
    // std::, then add std::.
    if (strncmp (vec[0].c_str(), "std::", 5) != 0 &&
        (TClassEdit::STLKind (vec[0].c_str()) != 0 ||
         vec[0] == "pair" ||
         vec[0] == "string"))
    {
      out = "std::";
    }
    out += vec[0];

    // Now add the arguments.  Process each one recursively.
    if (vec.size() > 1) {
      out += "<";
      for (size_t i = 1; i < vec.size(); i++) {
        if (i > 1) out += ",";
        out += addstd (vec[i]);
      }
      if (out[out.size()-1] == '>')
        out += " ";
      out += ">";
    }
    return out;
  }
}

// Need to convert
//__gnu_cxx::__normal_iterator<HepMC::GenParticle*const*,vector<HepMC::GenParticle*> >
//to
//__gnu_cxx::__normal_iterator<HepMC::GenParticle* const*,vector<HepMC::GenParticle*> >

/**
 * @brief Make a new class name, inserting any missing spaces.
 * @param in The name to edit.
 * @return The new name (may be the same as @a in).
 */
std::string addspc (const std::string& in)
{
  if (strchr (in.c_str(), '<') == 0) {
    std::string::size_type i = in.find ("*const*");
    if (i != std::string::npos) {
      std::string out = in;
      out.insert (i+1, " ");
      return out;
    }
    return in;
  }

  // Split apart template arguments.
  std::vector<std::string> vec;
  int loc;
  TClassEdit::GetSplit (in.c_str(), vec, loc);
  if (vec.empty())
    return "";
  else {
    std::string out;

    out += vec[0];

    // Now add the arguments.  Process each one recursively.
    if (vec.size() > 1) {
      out += "<";
      for (size_t i = 1; i < vec.size(); i++) {
        if (i > 1) out += ",";
        out += addspc (vec[i]);
      }
      if (out[out.size()-1] == '>')
        out += " ";
      out += ">";
    }
    return out;
  }
}


#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
extern "C" G__pMethodUpdateClassInfo G__UserSpecificUpdateClassInfo;


G__pMethodUpdateClassInfo s_oldUpdateClassInfo = 0;


/**
 * @brief @c UpdateClassInfo wrapper to prevent reflex dictionary
 *        info from being clobbered by rootcint.
 *        See comments in the header file for more details.
 * @param item Name of the class.
 * @param tagnum CINT tag number of the class.
 */
extern "C"
void updateClassInfo (char* item, long tagnum)
{
  // Remember a previous Cint tagnum if:
  //   - We had to translate the class name (eg, by removing `allocator')
  //     to find the TClass, and
  //   - The class info had the ISCPPCOMPILED flag set.
  int old_tagnum = -1;
  TClass* cl = 0;
  if (strchr (item, '<') != 0) {
    TString shortname =
      TClassEdit::ShortType (item, TClassEdit::kDropStlDefault);
    if (shortname != item) {
      cl = gROOT->GetClass (shortname, false);
      if (cl && (gCint->ClassInfo_Property (cl->GetClassInfo()) &
                 G__BIT_ISCPPCOMPILED))
      {
        old_tagnum = gCint->ClassInfo_Tagnum (cl->GetClassInfo());
      }
    }
  }

  // Call the previous hook.
  if (s_oldUpdateClassInfo)
    s_oldUpdateClassInfo (item, tagnum);

  // If we replaced a previous CPPCOMPILED tagnum with one that
  // isn't, restore the previous tagnum.
  if (old_tagnum >= 0 &&
      gCint->ClassInfo_Tagnum (cl->GetClassInfo()) != old_tagnum &&
      ! (gCint->ClassInfo_Property (cl->GetClassInfo()) & G__BIT_ISCPPCOMPILED))
  {
    gCint->ClassInfo_Init (cl->GetClassInfo(), old_tagnum);
  }
}
#endif


} // anonymous namespace


namespace RootUtils {


/**
 * @brief Initialize.
 *        Create an instance of this class and register it as a generator.
 */
void StdHackGenerator::initialize()
{
  // Only do this once.
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
    gROOT->AddClassGenerator (new StdHackGenerator);

#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
    s_oldUpdateClassInfo = G__UserSpecificUpdateClassInfo;
    G__InitUpdateClassInfo (updateClassInfo);
#endif
  }
}


/**
 * @brief Look up a class by name.
 * @param classname The name of the class to find.
 * @param load If true, enable autoloading.
 * @param silent If true, suppress messages.
 */
TClass* StdHackGenerator::GetClass (const char* classname,
                                    Bool_t load,
                                    Bool_t
#if ROOT_VERSION_CODE < ROOT_VERSION(6,1,2)
                                       silent
#endif
                                    )
{
  // Don't do anything if no autoloading.
  if (!load)
    return 0;

  // Add any missing std::'s to the name.
  std::string newname = addstd (classname);

  // If it's different, retry the autoloading.
  if (newname != classname && gInterpreter->AutoLoad (newname.c_str())) {
    // From here, we're just copying what the root code does.
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,1,2)
    DictFuncPtr_t dict = TClassTable::GetDict (newname.c_str());
    if (!dict)
      dict = TClassTable::GetDict (classname);
    if (dict) {
      TClass* cl = (dict)();
      if (cl) cl->PostLoadCheck();
      return cl;
    }
#else
    VoidFuncPtr_t dict = TClassTable::GetDict (newname.c_str());
    if (!dict)
      dict = TClassTable::GetDict (classname);
    if (dict) {
      (dict)();
      return gROOT->GetClass (newname.c_str(), kFALSE, silent);
    }
#endif
  }

  newname = addspc (classname);
  // If it's different, retry the autoloading.
  if (newname != classname && gInterpreter->AutoLoad (newname.c_str())) {
    // From here, we're just copying what the root code does.
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,1,2)
    DictFuncPtr_t dict = TClassTable::GetDict (newname.c_str());
    if (!dict)
      dict = TClassTable::GetDict (classname);
    if (dict) {
      TClass* cl = (dict)();
      if (cl) cl->PostLoadCheck();
      return cl;
    }
#else
    VoidFuncPtr_t dict = TClassTable::GetDict (newname.c_str());
    if (!dict)
      dict = TClassTable::GetDict (classname);
    if (dict) {
      (dict)();
      return gROOT->GetClass (newname.c_str(), kFALSE, silent);
    }
#endif
  }

  return 0;
}


/**
 * @brief Look up a class by @c type_info.
 * @param typeinfo The @c type_info of the class to find.
 * @param load If true, enable autoloading.
 * @param silent If true, suppress messages.
 */
TClass* StdHackGenerator::GetClass (const std::type_info& typeinfo,
                                    Bool_t load,
                                    Bool_t silent)
{
  // Just get the name and pass to the other overload.
  return GetClass (typeinfo.name(), load, silent);
}


/**
 * @brief Look up a class by name.
 * @param classname The name of the class to find.
 * @param load If true, enable autoloading.
 */
TClass* StdHackGenerator::GetClass (const char* classname, Bool_t load)
{
  return GetClass (classname, load, kFALSE);
}


/**
 * @brief Look up a class by @c type_info.
 * @param typeinfo The @c type_info of the class to find.
 * @param load If true, enable autoloading.
 */
TClass* StdHackGenerator::GetClass(const std::type_info& typeinfo, Bool_t load)
{
  return GetClass (typeinfo, load, kFALSE);
}


//***********************************************************************
//  Dummy methods required by root.
//


TClass* StdHackGenerator::Class()
{
  if (!fgIsA)
    fgIsA = TClass::GetClass ("RootUtils::StdHackGenerator");
  return fgIsA;
}


#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
void StdHackGenerator::ShowMembers (TMemberInspector& R__insp)
{
  TClassGenerator::ShowMembers (R__insp);
}
#endif


void StdHackGenerator::Streamer (TBuffer& b)
{
  TClassGenerator::Streamer (b);
}


#if ROOT_VERSION_CODE >= ROOT_VERSION(6,1,0) || (ROOT_VERSION_CODE>=ROOT_VERSION(5,34,22) && ROOT_VERSION_CODE<ROOT_VERSION(6,0,0))
atomic_TClass_ptr StdHackGenerator::fgIsA;
#else
TClass* StdHackGenerator::fgIsA = 0;
#endif


} // namespace RootUtils
