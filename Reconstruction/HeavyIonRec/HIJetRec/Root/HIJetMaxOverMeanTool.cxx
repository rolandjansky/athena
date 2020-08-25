/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "StoreGate/WriteDecorHandle.h"

#include "HIJetMaxOverMeanTool.h"

HIJetMaxOverMeanTool::HIJetMaxOverMeanTool(const std::string& t) : asg::AsgTool(t)
{
  declareInterface<IJetDecorator>(this);
}


StatusCode HIJetMaxOverMeanTool::initialize()
{
  if( m_jetContainerName.empty() ){
    ATH_MSG_ERROR("HIJetMaxOverMeanTool needs to have its input jet container name configured!");
    return StatusCode::FAILURE;
  }
  //Preped Jet Container Name
  m_jetMaxConstituentETKey = m_jetContainerName + "." + m_jetMaxConstituentETKey.key();
  m_jetMaxOverMeanKey = m_jetContainerName + "." + m_jetMaxOverMeanKey.key();

  ATH_CHECK( m_jetMaxConstituentETKey.initialize() );
  ATH_CHECK( m_jetMaxOverMeanKey.initialize() );

  return StatusCode::SUCCESS;

}

 StatusCode HIJetMaxOverMeanTool::decorate(const xAOD::JetContainer& jets) const
 {
  //Decorator handlers
  SG::WriteDecorHandle<xAOD::JetContainer, float> jetMaxConstituentETDecorHandle( m_jetMaxConstituentETKey );
  SG::WriteDecorHandle<xAOD::JetContainer, float> jetMaxOverMeanDecorHandle ( m_jetMaxOverMeanKey );
  for(const xAOD::Jet* jet : jets)
  {
    float max=-999;
    float sum=0;
    float count=0;
    const xAOD::JetConstituentVector constituents = jet->getConstituents();
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

    // set the attributes
    jetMaxConstituentETDecorHandle(*jet) = max;
    jetMaxOverMeanDecorHandle(*jet) = D;

  }
  return StatusCode::SUCCESS;
}
