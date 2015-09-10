/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetRec/HIUEModulatorTool.h"
#include "CxxUtils/sincos.h"
#include "xAODHIEvent/HIEventShapeContainer.h"

HIUEModulatorTool::HIUEModulatorTool(const std::string& n) : asg::AsgTool(n),
							     m_shape(nullptr)
{
#ifdef ASGTOOL_ATHENA
  //should not need this due to ASG_TOOL_CLASS macro since 
  //athena only calls 3 arg constructor which explicitly declares athena interface?
  declareInterface<IHIUEModulatorTool>(this); 
#endif

  declareProperty("EventShapeKey",m_shape_key="NULL");
  declareProperty("DoV2",m_do_v2=true);
  declareProperty("DoV3",m_do_v3=true);
  declareProperty("DoV4",m_do_v4=true);
}

StatusCode HIUEModulatorTool::initialize()
{
  
  m_nh_vector.reserve(3);
  if(m_do_v2) m_nh_vector.push_back(2);
  if(m_do_v3) m_nh_vector.push_back(3);
  if(m_do_v4) m_nh_vector.push_back(4);
  if(m_nh_vector.size()!=0 &&(m_shape_key.compare("NULL")==0)) 
    ATH_MSG_WARNING("Requested modulation, but provided no name for HIEventShapeContainer, no modulation will beapplied");

  return StatusCode::SUCCESS;
}


StatusCode HIUEModulatorTool::setEventShapeForModulation(const xAOD::HIEventShape* shape)
{
  m_shape=shape;
  return checkCompatibility();
}


void HIUEModulatorTool::setHarmonics(const std::vector<unsigned int>& v)
{
  m_nh_vector.assign(v.begin(),v.end());
}

float HIUEModulatorTool::getModulation(float phi) const
{
  if(!m_shape) return 1;
  return modulate(m_nh_vector,m_shape,phi);
}

StatusCode HIUEModulatorTool::retrieveShape()
{
  if(m_shape_key.compare("NULL")==0) return StatusCode::SUCCESS;
  const xAOD::HIEventShapeContainer* mod_shape=0;
  CHECK(evtStore()->retrieve(mod_shape,m_shape_key));
  if(mod_shape->size()==0)
  {
    ATH_MSG_ERROR("Modulation container empty : " << m_shape_key);
    return StatusCode::FAILURE;
  }
  m_shape=mod_shape->at(0);
  return StatusCode::SUCCESS;
}

float HIUEModulatorTool::modulate(const std::vector<unsigned int>& nh_vector, const xAOD::HIEventShape* shape, float phi)
{
  float modulation=0;
  float et=shape->et();
  if(et > 0.)
  {
    for(unsigned int i=0; i < nh_vector.size(); i++)
    {
      
      unsigned int ih=nh_vector.at(i)-1;
      float qx=shape->etCos().at(ih);
      float qy=shape->etSin().at(ih);
      CxxUtils::sincos sc(phi*ih);
      modulation+=2.*sc.apply(qx,qy);
    }
    modulation/=et;
  }
  return modulation+1;
}

StatusCode HIUEModulatorTool::checkQVectorSize(const xAOD::HIEventShape* shape, unsigned int n) const
{
  return ( n+1 < shape->etCos().size() ) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

StatusCode HIUEModulatorTool::checkCompatibility() const
{
  for(unsigned int i=0; i < m_nh_vector.size(); i++) CHECK(checkQVectorSize(m_shape,m_nh_vector.at(i)));
  return StatusCode::SUCCESS;
}
