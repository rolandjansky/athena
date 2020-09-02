/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIUEModulatorTool.h"
#include "CxxUtils/sincos.h"
#include "xAODHIEvent/HIEventShapeContainer.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

HIUEModulatorTool::HIUEModulatorTool(const std::string& n) : asg::AsgTool(n),
							     m_shape(nullptr)
{
#ifndef XAOD_STANDALONE
  //should not need this due to ASG_TOOL_CLASS macro since
  //athena only calls 3 arg constructor which explicitly declares athena interface?
  declareInterface<IHIUEModulatorTool>(this);
#endif
}

StatusCode HIUEModulatorTool::initialize()
{
	//First we initialize keys - after initialization they are frozen
  ATH_CHECK( m_shape_key.initialize( !m_shape_key.key().empty()) );

	m_nh_vector.reserve(3);
  if(m_do_v2) m_nh_vector.push_back(2);
  if(m_do_v3) m_nh_vector.push_back(3);
  if(m_do_v4) m_nh_vector.push_back(4);
  if(m_nh_vector.size()!=0 && (m_shape_key.key().compare("NULL") == 0))
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
  if( m_shape_key.key().compare("NULL") == 0) return StatusCode::SUCCESS;

	SG::ReadHandle<xAOD::HIEventShapeContainer>  read_handle_evtShape ( m_shape_key );
	if (!read_handle_evtShape.isValid()) {
			 ATH_MSG_FATAL( "Could not find HI event shape!" );
			 return(StatusCode::FAILURE);
	}

  const xAOD::HIEventShapeContainer* mod_shape = read_handle_evtShape.get();

  if(mod_shape->size()==0)
  {
    ATH_MSG_ERROR("Modulation container empty : " << m_shape_key);
    return StatusCode::FAILURE;
  }
  shape = mod_shape->at(0);
  return StatusCode::SUCCESS;
}

StatusCode HIUEModulatorTool::retrieveShape() const
{
  return  getShape (const_cast<const xAOD::HIEventShape* &> (m_shape)); //TODO check for a smarter implementation
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
