/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * File: TileTrackFillerTool.h
 * Author: Marco van Woerden <mvanwoer@cern.ch>, Gang Qin <gang.qin@cern.ch>
 * Date: May 2013
 *
 */

#ifndef TILETRACKFILLERTOOL_H
#define TILETRACKFILLERTOOL_H

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
//#include "TrackIsolationTools/TrackIsolationTool.h"
#include "IsolationTool/ITrackIsolationTool.h"

// TRACK INCLUDES
#include "Particle/TrackParticle.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "ITrackToVertex/ITrackToVertex.h"

// TRACK TOOL INCLUDES
#include "ITrackTools.h"
#include "TrackTools.h"
#include "ITrackInCaloTools/ITrackExtrapolatorToCalo.h"
#include "TrackToVertex/TrackToVertex.h"

// CALORIMETER INCLUDES
#include "CaloEvent/CaloCell.h"
#include "TileEvent/TileCell.h"
#include "CaloEvent/CaloCellContainer.h"

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

namespace Rec{
class TrackParticle;
}
namespace Trk{
class Track;
}

using namespace TICT;
using namespace std;

// ADD TILEMUONTRACKFILLERTOOL TO D3PD NAMESPACE
namespace D3PD {
//=========================================================================
class TileTrackFillerTool:public D3PD::BlockFillerTool<Rec::TrackParticle>{
//=========================================================================

    public:
        // CONSTRUCTOR
        TileTrackFillerTool(const std::string& type,const std::string& name,const IInterface* parent);

        // FUNCTIONS
        virtual StatusCode initialize();
        virtual StatusCode book();
        virtual StatusCode fill (const Rec::TrackParticle &p);

    private:
        // DATA MEMBERS
        float m_defaultValue;          // DEFAULT PARAMETER VALUE
        std::string m_standalone;
        const CaloCellContainer* m_calocellContainer;

        // LEVEL OF DETAILS
        short m_LevelOfDetails;

        // TRACK PARAMETERS
        int *m_nucone05;
        int *m_nucone10;
        int *m_nucone20;
        int *m_nucone30;
        int *m_nucone40;

	int *m_numberOfBLayerHits;
	int *m_numberOfPixelHits;
	int *m_numberOfSCTHits;
	int *m_numberOfTRTHits;
	int *m_numberOfTRTHighThresholdHits;
	int *m_numberOfBLayerSharedHits;
	int *m_numberOfPixelSharedHits;
	int *m_numberOfPixelHoles;
	int *m_numberOfSCTSharedHits;
	int *m_numberOfSCTHoles;
	int *m_numberOfTRTOutliers;
	int *m_numberOfTRTHighThresholdOutliers;
	int *m_numberOfGangedPixels;
	int *m_numberOfOutliersOnTrack;
	int *m_numberOfTrackSummaryTypes;
	int *m_numberOfFriendTracks; 


        float *m_ptcone05;
        float *m_ptcone10;
        float *m_ptcone20;
        float *m_ptcone30;
        float *m_ptcone40;
	float dif_eta;
	float dif_phi;
        float *m_pt; 

        float *m_vtxX;
        float *m_vtxY;
        float *m_vtxZ;
        float *m_d0;
        float *m_z0;
        float *m_phi0;
        float *m_theta;
        float *m_qOverP;


	std::vector<float>* m_eta;
	std::vector<float>* m_phi;
	std::vector<float>* m_x;
	std::vector<float>* m_xx;
	std::vector<float>* m_y;
	std::vector<float>* m_z;

        // TOOLHANDLES
        StoreGateSvc* m_storeGate;
        ToolHandle<ITrackTools>      m_inCalo;
        ToolHandle<xAOD::ITrackIsolationTool>      m_track_iso_tool;
        ToolHandle<Reco::ITrackToVertex>   m_trackToVertexTool;
        std::string m_calocellContainerName;         // INPUT CELL CONTAINER
        std::string m_MCeventCollection;
        const McEventCollection* m_MCtruth;

}; // class TrkTrackParametersAtBLFillerTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_TRKTRACKPARAMETERSATBLFILLERTOOL_H
