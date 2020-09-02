/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */

#ifndef INDETD3PDMAKER_TRUTHTRACKFILLERTOOL_H
#define INDETD3PDMAKER_TRUTHTRACKFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "HepPDT/ParticleDataTable.hh"

#include "AtlasHepMC/GenParticle_fwd.h"

namespace D3PD {

class TruthTrackFillerTool
  : public D3PD::BlockFillerTool<HepMC::GenParticle>
{
public:
  typedef D3PD::BlockFillerTool<HepMC::GenParticle> Base;

  TruthTrackFillerTool (const std::string& type,
			const std::string& name,
			const IInterface* parent);

  StatusCode initialize();
  virtual StatusCode book();

  virtual StatusCode fill (const HepMC::GenParticle& p);

private:

  void clearData();

  /* Helper used to get the charge of a particle from it's pdgId **/
  const HepPDT::ParticleDataTable*  m_particleDataTable;

  /* NTuple variables: **/
  // truth id
  float *m_charge;
  int *m_child_n;
  std::vector<int> *m_child_barcode;
  std::vector<int> *m_child_pdg;
  // origin vertex
  float *m_begVtx_x;
  float *m_begVtx_y;
  float *m_begVtx_z;
  float *m_begVtx_barcode;
  // end vertex
  float *m_endVtx_x;
  float *m_endVtx_y;
  float *m_endVtx_z;
  float *m_endVtx_barcode;

}; // class TruthTrackFillerTool

} // namespace D3PD

#endif // not INDETD3PDMAKER_TRUTHTRACKFILLERTOOL_H
