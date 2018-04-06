/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *   */

 /*
 *  NAME:     HLTCaloCellSumMaker.cxx
 *  PACKAGE:  Trigger/TrigDataAccess/TrigCaloDataAccessSvc
 *
 *  AUTHOR:   Denis Oliveira Damazio
 *
 *  PURPOSE:  New Algorithm to produce CaloCellContainer Vector
 *
 **/

#include "HLTCaloCellSumMaker.h"

HLTCaloCellSumMaker::HLTCaloCellSumMaker(const std::string & name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator),
    m_cellContainerV("CaloClusters")
{
    declareProperty("ClustersName", m_cellContainerVKey = std::string("CaloClusters"), "Calo cluster container");
}

HLTCaloCellSumMaker::~HLTCaloCellSumMaker()
{
}

StatusCode HLTCaloCellSumMaker::initialize() {
        ATH_CHECK( m_cellContainerVKey.initialize() );
        return StatusCode::SUCCESS;
}

StatusCode HLTCaloCellSumMaker::execute_r( const EventContext& /*context*/ ) const {


  auto roisHandle = SG::makeHandle( m_cellContainerVKey );
  const ConstDataVector<CaloCellContainerVector>* roiCollection = roisHandle.cptr();


  int c=0;
  // datahandle
  for(const CaloCellContainer* roiDescriptor : *roiCollection ) {
	if ( roiDescriptor->size() > 0 ) {
	float sum=0.;
	for(auto c : *roiDescriptor ) sum+=c->et();
	ATH_MSG_INFO ( "Executing " << name() << "..." << "RoIToSum : " << c++ << " " << roiDescriptor->size() << " " << roiDescriptor->at(0)->eta() << " " << sum );
	} else { ATH_MSG_INFO( "empty container"); }

  }
  return StatusCode::SUCCESS;
}
