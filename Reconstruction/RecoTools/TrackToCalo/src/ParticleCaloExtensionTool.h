/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

/*
 * ParticleCaloExtensionTool.h  - implements the IParticleCaloExtenions Interface
 * begin : Summer 2014
 * updated : 2018 for AthenaMT
 * authors : Niels van Eldik (CERN PH-ATC),Christos Anastopoulos
 */

#ifndef TRKPARTICLECREATOR_PARTICLECALOEXTENSIONTOOL_H
#define TRKPARTICLECREATOR_PARTICLECALOEXTENSIONTOOL_H
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "xAODTracking/TrackParticle.h"
#include "TrkEventPrimitives/ParticleHypothesis.h" 
#include "xAODTracking/NeutralParticle.h"
#include "xAODTruth/TruthParticle.h"

class AtlasDetectorID;

namespace Trk {

class IExtrapolator;

class ParticleCaloExtensionTool : virtual public IParticleCaloExtensionTool, public AthAlgTool {
public:

  ParticleCaloExtensionTool(const std::string&,const std::string&,const IInterface*);

  virtual ~ParticleCaloExtensionTool();

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

  /* 
   * Implement the IParticleCaloExtension methods
   */
  virtual std::unique_ptr<Trk::CaloExtension> caloExtension(const xAOD::IParticle& particle) const override final;

  virtual const Trk::CaloExtension*  caloExtension( const xAOD::IParticle& particle, 
                                                    IParticleCaloExtensionTool::Cache& cache ) const override final;

  virtual  const Trk::CaloExtension* caloExtension( const xAOD::IParticle& particle,
                                                    const CaloExtensionCollection& cache ) const override final;

  virtual StatusCode  caloExtensionCollection( const xAOD::IParticleContainer& particles, 
                                               const std::vector<bool>& mask,
                                               CaloExtensionCollection& caloextensions) const override final;

  virtual std::unique_ptr<Trk::CaloExtension> caloExtension( const TrackParameters& startPars, 
                                                             PropDirection propDir, 
                                                             ParticleHypothesis particleType ) const  override final;


private:

  const xAOD::TrackParticle* getTrackParticle(const xAOD::IParticle& particle ) const;
  std::unique_ptr<Trk::CaloExtension>  caloExtension( const xAOD::TruthParticle& particle ) const;
  std::unique_ptr<Trk::CaloExtension> caloExtension( const xAOD::NeutralParticle& particle ) const;
  std::unique_ptr<Trk::CaloExtension>  caloExtension( const xAOD::TrackParticle& particle ) const;

  PublicToolHandle<Trk::IExtrapolator> m_extrapolator {this, "Extrapolator", "Trk::Extrapolator/AtlasExtrapolator"};
  Gaudi::Property<std::string>  m_particleTypeName{this,"ParticleType","muon","The particle type : muon, pion, nonInteracting"};
  Gaudi::Property<bool>  m_startFromPerigee{this,"StartFromPerigee",false, "Start from Perigee"};

  const AtlasDetectorID* m_detID;
  ParticleHypothesis  m_particleType ;
};
}

#endif
