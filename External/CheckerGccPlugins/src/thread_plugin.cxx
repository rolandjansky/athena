// $Id$
/**
 * @file CheckerGccPlugins/src/thread_plugin.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2015
 * @brief Check for possible thread-safety violations.
 */

#include <unordered_set>
#include "checker_gccplugins.h"
#include "tree.h"
#include "function.h"
#include "basic-block.h"
#include "coretypes.h"
#include "is-a.h"
#include "predict.h"
#include "internal-fn.h"
#include "tree-ssa-alias.h"
#if GCC_VERSION >= 4009
# include "gimple-expr.h"
#endif
#include "gimple.h"
#if GCC_VERSION >= 4009
# include "gimple-iterator.h"
# include "tree-ssa-loop.h"
#endif
#include "cp/cp-tree.h"
#include "diagnostic.h"
#if GCC_VERSION >= 4009
# include "context.h"
#endif
#include "tree-pass.h"
#include "gimple-pretty-print.h"
#if GCC_VERSION >= 4009
# include "print-tree.h"
# include "tree-cfg.h"
#endif
#include "cfgloop.h"
#include "tree-ssa-operands.h"
#if GCC_VERSION >= 4009
# include "tree-phinodes.h"
# include "gimple-ssa.h"
# include "ssa-iterators.h"
#else
# include "tree-flow.h"
#endif


using namespace CheckerGccPlugins;


namespace {


// stdlib functions assumed to be safe unless listed here.
// This list collected from those tagged as MT-Unsafe on the linux man pages.
const std::unordered_set<std::string> unsafe_stdlib_fns {
  "bindresvport", // glibc < 2.17
  "crypt",
  "cuserid",
  "drand48",
  "ecvt",
  "encrypt",
  "endfsent",
  "endgrent",
  "endpwent",
  "endttyent",
  "endusershell",
  "erand48",
  "ether_aton",
  "ether_ntoa",
  "exit",
  "fcloseall",
  "fcvt",
  "fmtmsg", // for glibc < 2.16
  "gamma",
  "gammaf",
  "gammal",
  "getdate",
  "getfsent",
  "getfsfile",
  "getfsspec",
  "getgrent",
  "getgrgid",
  "getgrname",
  "getlogin",
  "getlogin_r",
  "getopt",
  "getopt_long",
  "getopt_long_only",
  "getpass",
  "getpwent",
  "getpwnam",
  "getpwuid",
  "getttyent",
  "getttynam",
  "getusershell",
  "hcreate",
  "hdestroy",
  "hsearch",
  "jrand48",
  "l64a",
  "lcong48",
  "localeconv",
  "lrand48",
  "mblen",
  "mbrlen",
  "mbrtowc",
  "mbtowc",
  "mrand48",
  "mtrace",
  "muntrace",
  "nrand48",
  "ptsname",
  "putenv",
  "qecvt",
  "qfcvt",
  "re_comp",
  "re_exec",
  "readdir",
  "rexec",
  "rexec_af",
  "seed48",
  "setenv",
  "setfsent",
  "setgrent",
  "setkey",
  "setpwent",
  "setttyent",
  "setusershell",
  "siginterrupt",
  "srand48",
  "strerror",
  "strtok",
  "tmpnam",
  "ttyname",
  "ttyslot",
  "unsetenv",
  "wctomb",

  "asctime",
  "ctime",
  "gmtime",
  "localtime",
};


const char* url = "<https://twiki.cern.ch/twiki/bin/view/AtlasComputing/CheckerGccPlugins#thread_plugin>";


#if GCC_VERSION >= 4009

const pass_data thread_pass_data =
{
  GIMPLE_PASS, /* type */
  "thread", /* name */
  0, /* optinfo_flags */
#if GCC_VERSION < 5000
  false, /* has_gate */
  true, /* has_execute */
#endif
  TV_NONE, /* tv_id */
  0, /* properties_required */
  0, /* properties_provided */
  0, /* properties_destroyed */
  0, /* todo_flags_start */
  0  /* todo_flags_finish */
};


class thread_pass : public gimple_opt_pass
{
public:
  thread_pass (gcc::context* ctxt)
    : gimple_opt_pass (thread_pass_data, ctxt)
  {}

#if GCC_VERSION >= 5000
  virtual unsigned int execute (function* fun) override
  { return thread_execute(fun); }
#else
  virtual unsigned int execute () override
  { return thread_execute (cfun); }
#endif

  unsigned int thread_execute (function* fun);

  virtual opt_pass* clone() override { return new thread_pass(*this); }
};

#else

class thread_pass
{
public:
  static unsigned int execute ()
  { return thread_execute (cfun); }

  static unsigned int thread_execute (function* fun);
};


opt_pass thread_pass_data =
{
  GIMPLE_PASS, /* type */
  "thread", /* name */
  0, /* optinfo_flags */
  0, /* gate */
  thread_pass::execute, /* execute */
  0, /* sub */
  0, /* next */
  0, /* static_pass_number */
  TV_NONE, /* tv_id */
  0, /* properties_required */
  0, /* properties_provided */
  0, /* properties_destroyed */
  0, /* todo_flags_start */
  0  /* todo_flags_finish */
};


#endif


// If EXPR is a COMPONENT_REF, array reference, etc, return the complete
// object being referenced.
tree get_inner (tree expr)
{
  if (expr && handled_component_p (expr)) {
    HOST_WIDE_INT pbitsize, pbitpos;
    tree poffset;
    machine_mode pmode;
    int punsignedp, pvolatilep;
    expr = get_inner_reference (expr,
                                &pbitsize,
                                &pbitpos, &poffset,
                                &pmode, &punsignedp,
                                &pvolatilep, false);
  }
  return expr;
}


bool has_thread_safe_attrib (tree decl)
{
  return lookup_attribute ("thread_safe", DECL_ATTRIBUTES (decl));
}


// Does EXPR represent a non-const static object
// (that's not marked as thread-safe)?
bool static_p (tree expr)
{
  if (!expr) return false;
  if (!TREE_STATIC (expr)) return false;
  if (TREE_CONSTANT (expr) || TREE_READONLY (expr)) return false;

  if (DECL_P (expr)) {
    if (DECL_THREAD_LOCAL_P (expr)) return false;
    if (has_thread_safe_attrib (expr)) return false;
  }

  return true;
}


void check_mutable (tree expr, gimple stmt, function* fun, const char* what)
{
  while (true) {
    switch (TREE_CODE (expr)) {
    case COMPONENT_REF:
      if (TREE_READONLY(TREE_TYPE(TREE_OPERAND(expr, 0))) &&
          DECL_MUTABLE_P (TREE_OPERAND (expr, 1)))
      {
        if (has_thread_safe_attrib (TREE_OPERAND (expr, 1))) return;
        warning_at (gimple_location (stmt), 0,
                    "%s %<%E%> within thread-safe function %<%D%>; may not be thread-safe.",
                    what, TREE_OPERAND(expr, 1), fun->decl);
        CheckerGccPlugins::inform_url (gimple_location (stmt), url);
        return;
      }
      expr = TREE_OPERAND (expr, 0);
      break;

    case ARRAY_REF:
    case ARRAY_RANGE_REF:
    case BIT_FIELD_REF:
    case REALPART_EXPR:
    case IMAGPART_EXPR:
    case VIEW_CONVERT_EXPR:
      expr = TREE_OPERAND (expr, 0);
      break;

    default:
      return;
    }
  }
}


// Check for direct use of a static value.
void check_direct_static_use (gimple stmt, function* fun)
{
  size_t nop = gimple_num_ops (stmt);
  for (size_t i = 0; i < nop; i++) {
    tree op = gimple_op (stmt, i);
    //debug_tree (op);

    tree optest = get_inner (op);
    if (static_p (optest)) {
        warning_at (gimple_location (stmt), 0,
                    "Use of static expression %<%E%> within thread-safe function %<%D%> may not be thread-safe.",
                    op, fun->decl);
        if (DECL_P (optest)) {
          inform (DECL_SOURCE_LOCATION (optest),
                  "Declared here:");
        }
        CheckerGccPlugins::inform_url (gimple_location (stmt), url);
    }
  }
}


// Check for assigning from an address of a static object
// into a pointer/reference, or for discarding const.
void check_assign_address_of_static (gimple stmt, function* fun)
{
  if (gimple_code (stmt) != GIMPLE_ASSIGN) return;
  size_t nop = gimple_num_ops (stmt);
  if (nop < 2) return;

  tree lhs = gimple_op (stmt, 0);
  if (!lhs) return;

  check_mutable (lhs, stmt, fun, "Setting mutable field");

  // Is the LHS a pointer/ref?
  tree lhs_type = TREE_TYPE (lhs);
  if (!POINTER_TYPE_P (lhs_type)) return;
  bool lhs_const = TREE_READONLY (TREE_TYPE (lhs_type));

  // Does RHS point to something static, or is it const or mutable?
  for (size_t i = 1; i < nop; i++) {
    tree op = gimple_op (stmt, i);

    // Check for discarding const if LHS is non-const.
    if (!lhs_const)
    {
      tree optest = get_inner (op);
      tree optype = TREE_TYPE (optest);
      if (POINTER_TYPE_P(optype) && TREE_READONLY (TREE_TYPE (optype))) {
        if (TREE_CODE (lhs) == SSA_NAME &&
            SSA_NAME_VAR (lhs) &&
            has_thread_safe_attrib (SSA_NAME_VAR (lhs)))
        {
          // Allow const_cast if LHS is explicitly marked thread_safe.
        }
        else {
          warning_at (gimple_location (stmt), 0,
                      "Const discarded from expression %<%E%> within thread-safe function %<%D%>; may not be thread-safe.",
                      op, fun->decl);
          CheckerGccPlugins::inform_url (gimple_location (stmt), url);
        }
      }
    }
    
    if (op && TREE_CODE (op) == ADDR_EXPR) {
      while (op && TREE_CODE (op) == ADDR_EXPR)
        op = TREE_OPERAND (op, 0);
    }

    if (!lhs_const)
      check_mutable (op, stmt, fun, "Taking non-const reference to mutable field");

    {
      tree optest = get_inner (op);
      if (static_p (optest)) {
        warning_at (gimple_location (stmt), 0,
                    "Pointer or reference bound to static expression %<%E%> within thread-safe function %<%D%>; may not be thread-safe.",
                    op, fun->decl);
        if (DECL_P (optest)) {
          inform (DECL_SOURCE_LOCATION (optest),
                  "Declared here:");
        }
        CheckerGccPlugins::inform_url (gimple_location (stmt), url);
      }
    }
  }
}


void check_thread_safe_call (tree fndecl, gimple stmt, function* fun)
{
  if (is_thread_safe (fndecl)) return;
  std::string fnname = decl_as_string (fndecl, TFF_SCOPE + TFF_NO_FUNCTION_ARGUMENTS);

  location_t loc = DECL_SOURCE_LOCATION (fndecl);
  if (loc != UNKNOWN_LOCATION) {
    const char* file = LOCATION_FILE (loc);
    if (startswith (file, "/usr/include/")) {
      // Assume functions from the system library are ok unless we know
      // they aren't.
      if (unsafe_stdlib_fns.count (fnname) == 0)
        return;
    }
  }
  
  warning_at (gimple_location (stmt), 0,
              "Non-thread-safe function %<%D%> called from thread-safe function %<%D%>; may not be thread-safe.",
              fndecl, fun->decl);
  CheckerGccPlugins::inform_url (gimple_location (stmt), url);
}


// Check passing an address of a static object to a called function
// by non-const pointer/ref.
void check_pass_static_by_call (gimple stmt, function* fun)
{
  if (gimple_code (stmt) != GIMPLE_CALL) return;

  tree fndecl = gimple_call_fndecl (stmt);
  if (fndecl)
    check_thread_safe_call (fndecl, stmt, fun);

  unsigned nargs = gimple_call_num_args (stmt);
  if (nargs < 1) return;
  tree fntype = TREE_TYPE (gimple_call_fn (stmt));
  if (TREE_CODE (fntype) == POINTER_TYPE)
    fntype = TREE_TYPE (fntype);
  tree arg_types = TYPE_ARG_TYPES (fntype);
  
  for (unsigned i=0; arg_types && i < nargs; i++, arg_types = TREE_CHAIN(arg_types)) {
    tree arg_type = TREE_VALUE (arg_types);
    tree arg = gimple_call_arg (stmt, i);

    if (!POINTER_TYPE_P (arg_type)) continue;
    bool lhs_const = TREE_READONLY (TREE_TYPE (arg_type));

    if (arg && TREE_CODE (arg) == ADDR_EXPR) {
      while (arg && TREE_CODE (arg) == ADDR_EXPR)
        arg = TREE_OPERAND (arg, 0);
    }

    tree argtest = get_inner (arg);

    tree ctest = TREE_TYPE (argtest);
    if (POINTER_TYPE_P (ctest))
      ctest = TREE_TYPE (ctest);

    if (!lhs_const && ctest && TREE_READONLY (ctest))
    {
      warning_at (gimple_location (stmt), 0,
                  "Const discarded from expression %<%E%> in call from thread-safe function %<%D%>; may not be thread-safe.",
                  arg, fun->decl);
      CheckerGccPlugins::inform_url (gimple_location (stmt), url);
    }
    
    if (static_p (argtest)) {
      warning_at (gimple_location (stmt), 0,
                  "Static expression %<%E%> passed to pointer or reference function argument within thread-safe function %<%D%>; may not be thread-safe.",
                  arg, fun->decl);
      if (DECL_P (argtest)) {
        inform (DECL_SOURCE_LOCATION (argtest),
                "Declared here:");
      }
      CheckerGccPlugins::inform_url (gimple_location (stmt), url);
    }
  }
}


unsigned int thread_pass::thread_execute (function* fun)
{
  if (!is_thread_safe (fun->decl))
    return 0;

  basic_block bb;
  FOR_EACH_BB_FN(bb, fun) {
    for (gimple_stmt_iterator si = gsi_start_bb (bb); 
         !gsi_end_p (si);
         gsi_next (&si))
    {
      gimple stmt = gsi_stmt (si);
      //debug_gimple_stmt (stmt);

      check_direct_static_use (stmt, fun);
      check_assign_address_of_static (stmt, fun);
      check_pass_static_by_call (stmt, fun);
    }
  }
  
  return 0;
}


// Called after a type declaration.
// Check class/struct members here.
void thread_finishtype_callback (void* gcc_data, void* /*user_data*/)
{
  tree type = (tree)gcc_data;
  if (TREE_CODE (type) == RECORD_TYPE) {
    if (CP_AGGREGATE_TYPE_P (type)) return;
    tree decl = TYPE_NAME (type);
    if (!is_thread_safe(decl)) return;
    for (tree f = TYPE_FIELDS (type); f; f = DECL_CHAIN (f)) {
      if (TREE_CODE (f) == FIELD_DECL)
      {
        if (DECL_MUTABLE_P (f) && !has_thread_safe_attrib (f)) {
          warning_at (DECL_SOURCE_LOCATION(f), 0,
                      "Mutable member %<%D%> within thread-safe class %<%D%>; may not be thread-safe.",
                      f, decl);
          CheckerGccPlugins::inform_url (DECL_SOURCE_LOCATION(f), url);
        }
      }
      else if (TREE_CODE (f) == VAR_DECL)
      {
        if (TREE_STATIC(f) && !has_thread_safe_attrib (f)) {
          warning_at (DECL_SOURCE_LOCATION(f), 0,
                      "Static member %<%D%> within thread-safe class %<%D%>; may not be thread-safe.",
                      f, decl);
          CheckerGccPlugins::inform_url (DECL_SOURCE_LOCATION(f), url);
        }
      }
    }
  }
}


} // anonymous namespace


void init_thread_checker (plugin_name_args* plugin_info)
{
  struct register_pass_info pass_info = {
#if GCC_VERSION >= 4009
    new thread_pass(g),
#else
    &thread_pass_data,
#endif
    "ssa",
    0,
    PASS_POS_INSERT_AFTER
  };
  
  register_callback (plugin_info->base_name,
                     PLUGIN_PASS_MANAGER_SETUP,
                     NULL,
                     &pass_info);

  register_callback (plugin_info->base_name,
                     PLUGIN_FINISH_TYPE,
                     thread_finishtype_callback,
                     NULL);
}
