/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
   */

//////////////////////////////////////////////////////////////////////////////
// TruthParticlesInConeTool
//
//  (c) ATLAS software
//////////////////////////////////////////////////////////////////////////////

#include "TruthParticlesInConeTool.h"

namespace xAOD {

    TruthParticlesInConeTool::TruthParticlesInConeTool (const std::string& type, const std::string& name, const IInterface* parent)
        :	AthAlgTool(type, name, parent){
        declareInterface<ITruthParticlesInConeTool>(this);
    }

    TruthParticlesInConeTool::~TruthParticlesInConeTool(){
    }

    StatusCode TruthParticlesInConeTool::initialize() {
        ATH_CHECK(m_truthParticleLocation.initialize());
        return StatusCode::SUCCESS;
    }

    StatusCode TruthParticlesInConeTool::finalize() {
        return StatusCode::SUCCESS;
    }

    const TruthParticlesInConeTool::LookUpTable* TruthParticlesInConeTool::getTable() const{
        const std::string tableName = name() + "LookUpTable";
        SG::ReadHandle<LookUpTable> rh (tableName);
        if (rh.isValid()){
            return &*rh;
        }
	SG::ReadHandle<TruthParticleContainer> truthParticles(m_truthParticleLocation);
        if( !truthParticles.isValid() ) {
	  ATH_MSG_ERROR("Could not open a TruthParticle container with key " << 
			m_truthParticleLocation.key());
	  return nullptr;
	}
        auto lut = std::make_unique<LookUpTable>();
        lut->init(*truthParticles);
        SG::WriteHandle<LookUpTable> wh (tableName);
        return wh.put (std::move (lut), true);
    }

    bool TruthParticlesInConeTool::particlesInCone( float eta, float phi, float dr, 
            std::vector< const TruthParticle*>& output ) const {

        /// initialize if needed
        const LookUpTable* lut = getTable();
        return lut->iParticlesInCone( eta, phi, dr, output );
    }
}	// end of namespace
