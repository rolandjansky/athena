/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * File: TileCaloClusterFillerTool.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>, Gang Qin <gang.qin@cern.ch> 
 * Date: September 2012
 *
 */

#ifndef TILECALOCLUSTERFILLERTOOL_H
#define TILECALOCLUSTERFILLERTOOL_H

// INCLUDE HEADER FILES
// D3PD INCLUDES
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "Identifier/HWIdentifier.h"

// GENERIC INCLUDES
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"

// PHYSICS INCLUDES
#include "muonEvent/Muon.h"

// TRACK INCLUDES
#include "Particle/TrackParticle.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterContainer.h"

// TRACK TOOL INCLUDES
#include "ITrackTools.h"
#include "TrackTools.h"
#include "ITrackInCaloTools/ITrackExtrapolatorToCalo.h"
#include "TrackToVertex/TrackToVertex.h"

// CALORIMETER INCLUDES
#include "CaloEvent/CaloCell.h"
#include "TileEvent/TileCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"

// MONTE CARLO INCLUDES
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"


// MONTE CARLO (TRUTH) INFORMATION
#include "GeneratorObjects/McEventCollection.h"

// C++ INCLUDES
#include <vector>
#include <algorithm>
#include <math.h>
#include <string>


class CaloCluster;
class CaloClusterContainer;

using namespace TICT;

// ADD TILECALOCLUSTERFILLERTOOL TO D3PD NAMESPACE
namespace D3PD {
//=========================================================================
class TileCaloClusterFillerTool:public D3PD::BlockFillerTool<CaloCluster>{
//=========================================================================

    public:
        // CONSTRUCTOR
        TileCaloClusterFillerTool(const std::string& type,const std::string& name,const IInterface* parent);

        // FUNCTIONS
        virtual StatusCode initialize();
        virtual StatusCode book();
        virtual StatusCode fill (const CaloCluster &p);

    private:
        // DATA MEMBERS
        float m_defaultValue;          // DEFAULT PARAMETER VALUE

        // VARIABLES
        float *m_e;
        float *m_eta;
        float *m_phi;
        int *m_nSamples;
        float *m_getTime;
        int *m_getClusterSize;
        unsigned int *m_getClusterEtaSize;
        unsigned int *m_getClusterPhiSize;
        float *m_eta0;
        float *m_phi0;

        std::vector<float> *m_eSample;
        std::vector<float> *m_etaSample;
        std::vector<float> *m_phiSample;

        std::string m_caloclusterContainerName;         // INPUT cluster CONTAINER
        const CaloClusterContainer* m_caloclusterContainer;

        // LEVEL OF DETAILS
        short m_LevelOfDetails;

        // TOOLHANDLES
        StoreGateSvc* m_storeGate;
        ToolHandle<ITrackTools>      m_inCalo;

}; // class CaloClusterContainer 

} // namespace D3PD

#endif // not TRACKD3PDMAKER_TRKTRACKPARAMETERSATBLFILLERTOOL_H
