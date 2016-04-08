/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//MuonCalibEventBase
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibEventBase/MdtCalibHitBase.h"

//MdtCalibFitters
#include "MdtCalibFitters/QuasianalyticLineReconstruction.h"


//this
#include "MuonCalibStandAloneTools/NtupleResidualTool.h"
//root
#include "TFile.h"
#include "TH2.h"
#include "TH1.h"

namespace MuonCalib{

NtupleResidualTool::NtupleResidualTool(const std::string& t, const std::string& n, const IInterface* p): AthAlgTool(t, n, p)
	{
	declareInterface< NtupleCalibrationTool >(this) ;
	}
	
	
	
StatusCode NtupleResidualTool::initialize()
	{
	m_outfile=new TFile("Residuals.root", "RECREATE");
	m_residual_distribution = new TH2F ("residual", "residuals" , 152, -0.1, 15.1, 200, -1.0, 1.0);
	m_r_spec=new TH1F("r_spec", "r_spec", 100, 0, 15);
	m_chi2=new TH1F("chi2", "chi^{2}", 100, 0, 50);
	m_nhits=new TH1F("nhits", "nhits", 20, -0.5, 19.5);
	m_fitter=new QuasianalyticLineReconstruction(1.0);
//	m_fitter.setRoadWidth(2);
	m_fitter->switchOnRefit();
	return StatusCode::SUCCESS;
	}
	
	
StatusCode  NtupleResidualTool::handleEvent(const MuonCalibEvent &/*event*/, int /*evnt_nr*/, const std::vector<MuonCalibSegment *> &segments, unsigned int position)
	{
//loop on segewmnts
	for(unsigned int i=position; i<segments.size(); i++)
		{
		m_fitter->fit(*(segments[i]));
//		if(segments[i]->hitsOnTrack()<4) continue;
		//loop on hits
		for(std::vector<const MdtCalibHitBase*>::const_iterator it = m_fitter->trackHits().begin(); it!=m_fitter->trackHits().end(); it++)
//		for(MuonCalibSegment::MdtHitCit it=segments[i]->mdtHOTBegin(); it!=segments[i]->mdtHOTEnd(); it++)
			{
//			double rsign(1.0);
//			if((*it)->signedDistanceToTrack() < 0)
//				rsign=-1.0;
//			double track_residual((*it)->signedDistanceToTrack() - (*it)->driftRadius() * rsign);
			double track_residual(std::fabs((*it)->signedDistanceToTrack()) - (*it)->driftRadius());
			m_residual_distribution -> Fill(static_cast<Axis_t>(fabs((*it)->signedDistanceToTrack())), static_cast<Axis_t>(track_residual));
			m_r_spec->Fill( static_cast<Axis_t>((*it)->driftRadius()));
			}
		m_chi2->Fill(segments[i]->chi2());
		m_nhits->Fill(segments[i]->hitsOnTrack());
		}
	return StatusCode::SUCCESS;
	}
	
StatusCode NtupleResidualTool::finalize()
	{
	m_outfile->Write();
	return StatusCode::SUCCESS;
	}
	
	
	
}
