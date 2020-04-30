/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */

#ifndef INDETD3PDMAKER_TRACKPARTICLETRUTHASSOCIATIONTOOL_H
#define INDETD3PDMAKER_TRACKPARTICLETRUTHASSOCIATIONTOOL_H

#include "D3PDMakerUtils/SingleAssociationTool.h"

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/StoreGateSvc.h"

#include "AtlasHepMC/GenParticle_fwd.h"

namespace Rec {
class TrackParticle;
}

namespace D3PD {

class TrackParticleTruthAssociationTool
  : public SingleAssociationTool<Rec::TrackParticle, HepMC::GenParticle>
{
public:
  typedef SingleAssociationTool<Rec::TrackParticle, HepMC::GenParticle> Base;

  TrackParticleTruthAssociationTool (const std::string& type,
				       const std::string& name,
				       const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode book();

  virtual const HepMC::GenParticle* get (const Rec::TrackParticle& p);

private:
  /* StoreGate keys **/
  std::string m_TruthMap;
  std::string m_TPContainer;

  /* NTuple variables **/
  float* m_trueProb;
  int* m_trueBarcode;

}; // class TrackParticleTruthAssociationTool

} // namespace D3PD

#endif // not INDETD3PDMAKER_TRACKPARTICLETRUTHASSOCIATIONTOOL_H
