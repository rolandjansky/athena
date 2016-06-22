// $Id$
/**
 * @file CheckerGccPlugins/src/is_thread_safe.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2015
 * @brief Test to see if something has been declared as thread-safe.
 */


#include <unordered_set>
#include <unordered_map>
#include "checker_gccplugins.h"
#include "tree.h"
#include "diagnostic.h"
#if GCC_VERSION >= 4009
# include "print-tree.h"
#endif


namespace {


std::unordered_set<location_t> seen_loc;
void thread_safe_debug_finishdecl_callback (void* gcc_data, void* /*user_data*/)
{
  tree decl = (tree)gcc_data;
  location_t loc = DECL_SOURCE_LOCATION (decl);
  if (!loc) return;
  if (!seen_loc.insert (loc).second)
    return;
  if (lookup_attribute ("thread_safe_debug", DECL_ATTRIBUTES (decl)))
  {
    const char* flag = " not";
    if (CheckerGccPlugins::is_thread_safe (decl))
      flag = "";
    warning_at (DECL_SOURCE_LOCATION (decl), 0,
                "%<%D%> is%s marked thread-safe.", decl, flag);
    
  }
}


void thread_safe_debug_pregen_callback (void* gcc_data, void* user_data)
{
  tree ast = (tree)gcc_data;
  if (TREE_CODE (ast) == FUNCTION_DECL)
    thread_safe_debug_finishdecl_callback (gcc_data, user_data);
}


typedef std::unordered_map<std::string, bool> thread_safe_files_t;
thread_safe_files_t thread_safe_files;


std::unordered_set<std::string> stop_walk_dirs
{ "AtlasAnalysis",
    "AtlasConditions",
    "AtlasCore",
    "AtlasEvent",
    "AtlasHLT",
    "AtlasOffline,"
    "AtlasProduction",
    "AtlasReconstruction",
    "AtlasSimulation",
    "AtlasTrigger",
    "DetCommon",

    "AtlasTest",
    "Calorimeter",
    "Commission",
    "Control",
    "Database",
    "DataQuality",
    "DetectorDescription",
    "Event",
    "External",
    "ForwardDetectors",
    "Generators",
    "graphics",
    "InnerDetector",
    "LArCalorimeter",
    "LumiBlock",
    "MagneticField",
    "MuonSpectrometer",
    "PhysicsAnalysis",
    "Reconstruction",
    "Simulation",
    "TestBeam",
    "TileCalorimeter",
    "Tools",
    "Tracking",
    "Trigger",

    "boost",
    "root",
    "Eigen",
    "fastjet",
    "clhep",
    };


} // anonymous namespace


namespace CheckerGccPlugins {


bool is_thread_safe_dir (const std::string& dir, int nwalk = 5);
bool is_thread_safe_dir1 (const std::string& dir, int nwalk = 5)
{
  std::string flagfile = dir + "/ATLAS_THREAD_SAFE";
  if (access (flagfile.c_str(), R_OK) == 0)
    return true;

  std::string::size_type dpos = dir.rfind ('/');
  std::string dir2 = "..";
  std::string dname = dir;
  if (dpos != std::string::npos) {
    dir2 = dir.substr (0, dpos);
    dname = dir.substr (dpos+1, std::string::npos);
  }

  if (nwalk <= 0 || stop_walk_dirs.count (dname) > 0)
    return false;

  if (dname == "..")
    dir2 += "/../..";

  return is_thread_safe_dir (dir2, nwalk-1);
}


bool is_thread_safe_dir (const std::string& dir, int nwalk /*= 5*/)
{
  thread_safe_files_t::iterator it = thread_safe_files.find (dir);
  if (it != thread_safe_files.end()) return it->second;
  bool ret = is_thread_safe_dir1 (dir, nwalk);
  thread_safe_files[dir] = ret;
  return ret;
}


bool is_thread_safe_location (location_t loc)
{
  std::string file = LOCATION_FILE(loc);
  thread_safe_files_t::iterator it = thread_safe_files.find (file);
  if (it != thread_safe_files.end()) return it->second;

  std::string::size_type dpos = file.rfind ('/');
  std::string dir = ".";
  if (dpos != std::string::npos)
    dir = file.substr (0, dpos);

  bool ret = is_thread_safe_dir (dir);
  thread_safe_files[file] = ret;
  return ret;
}


/// Has DECL been declared thread-safe?
bool is_thread_safe (tree decl)
{
  // Check if the attribute is present directly.
  if (lookup_attribute ("thread_safe", DECL_ATTRIBUTES (decl)))
    return true;

  // If it's a function or class, check the containing function or class.
  if (TREE_CODE (decl) == FUNCTION_DECL  ||
      TREE_CODE (decl) == TYPE_DECL)
  {
    tree ctx = DECL_CONTEXT (decl);
    while (ctx && !SCOPE_FILE_SCOPE_P (ctx)) {
      if (TREE_CODE (ctx) == RECORD_TYPE) {
        if (lookup_attribute ("thread_safe", TYPE_ATTRIBUTES (ctx)))
          return true;
        if (is_thread_safe_location (DECL_SOURCE_LOCATION (TYPE_NAME (ctx))))
          return true;
        ctx = TYPE_CONTEXT (ctx);
      }

      else if (TREE_CODE (ctx) == FUNCTION_DECL) {
        if (lookup_attribute ("thread_safe", DECL_ATTRIBUTES (ctx)))
          return true;
        if (is_thread_safe_location (DECL_SOURCE_LOCATION (ctx)))
          return true;
        ctx = DECL_CONTEXT (ctx);
      }

      else
        break;
    }
  }

  // Check the file in which it was declared.
  if (is_thread_safe_location (DECL_SOURCE_LOCATION (decl)))
    return true;

  return false;
}


void handle_thread_safe_pragma (cpp_reader*)
{
  thread_safe_files[LOCATION_FILE (input_location)] = true;
}


} // namespace CheckerGccPlugins


void init_thread_safe_debug_checker (plugin_name_args* plugin_info)
{
  register_callback (plugin_info->base_name,
                     PLUGIN_FINISH_DECL,
                     thread_safe_debug_finishdecl_callback,
                     NULL);
  register_callback (plugin_info->base_name,
                     PLUGIN_PRE_GENERICIZE,
                     thread_safe_debug_pregen_callback,
                     NULL);
}
