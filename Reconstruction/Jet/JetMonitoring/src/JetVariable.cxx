/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/JetVariable.h"


namespace JetVar {

  std::unique_ptr<Variable> Variable::create(const std::string & name, const std::string &type, int index){

    // if known name, return the corresponding dedicated class
    if(name=="e") return std::make_unique<EVar>(name);
    if(name=="et") return std::make_unique<EtVar>(name);
    if(name=="pz") return std::make_unique<PzVar>(name);
    if(name=="nconstit") return std::make_unique<NconstitVar>(name);
    if(name=="abseta") return std::make_unique<AbsEtaVar>(name);
    if(name=="|eta|") return std::make_unique<AbsEtaVar>(name);
    if(name=="rapidity") return std::make_unique<Rapidity>(name);
    if(name=="fCharged") return std::make_unique<FChargedVar>(name);
    if(name=="EM3Frac") return std::make_unique<EM3FracVar>(name);
    if(name=="Tile0Frac") return std::make_unique<Tile0FracVar>(name);

    // Else assume we're looking for an attribute attached to the jet
    if(type=="float") return std::make_unique<VariableAtt<float> >(name);
    if(type=="int") return std::make_unique<VariableAtt<int> >(name);
    if(type=="vecfloat") return std::make_unique<VariableAtt<std::vector<float> > >(name, index);
    if(type=="vecint") return std::make_unique<VariableAtt< std::vector<int> > >(name, index);
    

    return nullptr;
  }


}
