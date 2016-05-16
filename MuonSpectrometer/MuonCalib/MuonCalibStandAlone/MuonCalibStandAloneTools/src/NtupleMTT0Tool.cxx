/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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

NtupleMTT0Tool::NtupleMTT0Tool(const std::string& t, const std::string& n, const IInterface* p)
        : AthAlgTool(t, n, p), p_alg(NULL), m_reg_sel_svc("RegionSelectionSvc", n)
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
	declareProperty("RegionSelectionSvc", m_reg_sel_svc);
	}


	
StatusCode NtupleMTT0Tool::initialize()
	{
	ATH_MSG_INFO( "Initializing MT T0 Tool" );
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
			ATH_MSG_FATAL( "Invalid GroupBy option!" );
			return StatusCode :: FAILURE;
			}
		}
	m_adc_sort_by.resize(m_adc_group_by.size());
	for(unsigned int i=0; i<m_adc_group_by.size(); i++)
		{
		if(!eval_group_by(m_adc_group_by[i], m_adc_sort_by[i]))
			{
			ATH_MSG_FATAL( "Invalid ADCGroupBy option!" );
			return StatusCode :: FAILURE;
			}
		}
//get region selection service
	ATH_CHECK( m_reg_sel_svc.retrieve() );
	setRegion();
	return StatusCode :: SUCCESS;
	}
	
StatusCode NtupleMTT0Tool::handleEvent(const MuonCalibEvent &/*event*/, int /*evnt_nr*/, const std::vector<MuonCalibSegment *> &segments, unsigned int position)
	{
	if(p_alg == NULL)
		{
		ATH_MSG_FATAL( "Not correctly initialized!" );
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
	if(p_alg == NULL)
		{
		ATH_MSG_FATAL( "Not correctly initialized!" );
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
	for(std::vector<MuonCalib :: NtupleStationId> :: const_iterator it=m_reg_sel_svc->GetStationsInRegions().begin(); it!=m_reg_sel_svc->GetStationsInRegions().end(); it++)
		{
		input.GetMap()[*it]=new MdtTubeFitContainer(it->regionId(), 2, 4, 78);
		}
	p_alg->setInput(&input);
	}
	
const IMdtCalibrationOutput * NtupleMTT0Tool::getResults() const
	{
	if(p_alg == NULL)
		{
		ATH_MSG_FATAL( "Not correctly initialized!" );
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
