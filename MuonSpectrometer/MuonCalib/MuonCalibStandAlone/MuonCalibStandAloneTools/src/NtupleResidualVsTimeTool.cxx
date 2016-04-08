/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//MuonCalibEventBase
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibEventBase/MdtCalibHitBase.h"
#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MuonCalibEventBase/MuonCalibEventInfo.h"

//MdtCalibFitters
#include "MdtCalibFitters/StraightPatRec.h"
#include "MdtCalibFitters/CurvedPatRec.h"

//this
#include "MuonCalibStandAloneTools/NtupleResidualVsTimeTool.h"
//root
#include "TFile.h"
#include "TH2.h"
#include "cmath"

namespace MuonCalib{

NtupleResidualVsTimeTool::NtupleResidualVsTimeTool(const std::string& t, const std::string& n, const IInterface* p): AthAlgTool(t, n, p)
	{
	declareInterface< NtupleCalibrationTool >(this) ;
	m_curved_segments=false;
	declareProperty("CurvedSegment", m_curved_segments);
	m_min_hits=5;
	declareProperty("MinHits", m_min_hits);
	m_last_lumiblock=std::pair<unsigned int, unsigned int>(0,0);
	m_last_residual_distribution=NULL;
	}
	
	
	
StatusCode NtupleResidualVsTimeTool::initialize()
	{
	m_outfile=new TFile("Residuals.root", "RECREATE");
	if(m_curved_segments)
		{
		CurvedPatRec *cfitter=new CurvedPatRec();
		cfitter->setTimeOut(2);
		m_fitter=cfitter;
		}
	else
		{
		m_fitter=new StraightPatRec();
		}
	m_fitter->setRoadWidth(2.);
	return StatusCode::SUCCESS;
	}
	
	
StatusCode  NtupleResidualVsTimeTool::handleEvent(const MuonCalibEvent &event, int /*evnt_nr*/, const std::vector<MuonCalibSegment *> &segments, unsigned int position)
	{
	std::pair<unsigned int, unsigned int> current(event.eventInfo().runNumber(), event.eventInfo().lumiBlock());
//create new histogram if needed
	if(current != m_last_lumiblock || !m_last_residual_distribution)
		{
		m_last_lumiblock=current;
		std::map<std::pair<unsigned int, unsigned int> ,TH2F *>::iterator it=m_residual_distribution.find(m_last_lumiblock);
		if(it==m_residual_distribution.end())
			{
			std::ostringstream nm;
			nm<<"residual_r"<<m_last_lumiblock.first<<"_lb"<<m_last_lumiblock.second;
			m_last_residual_distribution = new TH2F(nm.str().c_str(), "", 15, 0, 15, 200, -1.0, 1.0);
			m_residual_distribution[m_last_lumiblock] = m_last_residual_distribution;
			}
		else
			{
			m_last_residual_distribution=it->second;
			}
		}
//loop on segewmnts
	for(unsigned int i=position; i<segments.size(); i++)
		{
		MuonCalibSegment *seg=segments[i];
		if(seg->hitsOnTrack() < static_cast<unsigned int>(m_min_hits))
			continue;
		IMdtSegmentFitter::HitSelection sel(segments[i]->hitsOnTrack(), 0);
		for(unsigned int j=0; j<seg->hitsOnTrack(); j++)
			{
			if(j>0)
				sel[j-1]=0;
			sel[j]=1;
			m_fitter->fit(*seg, sel);
			MdtCalibHitBase *hit(seg->mdtHOT()[j]);
			m_last_residual_distribution->Fill(std::fabs(hit->signedDistanceToTrack()) , hit->trackResidual());
			}
		}
	return StatusCode::SUCCESS;
	}
	
StatusCode NtupleResidualVsTimeTool::finalize()
	{
	m_outfile->Write();
	return StatusCode::SUCCESS;
	}
	
	
	
}
