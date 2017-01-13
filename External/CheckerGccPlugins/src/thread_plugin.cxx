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
#if GCC_VERSION >= 6000
    int preversep;
#endif
    expr = get_inner_reference (expr,
                                &pbitsize,
                                &pbitpos, &poffset,
                                &pmode, &punsignedp,
#if GCC_VERSION >= 6000
                                &preversep,
#endif
                                &pvolatilep
#if GCC_VERSION < 7000                                
                                , false
#endif
                                );
  }
  return expr;
}


bool has_attrib (tree decl, const char* attrname)
{
  return lookup_attribute (attrname, DECL_ATTRIBUTES (decl));
}


bool has_attrib (function* fun, const char* attrname)
{
  return lookup_attribute (attrname, DECL_ATTRIBUTES (fun->decl));
}


bool has_thread_safe_attrib (tree decl)
{
  return has_attrib (decl, "thread_safe");
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


void check_mutable (tree expr, gimplePtr stmt, function* fun, const char* what)
{
  while (true) {
    switch (TREE_CODE (expr)) {
    case COMPONENT_REF:
      if (TYPE_READONLY(TREE_TYPE(TREE_OPERAND(expr, 0))) &&
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
void check_direct_static_use (gimplePtr stmt, function* fun)
{
  if (has_attrib (fun, "not_reentrant"))
    return;

  size_t nop = gimple_num_ops (stmt);
  for (size_t i = 0; i < nop; i++) {
    tree op = gimple_op (stmt, i);

    tree optest = get_inner (op);
    if (static_p (optest)) {
        warning_at (gimple_location (stmt), 0,
                    "Use of static expression %<%E%> within function %<%D%> may not be thread-safe.",
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
//   OP: operand to check
//   STMT: gimple statement being checked
//   FUN: function being checked
void check_assign_address_of_static (tree op, gimplePtr stmt, function* fun)
{
  if (op && TREE_CODE (op) == ADDR_EXPR) {
    while (op && TREE_CODE (op) == ADDR_EXPR)
      op = TREE_OPERAND (op, 0);
  }

  tree optest = get_inner (op);
  if (static_p (optest) && !has_attrib (fun, "not_reentrant")) {
    warning_at (gimple_location (stmt), 0,
                "Pointer or reference bound to static expression %<%E%> within function %<%D%>; may not be thread-safe.",
                op, fun->decl);
    if (DECL_P (optest)) {
      inform (DECL_SOURCE_LOCATION (optest),
              "Declared here:");
    }
    CheckerGccPlugins::inform_url (gimple_location (stmt), url);
  }
}


// Test to see if a pointer value comes directly or indirectly from
// a const pointer function argument.
tree pointer_is_const_arg (tree val)
{
  //fprintf (stderr, "pointer_is_const_arg_p\n");
  //debug_tree(val);
  tree valtest = get_inner (val);
  tree valtype = TREE_TYPE (valtest);
  if (!POINTER_TYPE_P(valtype) || !TYPE_READONLY (TREE_TYPE (valtype)))
    return NULL_TREE;

  if (TREE_CODE (val) == ADDR_EXPR)
    val = TREE_OPERAND (val, 0);
  if (TREE_CODE (val) == COMPONENT_REF)
    val = get_inner (val);
  if (TREE_CODE (val) == MEM_REF)
    val = TREE_OPERAND (val, 0);

  if (TREE_CODE (val) != SSA_NAME) return NULL_TREE;
  if (SSA_NAME_VAR (val) && TREE_CODE (SSA_NAME_VAR (val)) == PARM_DECL) return val;

  gimplePtr stmt = SSA_NAME_DEF_STMT (val);
  if (!stmt) return NULL_TREE;
  //debug_gimple_stmt (stmt);
  //fprintf (stderr, "code %s\n", get_tree_code_name(gimple_expr_code(stmt)));
  
  if (is_gimple_assign (stmt) && (gimple_expr_code(stmt) == VAR_DECL ||
                                  gimple_expr_code(stmt) == PARM_DECL ||
                                  gimple_expr_code(stmt) == POINTER_PLUS_EXPR ||
                                  gimple_expr_code(stmt) == ADDR_EXPR))
  {
    //fprintf (stderr, "recurse\n");
    return pointer_is_const_arg (gimple_op(stmt, 1));
  }
  else if (gimple_code (stmt) == GIMPLE_PHI) {
    size_t nop = gimple_num_ops (stmt);
    for (size_t i = 0; i < nop; i++) {
      tree op = gimple_op (stmt, i);
      tree ret = pointer_is_const_arg (op);
      if (ret) return ret;
    }
  }
  return NULL_TREE;
}


void warn_about_discarded_const (tree expr, gimplePtr stmt, function* fun)
{
  tree parm = pointer_is_const_arg (expr);
  if (parm) {
    if (has_attrib (fun, "argument_not_const_thread_safe")) return;
    if (expr == parm) {
      warning_at (gimple_location (stmt), 0,
                  "Const discarded from expression %<%E%> within function %<%D%>; may not be thread-safe.",
                  expr, fun->decl);
    }
    else {
      warning_at (gimple_location (stmt), 0,
                  "Const discarded from expression %<%E%> (deriving from parameter %<%E%>) within function %<%D%>; may not be thread-safe.",
                  expr, parm, fun->decl);
    }
  }
  else {
    if (has_attrib (fun, "not_const_thread_safe")) return;
    warning_at (gimple_location (stmt), 0,
                "Const discarded from expression %<%E%> within function %<%D%>; may not be thread-safe.",
                expr, fun->decl);
  }
  CheckerGccPlugins::inform_url (gimple_location (stmt), url);
}


// Called when LHS does not have const type.
void check_discarded_const (tree op, tree lhs, gimplePtr stmt, function* fun)
{
  tree optest = get_inner (op);
  tree optype = TREE_TYPE (optest);
  if (POINTER_TYPE_P(optype) && TYPE_READONLY (TREE_TYPE (optype))) {
    if (TREE_CODE (lhs) == SSA_NAME &&
        SSA_NAME_VAR (lhs) &&
        has_thread_safe_attrib (SSA_NAME_VAR (lhs)))
    {
      // Allow const_cast if LHS is explicitly marked thread_safe.
    }
    else {
      warn_about_discarded_const (op, stmt, fun);
    }
  }
}


void check_assignments (gimplePtr stmt, function* fun)
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
  bool lhs_const = TYPE_READONLY (TREE_TYPE (lhs_type));

  // Does RHS point to something static, or is it const or mutable?
  for (size_t i = 1; i < nop; i++) {
    tree op = gimple_op (stmt, i);

    // Check for discarding const if LHS is non-const.
    if (!lhs_const)
      check_discarded_const (op, lhs, stmt, fun);
    
    check_assign_address_of_static (op, stmt, fun);

    if (op && TREE_CODE (op) == ADDR_EXPR) {
      while (op && TREE_CODE (op) == ADDR_EXPR)
        op = TREE_OPERAND (op, 0);
    }

    if (!lhs_const)
      check_mutable (op, stmt, fun, "Taking non-const reference to mutable field");
  }
}


void check_thread_safe_call (tree fndecl, gimplePtr stmt, function* fun)
{
  if (check_thread_safety_p (fndecl)) return;
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
//   ARG_TYPE: type of argument
//   ARG: argument to test
//   STMT: gimple statement being checked
//   FUN: function being checked
void check_pass_static_by_call (tree arg_type, tree arg, gimplePtr stmt, function* fun)
{
  if (!POINTER_TYPE_P (arg_type)) return;

  if (arg && TREE_CODE (arg) == ADDR_EXPR) {
    while (arg && TREE_CODE (arg) == ADDR_EXPR)
      arg = TREE_OPERAND (arg, 0);
  }
  tree argtest = get_inner (arg);

  if (static_p (argtest) && !has_attrib (fun, "not_reentrant")) {
    warning_at (gimple_location (stmt), 0,
                "Static expression %<%E%> passed to pointer or reference function argument within function %<%D%>; may not be thread-safe.",
                arg, fun->decl);
    if (DECL_P (argtest)) {
      inform (DECL_SOURCE_LOCATION (argtest),
              "Declared here:");
    }
    CheckerGccPlugins::inform_url (gimple_location (stmt), url);
  }
}


// Check for discarding const from a pointer/ref in a function call.
//   ARG_TYPE: type of argument
//   ARG: argument to test
//   STMT: gimple statement being checked
//   FUN: function being checked
void check_discarded_const_in_funcall (tree arg_type, tree arg, gimplePtr stmt, function* fun)
{
  bool lhs_const = TYPE_READONLY (TREE_TYPE (arg_type));

  tree argtest = get_inner (arg);

  tree ctest = TREE_TYPE (argtest);
  if (POINTER_TYPE_P (ctest))
    ctest = TREE_TYPE (ctest);

  if (!lhs_const && ctest && TYPE_READONLY (ctest))
  {
    warn_about_discarded_const (arg, stmt, fun);
  }
}


// Check for discarding const from the return value of a function.
//   STMT: gimple statement being checked
//   FUN: function being checked
void check_discarded_const_from_return (gimplePtr stmt, function* fun)
{
  tree lhs = gimple_call_lhs (stmt);
  if (!lhs) return;
  tree lhs_type = TREE_TYPE (lhs);
  if (!POINTER_TYPE_P (lhs_type)) return;
  bool lhs_const = TYPE_READONLY (TREE_TYPE (lhs_type));
  if (lhs_const) return;

  if (TREE_CODE (lhs) == SSA_NAME &&
      SSA_NAME_VAR (lhs) &&
      has_thread_safe_attrib (SSA_NAME_VAR (lhs)))
    return;

  tree rhs_type = gimple_expr_type (stmt);
  if (POINTER_TYPE_P (rhs_type))
    rhs_type = TREE_TYPE (rhs_type);

  if (rhs_type && TYPE_READONLY (rhs_type))
  {
    warn_about_discarded_const (gimple_call_fn(stmt), stmt, fun);
  }
}


void check_calls (gimplePtr stmt, function* fun)
{
  if (gimple_code (stmt) != GIMPLE_CALL) return;

  check_discarded_const_from_return (stmt, fun);

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

    if (arg && TREE_CODE (arg) == ADDR_EXPR) {
      while (arg && TREE_CODE (arg) == ADDR_EXPR)
        arg = TREE_OPERAND (arg, 0);
    }

    check_discarded_const_in_funcall (arg_type, arg, stmt, fun);
    check_pass_static_by_call (arg_type, arg, stmt, fun);
  }
}


unsigned int thread_pass::thread_execute (function* fun)
{
  if (!check_thread_safety_p (fun->decl))
    return 0;

  basic_block bb;
  FOR_EACH_BB_FN(bb, fun) {
    for (gimple_stmt_iterator si = gsi_start_bb (bb); 
         !gsi_end_p (si);
         gsi_next (&si))
    {
      gimplePtr stmt = gsi_stmt (si);
      //debug_gimple_stmt (stmt);

      check_direct_static_use (stmt, fun);
      check_assignments (stmt, fun);
      check_calls (stmt, fun);
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
    if (!check_thread_safety_p(decl)) return;
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
