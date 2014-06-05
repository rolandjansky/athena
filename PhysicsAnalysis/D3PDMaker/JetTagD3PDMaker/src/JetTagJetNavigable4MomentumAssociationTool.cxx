/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagJetNavigable4MomentumAssociationTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {


JetTagJetNavigable4MomentumAssociationTool::JetTagJetNavigable4MomentumAssociationTool 
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : MultiAssociationTool<Jet,TruthParticle>(type, name, parent)
{

  declareProperty("Navigable4MomentumAssocName", m_navAssocName="TruthBHadron");
}


StatusCode JetTagJetNavigable4MomentumAssociationTool::initialize()
{

  typedef  MultiAssociationTool<Jet,TruthParticle>  base;

  CHECK ( base::initialize() );

  return StatusCode::SUCCESS;
}



StatusCode JetTagJetNavigable4MomentumAssociationTool::reset (const  Jet& p)
{
  

  const JetINav4MomAssociation* assoc = p.getAssociation<JetINav4MomAssociation>(m_navAssocName);

  if(assoc == 0){
    m_navItr = m_navEnd;
    ATH_MSG_WARNING(" JetINav4MomAssociation with name "<< m_navAssocName << " not found");
    return StatusCode::SUCCESS;
  }

  m_navItr = assoc->begin(); 
  m_navEnd = assoc->end(); 


  return StatusCode::SUCCESS;
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const TruthParticle* JetTagJetNavigable4MomentumAssociationTool::next()
{

  // reached the end
  if(m_navItr == m_navEnd){
    return 0;
  }
 
  return dynamic_cast<const TruthParticle*>(*(m_navItr++));


}

} // namespace D3PD
 
