/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */

#ifndef INDETD3PDMAKER_TRUTHTRACKGETTERTOOL_H
#define INDETD3PDMAKER_TRUTHTRACKGETTERTOOL_H

#include "D3PDMakerUtils/SGCollectionGetterTool.h"
#include "GeneratorObjects/McEventCollection.h"
#include "HepPDT/ParticleDataTable.hh"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"

namespace D3PD {

class TruthTrackGetterTool
  : public D3PD::SGCollectionGetterTool<McEventCollection>
{
public:
  typedef D3PD::SGCollectionGetterTool<McEventCollection> Base;

  TruthTrackGetterTool (const std::string& type,
			const std::string& name,
			const IInterface* parent);

  StatusCode initialize();
  size_t sizeHint(bool=false);
  StatusCode reset(bool=false);
  const void* nextUntyped();

  const std::type_info& typeinfo() const;
  const std::type_info& elementTypeinfo() const;

private:

  bool selectParticle(const  HepMC::GenParticle* particle);

  /* Event iterators **/
  McEventCollection::const_iterator m_evtItr;
  McEventCollection::const_iterator m_evtEnd;

  /* Particle iterators **/
  HepMC::GenEvent::particle_const_iterator m_partItr;
  HepMC::GenEvent::particle_const_iterator m_partEnd;
 
  /* Helper used to get the charge of a particle from it's pdgId **/
  const HepPDT::ParticleDataTable*  m_particleDataTable;

  /* selection settings **/
  bool m_keepOnlyChargedParticles;
  bool m_keepOnlyPrimaryParticles;
  bool m_keepV0;
  float m_trueTrackPtCut;
  float m_trueTrackEtaCut;
  float m_maxRAll;
  float m_maxZAll;
  float m_maxRPrimary;
  float m_maxZPrimary;

}; // class TruthTrackGetterTool

} // namespace D3PD

#endif // not INDETD3PDMAKER_TRUTHTRACKGETTERTOOL_H
