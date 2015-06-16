///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// xAODtoHepMCCnvAlg.h
// Header file for class xAODtoHepMCCnvAlg
// Authors: Josh McFayden, James Catmore
///////////////////////////////////////////////////////////////////
#ifndef DERIVATIONFRAMEWORK_XAODTOHEPMCCNVALG_H
#define DERIVATIONFRAMEWORK_XAODTOHEPMCCNVALG_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"

#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"

namespace HepMC {
    class GenEvent; class GenParticle; class GenVertex;
}

namespace DerivationFramework {
    
    class xAODtoHepMCCnvAlg : public AthAlgorithm
    {
        
        ///////////////////////////////////////////////////////////////////
        // Public methods:
        ///////////////////////////////////////////////////////////////////
    public:
        
        // Copy constructor:
        
        /// Constructor with parameters:
        xAODtoHepMCCnvAlg( const std::string& name, ISvcLocator* pSvcLocator );
        
        // Athena algorithm's Hooks
        StatusCode  initialize();
        StatusCode  execute();
        StatusCode  finalize();
        
        ///////////////////////////////////////////////////////////////////
        // Const methods:
        ///////////////////////////////////////////////////////////////////
        
        ///////////////////////////////////////////////////////////////////
        // Non-const methods:
        ///////////////////////////////////////////////////////////////////
        
        const HepMC::GenEvent createHepMCEvent(const xAOD::TruthEvent*);
	HepMC::GenVertex* vertexHelper(const xAOD::TruthVertex*,std::map<const xAOD::TruthVertex*,HepMC::GenVertex*>&,bool&);
	HepMC::GenParticle* createHepMCParticle(const xAOD::TruthParticle*);
	HepMC::GenVertex* createHepMCVertex(const xAOD::TruthVertex*);          

        void printxAODEvent(const xAOD::TruthEvent* event) const;
 
        ///////////////////////////////////////////////////////////////////
        // Private data:
        ///////////////////////////////////////////////////////////////////
    private:
        
        /// Input container key (job property)
        float m_momFac,m_lenFac;
        std::string m_xaodTruthEventKey;
        
        /// Output container key (job property)
        std::string m_outKey;

        /// Flag to keep signal event only
        bool m_signalOnly;
        int m_maxCount;

        /// Counter
        int m_evtCount;
        int m_badSuggest;
        int m_noProdVtx;

    };
    
}



#endif
