/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_TRACKPARTICLEPARAMETERSATCALOFILLERTOOL_H
#define TRACKD3PDMAKER_TRACKPARTICLEPARAMETERSATCALOFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "RecoToolInterfaces/IExtrapolateToCaloTool.h" 

#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDepthTool.h"

class CaloDetDescrManager;

namespace Rec{
class TrackParticle;
}

namespace D3PD {

class TrackParticleParametersAtCaloFillerTool
  : public D3PD::BlockFillerTool<Rec::TrackParticle>
{
public:
  TrackParticleParametersAtCaloFillerTool (const std::string& type,
					   const std::string& name,
					   const IInterface* parent);
  virtual StatusCode initialize();
  virtual StatusCode book();
  virtual StatusCode fill (const Rec::TrackParticle &p);

private:

  StatusCode extrapolateTrack(const Rec::TrackParticle *trk, float *eta, float *phi);

  /* */
  std::string m_subCaloStr;
  CaloCell_ID::SUBCALO m_subCalo;
  int m_sampling;
	
  /* NTuple variables: **/
  float *m_eta_atCalo;
  float *m_phi_atCalo;

  /* Track to vertex extrapolator **/
  ToolHandle<IExtrapolateToCaloTool> m_trackToCaloTool; 
  ToolHandle<CaloDepthTool> m_caloDepthTool;

  const CaloDetDescrManager* m_calo_dd;

}; // class TrackParticleParametersAtCaloFillerTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_TRKTRACKPARAMETERSATCALOFILLERTOOL_H
