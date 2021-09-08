/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
      asg::msgUserCode::ANA_MSG_WARNING("Def::RegisterVar - WARNING - variable with this name already exists: var=" << var << ", name=" << name);
      return false;      
    }
  }

  pair<VarMap::iterator, bool> vit = gPromptVars.insert(VarMap::value_type(var, name));

  if(!vit.second) {
    asg::msgUserCode::ANA_MSG_WARNING("Def::RegisterVar - WARNING - variable with this key already exists: var=" << var << ", name=" << name);
    return false;
  }

  if(gDebugPromptVars) {
    asg::msgUserCode::ANA_MSG_INFO("Def::RegisterVar - DEBUG - add variable: " << var << ", name=" << name);
  }

  return true;
}

//======================================================================================================
Prompt::Def::Var Prompt::Def::RegisterDynamicVar(const std::string &name)
{
  unsigned last_key = Def::NONE;

  for(const VarMap::value_type &v: gPromptVars) {
    if(name == v.second) {
      if(gDebugPromptVars) {
	asg::msgUserCode::ANA_MSG_INFO("Def::RegisterDynamicVar - variable with this name already exists: " << name);
      }

      return v.first;
    }

    last_key = max<unsigned>(last_key, v.first);
  }

  const Def::Var new_key = static_cast<Def::Var>(last_key+1);
 

  if(!RegisterVar(new_key, name)) {
    asg::msgUserCode::ANA_MSG_WARNING("Def::RegisterDynamicVar - WARNING - failed to register variable name=" << name);
    return NONE;
  }

  if(gDebugPromptVars) {
    asg::msgUserCode::ANA_MSG_INFO("Def::RegisterDynamicVar - DEBUG - add variable: " << new_key << ", name=" << name);
  }

  return new_key;
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
    asg::msgUserCode::ANA_MSG_WARNING("Def::Convert2Str - WARNING - unknown variable: " << var);
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
    asg::msgUserCode::ANA_MSG_WARNING("Def::Convert2Var - WARNING - unknown variable: " << var);
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

  asg::msgUserCode::ANA_MSG_WARNING("Def::Convert2Var - WARNING - unknown key: " << key);

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
      asg::msgUserCode::ANA_MSG_WARNING("Prompt::Def::ReadVars - unknown variable name: " << keys.at(i));
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
      asg::msgUserCode::ANA_MSG_WARNING("Prompt::Def::ReadVars - unknown variable name: " << keys.at(i));
    }
  }

  return vars;
}

//======================================================================================================
void Prompt::Def::PrintVarNames()
{
  const vector<Var> vars = Def::GetAllVarEnums();
  
  asg::msgUserCode::ANA_MSG_INFO("PrintCutNames - print " << vars.size() << " enum(s)");
  
  for(unsigned i = 0; i < vars.size(); ++i) {
    asg::msgUserCode::ANA_MSG_INFO("   " << setw(10) << vars.at(i) << ": " << Def::AsStr(vars.at(i)));
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
  result += RegisterVar( TopoEtCone20Rel, "TopoEtCone20Rel");
  result += RegisterVar( TopoEtCone30Rel, "TopoEtCone30Rel");
  result += RegisterVar( PtVarCone20Rel,  "PtVarCone20Rel");
  result += RegisterVar( PtVarCone30Rel,  "PtVarCone30Rel");
  result += RegisterVar( PtVarCone30TightTTVAPt500Rel,  "PtVarCone30TightTTVAPt500Rel");
  result += RegisterVar( DRlj,            "DRlj");
  result += RegisterVar( LepJetPtFrac,    "LepJetPtFrac");
  result += RegisterVar( PtFrac,          "PtFrac");
  result += RegisterVar( PtRel,           "PtRel");
  result += RegisterVar( DL1mu,           "DL1mu");
  result += RegisterVar( rnnip,           "rnnip");
  result += RegisterVar( MV2c10,          "MV2c10");
  result += RegisterVar( MV2rmu,          "MV2rmu");
  result += RegisterVar( SV1,             "SV1");
  result += RegisterVar( JetF,            "JetF");
  result += RegisterVar( JetPt,           "JetPt");
  result += RegisterVar( JetEta,          "JetEta");
  result += RegisterVar( JetPhi,          "JetPhi");
  result += RegisterVar( JetM,            "JetM");

  // track VarHolder
  result += RegisterVar( LepTrackDR,            "LepTrackDR");
  result += RegisterVar( Pt,                    "Pt");
  result += RegisterVar( AbsEta,                "AbsEta");
  result += RegisterVar( NumberOfPIXHits,       "NumberOfPIXHits");
  result += RegisterVar( NumberOfSCTHits,       "NumberOfSCTHits");
  result += RegisterVar( NumberOfSiHits,        "NumberOfSiHits");
  result += RegisterVar( NumberOfSharedSiHits,  "NumberOfSharedSiHits");
  result += RegisterVar( NumberOfSiHoles,       "NumberOfSiHoles");
  result += RegisterVar( NumberOfPixelHoles,    "NumberOfPixelHoles");
  result += RegisterVar( TrackJetDR,            "TrackJetDR");
  result += RegisterVar( TrackPtOverTrackJetPt, "TrackPtOverTrackJetPt");
  result += RegisterVar( Z0Sin,                 "Z0Sin");
  result += RegisterVar( D0Sig,                 "D0Sig");
      
  // PromptLeptonImproved
  result += RegisterVar( MVAXBin,                                                 "MVAXBin");
  result += RegisterVar( RawPt,                                                   "RawPt");
  result += RegisterVar( PromptLeptonRNN_prompt,                                  "PromptLeptonRNN_prompt");
  result += RegisterVar( CaloClusterERel,                                         "CaloClusterERel");
  result += RegisterVar( topoetcone30rel,                                         "topoetcone30rel");
  result += RegisterVar( ptvarcone30rel,                                          "ptvarcone30rel");
  result += RegisterVar( ptvarcone30_TightTTVA_pt500rel,                          "ptvarcone30_TightTTVA_pt500rel");
  result += RegisterVar( CaloClusterSumEtRel,                                     "CaloClusterSumEtRel");
  result += RegisterVar( CandVertex_normDistToPriVtxLongitudinalBest,             "CandVertex_normDistToPriVtxLongitudinalBest");
  result += RegisterVar( CandVertex_normDistToPriVtxLongitudinalBest_ThetaCutVtx, "CandVertex_normDistToPriVtxLongitudinalBest_ThetaCutVtx");
  result += RegisterVar( CandVertex_NPassVtx,                                     "CandVertex_NPassVtx");

  return result;
}

