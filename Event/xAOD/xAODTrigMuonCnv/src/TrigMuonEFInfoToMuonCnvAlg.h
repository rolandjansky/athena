/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  TrigMuonEFInfoToMuonCnvAlg.h
//
//  Created by Mark Owen on 13/03/2014.
//

#ifndef xAODTrigMuonCnv_TrigMuonEFInfoToMuonCnvAlg_h
#define xAODTrigMuonCnv_TrigMuonEFInfoToMuonCnvAlg_h

// System include(s):
#include <string>

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Local includes
#include "xAODTrigMuonCnv/ITrigMuonEFInfoToMuonCnvTool.h"

namespace xAODMaker {
    
    /**
     *  @short Algorithm creating xAOD::Muons from EF Muons (class TrigMuonEFInfo)
     *
     *         This algorithm can be used to translate the EF Muons coming
     *         from an AOD, and create xAOD::Muon objects out of them
     *         for an output xAOD.
     *
     * @author Mark Owen <markowen@cern.ch>
     *
     */
    class TrigMuonEFInfoToMuonCnvAlg : public AthAlgorithm {
        
    public:
        /// Regular algorithm constructor
        TrigMuonEFInfoToMuonCnvAlg( const std::string& name, ISvcLocator* svcLoc );
        
        /// Function initialising the algorithm
        virtual StatusCode initialize();
        /// Function initialising the algorithm
        virtual StatusCode finalize();
        /// Function executing the algorithm
        virtual StatusCode execute();
        
    private:
        /// The key of the input TrigMuonEFInfoContainer
        std::string m_aodContainerName;
        /// The key for the output xAOD::MuonContainer
        std::string m_xaodMuonContainerName;
        /// The key for the output xAOD::TrackParticleContainer for combined tracks
        std::string m_xaodCombTrkPartContainerName;
        /// The key for the output xAOD::TrackParticleContainer for extrapolated tracks
        std::string m_xaodExtrapTrkPartContainerName;
        
        /// Tool that actually does the conversion
        ToolHandle<ITrigMuonEFInfoToMuonCnvTool> m_converterTool;
       
	/// counters
	unsigned int m_nMuonsCreated;
	unsigned int m_nCombTrksCreated;
	unsigned int m_nExtrapTrksCreated;
	unsigned int m_nproc;
 
        /// Utility function to record xAOD containers
        template<class CONT, class AUXCONT>
        StatusCode recordxAODContainers(const std::string& containerName, const std::string& auxContainerName, CONT& xaod, AUXCONT& aux);
        
    };//class MuonCnvAlg
    
}//namespace xAODMaker

#include "TrigMuonEFInfoToMuonCnvAlg.icc"

#endif // xAODTrigMuonCnv_TrigMuonEFInfoToMuonCnvAlg_h
