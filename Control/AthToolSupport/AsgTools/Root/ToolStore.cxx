// ToolStore.cxx

#include "AsgTools/ToolStore.h"
#include <map>

using asg::ToolStore;
using asg::IAsgTool;
using std::string;

typedef std::map<string, IAsgTool*> ToolMap;

namespace {

ToolMap& tools() {
  static ToolMap map;
  return map;
}

}

//**********************************************************************

int ToolStore::put(IAsgTool* ptool) {
  if ( ptool == 0 ) return 1;
  string name = ptool->name();
  if ( name.size() == 0 ) return 2;
  if ( tools().find(name) != tools().end() ) return 3;
  tools()[name] = ptool;
  return 0;
}

//**********************************************************************

int ToolStore::put(IAsgTool* ptool, string name) {
  if ( ptool == 0 ) return 1;
  if ( name.size() == 0 ) return 4;
  ptool->setName(name);
  return put(ptool);
}

//**********************************************************************

IAsgTool* ToolStore::get(string name) {
  ToolMap::const_iterator itool = tools().find(name);
  if ( itool == tools().end() ) return 0;
  return itool->second;
}

//**********************************************************************
