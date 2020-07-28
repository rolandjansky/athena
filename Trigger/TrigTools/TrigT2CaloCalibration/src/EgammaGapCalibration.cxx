/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


// ********************************************************************
//
// NAME:     EgammaGapCalibration.cxx
// PACKAGE:  Trigger/TrigTools/EgammaGapCalibration.cxx
//
// AUTHOR:   Denis Oliveira Damazio
//
// REFERENCES: Tool to provide Longitudinal Weight calibration
// Based on Scott's code for the offline
//
// ********************************************************************

#include "EgammaGapCalibration.h"
//#include "TrigCaloEvent/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "GaudiKernel/MsgStream.h"
#include "CaloGeoHelpers/CaloSampling.h"

using CaloClusterCorr::interpolate;

StatusCode EgammaGapCalibration::initialize(){

	CHECK (base_class::initialize());
	m_log = new MsgStream(AthAlgTool::msgSvc(), name() );

	(*m_log) << MSG::DEBUG << "Initialize Tool : " << name() << endmsg;

	return StatusCode::SUCCESS;

}


StatusCode EgammaGapCalibration::finalize(){

	(*m_log) << MSG::DEBUG << "Finalize Tool : " << name() << endmsg;
	delete m_log;

	return StatusCode::SUCCESS;

}

void EgammaGapCalibration::makeCorrection(xAOD::TrigEMCluster* clus,
					  const void * /*param*/){
	
#ifndef NDEBUG
	(*m_log) << MSG::DEBUG << "makeCorrection for tool : "
		<< name() << endmsg;
	(*m_log) << MSG::DEBUG << "Cluster E input : " <<
		clus->energy() << endmsg;
#endif
	float the_aeta=(clus->eta());
	if (the_aeta<0) the_aeta=-the_aeta;
	// If far from the crack, nothing to do
	if (the_aeta < m_eta_start_crack() || the_aeta > m_eta_end_crack()) return;

        CxxUtils::Array<2> correction = m_correction();
        int degree = m_degree();
	float a      = interpolate(correction, the_aeta, degree,1);
	float alpha  = interpolate(correction, the_aeta, degree,2);
	float offset = interpolate(correction, the_aeta, degree,3);
	float eh_scint = clus->energy(CaloSampling::TileGap3);
	float ec = clus->energy();
	clus->setEnergy(a*(ec+alpha*eh_scint + offset));
	clus->setEt(clus->energy()/cosh(clus->eta()));
#ifndef NDEBUG
	(*m_log) << MSG::DEBUG << "Cluster E output : " <<
		clus->energy() << endmsg;
#endif

}

