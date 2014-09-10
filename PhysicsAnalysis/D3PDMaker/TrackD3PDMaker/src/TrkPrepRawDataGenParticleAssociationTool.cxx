/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkPrepRawDataGenParticleAssociationTool.h"

#include "AthenaKernel/errorcheck.h"

namespace D3PD {

TrkPrepRawDataGenParticleAssociationTool::TrkPrepRawDataGenParticleAssociationTool (const std::string& type,
										const std::string& name,
										const IInterface* parent)
  : Base (type, name, parent)
{
  declareProperty ("MapKey", m_mapKey = "PRD_MultiTruthPixel");
  declareProperty ("StoreBarcode", m_storeBarcode = false);

  book().ignore();  // Avoid coverity warnings.
}

StatusCode TrkPrepRawDataGenParticleAssociationTool::initialize()
{
  CHECK( Base::initialize() );
  return StatusCode::SUCCESS;
}


StatusCode TrkPrepRawDataGenParticleAssociationTool::book ()
{

  CHECK( Base::book() );
  if(m_storeBarcode) CHECK( addVariable ("barcode", m_barcode) );

  return StatusCode::SUCCESS;
}

StatusCode TrkPrepRawDataGenParticleAssociationTool::reset(const Trk::PrepRawData& prd){

  if(m_storeBarcode) m_barcode->clear();

  const PRD_MultiTruthCollection* prdmap=0;

  if( evtStore()->retrieve(prdmap, m_mapKey).isFailure() ){
    REPORT_MESSAGE (MSG::WARNING) << "Could not retrieve " << m_mapKey;
    m_itr = m_end;
    return StatusCode::SUCCESS;
  }

  Identifier id = prd.identify();

  std::pair<PRDTruthLink, PRDTruthLink> links = prdmap->equal_range(id);
  m_itr = links.first;
  m_end = links.second;

  return StatusCode::SUCCESS;
}

const HepMC::GenParticle *TrkPrepRawDataGenParticleAssociationTool::next (){

  if(m_itr == m_end) return 0;

  int barcode = 0;

  const HepMC::GenParticle *particle = 0;

  if(m_itr->second.isValid()){
    particle = &(*(m_itr->second));
    barcode = particle->barcode();
  }else{
    particle = &m_dummy;
  }

  if(m_storeBarcode) m_barcode->push_back(barcode);

  m_itr++;

  return particle;
}

} // namespace D3PD
