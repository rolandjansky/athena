/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//RtCalibrationIntegration
#include "MdtCalibRt/RtCalibrationIntegration.h"

//root
#include "TFile.h"

//this
#include "MuonCalibStandAloneTools/NtupleIntegrationTool.h"
#include "MuonCalibStandAloneBase/NtupleStationId.h"

namespace MuonCalib {

  NtupleIntegrationTool::NtupleIntegrationTool(const std::string& t, 
					       const std::string& n, 
					       const IInterface* p):AthAlgTool(t, n, p),
								    m_t_max_int(710.0), 
								    m_has_close_hits(false),
								    m_inner_tube_radius(14.6),
								    m_lower_extrapolation_radius(13.0),
								    m_upper_extrapolation_radius(14.0),
m_multilayer_tmax_diff(false),
p_file(NULL)	    
  {
        declareInterface< NtupleCalibrationTool >(this);
	declareProperty("TMaxInt", m_t_max_int);
	declareProperty("HaseCloseHits",m_has_close_hits);
	declareProperty("InnerTubeRadius", m_inner_tube_radius);
	declareProperty("LowerExtrapolationRadius", m_lower_extrapolation_radius);
	declareProperty("UpperExtrapolationRadius", m_upper_extrapolation_radius);
	declareProperty("StoreMultilayerTmaxDiff", m_multilayer_tmax_diff);
  }
	

StatusCode NtupleIntegrationTool::initialize()
	{
	ATH_MSG_INFO( "Initializing Integration Tool" );
//create algorithm
	m_integration = new RtCalibrationIntegration("IM",
						     m_has_close_hits,
						     m_inner_tube_radius,
						     m_lower_extrapolation_radius,
						     m_upper_extrapolation_radius, m_multilayer_tmax_diff);
	p_file = new TFile("Integration.root", "RECREATE");
	return StatusCode :: SUCCESS;
	}


StatusCode NtupleIntegrationTool::handleEvent(const MuonCalibEvent &/*event*/, int /*evnt_nr*/, const std::vector<MuonCalibSegment *> &segments, unsigned int position)
	{
	if(m_integration == NULL)
		{
		ATH_MSG_FATAL( "Not correctly initialized!" );
		return StatusCode :: FAILURE;
		}
	for(unsigned int i=position; i<segments.size(); i++)
		{
		if(!m_integration->handleSegment((*segments[i]))) return StatusCode::FAILURE;
		}
	return StatusCode::SUCCESS;
	}
	
	
	
StatusCode NtupleIntegrationTool::analyseSegments(const std::vector<MuonCalibSegment *> & /*segemnts*/)
	{
	if(m_integration == NULL)
		{
		ATH_MSG_FATAL( "Not correctly initialized!" );
		return StatusCode :: FAILURE;
		}
	if(m_integration->analyse())
		return  StatusCode :: SUCCESS;
	else 
		return StatusCode :: FAILURE;
	}

const IMdtCalibrationOutput * NtupleIntegrationTool :: getResults() const
	{
	return m_integration->getResults();
	}
		
		
StatusCode NtupleIntegrationTool :: finalize()
	{
	p_file->Write();
	return StatusCode::SUCCESS;
	}
}
