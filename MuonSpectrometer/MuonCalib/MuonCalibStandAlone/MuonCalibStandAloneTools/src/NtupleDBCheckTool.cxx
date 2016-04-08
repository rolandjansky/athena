/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//this
#include "MuonCalibStandAloneTools/NtupleDBCheckTool.h"

//MdtCalibIoSvc
#include "MdtCalibIOSvc/MdtCalibInputSvc.h"

//MuonCalibEventBase
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibEventBase/MdtCalibHitBase.h"

//MuonCalibIdentifier
#include "MuonCalibIdentifier/MuonFixedId.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include "MuonCalibStandAloneBase/MdtStationT0Container.h"
#
//MdtCalibIOSvc
#include "MdtCalibIOSvc/MdtCalibInputSvc.h"


//MdtCalibData
#include "MdtCalibData/IRtRelation.h"

namespace MuonCalib {

NtupleDbCheckTool::NtupleDbCheckTool(const std::string & t, const std::string & n, const IInterface * p)
        : AthAlgTool(t, n, p), m_input_service("MdtCalibInputSvc", n)
	{
	declareProperty("MdtCalibInputSvc", m_input_service);
	declareInterface< NtupleCalibrationTool >(this);
	}
	

NtupleDbCheckTool::~NtupleDbCheckTool()
	{
	}
	
StatusCode NtupleDbCheckTool::initialize(void)
	{
	m_outfile = new TFile("NtupleDbCheckTool.root", "RECREATE");
	m_outtree = new TTree("tree", "tree");
	
	m_outtree->Branch("station", &m_station, "station/I");
	m_outtree->Branch("eta", &m_eta, "eta/I");
	m_outtree->Branch("phi", &m_phi, "phi/I");
	m_outtree->Branch("ntuple_t0", &m_ntuple_t0, "ntuple_t0/D");
	m_outtree->Branch("calib_t0", &m_calib_t0, "calib_t0/D");
	m_outtree->Branch("ntuple_r", &m_ntuple_r, "ntuple_r/D");
	m_outtree->Branch("calib_r", &m_calib_r, "calib_r/D");
// calibration input service //
 	ATH_CHECK( m_input_service.retrieve() );
	return StatusCode::SUCCESS;
	}

	
StatusCode NtupleDbCheckTool::handleEvent(const MuonCalibEvent & /*event*/, int /*evnt_nb*/, const std::vector<MuonCalibSegment *> & segments, unsigned int position)
	{
	for(unsigned int i=position; i<segments.size(); i++)
		{
		MuonCalibSegment seg(*segments[i]);
		for(MuonCalibSegment::MdtHitCit it=segments[i]->mdtHOTBegin(); it!= segments[i]->mdtHOTEnd(); it++)
			{
			const MdtCalibHitBase *hit(*it);
			const MuonFixedId &hit_id(hit->identify());
			const NtupleStationId st_id(hit_id);
			m_station=st_id.GetStation();
			m_eta=st_id.GetEta();
			m_phi=st_id.GetPhi();
			m_ntuple_t0=hit->tubeT0();
			const MdtStationT0Container *t0(m_input_service->GetT0(st_id));
			if(t0!=NULL)
				{
				m_calib_t0 = t0->t0(hit_id.mdtMultilayer(), hit_id.mdtTubeLayer(), hit_id.mdtTube());
				}
			else
				{
				m_calib_t0=9e9;
				}
			const IRtRelation *rt=m_input_service-> GetRtRelation(st_id);
			if(rt!=NULL)
				{
				m_calib_r = std::fabs(rt->radius(hit->driftTime()));
				}
			else
				{
				m_calib_r = 9e9;
				}
			m_ntuple_r = std::fabs(hit->driftRadius());
			m_outtree->Fill();
			}
		}
	return StatusCode::SUCCESS;
	}

StatusCode NtupleDbCheckTool::finalize(void)
	{
	m_outfile->Write();
	m_outfile->Close();
	return StatusCode::SUCCESS;
	}

} //namespace MuonCalib
