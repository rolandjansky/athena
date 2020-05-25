/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/EventVariable.h"


namespace EventVar {

  std::unique_ptr<Variable> Variable::create(const std::string & name, const std::string &type, int index){

    // if known name, return the corresponding dedicated class
    if(name=="ActMu") return std::make_unique<ActMuVar>(name);
    if(name=="AvgMu") return std::make_unique<AvgMuVar>(name);


    // Else assume we're looking for an attribute attached to the jet
    if(type=="float") return std::make_unique<VariableAtt<float> >(name);
    if(type=="int") return std::make_unique<VariableAtt<int> >(name);
    if(type=="vecfloat") return std::make_unique<VariableAtt<std::vector<float> > >(name, index);
    if(type=="vecint") return std::make_unique<VariableAtt< std::vector<int> > >(name, index);
    

    return nullptr;
  }


}
