/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

// TRACK TOOL INCLUDES
#include "TrackTools.h"
#include "ITrackToVertex/ITrackToVertex.h"


// MONTE CARLO (TRUTH) INFORMATION
#include "GeneratorObjects/McEventCollection.h"

// xAOD Muon
#include "xAODMuon/MuonContainer.h"



// ADD TILEMUONTRACKFILLERTOOL TO D3PD NAMESPACE
namespace D3PD {
//=========================================================================
class TileMuonFillerTool:public D3PD::BlockFillerTool<xAOD::Muon>{
//=========================================================================

    public:
        // CONSTRUCTOR
        TileMuonFillerTool(const std::string& type,const std::string& name,const IInterface* parent);
        ~TileMuonFillerTool(){ };

        // FUNCTIONS
        virtual StatusCode initialize();
        virtual StatusCode book();
        virtual StatusCode fill (const xAOD::Muon &p);

    private:
        virtual StatusCode book2();

        // DATA MEMBERS
        float m_defaultValue;          // DEFAULT PARAMETER VALUE
        std::string m_standalone;

        // LEVEL OF DETAILS
        short m_LevelOfDetails;
        short m_trackType;

        short * m_muonAuthor;
        short * m_muonType;
        short * m_muonQuality;

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
        unsigned short *m_numberOfPixelDeadSensors;
        unsigned short *m_numberOfSCTDeadSensors;



        // MUON TRACK PARAMETERS
        float *m_pT;
        float *m_p;
        float *m_charge;
        float *m_eta;
        float *m_phi;

        // MUON QUALITY INFORMATION
        float *m_matchChi2;
        float *m_matchChi2OverDoF;
        short *m_matchNumberDoF;
        float *m_fitChi2;
        float *m_fitNumberDoF;
        float *m_fitChi2OverDoF;
        bool  *m_bestMatch;
        bool  *m_isCorrected;

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

        // MONTE CARLO TRUTH VARIABLES
        StatusCode m_truthRetrieved;
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
        ToolHandle<ITrackTools>      m_trackInCalo;
        ToolHandle<Reco::ITrackToVertex>   m_trackToVertexTool;
        std::string m_calocellContainerName;         // INPUT CELL CONTAINER
        std::string m_MCeventCollection;
        const McEventCollection* m_MCtruth;

}; // class TrkTrackParametersAtBLFillerTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_TRKTRACKPARAMETERSATBLFILLERTOOL_H
