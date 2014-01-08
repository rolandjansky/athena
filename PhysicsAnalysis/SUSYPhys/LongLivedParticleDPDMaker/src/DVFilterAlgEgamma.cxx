/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LongLivedParticleDPDMaker/DVFilterAlgEgamma.h"
#include "ITrackToVertex/ITrackToVertex.h"
#include "VxVertex/VxContainer.h"
#include "egammaEvent/ElectronContainer.h"
#include "ITrackToVertex/ITrackToVertex.h"

/// Standard Service Constructor
DVFilterAlgEgamma::DVFilterAlgEgamma(const std::string& name, ISvcLocator* pSvcLocator)
  : AthFilterAlgorithm(name, pSvcLocator), 
    m_nProcessed(0),
    m_nPassElectronCut(0),
    m_trackToVertexTool("Reco::TrackToVertex")
{
	declareProperty("passAll", m_passAll = false); 
	declareProperty("primVtxColl",m_primVtxName="VxPrimaryCandidate");
	declareProperty("ElectronCollection",m_electronCollName="ElectronCollection");
	declareProperty("electronD0Cut",m_electronD0Cut=1.);
	declareProperty("electronPtCut",m_electronPtCut=20000.);
	declareProperty("electronEtaCut",m_electronEtaCut=2.5);
}

/// Destructor
DVFilterAlgEgamma::~DVFilterAlgEgamma() {}

StatusCode DVFilterAlgEgamma::initialize()
{
	// Declare the simple StatusCode
	StatusCode sc(StatusCode::SUCCESS);
	sc = m_trackToVertexTool.retrieve();
	if(sc.isFailure())
	{
		msg(MSG::INFO) << "Could not retrieve TrackToVertexTool";
		return StatusCode::FAILURE;
	}

	return sc;
}

StatusCode DVFilterAlgEgamma::execute()
{

	StatusCode sc(StatusCode::SUCCESS);

	m_nProcessed += 1;
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "==> execute " << name() << " on " << m_nProcessed << " event..." << endreq;

	///If passAll is selected, accept all events
	if(m_passAll)
	{
		if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << name() << " event passed because passAll is true" << endreq;
		setFilterPassed(true);
		return sc;
	}

	const VxContainer* primVertices(0);
	sc = evtStore()->retrieve(primVertices,m_primVtxName);
	if (sc.isFailure()) 
	msg(MSG::ERROR)<<"Failed to retrieve VxPrimaryCandidate collection "<<m_primVtxName<<endreq;
	else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"retrieved VxPrimaryCandidate size "<<primVertices->size()<<endreq;  

	if (primVertices->size() == 0) 
	{
		msg(MSG::WARNING)<<"Primary vertex not found, will skip this event"<<endreq;
		setFilterPassed(false);
		return StatusCode::SUCCESS;
	}

	const  ElectronContainer*  electron_reco=0;
	sc=evtStore()->retrieve(electron_reco , m_electronCollName);
	if( sc.isFailure()  ||  !electron_reco ) 
	{
		msg(MSG::WARNING) << "No electron container found, will skip this event" << endreq; 
		setFilterPassed(false);
		return StatusCode::SUCCESS;
	} 

	ElectronContainer::const_iterator electronItr = electron_reco->begin();  
	ElectronContainer::const_iterator electronItrE = electron_reco->end(); 
	  
	for (; electronItr!=electronItrE; ++electronItr) 
	{ 	
		if (fabs((*electronItr)->pt()) < m_electronPtCut ) continue;
		// if (fabs((*electronItr)->eta()) > m_electronEtaCut ) continue;                   
		
		Rec::TrackParticle* elTrack = const_cast<Rec::TrackParticle*>((*electronItr)->trackParticle());
		if (elTrack==0) continue;
			
		// Find d0 wrt all PVs in the event
		bool elGoodD0=true;
		for (unsigned int i_vtx=0; i_vtx<primVertices->size(); ++i_vtx) 
		{
			const Trk::RecVertex pVtx = (primVertices->at(i_vtx))->recVertex();
			const Trk::Perigee* perigeeAtPV = m_trackToVertexTool->perigeeAtVertex(*elTrack, pVtx.position());
			if (perigeeAtPV == 0) 
			{
				msg(MSG::DEBUG)<<"perigee at PV is null"<<endreq;
				continue;
			}
			double d0 = perigeeAtPV->parameters()[Trk::d0];
			
			if (fabs(d0)< m_electronD0Cut ) elGoodD0=false;
			break;
		}
		if (elGoodD0) 
		{ 
			m_nPassElectronCut++;
			setFilterPassed(true);
			return sc;
		}
	}
	setFilterPassed(false);
	return sc;  
}

StatusCode DVFilterAlgEgamma::finalize()
{

	StatusCode sc(StatusCode::SUCCESS);

	if (msgLvl(MSG::INFO)) 
	{
		msg(MSG::INFO) << "Finalize " << name() << endreq;
		msg(MSG::INFO) <<"d0 cut = " << m_electronD0Cut << ", pT cut = " << m_electronPtCut << ", Eta cut = " << m_electronEtaCut << endreq;
		msg(MSG::INFO) <<"nProcessed "<<m_nProcessed<<endreq;
		msg(MSG::INFO) <<"nPassElectronCut "<<m_nPassElectronCut<<endreq;
		msg(MSG::INFO) <<"Efficiency =  "<<100.0*(float)m_nPassElectronCut/ (float)m_nProcessed<<"%"<<endreq;
	}
	return sc;  
}
