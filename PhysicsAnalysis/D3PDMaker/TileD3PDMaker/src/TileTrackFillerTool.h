/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 * File: TileTrackFillerTool.h
 * Author: Marco van Woerden <mvanwoer@cern.ch>, Gang Qin <gang.qin@cern.ch>, Archil Durglishvili <archil.durglishvili@cern.ch>
 * Date: Nov 2014
 * 
 */

#ifndef TILETRACKFILLERTOOL_H
#define TILETRACKFILLERTOOL_H

// INCLUDE HEADER FILES
// D3PD INCLUDES
#include "D3PDMakerUtils/BlockFillerTool.h"

// TRACK TOOL INCLUDES
#include "TrackTools.h"
#include "ITrackToVertex/ITrackToVertex.h"

// MONTE CARLO INCLUDES
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenEvent.h"

// MONTE CARLO (TRUTH) INFORMATION
#include "GeneratorObjects/McEventCollection.h"

using namespace std;

// ADD TILEMUONTRACKFILLERTOOL TO D3PD NAMESPACE
namespace D3PD {
//=========================================================================
class TileTrackFillerTool:public D3PD::BlockFillerTool<xAOD::TrackParticle>{
//=========================================================================

    public:
        // CONSTRUCTOR
        TileTrackFillerTool(const std::string& type,const std::string& name,const IInterface* parent);

        // FUNCTIONS
        virtual StatusCode initialize();
        virtual StatusCode book();
        virtual StatusCode fill (const xAOD::TrackParticle &p);

    private:
        // DATA MEMBERS
        float m_defaultValue;          // DEFAULT PARAMETER VALUE
        int m_trkParType;
        std::string m_standalone;

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


        float *m_ptcone10;
        float *m_ptcone20;
        float *m_ptcone30;
        float *m_ptcone40;
        float *m_ptcone45;

        float *m_pt; 
        float *m_p;


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
	std::vector<float>* m_y;
	std::vector<float>* m_z;

        // TOOLHANDLES
        StoreGateSvc* m_storeGate;
        ToolHandle<ITrackTools>      m_trackInCalo;
        ToolHandle<Reco::ITrackToVertex>   m_trackToVertexTool;
        std::string m_calocellContainerName;         // INPUT CELL CONTAINER
        std::string m_MCeventCollection;

}; // class TrkTrackParametersAtBLFillerTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_TRKTRACKPARAMETERSATBLFILLERTOOL_H
