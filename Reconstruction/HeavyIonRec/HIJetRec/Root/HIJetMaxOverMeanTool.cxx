/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetRec/HIJetMaxOverMeanTool.h"

HIJetMaxOverMeanTool::HIJetMaxOverMeanTool(const std::string& t) : JetModifierBase(t)
{
}

int HIJetMaxOverMeanTool::modifyJet(xAOD::Jet& jet) const 
{
  float max=-999;
  float sum=0;
  float count=0;
  const xAOD::JetConstituentVector constituents = jet.getConstituents();
  for (xAOD::JetConstituentVector::iterator itr = constituents.begin(); itr != constituents.end(); ++itr) 
  {
    float et=(*itr)->e()/std::cosh((*itr)->eta());
    if(et > max) max=et;
    sum+=et;
    count++;
  }
  if(count==0.) sum=0;
  else sum/=count;
  float D=0;
  if(sum!=0.) D=max/sum;

  jet.setAttribute("MaxConstituentET",max);
  jet.setAttribute("MaxOverMean",D);
  return 0;
}
