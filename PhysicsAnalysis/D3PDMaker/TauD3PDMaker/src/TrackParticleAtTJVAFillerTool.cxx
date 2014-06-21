// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrackParticleAtTJVAFillerTool.cxx 577665 2014-01-09 23:18:59Z ssnyder $
/**
 * @file TauD3PDMaker/src/TrackParticleAtTJVAFillerTool.cxx
 * @author Michel Trottier-McDonald <mtm@cern.ch>
 * @date June, 2012
 * @brief Block filler tool for Track kinematics w.r.t. the different vertices associated to tau candidates
 */

#include "TrackParticleAtTJVAFillerTool.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/StoreGateSvc.h"


namespace D3PD {
    
    
/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
    TrackParticleAtTJVAFillerTool::TrackParticleAtTJVAFillerTool (const std::string& type,
								  const std::string& name,
								  const IInterface* parent)
	: BlockFillerTool<Rec::TrackParticle> (type, name, parent),
	  m_trk_atTJVA_phi(0),
	  m_trk_atTJVA_d0(0),
	  m_trk_atTJVA_z0(0),
	  m_trk_atTJVA_theta(0),
	  m_trk_atTJVA_qoverp(0),
	  m_trk_atTJVA_phi_err(0),
	  m_trk_atTJVA_d0_err(0),
	  m_trk_atTJVA_z0_err(0),
	  m_trk_atTJVA_theta_err(0),
	  m_trk_atTJVA_qoverp_err(0),
	  m_trk_atTJVA_cov_phi_d0(0),
	  m_trk_atTJVA_cov_phi_z0(0),
	  m_trk_atTJVA_cov_phi_theta(0),
	  m_trk_atTJVA_cov_phi_qoverp(0),
	  m_trk_atTJVA_cov_d0_z0(0),
	  m_trk_atTJVA_cov_d0_theta(0),
	  m_trk_atTJVA_cov_d0_qoverp(0),
	  m_trk_atTJVA_cov_z0_theta(0),
	  m_trk_atTJVA_cov_z0_qoverp(0),
	  m_trk_atTJVA_cov_theta_qoverp(0),
	  m_trk_atTJVA_n(0),
	  m_trackToVertexTool("Reco::TrackToVertex")
    {
	declareProperty ("TrackToVertexTool", m_trackToVertexTool);

        book().ignore(); // Avoid coverity warnings.
    }
    
    
    
/**
 * @brief Book variables for this block.
 */
    StatusCode TrackParticleAtTJVAFillerTool::book()
    {
	CHECK( addVariable ("phi",    m_trk_atTJVA_phi) );
	CHECK( addVariable ("d0",     m_trk_atTJVA_d0)  );
	CHECK( addVariable ("z0",     m_trk_atTJVA_z0)  );
	CHECK( addVariable ("theta",  m_trk_atTJVA_theta));
	CHECK( addVariable ("qoverp", m_trk_atTJVA_qoverp));
	CHECK( addVariable ("phi_err",    m_trk_atTJVA_phi_err));
	CHECK( addVariable ("d0_err",     m_trk_atTJVA_d0_err));
	CHECK( addVariable ("z0_err",     m_trk_atTJVA_z0_err));
	CHECK( addVariable ("theta_err",  m_trk_atTJVA_theta_err));
	CHECK( addVariable ("qoverp_err", m_trk_atTJVA_qoverp_err));
	CHECK( addVariable ("cov_phi_d0",       m_trk_atTJVA_cov_phi_d0));
	CHECK( addVariable ("cov_phi_z0",       m_trk_atTJVA_cov_phi_z0));
	CHECK( addVariable ("cov_phi_theta",    m_trk_atTJVA_cov_phi_theta));
	CHECK( addVariable ("cov_phi_qoverp",   m_trk_atTJVA_cov_phi_qoverp));
	CHECK( addVariable ("cov_d0_z0",        m_trk_atTJVA_cov_d0_z0));
	CHECK( addVariable ("cov_d0_theta",     m_trk_atTJVA_cov_d0_theta));
	CHECK( addVariable ("cov_d0_qoverp",    m_trk_atTJVA_cov_d0_qoverp));
	CHECK( addVariable ("cov_z0_theta",     m_trk_atTJVA_cov_z0_theta));
	CHECK( addVariable ("cov_z0_qoverp",    m_trk_atTJVA_cov_z0_qoverp));
	CHECK( addVariable ("cov_theta_qoverp", m_trk_atTJVA_cov_theta_qoverp));
	CHECK( addVariable ("n",      m_trk_atTJVA_n )  );
	
	return StatusCode::SUCCESS;
    }
    
    
    
    StatusCode TrackParticleAtTJVAFillerTool::initialize()
    {
	// get StoreGate 
	StatusCode sc = service("StoreGateSvc", m_storeGate);
	
	if ( sc == StatusCode::FAILURE )
	{
	    REPORT_MESSAGE(MSG::ERROR) << "   could not access StoreGate ";
	    return StatusCode::FAILURE;
	}

	// Get Track vertex Association tool
	sc = m_trackToVertexTool.retrieve();

	if(sc.isFailure())
	{
	    REPORT_MESSAGE (MSG::ERROR) << "Could not retrieve TrackToVertexTool";
	    return StatusCode::FAILURE;
	}
	
	return StatusCode::SUCCESS;
    }  
    
/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
    StatusCode TrackParticleAtTJVAFillerTool::fill (const Rec::TrackParticle& p)
    {
	
        // ??? Still needed?
	if (p.measuredPerigee()->covariance() == 0)
	{
	    REPORT_MESSAGE (MSG::WARNING) << "Bad track: can't find perigee at vertex.";
	    return 0;
	}
	
	// Retrieve the Tau container first
	const Analysis::TauJetContainer* recoTauContainer = 0;
	
	StatusCode sc = m_storeGate->retrieve(recoTauContainer, "TauRecContainer");
	
	if( StatusCode::SUCCESS != sc  || !recoTauContainer )
	{
	    REPORT_MESSAGE(MSG::DEBUG) << "Unable to retrieve the TauJet container: TauRecContainer" << endreq;
	    return StatusCode::FAILURE;
	}
	
	Analysis::TauJetContainer::const_iterator tauItr  = recoTauContainer->begin();
	Analysis::TauJetContainer::const_iterator tauItrE = recoTauContainer->end();
	
	unsigned int nTaus = 0;

	m_trk_atTJVA_phi->clear();
	m_trk_atTJVA_d0->clear();
	m_trk_atTJVA_z0->clear();
	m_trk_atTJVA_theta->clear();
	m_trk_atTJVA_qoverp->clear();
	m_trk_atTJVA_phi_err->clear();
	m_trk_atTJVA_d0_err->clear();
	m_trk_atTJVA_z0_err->clear();
	m_trk_atTJVA_theta_err->clear();
	m_trk_atTJVA_qoverp_err->clear();
	m_trk_atTJVA_cov_phi_d0->clear();
	m_trk_atTJVA_cov_phi_z0->clear();
	m_trk_atTJVA_cov_phi_theta->clear();
	m_trk_atTJVA_cov_phi_qoverp->clear();
	m_trk_atTJVA_cov_d0_z0->clear();
	m_trk_atTJVA_cov_d0_theta->clear();
	m_trk_atTJVA_cov_d0_qoverp->clear();
	m_trk_atTJVA_cov_z0_theta->clear();
	m_trk_atTJVA_cov_z0_qoverp->clear();
	m_trk_atTJVA_cov_theta_qoverp->clear();
	
	for (; tauItr != tauItrE; ++tauItr)
	{
            if ((*tauItr)->origin() /*&& (*tauItr)->origin()->position()*/)
	    {
		const Trk::Perigee* perigee = m_trackToVertexTool->perigeeAtVertex(p, (*tauItr)->origin()->position());
		
		m_trk_atTJVA_phi->push_back(perigee->parameters()[Trk::phi0]); 
		m_trk_atTJVA_d0->push_back(perigee->parameters()[Trk::d0]);
		m_trk_atTJVA_z0->push_back(perigee->parameters()[Trk::z0]);
		m_trk_atTJVA_theta->push_back(perigee->parameters()[Trk::theta]);
		m_trk_atTJVA_qoverp->push_back(perigee->parameters()[Trk::qOverP]);
		
		const Trk::Perigee* mperigee = m_trackToVertexTool->perigeeAtVertex(p, (*tauItr)->origin()->position());

                const AmgSymMatrix(5)& cov = *mperigee->covariance();
		m_trk_atTJVA_phi_err->push_back   (Amg::error (cov,Trk::phi0));
		m_trk_atTJVA_d0_err->push_back    (Amg::error (cov,Trk::d0));
		m_trk_atTJVA_z0_err->push_back    (Amg::error (cov,Trk::z0));
		m_trk_atTJVA_theta_err->push_back (Amg::error (cov,Trk::theta));
		m_trk_atTJVA_qoverp_err->push_back(Amg::error (cov,Trk::qOverP));
		
		m_trk_atTJVA_cov_phi_d0->push_back    (cov(Trk::phi0,Trk::d0));
		m_trk_atTJVA_cov_phi_z0->push_back    (cov(Trk::phi0,Trk::z0));
		m_trk_atTJVA_cov_phi_theta->push_back (cov(Trk::phi0,Trk::theta));
		m_trk_atTJVA_cov_phi_qoverp->push_back(cov(Trk::phi0,Trk::qOverP));
		
		m_trk_atTJVA_cov_d0_z0->push_back     (cov(Trk::d0,Trk::z0));
		m_trk_atTJVA_cov_d0_theta->push_back  (cov(Trk::d0,Trk::theta));
		m_trk_atTJVA_cov_d0_qoverp->push_back (cov(Trk::d0,Trk::qOverP));
		
		m_trk_atTJVA_cov_z0_theta->push_back  (cov(Trk::z0,Trk::theta));
		m_trk_atTJVA_cov_z0_qoverp->push_back (cov(Trk::z0,Trk::qOverP));
                m_trk_atTJVA_cov_theta_qoverp->push_back(cov(Trk::theta,Trk::qOverP));
		
		delete perigee;
		delete mperigee;
	    }
	    else 
	    {
		m_trk_atTJVA_phi->push_back(-1111.); 
		m_trk_atTJVA_d0->push_back(-1111.);
		m_trk_atTJVA_z0->push_back(-1111.);
		m_trk_atTJVA_theta->push_back(-1111.);
		m_trk_atTJVA_qoverp->push_back(-1111.);
		
		m_trk_atTJVA_phi_err->push_back(-1111.);
		m_trk_atTJVA_d0_err->push_back(-1111.);
		m_trk_atTJVA_z0_err->push_back(-1111.);
		m_trk_atTJVA_theta_err->push_back(-1111.);
		m_trk_atTJVA_qoverp_err->push_back(-1111.);
		
		m_trk_atTJVA_cov_phi_d0->push_back(-1111.);
		m_trk_atTJVA_cov_phi_z0->push_back(-1111.);
		m_trk_atTJVA_cov_phi_theta->push_back(-1111.);
		m_trk_atTJVA_cov_phi_qoverp->push_back(-1111.);
		
		m_trk_atTJVA_cov_d0_z0->push_back(-1111.);
		m_trk_atTJVA_cov_d0_theta->push_back(-1111.);
		m_trk_atTJVA_cov_d0_qoverp->push_back(-1111.);
		
		m_trk_atTJVA_cov_z0_theta->push_back(-1111.);
		m_trk_atTJVA_cov_z0_qoverp->push_back(-1111.);
		
		m_trk_atTJVA_cov_theta_qoverp->push_back(-1111.);
	    }

	    ++nTaus;
	}
	
	*m_trk_atTJVA_n = nTaus;

	return StatusCode::SUCCESS;
	
    } // end fill method
    
} // namespace D3PD
