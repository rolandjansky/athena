/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagJetMuonAssociationTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


JetTagJetMuonAssociationTool::JetTagJetMuonAssociationTool 
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : MultiAssociationTool<Jet,Analysis::Muon>(type, name, parent)
{
  declareProperty("MuonsName", m_muonsName="Muons");

}


StatusCode JetTagJetMuonAssociationTool::initialize()
{

  typedef MultiAssociationTool<Jet,Analysis::Muon> base;

  CHECK ( base::initialize() );

  /// retrieve tools here
  //// none for now

  return StatusCode::SUCCESS;
}


StatusCode JetTagJetMuonAssociationTool::book()
{

  /// book variable here
  //// none for now

  return StatusCode::SUCCESS;
}


StatusCode JetTagJetMuonAssociationTool::reset (const  Jet& p)
{

  m_muItr = m_muEnd;
  const Analysis::MuonAssociation* ma = p.getAssociation<Analysis::MuonAssociation>(m_muonsName);

  if (ma){
    m_muItr = ma->begin();
    m_muEnd = ma->end();
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const Analysis::Muon* JetTagJetMuonAssociationTool::next()
{

  // reached the end
  if(m_muItr == m_muEnd) return 0;

  return *(m_muItr++);


}

} // namespace D3PD
 
