/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef PARTICLESINCONETOOLS_TRUTHPARTICLESINCONETOOL_H
#define PARTICLESINCONETOOLS_TRUTHPARTICLESINCONETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/ReadHandleKey.h"

#include "ParticlesInConeTools/ITruthParticlesInConeTool.h"
#include "IParticlesLookUpTable.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"

namespace xAOD {

    class TruthParticlesInConeTool: public AthAlgTool, virtual public ITruthParticlesInConeTool{
    public:
        /** constructor */
        TruthParticlesInConeTool(const std::string& type, const std::string& name, const IInterface* parent);

        /** destructor */
        ~TruthParticlesInConeTool(void); 

        /** initialize */
        StatusCode initialize() final;

        /** finalize */
        StatusCode finalize() final;

        /**ITruthParticlesInConeTool interface */    
        bool particlesInCone( float eta, float phi, float dr, std::vector< const TruthParticle*>& output ) const final;

        typedef IParticlesLookUpTable<TruthParticle> LookUpTable;

    private:

        // init look-up table
        const LookUpTable* getTable() const;

        /** Truth Particle collection name */
        SG::ReadHandleKey<TruthParticleContainer> m_truthParticleLocation {this,
	    "TruthParticleLocation", "TruthParticles"};
    };
}	// end of namespace

CLASS_DEF( xAOD::TruthParticlesInConeTool::LookUpTable, 151156931 , 0 )
#endif


