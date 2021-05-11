/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGTAUHYPO_TRIGTRKPRECHYPOALG_H
#define TRIGTAUHYPO_TRIGTRKPRECHYPOALG_H

#include <string>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthViews/View.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "DecisionHandling/HypoBase.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "ITrigTrkPrecHypoTool.h"

class TrigTrkPrecHypoAlg : public ::HypoBase {
 public: 

  TrigTrkPrecHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode  initialize() override;
  virtual StatusCode  execute( const EventContext& context ) const override;

 private: 
  ToolHandleArray< ITrigTrkPrecHypoTool > m_hypoTools { this, "HypoTools", {}, "Hypo tools" };
     
  SG::ReadHandleKey< xAOD::TrackParticleContainer > m_fastTracksKey { this, "trackparticles", "trackparticles", "precision tracks in view" };
  SG::ReadHandleKey< TrigRoiDescriptorCollection > m_roiForID2ReadKey{ this, "RoIForIDReadHandleKey", "UpdatedTrackRoI", "Updated narrow ROI produced in view" };

}; 

#endif //> !TRIGTAUHYPO_TRIGTRKPRECHYPOALG_H
