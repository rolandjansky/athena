/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// C/C++
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>

// Local
#include "VarHolder.h"

using namespace std;

namespace Prompt
{
  namespace Def
  {
    typedef std::map<Def::Var, std::string> VarMap;
    
    static VarMap gPromptVars;

    static int VarFlag = RegisterAllVars();

    static bool gDebugPromptVars = false;
  }
}

//======================================================================================================
bool Prompt::Def::RegisterVar(Var var, const std::string &name)
{
  for(const VarMap::value_type &v: gPromptVars) {
    if(name == v.second) {
      cout << "Def::RegisterVar - WARNING - variable with this name already exists: var=" << var << ", name=" << name << endl;
      return false;      
    }
  }

  pair<VarMap::iterator, bool> vit = gPromptVars.insert(VarMap::value_type(var, name));

  if(!vit.second) {
    cout << "Def::RegisterVar - WARNING - variable with this key already exists: var=" << var << ", name=" << name << endl;
    return false;
  }

  if(gDebugPromptVars) {
    cout << "Def::RegisterVar - DEBUG - add variable: " << var << ", name=" << name << endl;
  }

  return true;
}

//======================================================================================================
const std::vector<std::string>& Prompt::Def::GetAllVarNames()
{
  static vector<string> names;

  if(names.empty()) {
    for(const VarMap::value_type &v: gPromptVars) {
      names.push_back(v.second);
    }
  }

  return names;
}

//======================================================================================================
const std::vector<Prompt::Def::Var>& Prompt::Def::GetAllVarEnums()
{
  static vector<Prompt::Def::Var> enums;

  if(enums.empty()) {
    for(const VarMap::value_type &v: gPromptVars) {
      enums.push_back(v.first);
    }
  }

  return enums;
}

//======================================================================================================
std::string Prompt::Def::Convert2Str(Var var)
{
  const VarMap::const_iterator vit = gPromptVars.find(var);

  if(vit == gPromptVars.end()) {
    cout << "Def::Convert2Str - WARNING - unknown variable: " << var << endl;
    return "UNKNOWN";
  }
  
  return vit->second;
}

//======================================================================================================
Prompt::Def::Var Prompt::Def::Convert2Var(const std::string &var)
{
  for(const VarMap::value_type &v: gPromptVars) {
    if(var == v.second) {
      return v.first;
    }
  }

  if(gDebugPromptVars) {
    cout << "Def::Convert2Var - WARNING - unknown variable: " << var << endl;
  }

  return NONE;
}

//======================================================================================================
Prompt::Def::Var Prompt::Def::Convert2Var(uint32_t key)
{
  const vector<Var> &vars = GetAllVarEnums();
  
  //
  // Find matching enum by value
  //
  for(Var var: vars) {
    if(static_cast<uint32_t>(var) == key) {
      return var;
    }
  }

  cout << "Def::Convert2Var - WARNING - unknown key: " << key << endl;

  return NONE;
}

//======================================================================================================
std::vector<Prompt::Def::Var> Prompt::Def::ReadVars(const std::string &config)
{
  //
  // Read vector of variable names and convert to Var enums
  //
  vector<string> keys;
  Def::StringTok(keys, config, ", ");

  vector<Var> vars;  

  for(unsigned i = 0; i < keys.size(); ++i) {
    const Var var = Def::Convert2Var(keys.at(i));
    if(var != NONE) {
      vars.push_back(var);
    }
    else {
      cout << "Prompt::Def::ReadVars - unknown variable name: " << keys.at(i) << endl;
    }
  }

  return vars;
}

//======================================================================================================
std::vector<Prompt::Def::Var> Prompt::Def::ReadVectorVars(const std::vector<std::string> &keys)
{
  //
  // Read vector of variable names and convert to Var enums
  //
  std::vector<Var> vars;  

  for(unsigned i = 0; i < keys.size(); ++i) {
    const Var var = Def::Convert2Var(keys.at(i));
    if(var != NONE) {
      vars.push_back(var);
    }
    else {
      cout << "Prompt::Def::ReadVars - unknown variable name: " << keys.at(i) << endl;
    }
  }

  return vars;
}

//======================================================================================================
void Prompt::Def::PrintVarNames()
{
  const vector<Var> vars = Def::GetAllVarEnums();
  
  cout << "PrintCutNames - print " << vars.size() << " enum(s)" << endl;
  
  for(unsigned i = 0; i < vars.size(); ++i) {
    cout << "   " << setw(10) << vars.at(i) << ": " << Def::AsStr(vars.at(i)) << endl;
  }
}

//======================================================================================================
std::string Prompt::Def::AsStr(Var var)
{
  return Convert2Str(var);
}

//======================================================================================================
std::string Prompt::Def::AsStr(uint32_t key, double val)
{
  stringstream s;

  const Var var = Convert2Var(key);
  
  if(var != NONE) {
    s << AsStr(var) << ": " << val;
  }
  else {
    s <<       var  << ": " << val;
  }

  return s.str();
}

//======================================================================================================
void Prompt::Def::StringTok(std::vector<std::string>& ls,
			    const std::string& str,
			    const std::string& tok)
{
  //======================================================================
  // Split a long string into a set of shorter strings spliting along
  // divisions makers by the characters listed in the token string
  //======================================================================
  const string::size_type S = str.size();
  string::size_type  i = 0;  

  while (i < S) {
    // eat leading whitespace
    while (i < S && tok.find(str[i]) != string::npos) {
      ++i;
    }
    if (i == S) break;  // nothing left but WS
    
    // find end of word
    string::size_type  j = i+1;
    while (j < S && tok.find(str[j]) == string::npos) {
      ++j;
    }
    
    // add word
    ls.push_back(str.substr(i,j-i));
    
    // set up for next loop
    i = j+1;
  }  
}

//======================================================================================================
int Prompt::Def::RegisterAllVars()
{
  int result = 0;

  result += RegisterVar( NONE,            "NONE");
  result += RegisterVar( TrackJetNTrack,  "TrackJetNTrack");
  result += RegisterVar( sv1_ntkv,        "sv1_ntkv");
  result += RegisterVar( jf_ntrkv,        "jf_ntrkv");
  result += RegisterVar( sv1_jf_ntrkv,    "sv1_jf_ntrkv");
  result += RegisterVar( ip2,             "ip2");
  result += RegisterVar( ip2_cu,          "ip2_cu");
  result += RegisterVar( ip3,             "ip3");
  result += RegisterVar( ip3_cu,          "ip3_cu");
  result += RegisterVar( EtTopoCone20Rel, "EtTopoCone20Rel");
  result += RegisterVar( EtTopoCone30Rel, "EtTopoCone30Rel");
  result += RegisterVar( TopoEtCone30Rel, "TopoEtCone30Rel");
  result += RegisterVar( PtVarCone30Rel,  "PtVarCone30Rel");
  result += RegisterVar( DRlj,            "DRlj");
  result += RegisterVar( LepJetPtFrac,    "LepJetPtFrac");
  result += RegisterVar( PtFrac,          "PtFrac");
  result += RegisterVar( PtRel,           "PtRel");
  result += RegisterVar( DL1mu,           "DL1mu");
  result += RegisterVar( rnnip,           "rnnip");
  result += RegisterVar( MV2c10rnn,       "MV2c10rnn");
  result += RegisterVar( SV1,             "SV1");
  result += RegisterVar( JetF,            "JetF");
  
  return result;
}

