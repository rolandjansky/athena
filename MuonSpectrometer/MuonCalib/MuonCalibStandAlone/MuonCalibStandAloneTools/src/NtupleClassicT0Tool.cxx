/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//gaudi
#include "GaudiKernel/MsgStream.h"

//MdtCalibData
#include "MdtCalibData/MdtTubeFitContainer.h"

//MdtCalibT0
#include "MdtCalibT0/T0CalibrationOutput.h"

//this
#include "MuonCalibStandAloneTools/NtupleClassicT0Tool.h"
#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"

namespace MuonCalib{

NtupleClassicT0Tool::NtupleClassicT0Tool(const std::string& t, const std::string& n, const IInterface* p): AlgTool(t, n, p), m_adcBins(100), m_minAdc(0.0), m_maxAdc(300.0), m_timeBins(500), m_minTime(0.0), m_maxTime(1000.0), m_fitTime(true), m_minEntries(1000), m_initParam(1), m_chiMax(5.0)
	{
	declareInterface< NtupleCalibrationTool >(this) ;
	declareProperty("NAdcBins", m_adcBins);
	declareProperty("MinAdc", m_minAdc);
	declareProperty("MaxAdc", m_maxAdc);
	declareProperty("NTimeBins", m_timeBins);
	declareProperty("TimeMin", m_minTime);
	declareProperty("TimeMax", m_maxTime);
	declareProperty("FitTimeSpectrum", m_fitTime);
	declareProperty("MinEntries", m_minEntries);
	declareProperty("InitParam", m_initParam);
	declareProperty("MaxChi2", m_chiMax);
	}

	
StatusCode NtupleClassicT0Tool::initialize()
	{
	MsgStream log(msgSvc(), name());
	log<< MSG::INFO << "Initializing MT T0 Tool" <<endreq;
		double *params = new double[8];
		params[0] = 0. ;/* initial parameters for spectrum fit */
		params[1] = 9. ;
		params[2] = 3. ;
		params[3] = 200. ;
		params[4] = 800. ;
		params[5] = 1470. ;
		params[6] = 5. ;
		params[7] = 10. ;

	m_settings = new T0ClassicSettings(
						m_minAdc,
						m_maxAdc,
						m_adcBins,
						m_minTime,
						m_maxTime,
						m_timeBins,
						m_fitTime,
						m_minEntries,
						m_initParam,
						8, params,
						m_chiMax, 4
						);
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
	
StatusCode NtupleClassicT0Tool::handleEvent(const MuonCalibEvent &/*event*/, int /*evnt_nr*/, const std::vector<MuonCalibSegment *> &segments, unsigned int position)
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
	
	
StatusCode NtupleClassicT0Tool::analyseSegments(const std::vector<MuonCalibSegment *> & /*segments*/)
	{
	MsgStream log(msgSvc(), name());
	if(p_alg == NULL)
		{
		log<< MSG::FATAL << "Not correctly initialized!" << endreq;
		return StatusCode :: FAILURE;
		}
	if(p_alg->analyse()) return StatusCode :: SUCCESS;
	return  StatusCode :: FAILURE;
	}


void NtupleClassicT0Tool::setRegion()
	{
	p_alg = new T0CalibrationClassic(std::string("Classic_t0_fitter"), m_settings);
	p_alg->setInput(new T0CalibrationOutput(new MdtTubeFitContainer(p_reg_sel_svc->GetRegionSelection(), 2, 4, 72)));
	}
	
const IMdtCalibrationOutput * NtupleClassicT0Tool::getResults() const
	{
	MsgStream log(msgSvc(), name());
	if(p_alg == NULL)
		{
		log<< MSG::FATAL << "Not correctly initialized!" << endreq;
		return NULL;
		}
	return p_alg->getResults();
	}

}
