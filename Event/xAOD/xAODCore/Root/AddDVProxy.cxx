/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODCore/AddDVProxy.h"
#include "CxxUtils/checker_macros.h"

// ROOT include(s):
#include <TClass.h>
#include <TError.h>
#include <TInterpreter.h>
#include <TROOT.h>
#include <TClassGenerator.h>
#include <TClassEdit.h>

// System include(s):
#include <mutex>


namespace {


/**
 * @brief Work around for some cling problems.
 *
 * In some cases, some dictionary load requests will fail if they are done
 * in the `wrong' order.  This is a cling bug (as of root 6.20.00), but
 * we can work around it here.  We add a TClassGenerator so that if,
 * for example, we're trying to load ElementLink<T>, we first load T.
 * And similarly for a few other Athena template classes.
 * This is observed to be sufficient to fix the order-dependent failures.
 * See ATR-22681.
 */
class xAODClassGenerator : public TClassGenerator
{
public:
  using TClassGenerator::GetClass;
  virtual TClass *GetClass(const char* classname, Bool_t load) override;
  virtual TClass *GetClass(const std::type_info& , Bool_t ) override
  { return nullptr; }

  // Protect against recursive invocation.
  static thread_local bool m_active;
  struct BeActive
  {
    BeActive() { m_active = true; }
    ~BeActive() { m_active = false; }
  };
     
};



thread_local bool xAODClassGenerator::m_active = false;


TClass* xAODClassGenerator::GetClass(const char* classname, Bool_t load)
{
  static const std::string prefs[] = {
    "SG::AuxTypeVectorFactory<",
    "DataLink<",
    "ElementLink<",
    "SG::BaseInfo<",
    "DataVector<",
  };
  if (m_active) return nullptr;
  for (const std::string& pref : prefs) {
    if (strncmp (classname, pref.c_str(), pref.size()) == 0) {
      TClassEdit::TSplitType s (classname);
      if (s.fElements.size() >= 2) {
        TClass::GetClass (s.fElements[1].c_str(), load);
        BeActive active;
        return TClass::GetClass (classname, load);
      }
    }
  }
  return nullptr;
}


xAODClassGenerator generator ATLAS_THREAD_SAFE;


} // anonymous namespace


namespace xAOD {

   /// The constructor of TDVCollectionProxy needs the dictionary of
   /// DataVector<xAOD::TDVCollectionProxyDummy> to be loaded already.
   /// This function makes sure that this is the case.
   ///
   void AddDVProxy::loadDictionaries() {

      // Enable library auto-loading. Only once per job.
      static std::once_flag libLoadFlag;
      std::call_once( libLoadFlag, []( TInterpreter& interpreter ) {

         // Enable library auto-loading.
         TClass::ReadRules();
         interpreter.LoadLibraryMap();
         interpreter.SetClassAutoloading( true );

         // Make sure that the minimal set of dictionaries are loaded:
         if( ! TClass::GetClass( "DataVector<xAOD::TDVCollectionProxyDummy>" ) ) {
            ::Error( "xAOD::AddDVProxy::loadDictionaries",
                     "Couldn't load the dictionary for "
                     "DataVector<xAOD::TDVCollectionProxyDummy>" );
         }

         // Install the cling workaround (see above).
         gROOT->AddClassGenerator (&generator);

      }, *gInterpreter );

      return;
   }

} // namespace xAOD
