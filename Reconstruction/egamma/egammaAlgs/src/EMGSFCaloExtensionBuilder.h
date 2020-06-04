/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef EGAMMAALGS_EMGSFCALOEXTENSIONBUILDER_H
#define EGAMMAALGS_EMGSFCALOEXTENSIONBUILDER_H
/**
  @class EMGSFCaloExtensionBuilder
  Algorithm which creates calo extension for all GSF
  Track Particles
  */
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ToolHandle.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "TrkCaloExtension/CaloExtensionCollection.h"
#include "xAODTracking/TrackParticleContainerFwd.h"

class EMGSFCaloExtensionBuilder : public AthAlgorithm
{
public:
  /** @brief Default constructor*/
  EMGSFCaloExtensionBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  virtual StatusCode execute() override final { 
    return execute_r(Algorithm::getContext()); 
  }
  // This will become the normal execute when
  // inheriting from AthReentrantAlgorithm
  StatusCode execute_r(const EventContext& ctx) const;

private:
  /** @brief the Calo Extension tool*/
  ToolHandle<Trk::IParticleCaloExtensionTool> m_lastParticleCaloExtensionTool{
    this,
    "LastCaloExtensionTool",
    "Trk::ParticleCaloExtensionTool/EMLastCaloExtensionTool"
  };

  ToolHandle<Trk::IParticleCaloExtensionTool> m_perigeeParticleCaloExtensionTool{
    this,
    "PerigeeCaloExtensionTool",
    "Trk::ParticleCaloExtensionTool/EMParticleCaloExtensionTool"
  };

  // Cache collections for GSF Track Particle extrapolation Perigee
  SG::WriteHandleKey<CaloExtensionCollection> m_GSFPerigeeCacheKey{
    this,
    "GSFPerigeeCache",
    "GSFPerigeeCaloExtension",
    "Name of GSF Perigee extrapolation cache"
  };
  SG::WriteHandleKey<CaloExtensionCollection> m_GSFLastCacheKey{
    this,
    "GSFLastCache",
    "GSFLastCaloExtension",
    "Name of GSF Last measurement extrapolation cache"
  };

  // input GSF Track collection
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_GSFTrkPartContainerKey{
    this,
    "GFFTrkPartContainerName",
    "GSFTrackParticles",
    "GSF TrackParticles"
  };
};
#endif //
