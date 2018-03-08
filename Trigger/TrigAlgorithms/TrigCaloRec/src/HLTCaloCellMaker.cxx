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
#include "TrigT2CaloCommon/ITrigCaloDataAccessSvc.h"

HLTCaloCellMaker::HLTCaloCellMaker(const std::string & name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator),
    m_roiCollection("OutputRoIs"),
    m_dataAccessSvc( "TrigCaloDataAccessSvc/TrigCaloDataAccessSvc", name )
{
    declareProperty("RoIs", m_roiCollectionKey = std::string("OutputRoIs"), "RoIs to read in");
    declareProperty("CellsName", m_cellContainerVKey = std::string("CellsClusters"), "Calo cells container");
    declareProperty("TrigDataAccessMT",m_dataAccessSvc,"Data Access for LVL2 Calo Algorithms in MT");
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

  std::cout << "Will try to execute" << std::endl;

  SG::WriteHandle<ConstDataVector<CaloCellContainerVector> > cellContainerV( m_cellContainerVKey, context );
  auto cdv = CxxUtils::make_unique<ConstDataVector<CaloCellContainerVector> >();
  ATH_CHECK( cellContainerV.record( std::move(cdv) ) );

  auto roisHandle = SG::makeHandle( m_roiCollectionKey );
  const TrigRoiDescriptorCollection* roiCollection = roisHandle.cptr();

  std::cout << "still here" << roiCollection->size() << std::endl;

  // datahandle 
  for( const TrigRoiDescriptor* roiDescriptor : *roiCollection) {
  	std::cout << "still here roi" << std::endl;
	if ( roiDescriptor->isFullscan() ) {
  		std::cout << "still here roi FS" << std::endl;
		auto c = std::make_unique<ConstDataVector<CaloCellContainer> >(SG::VIEW_ELEMENTS);
		m_dataAccessSvc->loadFullCollections( context, *c );
		cellContainerV->push_back( c.release()->asDataVector() );
		
	} else {
  		std::cout << "still here roi RoI" << std::endl;
		LArTT_Selector<LArCellCont> sel;
		m_dataAccessSvc->loadCollections( context, *roiDescriptor, TTEM, 2, sel );
		auto c = std::make_unique<ConstDataVector<CaloCellContainer> >(SG::VIEW_ELEMENTS);
		int cc(0);
		for( const auto cell : sel ) {c->push_back( cell ); cc++;}
		std::cout << "RoI size in cells : " << cc << std::endl;
		cellContainerV->push_back( c.release()->asDataVector() );
	}
  }

  std::cout << "Finishing" << std::endl;

   
  return StatusCode::SUCCESS;
}
