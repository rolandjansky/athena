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
    ATH_MSG_WARNING("Requested modulation, but provided no name for HIEventShapeContainer, no modulation will be applied");
  ATH_MSG_DEBUG("Equipping " << m_do_v2 << "\t" << m_do_v3 << "\t" << m_do_v4);
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

float HIUEModulatorTool::getModulation(float phi,
                                       const xAOD::HIEventShape* shape /*=nullptr*/) const
{
  if (!shape)
    shape = m_shape;
  if (!shape)
    if (getShape (shape).isFailure() || !shape)
      return 1;
  return modulate(m_nh_vector,shape,phi);
}

StatusCode HIUEModulatorTool::getShape(const xAOD::HIEventShape* & shape) const
{
  shape = 0;
  if(m_shape_key.compare("NULL")==0) return StatusCode::SUCCESS;
  const xAOD::HIEventShapeContainer* mod_shape=0;
  if(evtStore()->retrieve(mod_shape,m_shape_key).isFailure())
  {
    ATH_MSG_ERROR("Could not retrieve shape " << m_shape_key);
    return StatusCode::FAILURE;
    
  }
  if(mod_shape->size()==0)
  {
    ATH_MSG_ERROR("Modulation container empty : " << m_shape_key);
    return StatusCode::FAILURE;
  }
  shape=mod_shape->at(0);
  return StatusCode::SUCCESS;
}

StatusCode HIUEModulatorTool::retrieveShape()
{
  return getShape (m_shape);
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
      float nphi=nh_vector.at(i);

      nphi*=phi;
      float qx=shape->etCos().at(ih);
      float qy=shape->etSin().at(ih);
      CxxUtils::sincos sc(nphi);
      modulation+=2.*sc.apply(qy,qx);
    }
    modulation/=et;
  }
  return modulation+1;
}

StatusCode HIUEModulatorTool::checkQVectorSize(const xAOD::HIEventShape* shape, unsigned int n) const
{
  return ( shape->etCos().size() >= n ) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

StatusCode HIUEModulatorTool::checkCompatibility() const
{
  for(unsigned int i=0; i < m_nh_vector.size(); i++) 
  {
    if(checkQVectorSize(m_shape,m_nh_vector.at(i)).isFailure())
    {
      ATH_MSG_ERROR("HIEventShapeContainer " << m_shape_key << " does not have requested harmonic " << m_nh_vector.at(i) );
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}
