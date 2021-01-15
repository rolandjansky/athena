/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagJetGenSoftLeptonAssociationTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenEvent.h"
#include "GeneratorObjects/McEventCollection.h"
#include "JetTagInfo/SoftLeptonTruthInfo.h"


namespace D3PD {


JetTagJetGenSoftLeptonAssociationTool::JetTagJetGenSoftLeptonAssociationTool 
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : base(type, name, parent),
      m_resolver (name, this->evtStore(), m_keys),
      m_lepItr(0),
      m_lepEnd(0)
{
  declareProperty("MCCollections", m_keys);
  declareProperty("FillVariables", m_fillVariables=true );

  book().ignore(); // Avoid coverity warnings.
}


StatusCode JetTagJetGenSoftLeptonAssociationTool::initialize()
{

  CHECK ( base::initialize() );

  CHECK ( m_resolver.initialize<McEventCollection>() );

  m_dummyParticle = HepMC::newGenParticlePtr();

  return StatusCode::SUCCESS;
}
StatusCode JetTagJetGenSoftLeptonAssociationTool::finalize()
{
#ifdef HEPMC3

#else
  delete m_dummyParticle;
#endif
  return base::finalize();
}

StatusCode JetTagJetGenSoftLeptonAssociationTool::book()
{

  if(m_fillVariables){
    CHECK( addVariable ("origin", m_origin) );
    CHECK( addVariable ("slbarcode", m_barcode) );
  }

  return StatusCode::SUCCESS;
}


StatusCode JetTagJetGenSoftLeptonAssociationTool::reset (const  Jet& p)
{
  m_slmcinfo = p.getTagInfo<Analysis::SoftLeptonTruthInfo>("SoftLeptonTruthInfo");

  m_lepItr = 0;
  m_lepEnd = 0;

  if (m_slmcinfo) {
    m_lepEnd = m_slmcinfo->numSLTrueInfo();
  }
  std::string key = m_resolver.key();

  const McEventCollection* mc = 0;

  CHECK( evtStore()->retrieve(mc, key) ); 

  m_genEvent=0;
  if(mc){
    if(mc->begin() != mc->end()){
      m_genEvent = *(mc->begin());
    }
  }
  else{
    ATH_MSG_WARNING(" MC collection " << key << " not found " );
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
#ifdef HEPMC3
const HepMC3::GenParticle* JetTagJetGenSoftLeptonAssociationTool::next()
#else
const HepMC::GenParticle* JetTagJetGenSoftLeptonAssociationTool::next()
#endif
{

  // reached the end
  if(m_lepItr == m_lepEnd) return 0;

  const Analysis::SLTrueInfo sltinfo = m_slmcinfo->getSLTrueInfo(m_lepItr);
  int barc = sltinfo.barcode();
  short orig = 0;
  if (sltinfo.FromGH()) orig += 1;
  if (sltinfo.FromD())  orig += 2;
  if (sltinfo.FromB())  orig += 4;

  if(m_fillVariables){
    *m_origin = orig;
    *m_barcode = barc;
  }
  
  HepMC::ConstGenParticlePtr mcpart{nullptr};
  if(m_genEvent){
    mcpart = HepMC::barcode_to_particle(m_genEvent,barc);
  }


  ////// if no mc particle/event the association will end at return 0
  ////// this should not be the case
  ////// but just in case create a dummy particle and deleted at the end;

  if(!mcpart) mcpart = m_dummyParticle; 

  ++m_lepItr;
#ifdef HEPMC3
  return mcpart.get();
#else
  return mcpart;
#endif


}

} // namespace D3PD
 
