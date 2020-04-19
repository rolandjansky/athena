/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/JetVariable.h"


namespace JetVar {

  Variable* Variable::create(const std::string & name, const std::string &type, int index){

    // if known name, return the corresponding dedicated class
    if(name=="e") return new EVar(name);
    if(name=="et") return new EtVar(name);
    if(name=="pz") return new PzVar(name);
    if(name=="nconstit") return new NconstitVar(name);
    if(name=="abseta") return new AbsEtaVar(name);
    if(name=="|eta|") return new AbsEtaVar(name);
    if(name=="rapidity") return new Rapidity(name);

    // Else assume we're looking for an attribute attached to the jet
    if(type=="float") return new VariableAtt<float>(name);
    if(type=="int") return new VariableAtt<int>(name);
    if(type=="vecfloat") return new VariableAtt<std::vector<float> >(name, index);
    if(type=="vecint") return new VariableAtt< std::vector<int> >(name, index);
    

    return nullptr;
  }


}
