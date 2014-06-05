/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagJetPhotonAssociationTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


JetTagJetPhotonAssociationTool::JetTagJetPhotonAssociationTool 
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : MultiAssociationTool<Jet,Analysis::Photon>(type, name, parent)
{

}


StatusCode JetTagJetPhotonAssociationTool::initialize()
{

  typedef MultiAssociationTool<Jet,Analysis::Photon> base;

  CHECK ( base::initialize() );

  /// retrieve tools here
  //// none for now

  return StatusCode::SUCCESS;
}


StatusCode JetTagJetPhotonAssociationTool::book()
{

  /// book variable here
  //// none for now

  return StatusCode::SUCCESS;
}


StatusCode JetTagJetPhotonAssociationTool::reset (const  Jet& p)
{

  m_phItr = m_phEnd;
  const Analysis::PhotonAssociation* pa = p.getAssociation<Analysis::PhotonAssociation>("Photons");

  if (pa){
    m_phItr = pa->begin();
    m_phEnd = pa->end();
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const Analysis::Photon* JetTagJetPhotonAssociationTool::next()
{

  if(m_phItr == m_phEnd) return 0;

  return *(m_phItr++);


}

} // namespace D3PD
 
