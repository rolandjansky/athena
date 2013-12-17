/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//gaudi
#include "GaudiKernel/MsgStream.h"

//MdtCalibData
#include "MdtCalibData/MdtTubeFitContainer.h"

//MdtCalibT0
#include "MdtCalibT0/T0CalibrationOutput.h"
#include "MdtCalibT0/HistogramId.h"

//this
#include "MuonCalibStandAloneTools/NtupleMTT0Tool.h"
#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"


namespace MuonCalib{

NtupleMTT0Tool::NtupleMTT0Tool(const std::string& t, const std::string& n, const IInterface* p): AlgTool(t, n, p), p_alg(NULL)
	{
	declareInterface< NtupleCalibrationTool >(this) ;
	m_settings.AddFitfun()=true;
	m_settings.DrawDebugGraphs()=true;
	m_n_bins_neg=1000;
	m_n_bins_pos=2000;
	m_min_entries_time=10000;
	m_min_entries_adc=1000;
	declareProperty("AddFitFun", m_settings.AddFitfun());
	declareProperty("DrawDebugGraphs", m_settings.DrawDebugGraphs());
	declareProperty("NumberOfNegativeBins", m_n_bins_neg);
	declareProperty("NumberOfPositiveBins", m_n_bins_pos);
	declareProperty("GroupBy", m_group_by);
	declareProperty("ADCGroupBy", m_adc_group_by);
	m_use_top_chi2=true;
	declareProperty("UseTopChi2", m_use_top_chi2);
	m_scramble_thresh=2.0;
	declareProperty("ScrambleThreshold", m_scramble_thresh);
	m_sclicing_thresh=3.0;
	declareProperty("SclicingThreshold", m_sclicing_thresh);
	declareProperty("MinumumEntriesPerTimeFit", m_min_entries_time);
	declareProperty("MinumumEntriesPerADCFit", m_min_entries_adc);
	m_correct_rel_t0=true;
	declareProperty("CorrectForRelativeT0", m_correct_rel_t0);
	}


	
StatusCode NtupleMTT0Tool::initialize()
	{
	MsgStream log(msgSvc(), name());
	log<< MSG::INFO << "Initializing MT T0 Tool" <<endreq;
//complete settings class
	m_settings.NBinsTime()=m_n_bins_neg+m_n_bins_pos;
	m_settings.TimeMin()= (25.0/32.0)*(0.5 - m_n_bins_neg);
	m_settings.TimeMax()= (25.0/32.0)*(0.5 + m_n_bins_pos);
	m_settings.T0Settings()->UseTopChi2() = m_use_top_chi2;
	m_settings.T0Settings()->ScrambleThreshold() = m_scramble_thresh;
	m_settings.T0Settings()->SlicingThreshold() = m_sclicing_thresh;
	m_settings.T0Settings()->CorrectRelT0s() = m_correct_rel_t0;
	m_settings.MinEntriesTime() = m_min_entries_time;
	m_settings.MinEntriesADC() = m_min_entries_adc;
	
//evaluate sort by arguments
	m_sort_by.resize(m_group_by.size());
	for(unsigned int i=0; i<m_group_by.size(); i++)
		{
		if(!eval_group_by(m_group_by[i], m_sort_by[i]))
			{
			log<< MSG::FATAL <<"Invalid GroupBy option!" <<endreq;
			return StatusCode :: FAILURE;
			}
		}
	m_adc_sort_by.resize(m_adc_group_by.size());
	for(unsigned int i=0; i<m_adc_group_by.size(); i++)
		{
		if(!eval_group_by(m_adc_group_by[i], m_adc_sort_by[i]))
			{
			log<< MSG::FATAL <<"Invalid ADCGroupBy option!" <<endreq;
			return StatusCode :: FAILURE;
			}
		}
//get region selection service
	StatusCode sc=service("RegionSelectionSvc", p_reg_sel_svc);
	if(!sc.isSuccess())
		{
		log << MSG::ERROR <<"Cannot retrieve RegionSelectionSvc!" <<endreq;
		return sc;
		}
	setRegion();
	return StatusCode :: SUCCESS;
	}
	
StatusCode NtupleMTT0Tool::handleEvent(const MuonCalibEvent &/*event*/, int /*evnt_nr*/, const std::vector<MuonCalibSegment *> &segments, unsigned int position)
	{
	MsgStream log(msgSvc(), name());
	if(p_alg == NULL)
		{
		log<< MSG::FATAL << "Not correctly initialized!" << endreq;
		return StatusCode :: FAILURE;
		}
	for(unsigned int i=position; i<segments.size(); i++)
		{
		if(!p_alg->handleSegment((*segments[i]))) return StatusCode::FAILURE;
		}
	return StatusCode::SUCCESS;
	}
	
	
	
StatusCode NtupleMTT0Tool::analyseSegments(const std::vector<MuonCalibSegment *> & /*segemnts*/)
	{
	MsgStream log(msgSvc(), name());
	if(p_alg == NULL)
		{
		log<< MSG::FATAL << "Not correctly initialized!" << endreq;
		return StatusCode :: FAILURE;
		}
	if(p_alg->analyse())
		return  StatusCode :: SUCCESS;
	else 
		return StatusCode :: FAILURE;
	}


void NtupleMTT0Tool::setRegion()
	{
	p_alg = new T0CalibrationMT("MT_t0_fitter", &m_settings, m_sort_by, m_adc_sort_by);
	T0CalibrationOutput input(NULL);
	for(std::vector<MuonCalib :: NtupleStationId> :: const_iterator it=p_reg_sel_svc->GetStationsInRegions().begin(); it!=p_reg_sel_svc->GetStationsInRegions().end(); it++)
		{
		input.GetMap()[*it]=new MdtTubeFitContainer(it->regionId(), 2, 4, 72);
		}
	p_alg->setInput(&input);
	}
	
const IMdtCalibrationOutput * NtupleMTT0Tool::getResults() const
	{
	MsgStream log(msgSvc(), name());
	if(p_alg == NULL)
		{
		log<< MSG::FATAL << "Not correctly initialized!" << endreq;
		return NULL;
		}
	return p_alg->getResults();
	}

inline bool NtupleMTT0Tool::eval_group_by(const std::string & group_by, int & sort_by)
	{
	sort_by=-99;
	if(group_by == "TUBE")
		{
		sort_by = HistogramId :: TUBE;
		return true;
		}
	if(group_by == "LAYER")
		{
		sort_by = HistogramId :: LAYER;
		return true;
		}
	if(group_by == "MULTILAYER")
		{
		sort_by = HistogramId :: MULTILAYER;
		return true;
		}
	if(group_by == "CHAMBER")
		{
		sort_by = HistogramId :: CHAMBER;
		return true;
		}
	if(group_by == "MEZZ_CARD")
		{
		sort_by = HistogramId :: MEZZ_CARD;
		return true;
		}
	return false;
	}


}
