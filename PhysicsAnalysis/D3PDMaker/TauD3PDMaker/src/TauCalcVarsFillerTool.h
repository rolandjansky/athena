// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauCalcVarsFillerTool.h
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @date December, 2009
 * @brief Block filler tool for calculated variables. These
 * are variables which are needed for MV discrimination
 * but are not (yet?) in EDM.
 */

#ifndef TAUD3PDMAKER_TAUCALCVARSFILLERTOOL_H
#define TAUD3PDMAKER_TAUCALCVARSFILLERTOOL_H

#include "xAODTau/TauJet.h"
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "GaudiKernel/ToolHandle.h"


class StoreGateSvc; 
class TauDetailsManager;


namespace D3PD {


/**
 * @brief Block filler tool for taujet.
 *
 */
    class TauCalcVarsFillerTool
	: public BlockFillerTool<xAOD::TauJet>
    {
    public:
	/**
	 * @brief Standard Gaudi tool constructor.
	 * @param type The name of the tool type.
	 * @param name The tool name.
	 * @param parent The tool's Gaudi parent.
	 */
	TauCalcVarsFillerTool (const std::string& type,
			       const std::string& name,
			       const IInterface* parent);
	virtual ~TauCalcVarsFillerTool ();


	virtual StatusCode initialize();
	/// Book variables for this block.
	virtual StatusCode book();


	/**
	 * @brief Fill one block --- type-safe version.
	 * @param p The input object.
	 *
	 * This is called once per object.  The caller
	 * is responsible for arranging that all the pointers for booked variables
	 * are set appropriately upon entry.
	 */
	virtual StatusCode fill (const xAOD::TauJet& p);


    private:
	TauDetailsManager* m_detailsManager;  //!< A pointer to the @c TauDetailsManager.
        bool   m_isTrigger ;
	float* m_etHadEMSumPt3Tracks;
	float* m_etEMEMSumPt3Tracks;
	float* m_ipSigLeadLooseTrk;
	float* m_drMin;
	float* m_calRadius;
	float* m_EMFractionAtEMScale;
	float* m_trackIso;
	float* m_ChPiEMEOverCaloEME;
	float* m_PSSFraction;
	float* m_EMPOverTrkSysP;
	float* m_pi0BDTPrimaryScore;
	float* m_pi0BDTSecondaryScore;
	float* m_corrCentFrac;
	float* m_corrFTrk;
	float* m_interAxisEta;
	float* m_interAxisPhi;

	float* m_seedTrk_SecMaxStripEtOverPt;
	float* m_absDeltaEta;
	float* m_absDeltaPhi;
	float* m_EMFractionAtEMScale_MoveE3;

	float* m_cl1_Pt;
	float* m_cl1_Eta;
	float* m_cl1_Phi;
	float* m_cl2_Pt;
	float* m_cl2_Eta;
	float* m_cl2_Phi;
	float* m_tau_vis_Pt;
	float* m_tau_vis_Eta;
	float* m_tau_vis_Phi;
	float* m_tau_vis_M;

	int* m_out_track_n;
	int* m_out_track_n_extended;

	int* m_nPi0s_cl;

	StoreGateSvc* m_storeGate;
	ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;

	inline float deltaR(float eta1, float eta2, float phi1, float phi2)
	{
		float pi = 3.1415926;
    		float deltaPhi = phi1 - phi2;
    		if(deltaPhi > pi) deltaPhi = deltaPhi - 2*pi;
    		if(deltaPhi < -pi) deltaPhi = deltaPhi +2*pi;
    
    		float deltaEta = eta1 - eta2;
    
    		float dR = sqrt(deltaEta*deltaEta + deltaPhi*deltaPhi);
    		return dR;

	} 

    };


} // namespace D3PD


#endif // not TAUD3PDMAKER_TAUCALCVARSFILLERTOOL_H
