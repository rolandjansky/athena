/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagJetElectronAssociationTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


JetTagJetElectronAssociationTool::JetTagJetElectronAssociationTool 
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : MultiAssociationTool<Jet,Analysis::Electron>(type, name, parent)
{

}


StatusCode JetTagJetElectronAssociationTool::initialize()
{

  typedef MultiAssociationTool<Jet,Analysis::Electron> base;

  CHECK ( base::initialize() );

  /// retrieve tools here
  //// none for now

  return StatusCode::SUCCESS;
}


StatusCode JetTagJetElectronAssociationTool::book()
{

  /// book variable here
  //// none for now

  return StatusCode::SUCCESS;
}


StatusCode JetTagJetElectronAssociationTool::reset (const  Jet& p)
{

  m_elecItr = m_elecEnd;
  const Analysis::ElectronAssociation* ea = p.getAssociation<Analysis::ElectronAssociation>("Electrons");

  if (ea){
    m_elecItr = ea->begin();
    m_elecEnd = ea->end();
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const Analysis::Electron* JetTagJetElectronAssociationTool::next()
{

  // reached the end
  if(m_elecItr == m_elecEnd) return 0;

  return *(m_elecItr++);


}

} // namespace D3PD
 
