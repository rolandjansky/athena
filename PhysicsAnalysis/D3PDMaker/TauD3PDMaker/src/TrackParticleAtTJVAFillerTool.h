// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackParticleAtTJVAFillerTool.h 577665 2014-01-09 23:18:59Z ssnyder $
/**
 * @file TauD3PDMaker/src/TrackParticleAtTJVAFillerTool.h
 * @author Michel Trottier-McDonald <mtm@cern.ch>
 * @date June, 2012
 * @brief Block filler tool for Track kinematics w.r.t. the different vertices associated to tau candidates
 */

#ifndef TAUD3PDMAKER_TRACKPARTICLEATTJVAFILLERTOOL_H
#define TAUD3PDMAKER_TRACKPARTICLEATTJVAFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "TrkParameters/TrackParameters.h"
#include "GaudiKernel/ToolHandle.h"

namespace Rec { class TrackParticle; }
class StoreGateSvc; 

namespace D3PD {
    
    
    /**
     * @brief Block filler tool for tau track details.
     *
     */
    class TrackParticleAtTJVAFillerTool
	: public BlockFillerTool<Rec::TrackParticle>
    {
    public:
	/**
	 * @brief Standard Gaudi tool constructor.
	 * @param type The name of the tool type.
	 * @param name The tool name.
	 * @param parent The tool's Gaudi parent.
	 */
	TrackParticleAtTJVAFillerTool (const std::string& type,
				       const std::string& name,
				       const IInterface* parent);
	
	
	/// Book variables for this block.
	virtual StatusCode book();
	
	// Have an initialize for storegate
	virtual StatusCode initialize();
	
	
	/**
	 * @brief Fill one block --- type-safe version.
	 * @param p The input object.
	 *
	 * This is called once per object.  The caller
	 * is responsible for arranging that all the pointers for booked variables
	 * are set appropriately upon entry.
	 */
	virtual StatusCode fill (const Rec::TrackParticle& p);
	
	
    private:
	std::vector<float>* m_trk_atTJVA_phi;
	std::vector<float>* m_trk_atTJVA_d0;
	std::vector<float>* m_trk_atTJVA_z0;
	std::vector<float>* m_trk_atTJVA_theta;
	std::vector<float>* m_trk_atTJVA_qoverp;
	std::vector<float>* m_trk_atTJVA_phi_err;
	std::vector<float>* m_trk_atTJVA_d0_err;
	std::vector<float>* m_trk_atTJVA_z0_err;
	std::vector<float>* m_trk_atTJVA_theta_err;
	std::vector<float>* m_trk_atTJVA_qoverp_err;
	std::vector<float>* m_trk_atTJVA_cov_phi_d0;
	std::vector<float>* m_trk_atTJVA_cov_phi_z0;
	std::vector<float>* m_trk_atTJVA_cov_phi_theta;
	std::vector<float>* m_trk_atTJVA_cov_phi_qoverp;
	std::vector<float>* m_trk_atTJVA_cov_d0_z0;
	std::vector<float>* m_trk_atTJVA_cov_d0_theta;
	std::vector<float>* m_trk_atTJVA_cov_d0_qoverp;
	std::vector<float>* m_trk_atTJVA_cov_z0_theta;
	std::vector<float>* m_trk_atTJVA_cov_z0_qoverp;
	std::vector<float>* m_trk_atTJVA_cov_theta_qoverp;
	unsigned int* m_trk_atTJVA_n;
	
	StoreGateSvc* m_storeGate;

        // Track to vertex extrapolator
	ToolHandle<Reco::ITrackToVertex> m_trackToVertexTool;
    };
    
    
} // namespace D3PD

#endif
