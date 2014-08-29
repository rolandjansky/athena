/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SimTestToolBase.h"

#include "HepMC/GenEvent.h"
#include "GeneratorObjects/McEventCollection.h"

SimTestToolBase::SimTestToolBase(const std::string& type, 
                   const std::string& name,
                   const IInterface* parent) : 
  AthAlgTool(type, name, parent)
{
  declareProperty("HistPath",  m_path="/truth/");
  declareProperty("McEventKey", m_key="TruthEvent");
}

StatusCode
SimTestToolBase::queryInterface(const InterfaceID& riid, void** ppvIf) {
  if ( riid == ISimTestTool::interfaceID() ) {
    *ppvIf = (ISimTestTool*)this; 
    addRef();
    return StatusCode::SUCCESS;
  }
  return AlgTool::queryInterface( riid, ppvIf );
}

const HepMC::GenParticle * SimTestToolBase::getPrimary() 
{
  const DataHandle<McEventCollection> mcCollection;
  if (evtStore()->retrieve(mcCollection,m_key).isSuccess()) {
    DataVector<HepMC::GenEvent>::const_iterator e;
    for (e=mcCollection->begin();e!=mcCollection->end(); e++) {
      HepMC::GenEvent::particle_const_iterator p;
      for (p= (**e).particles_begin(); p!= (**e).particles_end(); p++) {
	if ( (*p)->barcode()<200000 ) {
	  return (*p);
	}
      }
    }
  }
  return 0;
}

