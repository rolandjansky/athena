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
        declareProperty("TruthParticleLocation",m_truthParticleLocation = "TruthParticles");
    }

    TruthParticlesInConeTool::~TruthParticlesInConeTool(){
    }

    StatusCode TruthParticlesInConeTool::initialize() {
        return StatusCode::SUCCESS;
    }

    StatusCode TruthParticlesInConeTool::finalize() {
        return StatusCode::SUCCESS;
    }

    const TruthParticleContainer* TruthParticlesInConeTool::retrieveTruthParticleContainer() const {
        const TruthParticleContainer* truthParticles = 0;
        if(evtStore()->contains<TruthParticleContainer>(m_truthParticleLocation)) {
            if(evtStore()->retrieve(truthParticles,m_truthParticleLocation).isFailure()) {
                ATH_MSG_FATAL( "Unable to retrieve " << m_truthParticleLocation );
                return 0;
            }
        }
        return truthParticles;
    }

    const TruthParticlesInConeTool::LookUpTable* TruthParticlesInConeTool::getTable() const{
        const std::string tableName = name() + "LookUpTable";
        SG::ReadHandle<LookUpTable> rh (tableName);
        if (rh.isValid()){
            return &*rh;
        }
        const TruthParticleContainer* truthParticles = retrieveTruthParticleContainer();
        if( !truthParticles ) {return nullptr;}
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
