/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "EgammaLWCalibration.h"
//#include "TrigCaloEvent/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "GaudiKernel/MsgStream.h"
// Need Cosh
#include <math.h>

using CaloClusterCorr::interpolate;

StatusCode EgammaLWCalibration::initialize(){

	CHECK (base_class::initialize());
	m_log = new MsgStream(AthAlgTool::msgSvc(), name() );

	(*m_log) << MSG::DEBUG << "Initialize Tool : " << name() << endmsg;

	// Initialize here to speed up
	m_samps[0][0]=CaloSampling::PreSamplerB;
	m_samps[0][1]=CaloSampling::EMB1;
	m_samps[0][2]=CaloSampling::EMB2;
	m_samps[0][3]=CaloSampling::EMB3;
	m_samps[1][0]=CaloSampling::PreSamplerE;
	m_samps[1][1]=CaloSampling::EME1;
	m_samps[1][2]=CaloSampling::EME2;
	m_samps[1][3]=CaloSampling::EME3;

	m_interp_barriers[0] = m_eta_start_crack();
	m_interp_barriers[1] = m_eta_end_crack();

	return StatusCode::SUCCESS;
}

StatusCode EgammaLWCalibration::finalize(){
	(*m_log) << MSG::DEBUG << "Finalize Tool : " << name() << endmsg;
	delete m_log;
	return StatusCode::SUCCESS;
}

void EgammaLWCalibration::makeCorrection(xAOD::TrigEMCluster* clus,
					 const void * /*param*/){
	
#ifndef NDEBUG
	(*m_log) << MSG::DEBUG << "makeCorrection for tool : "
		<< name() << endmsg;
	(*m_log) << MSG::DEBUG << "Cluster E input : " <<
		clus->energy() << endmsg;
#endif
        const float etamax = m_etamax();
	float the_aeta=(clus->eta());
	if (the_aeta<0) the_aeta=-the_aeta;
	if (the_aeta >= etamax) return;
	int si;
	if (the_aeta < m_eta_start_crack()) si=0;
	else if ( the_aeta > m_eta_end_crack()) si=1;
	else return; // No Correction at the Gap.
	float pars[4];
        const CxxUtils::Array<2> correction = m_correction();
	int ibin=static_cast<int> (the_aeta/etamax * correction.size());
	pars[0] = correction[ibin][1];
	pars[1] = correction[ibin][2];
        const int degree = m_degree();
	if (m_interpolate()){
	for(int i=2;i<4;i++)
		pars[i] = interpolate(correction,
			the_aeta,degree,i+1,m_interp_barriers);
	}
	else{
		pars[2]=correction[ibin][3];
		pars[3]=correction[ibin][4];
	}

	float e_offset = 0;
	if (m_preserve_offset()) {
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
		clus->energy() << endmsg;
#endif

}

