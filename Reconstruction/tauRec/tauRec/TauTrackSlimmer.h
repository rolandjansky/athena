/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TRACKSLIMMER_TAU_H
#define TAUREC_TRACKSLIMMER_TAU_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IThinningSvc.h"
#include "ParticleEvent/ParticleBase.h"
#include "tauRecTools/TauEventData.h"

#include <string>

class StoreGateSvc;

/**
 * @brief Class for tau tracks slimming.
 * 
 * @author Anna Kaczmarska, Lukasz Janyst
 */

class TauTrackSlimmer: public AthAlgorithm
{
    public:

        //-----------------------------------------------------------------
        // Contructor and destructor
        //-----------------------------------------------------------------
        TauTrackSlimmer( const std::string &name, ISvcLocator *pSvcLocator );
        ~TauTrackSlimmer();

        //-----------------------------------------------------------------
        // Gaudi algorithm hooks
        //-----------------------------------------------------------------
        virtual StatusCode initialize();
        virtual StatusCode execute();
        virtual StatusCode finalize();

    private:
        std :: string                 m_tauContainerName;
        ServiceHandle<IThinningSvc>   m_thinningSvc;
        bool                          m_filterTaus;
        unsigned                      m_maxNTrack;
        ChargeType                    m_maxCharge;
        double                        m_maxEmRadius;
        double                        m_maxIsoFrac;

	TauEventData              m_data;//not used
};

#endif // TAUREC_TRACKSLIMMER_TAU_H
