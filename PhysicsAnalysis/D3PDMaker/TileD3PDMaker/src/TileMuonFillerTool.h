/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * File: TileMuonFillerTool.cxx
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Date: September 2012
 *
 */

#ifndef TILEMUONFILLERTOOL_H
#define TILEMUONFILLERTOOL_H

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

// TRACK TOOL INCLUDES
#include "ITrackTools.h"
#include "TrackTools.h"
#include "ITrackInCaloTools/ITrackExtrapolatorToCalo.h"
#include "TrackToVertex/TrackToVertex.h"

// CALORIMETER INCLUDES
#include "CaloEvent/CaloCell.h"

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

// ADD TILEMUONTRACKFILLERTOOL TO D3PD NAMESPACE
namespace D3PD {
//=========================================================================
class TileMuonFillerTool:public D3PD::BlockFillerTool<Analysis::Muon>{
//=========================================================================

    public:
        // CONSTRUCTOR
        TileMuonFillerTool(const std::string& type,const std::string& name,const IInterface* parent);

        // FUNCTIONS
        virtual StatusCode initialize();
        virtual StatusCode book();
        virtual StatusCode fill (const Analysis::Muon &p);

    private:
        virtual StatusCode book2();

        // DATA MEMBERS
        float m_defaultValue;          // DEFAULT PARAMETER VALUE
        std::string m_standalone;

        // LEVEL OF DETAILS
        short m_LevelOfDetails;
        short m_trackType;
        short *m_mutype;

        unsigned short *m_numberOfBLayerHits;
        unsigned short *m_numberOfPixelHits;
        unsigned short *m_numberOfSCTHits;
        unsigned short *m_numberOfTRTHits;
        unsigned short *m_numberOfTRTHighThresholdHits;
        unsigned short *m_numberOfBLayerSharedHits;
        unsigned short *m_numberOfPixelSharedHits;
        unsigned short *m_numberOfPixelHoles;
        unsigned short *m_numberOfSCTSharedHits;
        unsigned short *m_numberOfSCTHoles;
        unsigned short *m_numberOfTRTOutliers;
        unsigned short *m_numberOfTRTHighThresholdOutliers;
        unsigned short *m_numberOfMDTHits;
        unsigned short *m_numberOfMDTHoles;
        unsigned short *m_numberOfCSCEtaHits;
        unsigned short *m_numberOfCSCEtaHoles;
        unsigned short *m_numberOfCSCPhiHits;
        unsigned short *m_numberOfCSCPhiHoles;
        unsigned short *m_numberOfRPCEtaHits;
        unsigned short *m_numberOfRPCEtaHoles;
        unsigned short *m_numberOfRPCPhiHits;
        unsigned short *m_numberOfRPCPhiHoles;
        unsigned short *m_numberOfTGCEtaHits;
        unsigned short *m_numberOfTGCEtaHoles;
        unsigned short *m_numberOfTGCPhiHits;
        unsigned short *m_numberOfTGCPhiHoles;
        unsigned short *m_numberOfGangedPixels;
        unsigned short *m_numberOfOutliersOnTrack;
        unsigned short *m_numberOfTrackSummaryTypes;

        // MUON TRACK PARAMETERS
        float *m_pT;
        float *m_p;
        float *m_charge;
        float *m_eta;
        float *m_phi;
        float *m_beta;

        // MUON QUALITY INFORMATION
        float *m_matchChi2;
        float *m_matchChi2OverDoF;
        short *m_matchNumberDoF;
        float *m_fitChi2;
        float *m_fitNumberDoF;
        float *m_fitChi2OverDoF;
        bool  *m_bestMatch;
        bool  *m_isCorrected;
        bool  *m_hasCombinedMuon;
        bool  *m_hasInDetTrackParticle;
        bool  *m_hasMuonExtrapolatedTrackParticle;
        bool  *m_hasInnerExtrapolatedTrackParticle;
        bool  *m_hasCombinedMuonTrackParticle;
        bool  *m_hasCluster;
        bool  *m_isLoose;
        bool  *m_isMedium;
        bool  *m_isTight;

        float *m_nucone10;
        float *m_nucone20;
        float *m_nucone30;
        float *m_nucone40;
        float *m_etcone10;
        float *m_etcone20;
        float *m_etcone30;
        float *m_etcone40;
        float *m_ptcone10;
        float *m_ptcone20;
        float *m_ptcone30;
        float *m_ptcone40;
        float *m_vtxX;
        float *m_vtxY;
        float *m_vtxZ;
        float *m_d0;
        float *m_z0;
        float *m_phi0;
        float *m_theta;
        float *m_qOverP;

        // ISOLATION ENERGY VARIABLES
        float *m_LAr_iso_cone05;
        float *m_LAr_iso_cone10;
        float *m_LAr_iso_cone20;
        float *m_LAr_iso_cone30;
        float *m_LAr_iso_cone40;
        float *m_Tile_iso_cone05;
        float *m_Tile_iso_cone10;
        float *m_Tile_iso_cone20;
        float *m_Tile_iso_cone30;
        float *m_Tile_iso_cone40;
        float *m_LAr_cone05;
        float *m_LAr_cone10;
        float *m_LAr_cone20;
        float *m_LAr_cone30;
        float *m_LAr_cone40;
        float *m_Tile_cone05;
        float *m_Tile_cone10;
        float *m_Tile_cone20;
        float *m_Tile_cone30;
        float *m_Tile_cone40;

        // COORDINATES PER LAYER
        float *m_PreSamplerB_X;
        float *m_PreSamplerB_Y;
        float *m_PreSamplerB_Z;
        float *m_PreSamplerB_Eta;
        float *m_PreSamplerB_Phi;
        float *m_EMB1_X;
        float *m_EMB1_Y;
        float *m_EMB1_Z;
        float *m_EMB1_Eta;
        float *m_EMB1_Phi;
        float *m_EMB2_X;
        float *m_EMB2_Y;
        float *m_EMB2_Z;
        float *m_EMB2_Eta;
        float *m_EMB2_Phi;
        float *m_EMB3_X;
        float *m_EMB3_Y;
        float *m_EMB3_Z;
        float *m_EMB3_Eta;
        float *m_EMB3_Phi;
        float *m_PreSamplerE_X;
        float *m_PreSamplerE_Y;
        float *m_PreSamplerE_Z;
        float *m_PreSamplerE_Eta;
        float *m_PreSamplerE_Phi;
        float *m_EME1_X;
        float *m_EME1_Y;
        float *m_EME1_Z;
        float *m_EME1_Eta;
        float *m_EME1_Phi;
        float *m_EME2_X;
        float *m_EME2_Y;
        float *m_EME2_Z;
        float *m_EME2_Eta;
        float *m_EME2_Phi;
        float *m_EME3_X;
        float *m_EME3_Y;
        float *m_EME3_Z;
        float *m_EME3_Eta;
        float *m_EME3_Phi;
        float *m_HEC0_X;
        float *m_HEC0_Y;
        float *m_HEC0_Z;
        float *m_HEC0_Eta;
        float *m_HEC0_Phi;
        float *m_HEC1_X;
        float *m_HEC1_Y;
        float *m_HEC1_Z;
        float *m_HEC1_Eta;
        float *m_HEC1_Phi;
        float *m_HEC2_X;
        float *m_HEC2_Y;
        float *m_HEC2_Z;
        float *m_HEC2_Eta;
        float *m_HEC2_Phi;
        float *m_HEC3_X;
        float *m_HEC3_Y;
        float *m_HEC3_Z;
        float *m_HEC3_Eta;
        float *m_HEC3_Phi;
        float *m_TileBar0_X;
        float *m_TileBar0_Y;
        float *m_TileBar0_Z;
        float *m_TileBar0_Eta;
        float *m_TileBar0_Phi;
        float *m_TileBar1_X;
        float *m_TileBar1_Y;
        float *m_TileBar1_Z;
        float *m_TileBar1_Eta;
        float *m_TileBar1_Phi;
        float *m_TileBar2_X;
        float *m_TileBar2_Y;
        float *m_TileBar2_Z;
        float *m_TileBar2_Eta;
        float *m_TileBar2_Phi;
        float *m_TileGap1_X;
        float *m_TileGap1_Y;
        float *m_TileGap1_Z;
        float *m_TileGap1_Eta;
        float *m_TileGap1_Phi;
        float *m_TileGap2_X;
        float *m_TileGap2_Y;
        float *m_TileGap2_Z;
        float *m_TileGap2_Eta;
        float *m_TileGap2_Phi;
        float *m_TileGap3_X;
        float *m_TileGap3_Y;
        float *m_TileGap3_Z;
        float *m_TileGap3_Eta;
        float *m_TileGap3_Phi;
        float *m_TileExt0_X;
        float *m_TileExt0_Y;
        float *m_TileExt0_Z;
        float *m_TileExt0_Eta;
        float *m_TileExt0_Phi;
        float *m_TileExt1_X;
        float *m_TileExt1_Y;
        float *m_TileExt1_Z;
        float *m_TileExt1_Eta;
        float *m_TileExt1_Phi;
        float *m_TileExt2_X;
        float *m_TileExt2_Y;
        float *m_TileExt2_Z;
        float *m_TileExt2_Eta;
        float *m_TileExt2_Phi;

//         // COORDINATES PER LAYER
//         float *m_psX;
//         float *m_em1X;
//         float *m_em2X;
//         float *m_em3X;
//         float *m_tile1X;
//         float *m_tile2X;
//         float *m_tile3X;
//         float *m_hec0X;
//         float *m_hec1X;
//         float *m_hec2X;
//         float *m_hec3X;
//         float *m_psY;
//         float *m_em1Y;
//         float *m_em2Y;
//         float *m_em3Y;
//         float *m_tile1Y;
//         float *m_tile2Y;
//         float *m_tile3Y;
//         float *m_hec0Y;
//         float *m_hec1Y;
//         float *m_hec2Y;
//         float *m_hec3Y;
//         float *m_psZ;
//         float *m_em1Z;
//         float *m_em2Z;
//         float *m_em3Z;
//         float *m_tile1Z;
//         float *m_tile2Z;
//         float *m_tile3Z;
//         float *m_hec0Z;
//         float *m_hec1Z;
//         float *m_hec2Z;
//         float *m_hec3Z;
//         float *m_psEta;
//         float *m_em1Eta;
//         float *m_em2Eta;
//         float *m_em3Eta;
//         float *m_tile1Eta;
//         float *m_tile2Eta;
//         float *m_tile3Eta;
//         float *m_hec0Eta;
//         float *m_hec1Eta;
//         float *m_hec2Eta;
//         float *m_hec3Eta;
//         float *m_psPhi;
//         float *m_em1Phi;
//         float *m_em2Phi;
//         float *m_em3Phi;
//         float *m_tile1Phi;
//         float *m_tile2Phi;
//         float *m_tile3Phi;
//         float *m_hec0Phi;
//         float *m_hec1Phi;
//         float *m_hec2Phi;
//         float *m_hec3Phi;

        // MONTE CARLO TRUTH VARIABLES
        StatusCode truthRetrieved;
        float *m_truth_deltaR;
        float *m_truth_vtxX;
        float *m_truth_vtxY;
        float *m_truth_vtxZ;
        float *m_truth_eta;
        float *m_truth_phi;
        float *m_truth_p;
        float *m_truth_pT;
        float *m_truth_charge;
        float *m_truth_mass;

        // TOOLHANDLES
        StoreGateSvc* m_storeGate;
        ToolHandle<ITrackTools>      m_inCalo;
        ToolHandle<Reco::ITrackToVertex>   m_trackToVertexTool;
        std::string m_calocellContainerName;         // INPUT CELL CONTAINER
        std::string m_MCeventCollection;
        const McEventCollection* m_MCtruth;

}; // class TrkTrackParametersAtBLFillerTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_TRKTRACKPARAMETERSATBLFILLERTOOL_H
