/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SimTestToolBase.h"

#include "AtlasHepMC/GenEvent.h"
#include "GeneratorObjects/McEventCollection.h"

SimTestToolBase::SimTestToolBase(const std::string& type, 
                   const std::string& name,
                   const IInterface* parent) : 
  base_class(type, name, parent)
{
  declareProperty("HistPath",  m_path="/truth/");
  declareProperty("McEventKey", m_key="TruthEvent");
}

HepMC::ConstGenParticlePtr   SimTestToolBase::getPrimary() 
{
  const DataHandle<McEventCollection> mcCollection;
  if (evtStore()->retrieve(mcCollection,m_key).isSuccess()) {
    DataVector<HepMC::GenEvent>::const_iterator e;
    for (e=mcCollection->begin();e!=mcCollection->end(); e++) {
      for (auto p : (**e)) {
	if ( HepMC::barcode(p)<200000 ) {
	  return p;
	}
      }
    }
  }
  return 0;
}

