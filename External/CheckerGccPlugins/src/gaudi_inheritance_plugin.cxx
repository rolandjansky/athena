// $Id$
/**
 * @file CheckerGccPlugins/src/gaudi_inheritance_plugin.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2014
 * @brief gcc plugin to check for obsolete Gaudi inheritance.
 *
 * This plugin warns about any classes which directly derive from
 * one of the classes Algorithm, AlgTool, or Service, suggesting
 * to use the AthenaBaseComps versions instead.
 *
 * The AthenaBaseComps classes themselves (AthAlgorithm, AthAlgTool,
 * AthService) are excluded from the check.
 */


#include <iostream>
#include "checker_gccplugins.h"
#include "tree.h"
#include "cp/cp-tree.h"
#include "diagnostic.h"
#if defined(GCC_VERSION) && (GCC_VERSION >= 4009)
# include "print-tree.h"
#endif


namespace {


/**
 * @brief Helper to format the name of the type @c t.
 */
const char* type_name (tree t)
{
#if defined(GCC_VERSION) && (GCC_VERSION >= 4009)
  unsigned int save = flag_sanitize;
  flag_sanitize = 0;
#endif
  const char* ret = type_as_string (t,
                                    TFF_PLAIN_IDENTIFIER +
                                    //TFF_UNQUALIFIED_NAME +
                                    TFF_NO_OMIT_DEFAULT_TEMPLATE_ARGUMENTS);
#if defined(GCC_VERSION) && (GCC_VERSION >= 4009)
  flag_sanitize = save;
#endif
  return ret;
}


/**
 * @brief Gaudi inheritance checker.  Called on type definitions.
 * @param gcc_data The type that was defined.
 */
void type_callback (void* gcc_data, void* /*user_data*/)
{
  const char* url = "<https://twiki.cern.ch/twiki/bin/view/AtlasComputing/ImprovingSoftware> and <https://twiki.cern.ch/twiki/bin/view/AtlasComputing/AthenaBaseComps>";

  // Select complete named struct/class types.
  tree t = (tree)gcc_data;
  tree tt = t;//TYPE_MAIN_VARIANT(t);
  if (TREE_CODE(tt) != RECORD_TYPE ||
      TYPE_ANONYMOUS_P(tt) ||
      !COMPLETE_TYPE_P(tt))
  {
    return;
  }

  // Skip if no base info.
  tree binfo = TYPE_BINFO(tt);
  if (!binfo)
    return;

  // Core classes for which we should skip this test.
  std::string name = type_name(tt);
  if (name == "AthAlgorithm" ||
      name == "AthAlgTool" ||
      name == "AthService" ||
      name == "SegMemSvc" ||
      name == "ActiveStoreSvc" ||
      name == "StoreGateSvc" ||
      name == "SGImplSvc" ||
      name == "SG::HiveMgrSvc" ||
      name == "AddressProviderSvc" ||
      name == "Algtest" ||
      name == "Algtooltest" ||
      name == "Servtest" ||
      name == "ClassIDSvc" ||
      name == "TrigMessageSvc")
    return;
  if (name.substr (0, 12) == "__shadow__::")
    return;

  // Loop over direct base classes.
  int n_baselinks = BINFO_N_BASE_BINFOS (binfo);
  for (int i=0; i < n_baselinks; i++) {
    tree base_binfo = BINFO_BASE_BINFO(binfo, i);
    if (BINFO_TYPE(base_binfo)) {
      std::string bname = type_name (BINFO_TYPE (base_binfo));

      // Warn if we're deriving directly from one of the Gaudi
      // component base classes.

      if (bname == "Algorithm") {
        location_t loc = DECL_SOURCE_LOCATION(TYPE_MAIN_DECL(tt));
        warning_at (loc, 0, 
                    "%<%D%> derives directly from %<Algorithm%>; "
                    "should derive from %<AthAlgorithm%> instead and "
                    "use its methods.  (See %s.)",
                    tt, url);
      }

      else if (bname == "AlgTool") {
        location_t loc = DECL_SOURCE_LOCATION(TYPE_MAIN_DECL(tt));
        warning_at (loc, 0, 
                    "%<%D%> derives directly from %<AlgTool%>; "
                    "should derive from %<AthAlgTool%> instead and "
                    "use its methods.  (See %s.)",
                    tt, url);
      }

      else if (bname == "Service") {
        location_t loc = DECL_SOURCE_LOCATION(TYPE_MAIN_DECL(tt));
        warning_at (loc, 0, 
                    "%<%D%> derives directly from %<Service%>; "
                    "should derive from %<AthService%> instead and "
                    "use its methods.  (See %s.)",
                    tt, url);
      }
    }
  }
}


} // Anonymous namespace


/**
 * @brief Register the Gaudi inheritance checker.  Called on type definitions.
 */
void init_gaudi_inheritance_checker (plugin_name_args* plugin_info)
{
  register_callback (plugin_info->base_name,
                     PLUGIN_FINISH_TYPE,
                     type_callback,
                     NULL);
}
