/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// ********************************************************************
//
// NAME:     EgammaLWCalibration.cxx
// PACKAGE:  Trigger/TrigTools/EgammaLWCalibration.cxx
//
// AUTHOR:   Denis Oliveira Damazio
//
// REFERENCES: Tool to provide Longitudinal Weight calibration
//
// ********************************************************************

#include "TrigT2CaloCalibration/EgammaLWCalibration.h"
//#include "TrigCaloEvent/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "GaudiKernel/MsgStream.h"
// Need Cosh
#include <math.h>

using CaloClusterCorr::interpolate;

StatusCode EgammaLWCalibration::initialize(){

	CHECK (AthAlgTool::initialize());
	CHECK (CaloRec::ToolWithConstantsMixin::initialize() );
	m_log = new MsgStream(AthAlgTool::msgSvc(), name() );

	(*m_log) << MSG::DEBUG << "Initialize Tool : " << name() << endreq;

	// Initialize here to speed up
	m_samps[0][0]=CaloSampling::PreSamplerB;
	m_samps[0][1]=CaloSampling::EMB1;
	m_samps[0][2]=CaloSampling::EMB2;
	m_samps[0][3]=CaloSampling::EMB3;
	m_samps[1][0]=CaloSampling::PreSamplerE;
	m_samps[1][1]=CaloSampling::EME1;
	m_samps[1][2]=CaloSampling::EME2;
	m_samps[1][3]=CaloSampling::EME3;

	unsigned int shape[] = {2};
	m_interp_barriers = new CaloRec::WritableArrayData<1> (shape);
	(*m_interp_barriers)[0] = m_eta_start_crack;
	(*m_interp_barriers)[1] = m_eta_end_crack;

	return StatusCode::SUCCESS;
}

StatusCode EgammaLWCalibration::finalize(){
	(*m_log) << MSG::DEBUG << "Finalize Tool : " << name() << endreq;
	delete m_log;
	delete m_interp_barriers;
	return StatusCode::SUCCESS;
}

void EgammaLWCalibration::makeCorrection(xAOD::TrigEMCluster* clus,
					 const void * /*param*/){
	
#ifndef NDEBUG
	(*m_log) << MSG::DEBUG << "makeCorrection for tool : "
		<< name() << endreq;
	(*m_log) << MSG::DEBUG << "Cluster E input : " <<
		clus->energy() << endreq;
#endif
	float the_aeta=(clus->eta());
	if (the_aeta<0) the_aeta=-the_aeta;
	if (the_aeta >= m_etamax) return;
	int si;
	if (the_aeta < m_eta_start_crack) si=0;
	else if ( the_aeta > m_eta_end_crack) si=1;
	else return; // No Correction at the Gap.
	float pars[4];
	int ibin=static_cast<int> (the_aeta/m_etamax * m_correction.size());
	pars[0] = m_correction[ibin][1];
	pars[1] = m_correction[ibin][2];
	if (m_interpolate){
	for(int i=2;i<4;i++)
		pars[i] = interpolate(m_correction,
			the_aeta,m_degree,i+1,(*m_interp_barriers));
	}
	else{
		pars[2]=m_correction[ibin][3];
		pars[3]=m_correction[ibin][4];
	}

	float e_offset = 0;
	if (m_preserve_offset) {
	  double total0 = 0;
	  for (int sampling=0; sampling<4; ++sampling)
	     total0 += clus->energy(m_samps[si][sampling]);
	  e_offset = clus->energy() - total0;
	}
 
	clus->setEnergy (m_samps[si][0], clus->energy(m_samps[si][0]) * pars[0]);
	clus->setEnergy (m_samps[si][3], clus->energy(m_samps[si][3]) * pars[1]);
 
	double total = e_offset;
	for (int sampling=0; sampling<4; ++sampling)
	   total += clus->energy (m_samps[si][sampling]);
	clus->setEnergy((pars[2] * (pars[3] + total)));
	clus->setEt(clus->energy()/cosh(clus->eta()));
#ifndef NDEBUG
	(*m_log) << MSG::DEBUG << "Cluster E output : " <<
		clus->energy() << endreq;
#endif

}

StatusCode
EgammaLWCalibration::setProperty (const std::string& propname,
                                    const std::string& value)
{
  CHECK( AthAlgTool::setProperty(propname,value) );
  CHECK( CaloRec::ToolWithConstantsMixin::setProperty (propname, value) );
  return StatusCode::SUCCESS;
}

StatusCode
EgammaLWCalibration::setProperty (const Property& p)
{
  CHECK( AthAlgTool::setProperty(p) );
  CHECK( CaloRec::ToolWithConstantsMixin::setProperty (p) );
  return StatusCode::SUCCESS;
}
