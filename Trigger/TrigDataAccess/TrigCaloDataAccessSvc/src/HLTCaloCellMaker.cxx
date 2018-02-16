/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *   */

 /*
 *  NAME:     HLTCaloCellMaker.cxx
 *  PACKAGE:  Trigger/TrigDataAccess/TrigCaloDataAccessSvc
 *
 *  AUTHOR:   Denis Oliveira Damazio
 *
 *  PURPOSE:  New Algorithm to produce CaloCellContainer Vector
 *     
 **/

#include "HLTCaloCellMaker.h"
#include "TrigCaloDataAccessSvc/ITrigCaloDataAccessSvc.h"

HLTCaloCellMaker::HLTCaloCellMaker(const std::string & name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator),
    m_roiCollection("OutputRoIs"),
    m_dataAccessSvc( "TrigCaloDataAccessSvc/TrigCaloDataAccessSvc", name )
{
    declareProperty("RoIs", m_roiCollectionKey = std::string("OutputRoIs"), "RoIs to read in");
    declareProperty("ClustersName", m_cellContainerVKey = std::string("CaloClusters"), "Calo cluster container");
}

HLTCaloCellMaker::~HLTCaloCellMaker()
{
}

StatusCode HLTCaloCellMaker::initialize() {
	ATH_CHECK( m_roiCollectionKey.initialize() );
	ATH_CHECK( m_cellContainerVKey.initialize() );
	CHECK( m_dataAccessSvc.retrieve() );
	return StatusCode::SUCCESS;
}

//StatusCode HLTCaloCellMaker::execute() {
StatusCode HLTCaloCellMaker::execute_r( const EventContext& context ) const {

  SG::WriteHandle<ConstDataVector<CaloCellContainerVector> > cellContainerV( m_cellContainerVKey, context );
  auto cdv = CxxUtils::make_unique<ConstDataVector<CaloCellContainerVector> >();
  ATH_CHECK( cellContainerV.record( std::move(cdv) ) );

  auto roisHandle = SG::makeHandle( m_roiCollectionKey );
  const TrigRoiDescriptorCollection* roiCollection = roisHandle.cptr();


  // datahandle 
  for( const TrigRoiDescriptor* roiDescriptor : *roiCollection) {
	if ( roiDescriptor->isFullscan() ) {
		auto c = std::make_unique<ConstDataVector<CaloCellContainer> >(SG::VIEW_ELEMENTS);
		m_dataAccessSvc->loadFullCollections( context, *c );
		cellContainerV->push_back( c.release()->asDataVector() );
		
	} else {
		LArTT_Selector<LArCellCont> sel;
		m_dataAccessSvc->loadCollections( context, *roiDescriptor, TTEM, 2, sel );
		auto c = std::make_unique<ConstDataVector<CaloCellContainer> >(SG::VIEW_ELEMENTS);
		for( const auto cell : sel ) c->push_back( cell );
		cellContainerV->push_back( c.release()->asDataVector() );
	}
  }


   
  return StatusCode::SUCCESS;
}
