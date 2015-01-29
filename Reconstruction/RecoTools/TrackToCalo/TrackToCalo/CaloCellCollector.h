/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKTOCALO_CALOCELLCOLLECTOR_H
#define TRACKTOCALO_CALOCELLCOLLECTOR_H

#include "CaloGeoHelpers/CaloSampling.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include <vector>

// forward declarations
namespace Trk  { class CaloExtension; }
class CaloCellContainer;


namespace Rec {

    class CaloCellCollector {
    public:
        
        CaloCellCollector ();

        xAOD::CaloCluster* collectCells( const Trk::CaloExtension& extension, 
                                         const CaloCellContainer& cellContainer, 
                                         xAOD::CaloClusterContainer& clusterContainer ) const;
        
    private:
        std::vector<std::pair<float, float> > m_dEtadPhi;
        std::vector<CaloSampling::CaloSample> m_samples;
        std::vector<CaloSampling::CaloSample> m_samplesForDR;
        bool m_doDebug;
    };
}

#endif
