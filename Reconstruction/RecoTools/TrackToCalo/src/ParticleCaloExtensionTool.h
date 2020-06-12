/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/*
 * ParticleCaloExtensionTool.h  - implements the IParticleCaloExtenions Interface
 * begin : Summer 2014
 * updated : 2018-2019 for AthenaMT
 * authors : Niels van Eldik (CERN PH-ATC),Christos Anastopoulos
 */

#ifndef TRKPARTICLECREATOR_PARTICLECALOEXTENSIONTOOL_H
#define TRKPARTICLECREATOR_PARTICLECALOEXTENSIONTOOL_H
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

/*interface */
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"

/* interfce for the extrapolator tool*/

#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkEventPrimitives/ParticleHypothesis.h" 

/* 
 * xAOD includes
 */
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/NeutralParticle.h"
#include "xAODTruth/TruthParticle.h"

class AtlasDetectorID;

namespace Trk {

class IExtrapolator;

class ParticleCaloExtensionTool : virtual public IParticleCaloExtensionTool, public AthAlgTool {
public:

  /** This following "using" statements can be removed after the methods in IParticleCaloExtensionTool.h for the
   * old interfaces WITHOUT EventContext are removed, i.e. only the new ones with EventContext are
   * used throughout the sw */
  using IParticleCaloExtensionTool::caloExtension;
  using IParticleCaloExtensionTool::caloExtensionCollection;
    
    
  ParticleCaloExtensionTool(const std::string&,const std::string&,const IInterface*);
  virtual ~ParticleCaloExtensionTool();

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

  /* 
   * Implement the IParticleCaloExtension methods
   */
  virtual std::unique_ptr<Trk::CaloExtension> caloExtension(const EventContext& ctx,
                                                            const xAOD::IParticle& particle) const override final;

  virtual const Trk::CaloExtension*  caloExtension( const EventContext& ctx,
                                                    const xAOD::IParticle& particle, 
                                                    IParticleCaloExtensionTool::Cache& cache ) const override final;

  virtual  const Trk::CaloExtension* caloExtension( const xAOD::IParticle& particle,
                                                    const CaloExtensionCollection& cache ) const override final;

  virtual StatusCode  caloExtensionCollection( const EventContext& ctx,
                                               const xAOD::IParticleContainer& particles, 
                                               const std::vector<bool>& mask,
                                               CaloExtensionCollection& caloextensions) const override final;

  virtual std::unique_ptr<Trk::CaloExtension> caloExtension( const EventContext& ctx,
                                                             const TrackParameters& startPars, 
                                                             PropDirection propDir, 
                                                             ParticleHypothesis particleType ) const  override final;


private:

  std::unique_ptr<Trk::CaloExtension>  caloExtension( const EventContext& ctx,
                                                      const xAOD::TruthParticle& particle ) const;
  std::unique_ptr<Trk::CaloExtension>  caloExtension( const EventContext& ctx,
                                                      const xAOD::NeutralParticle& particle ) const;
  std::unique_ptr<Trk::CaloExtension>  caloExtension( const EventContext& ctx,
                                                      const xAOD::TrackParticle& particle ) const;

  ToolHandle<Trk::IExtrapolator> m_extrapolator {this, "Extrapolator", ""};
  Gaudi::Property<std::string>  m_particleTypeName{this,"ParticleType","muon",
    "The particle type : muon, pion, electron,nonInteracting"};
  Gaudi::Property<bool>  m_startFromPerigee{this,"StartFromPerigee",false, "Start from Perigee"};
  const AtlasDetectorID* m_detID;
  ParticleHypothesis  m_particleType ;
};
}

#endif
