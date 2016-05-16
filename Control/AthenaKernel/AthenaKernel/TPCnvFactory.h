// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TPCnvFactory.h 707243 2015-11-11 18:53:26Z ssnyder $

/**
 * @file  AthenaKernel/TPCnvFactory.h
 * @author scott snyder, S. Binet
 * @brief Set up plugin loading of TP converters.
 *
 * The definitions here are used to allow loading the TP converters directly
 * via the Gaudi plugin mechanism.
 *
 * Usage
 * =====
 *
 * In a TPCnv package, create a source file with the same name as the package,
 * add add this to your requirements file:
 *
 *@code
 *  apply_pattern tpcnv_library
 @endcode
 *
 * That file should contain a declaration like this for each TP converter:
 *
 *@code
 * #include "AthenaKernel/TPCnvFactory.h"
 * #include "FooTPCnv/FooCnv_p2.h"
 * DECLARE_TPCNV_FACTORY(FooCnv_p2,
 *                       Foo,
 *                       Foo_p2,
 *                       Athena::TPCnvVers::Current)
 @endcode
 *
 * The first three arguments should be the converter class, the transient class,
 * and the persistent class, respectively.  The fourth argument should be
 * @c Athena::TPCnvVers::Current if this is the most recent version of the
 * converter.  You should also declare previous versions of the converter,
 * using @c Old:
 *
 *@code
 * #include "FooTPCnv/FooCnv_p1.h"
 * DECLARE_TPCNV_FACTORY(FooCnv_p1,
 *                       Foo,
 *                       Foo_p1,
 *                       Athena::TPCnvVers::Old)
 @endcode
 *
 * Sometimes you need to use a different converter class for AthenaROOTAccess.
 * In that case, also use @c DECLARE_ARATPCNV_FACTORY:
 *
 *@code
 * #include "FooTPCnv/FooARACnv_p2.h"
 * DECLARE_ARATPCNV_FACTORY(FooARACnv_p2,
 *                          Foo,
 *                          Foo_p2,
 *                          Athena::TPCnvVers::Current)
 @endcode
 *
 * There is a separate macro @c DECLARE_TRIGTPCNV_FACTORY to allow declaring
 * separate converters to use for the trigger.
 *
 * Finally, in some cases you may need to use a different name for the
 * plugin than the C++ class name.  This is generally the case if the
 * converter class is actually a typedef.  In that case, use
 * @c DECLARE_NAMED_TPCNV_FACTORY:
 *
 *@code
 * #include "FooTPCnv/BarCnv_p2.h"
 * // BarCnv_p2 is actually a typedef.
 * DECLARE_NAMED_ARATPCNV_FACTORY(BarCnv_p2,
 *                                BarCnv_p2,
 *                                Bar,
 *                                Bar_p2,
 *                                Athena::TPCnvVers::Current)
 @endcode
 *
 * There is also a @c DECLARE_NAMED_ARATPCNV_FACTORY.
 *
 * Implementation
 * ==============
 *
 * The implementation is very simple.  Let the converter, transient,
 * and persistent types be C, T, and P, respectively.  We always make
 * plugin entries for C and _PERS_P.  If the converter is current,
 * we also make an entry for _TRANS_T.  If the converter is for ARA,
 * we add _ARA t the front of the pers and trans names; and similarly
 * _TRIG for trigger-specific conversions.
 *
 * Thus, to find the correct converter for a transient class, we look up
 * _TRANS_T; and for a persistent class we look up _PERS_P.  For ARA,
 * we first try the look up with _ARA in front of those names.
 * To get the transient class name corresponding to a persistent class,
 * we create an instance of the converter and use the @c transientTInfo
 * interface.  No registry is needed beyond the existing Gaudi plugin registry.
 *
 * One limitation: if you have multiple transient classes that use the
 * same persistent class, you'll get warnings from the plugin service.
 */


#ifndef ATHENAKERNEL_TPCNVFACTORY_H
#define ATHENAKERNEL_TPCNVFACTORY_H 1

#include "Gaudi/PluginService.h"

// Forward declaration
class ITPCnvBase;

namespace Athena {
  struct TPCnvVers {
    enum Value {
      Old = 0,
      Current = 1
    };
  };

  struct TPCnvType {
    enum Value {
      Athena = 0,
      ARA  = 1,
      Trigger = 2
    };
  };
}


#define DO_ATHTPCNV_FACTORY_REGISTER_CNAME(name, serial) \
  _register_ ## _ ## serial


#ifdef __COVERITY__
// Disable this in coverity --- otherwise, coverity warns about
// 'same value on both sides' in the if statements.
#define DO_ATHTPCNV_PLUGINSVC_FACTORY_WITH_ID(type, id, trans_type, pers_type, is_last_version, cnv_type, signature, serial) 
#else
#define DO_ATHTPCNV_PLUGINSVC_FACTORY_WITH_ID(type, id, trans_type, pers_type, is_last_version, cnv_type, signature, serial) \
  namespace { \
    class DO_ATHTPCNV_FACTORY_REGISTER_CNAME(type, serial) { \
    public: \
      typedef type::Factory s_t;      \
      typedef ::Gaudi::PluginService::Details::Factory<type> f_t; \
      static s_t::FuncType creator() { return &f_t::create<s_t>; } \
      DO_ATHTPCNV_FACTORY_REGISTER_CNAME(type, serial) () { \
        using ::Gaudi::PluginService::Details::Registry; \
        std::string prefix; \
        if (cnv_type == Athena::TPCnvType::ARA) \
          prefix = "_ARA"; \
        else if (cnv_type == Athena::TPCnvType::Trigger)   \
          prefix = "_TRIG"; \
        Registry::instance().add<s_t, type>(id, creator()); \
        if (is_last_version == Athena::TPCnvVers::Current) \
          Registry::instance().add<s_t, type>(prefix + "_TRANS_" + #trans_type, creator()); \
        Registry::instance().add<s_t, type>(prefix + "_PERS_" + #pers_type, creator()); \
      } \
    } DO_ATHTPCNV_FACTORY_REGISTER_CNAME(s_ ## type, serial); \
  }
#endif

#define DO_ATHTPCNV_PLUGINSVC_FACTORY(type, trans_type, pers_type, is_last_version, cnv_type, signature, serial) \
  DO_ATHTPCNV_PLUGINSVC_FACTORY_WITH_ID(type, ::Gaudi::PluginService::Details::demangle<type>(), trans_type, pers_type, is_last_version, cnv_type, signature, serial)



#define ATHTPCNV_PLUGINSVC_FACTORY(type, trans_type, pers_type, is_last_version, signature) \
  DO_ATHTPCNV_PLUGINSVC_FACTORY(type, trans_type, pers_type, is_last_version, Athena::TPCnvType::Athena, signature, __LINE__)
#define ARATPCNV_PLUGINSVC_FACTORY(type, trans_type, pers_type, is_last_version, signature) \
  DO_ATHTPCNV_PLUGINSVC_FACTORY(type, trans_type, pers_type, is_last_version, Athena::TPCnvType::ARA, signature, __LINE__)
#define TRIGTPCNV_PLUGINSVC_FACTORY(type, trans_type, pers_type, is_last_version, signature) \
  DO_ATHTPCNV_PLUGINSVC_FACTORY(type, trans_type, pers_type, is_last_version, Athena::TPCnvType::Trigger, signature, __LINE__)

#define ATHTPCNV_PLUGINSVC_FACTORY_WITH_ID(type, id, trans_type, pers_type, is_last_version, signature) \
  DO_ATHTPCNV_PLUGINSVC_FACTORY_WITH_ID(type, id, trans_type, pers_type, is_last_version, Athena::TPCnvType::Athena, signature, __LINE__)
#define ARATPCNV_PLUGINSVC_FACTORY_WITH_ID(type, id, trans_type, pers_type, is_last_version, signature) \
  DO_ATHTPCNV_PLUGINSVC_FACTORY_WITH_ID(type, id, trans_type, pers_type, is_last_version, Athena::TPCnvType::ARA, signature, __LINE__)
#define TRIGTPCNV_PLUGINSVC_FACTORY_WITH_ID(type, id, trans_type, pers_type, is_last_version, signature) \
  DO_ATHTPCNV_PLUGINSVC_FACTORY_WITH_ID(type, id, trans_type, pers_type, is_last_version, Athena::TPCnvType::Trigger, signature, __LINE__)



//********************************************************************
// Macros that users should use.
//

#define DECLARE_TPCNV_FACTORY(x,trans_type,pers_type,is_last_version)   \
  ATHTPCNV_PLUGINSVC_FACTORY(x,trans_type,pers_type,is_last_version,ITPCnvBase*())

#define DECLARE_ARATPCNV_FACTORY(x,trans_type,pers_type,is_last_version)   \
  ARATPCNV_PLUGINSVC_FACTORY(x,trans_type,pers_type,is_last_version,ITPCnvBase*())

#define DECLARE_TRIGTPCNV_FACTORY(x,trans_type,pers_type,is_last_version)   \
  TRIGTPCNV_PLUGINSVC_FACTORY(x,trans_type,pers_type,is_last_version,ITPCnvBase*())

#define DECLARE_NAMED_TPCNV_FACTORY(x,n,trans_type,pers_type,is_last_version) \
  ATHTPCNV_PLUGINSVC_FACTORY_WITH_ID(x,std::string(#n), trans_type,pers_type, is_last_version, ITPCnvBase*())

#define DECLARE_NAMED_ARATPCNV_FACTORY(x,n,trans_type,pers_type,is_last_version) \
  ARATPCNV_PLUGINSVC_FACTORY_WITH_ID(x,std::string(#n), trans_type,pers_type, is_last_version, ITPCnvBase*())

#define DECLARE_NAMED_TRIGTPCNV_FACTORY(x,n,trans_type,pers_type,is_last_version) \
  TRIGTPCNV_PLUGINSVC_FACTORY_WITH_ID(x,std::string(#n), trans_type,pers_type, is_last_version, ITPCnvBase*())



#endif //> !ATHENAKERNEL_TPCNVFACTORY_H
