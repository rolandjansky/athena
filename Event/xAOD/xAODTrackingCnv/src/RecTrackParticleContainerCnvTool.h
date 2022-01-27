// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODTRACKINGCNV_RECTRACKPARTICLECONTAINERCNVTOOL_H
#define XAODTRACKINGCNV_RECTRACKPARTICLECONTAINERCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// converting tool that will be invoked
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"

// Local include(s):
#include "xAODTrackingCnv/IRecTrackParticleContainerCnvTool.h"
#include "TrkValInterfaces/ITrkObserverTool.h"

namespace xAODMaker {

  class RecTrackParticleContainerCnvTool : public AthAlgTool,
			       public virtual IRecTrackParticleContainerCnvTool {

  public:
    /// Regular AlgTool constructor
    RecTrackParticleContainerCnvTool( const std::string& type, const std::string& name,
			  const IInterface* parent );
    
    /// Function initialising the tool
    virtual StatusCode initialize() override;
    
    /// Function that fills an existing xAOD::TrackParticleContainer
    virtual StatusCode convert( const Rec::TrackParticleContainer* aod,
			       xAOD::TrackParticleContainer* xaod ) const override;

    /// Function that fills an existing xAOD::TrackParticleContainer and augments track particles
    virtual StatusCode convertAndAugment( const Rec::TrackParticleContainer* aod,
						 xAOD::TrackParticleContainer* xaod, const ObservedTrackMap* trk_map ) const override;

    /// allow other algorithms to pass the tool in order to preserve initialisation
    virtual StatusCode setParticleCreatorTool(ToolHandle<Trk::ITrackParticleCreatorTool> *tool) override;

  private:
    ToolHandle<Trk::ITrackParticleCreatorTool> m_particleCreator;
    
    inline xAOD::TrackParticle* createParticle(xAOD::TrackParticleContainer* xaod, const Rec::TrackParticleContainer* container, const Rec::TrackParticle* tp) const;
    
    
  }; // class RecTrackParticleContainerCnvTool definition

} // xAODMaker namespace

#endif // XAODTRACKINGCNV_RECTRACKPARTICLECONTAINERCNVTOOL_H
