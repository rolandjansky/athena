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
  declareProperty("FullScanCells", m_fullScanCellsKey = std::string("FullScanCells"), "Full scan cells container");
  declareProperty("TrigDataAccessMT",m_dataAccessSvc,"Data Access for LVL2 Calo Algorithms in MT");
  declareProperty("roiMode",m_roiMode,"RoiMode roi->CaloCellCollection");
}

HLTCaloCellMaker::~HLTCaloCellMaker()
{
}

StatusCode HLTCaloCellMaker::initialize() {
  ATH_CHECK( m_roiCollectionKey.initialize() );
  if ( m_roiMode ){ 
    ATH_CHECK( m_cellContainerKey.initialize() );
  } else {
    ATH_CHECK( m_cellContainerVKey.initialize() );
    ATH_CHECK( m_fullScanCellsKey.initialize() );
  }
  
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
        ATH_CHECK(m_dataAccessSvc->loadFullCollections( context, *cdv ));
        std::cout << "HLTCaloCellMaker 0  full scan size " << cdv->size() << '\n'; 
        auto ss = cellContainer.record( std::move(cdv) );
        ATH_CHECK( ss );
        std::cout << "HLTCaloCellMaker:after move cll cont " << '\n';
      } else {
        LArTT_Selector<LArCellCont> sel;
        ATH_CHECK(m_dataAccessSvc->loadCollections( context, *roiDescriptor, TTEM, 2, sel ));
        for( const auto cell : sel ) {cdv->push_back( cell ); }
      }
      auto ss = cellContainer.record( std::move(cdv));
      ATH_CHECK( ss );
      std::cout << "HLTCellMaker 0: wrote Cells to " << cellContainer.key() << '\n';
      return StatusCode::SUCCESS;
    }
    
  } else {
    /*
    SG::WriteHandle<ConstDataVector<CaloCellContainerVector> > cellContainerV( m_cellContainerVKey, context );
    auto cdv = CxxUtils::make_unique<ConstDataVector<CaloCellContainerVector> >();
    ATH_CHECK( cellContainerV.record( std::move(cdv) ) );
    std::cout << "HLTCaloCellMaker 1: wrote Cells to " << cellContainerV.key() << '\n';
    for( const TrigRoiDescriptor* roiDescriptor : *roiCollection) {
      if ( roiDescriptor->isFullscan() ) {
        auto c = std::make_unique<ConstDataVector<CaloCellContainer> >(SG::VIEW_ELEMENTS);
        ATH_CHECK(m_dataAccessSvc->loadFullCollections( context, *c ));
        cellContainerV->push_back( c.release()->asDataVector() );
        std::cout << "HLTCaloCellMaker 1  full scan size " << (*cellContainerV)[0]->size() << '\n'; 
		
      } else {
        LArTT_Selector<LArCellCont> sel;
        ATH_CHECK(m_dataAccessSvc->loadCollections( context, *roiDescriptor, TTEM, 2, sel ));
        auto c = std::make_unique<ConstDataVector<CaloCellContainer> >(SG::VIEW_ELEMENTS);
        int cc(0);
        for( const auto cell : sel ) {c->push_back( cell ); cc++;}
        cellContainerV->push_back( c.release()->asDataVector() );
        }
      }
    }
    */
  
    SG::WriteHandle<ConstDataVector<CaloCellContainerVector> > cellContainerV( m_cellContainerVKey, context );
    auto cdv = CxxUtils::make_unique<ConstDataVector<CaloCellContainerVector> >();
    ATH_CHECK( cellContainerV.record( std::move(cdv) ) );
    std::cout << "HLTCaloCellMaker 1: wrote Cells to " << cellContainerV.key() << '\n';

    bool fullScanSeen{false};
    for( const TrigRoiDescriptor* roiDescriptor : *roiCollection) {
      if ( roiDescriptor->isFullscan() and !fullScanSeen) {
        fullScanSeen = true;
        ATH_CHECK(writeFullScan(context));
      } else { // either not full scan or full scan already written out
        LArTT_Selector<LArCellCont> sel;
        ATH_CHECK(m_dataAccessSvc->loadCollections( context, *roiDescriptor, TTEM, 2, sel ));
        auto c = std::make_unique<ConstDataVector<CaloCellContainer> >(SG::VIEW_ELEMENTS);
        int cc(0);
        for( const auto cell : sel ) {c->push_back( cell ); cc++;}
        cellContainerV->push_back( c.release()->asDataVector() );
      }
    }
  }
   
  return StatusCode::SUCCESS;
}

StatusCode HLTCaloCellMaker::writeFullScan(const EventContext& context) const {

  CaloCellContainer* cellContainer = new CaloCellContainer();
  
  ATH_CHECK(m_dataAccessSvc->loadFullCollections( context, cellContainer));
  cellContainer->updateCaloIterators();
  std::cout << "HLTCaloCellMaker 1.0  full scan size "
            << cellContainer->size() << '\n';
  
  // CaloCellContainer* ccc = new CaloCellContainer(c->begin(), c->end());
  // CaloCellContainer* ccc = new CaloCellContainer();
  // for(auto c : (*c)){(*ccc).push_back(c)};
  
  // cant move ccc: because is const?
  // const CaloCellContainer* ccc = c->asDataVector();
  
  // CaloCellContainer* ccc = new CaloCellContainer();
  // for(auto c : (*c)){(*ccc).push_back(c)};



  std::cout << "HLTCaloCellMaker 1.1  full scan size "
            << cellContainer->size() << '\n'; 

  // std::cout << "HLTCaloCellMaker 1.1  full scan size " << (*c)[0]->size() << '\n'; 

  SG::WriteHandle<CaloCellContainer> w_handle =
    SG::WriteHandle<CaloCellContainer>(m_fullScanCellsKey, context);
  ATH_CHECK(w_handle.record(std::move(std::unique_ptr<CaloCellContainer>(cellContainer))));
  return StatusCode::SUCCESS;
}
