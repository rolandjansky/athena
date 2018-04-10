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
    m_dataAccessSvc( "TrigCaloDataAccessSvc/TrigCaloDataAccessSvc", name ),
    m_roiMode(true)
{
    declareProperty("RoIs", m_roiCollectionKey = std::string("OutputRoIs"), "RoIs to read in");
    declareProperty("CellsVName", m_cellContainerVKey = std::string("CellsVClusters"), "Calo cells container");
    declareProperty("CellsName", m_cellContainerKey = std::string("CellsClusters"), "Calo cells container");
    declareProperty("TrigDataAccessMT",m_dataAccessSvc,"Data Access for LVL2 Calo Algorithms in MT");
    declareProperty("roiMode",m_roiMode,"RoiMode roi->CaloCellCollection");
}

HLTCaloCellMaker::~HLTCaloCellMaker()
{
}

StatusCode HLTCaloCellMaker::initialize() {
	ATH_CHECK( m_roiCollectionKey.initialize() );
	if ( m_roiMode ) 
	ATH_CHECK( m_cellContainerKey.initialize() );
	else
	ATH_CHECK( m_cellContainerVKey.initialize() );
	CHECK( m_dataAccessSvc.retrieve() );
	return StatusCode::SUCCESS;
}

StatusCode HLTCaloCellMaker::execute_r( const EventContext& context ) const {


  auto roisHandle = SG::makeHandle( m_roiCollectionKey );
  const TrigRoiDescriptorCollection* roiCollection = roisHandle.cptr();

  // datahandle 
  if ( m_roiMode ) {
  if ( roiCollection->size() > 1 ) 
     ATH_MSG_INFO ( "roiMode but multiple rois found, will only use the first one");
  SG::WriteHandle<ConstDataVector<CaloCellContainer> > cellContainer = SG::WriteHandle<ConstDataVector<CaloCellContainer> > ( m_cellContainerKey, context );
  auto cdv = CxxUtils::make_unique<ConstDataVector<CaloCellContainer> >(SG::VIEW_ELEMENTS);
  for( const TrigRoiDescriptor* roiDescriptor : *roiCollection) {
        if ( roiDescriptor->isFullscan() ) {
                m_dataAccessSvc->loadFullCollections( context, *cdv );

        } else {
                LArTT_Selector<LArCellCont> sel;
                m_dataAccessSvc->loadCollections( context, *roiDescriptor, TTEM, 2, sel );
                for( const auto cell : sel ) {cdv->push_back( cell ); }
        }
  	auto ss = cellContainer.record( std::move(cdv) );
  	ATH_CHECK( ss );
	return StatusCode::SUCCESS;
  }

  } else {
  SG::WriteHandle<ConstDataVector<CaloCellContainerVector> > cellContainerV( m_cellContainerVKey, context );
  auto cdv = CxxUtils::make_unique<ConstDataVector<CaloCellContainerVector> >();
  ATH_CHECK( cellContainerV.record( std::move(cdv) ) );
  for( const TrigRoiDescriptor* roiDescriptor : *roiCollection) {
	if ( roiDescriptor->isFullscan() ) {
		auto c = std::make_unique<ConstDataVector<CaloCellContainer> >(SG::VIEW_ELEMENTS);
		m_dataAccessSvc->loadFullCollections( context, *c );
		cellContainerV->push_back( c.release()->asDataVector() );
		
	} else {
		LArTT_Selector<LArCellCont> sel;
		m_dataAccessSvc->loadCollections( context, *roiDescriptor, TTEM, 2, sel );
		auto c = std::make_unique<ConstDataVector<CaloCellContainer> >(SG::VIEW_ELEMENTS);
		int cc(0);
		for( const auto cell : sel ) {c->push_back( cell ); cc++;}
		cellContainerV->push_back( c.release()->asDataVector() );
	}
  }
  }


   
  return StatusCode::SUCCESS;
}
