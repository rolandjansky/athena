/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGLONGLIVEDPARTICLESHYPO_IDCALIBHYPOALG_H
#define TRIGLONGLIVEDPARTICLESHYPO_IDCALIBHYPOALG_H

#include <string>

#include "Gaudi/Property.h"
#include "DecisionHandling/HypoBase.h"
#include "IDCalibHypoTool.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

/**
 * @class IDCalibHypoAlg
 * @brief Implements Hypo selection on IDCalib stream trigger
 * @author Kunihiro Nagano <kunihiro.nagano@cern.ch> - KEK
 **/

class IDCalibHypoAlg : public ::HypoBase 
{ 
public: 

   IDCalibHypoAlg( const std::string& name, ISvcLocator* pSvcLocator );
   
   virtual StatusCode  initialize() override;
   virtual StatusCode  execute(const EventContext& context) const override;
   
private: 

   SG::ReadHandleKey< xAOD::TrackParticleContainer > m_tracksKey {this,"tracksKey","","Key for precision tracks"};

   ToolHandleArray< IDCalibHypoTool >  m_hypoTools {this, "HypoTools", {}, "Tools to perfrom selection"};
   ToolHandle<GenericMonitoringTool>   m_monTool   {this, "MonTool", "", "Monitoring tool" };
}; 

#endif //> !TRIGLONGLIVEDPARTICLESHYPO_IDCALIBHYPOALG_H
