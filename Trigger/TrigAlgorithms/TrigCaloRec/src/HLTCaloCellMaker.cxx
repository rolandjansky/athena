/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/*
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
#include "AthenaMonitoringKernel/Monitored.h"

HLTCaloCellMaker::HLTCaloCellMaker(const std::string & name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode HLTCaloCellMaker::initialize() {

  ATH_CHECK( m_roiCollectionKey.initialize(SG::AllowEmpty) );

  if ( m_roiMode )
    ATH_CHECK( m_cellContainerKey.initialize() );
  else
    ATH_CHECK( m_cellContainerVKey.initialize() );
  ATH_CHECK( m_tileEMScaleKey.initialize() );
  ATH_CHECK( m_bcidAvgKey.initialize() );
  CHECK( m_dataAccessSvc.retrieve() );
  if (! m_monTool.empty() ) ATH_CHECK( m_monTool.retrieve() );
  return StatusCode::SUCCESS;
}

StatusCode HLTCaloCellMaker::execute( const EventContext& context ) const {

  auto timer = Monitored::Timer("TIME_exec");
  auto clN = Monitored::Scalar  ("Cells_N",-999.0);

  const bool seedLess = m_roiCollectionKey.empty();
  const TrigRoiDescriptorCollection* roiCollection; 
  if (!seedLess){
    auto roisHandle = SG::makeHandle( m_roiCollectionKey, context );
    if ( not roisHandle.isValid() ) {
      ATH_MSG_ERROR("Cell maker did not get a valid RoIs collection");
      return StatusCode::FAILURE;
    }
    roiCollection = roisHandle.cptr();
  } 
  else { // it is seedLess
    TrigRoiDescriptorCollection* roiCol = new TrigRoiDescriptorCollection();
    TrigRoiDescriptor* FS = new TrigRoiDescriptor(true);
    roiCol->push_back( FS );
    roiCollection = const_cast<TrigRoiDescriptorCollection*>(roiCol);
  }
  ATH_MSG_DEBUG("Operating on " << roiCollection->size() <<"RoI(s)");

  // datahandle 
  if ( m_roiMode ) {
    if ( roiCollection->size() > 1 ) 
      ATH_MSG_DEBUG ( "roiMode but multiple rois found, will only use the first one");

    SG::WriteHandle<CaloConstCellContainer > cellContainer = SG::WriteHandle< CaloConstCellContainer > ( m_cellContainerKey, context );
    auto cdv = std::make_unique<CaloConstCellContainer>(SG::VIEW_ELEMENTS);
    auto clET = Monitored::Collection ("Cells_eT",*cdv,getCellPt);
    auto clEta = Monitored::Collection ("Cells_eta",*cdv,&CaloCell::eta);
    auto clPhi = Monitored::Collection ("Cells_phi",*cdv,&CaloCell::phi);
    for( const TrigRoiDescriptor* roiDescriptor : *roiCollection) {
      ATH_MSG_DEBUG ( "Running on RoI " << *roiDescriptor<< " FS="<<roiDescriptor->isFullscan());
      if ( roiDescriptor->isFullscan() ) {
        ATH_CHECK(m_dataAccessSvc->loadFullCollections( context, *cdv ));
	cdv->setHasCalo(CaloCell_ID::LAREM);
	cdv->setHasCalo(CaloCell_ID::LARHEC);
	cdv->setHasCalo(CaloCell_ID::LARFCAL);
	cdv->setHasCalo(CaloCell_ID::TILE);
	cdv->updateCaloIterators();
        clN=cdv->size();
        if ( m_monCells ){
          auto monitoring = Monitored::Group( m_monTool, timer, clN, clET, clEta, clPhi);
        } else {
          auto monitoring = Monitored::Group( m_monTool, timer, clN );
        }
	
      } else {
	// TT EM PART
	for(int sampling=0;sampling<4;sampling++){
	LArTT_Selector<LArCellCont> sel;
	ATH_CHECK(m_dataAccessSvc->loadCollections( context, *roiDescriptor, TTEM, sampling, sel ));
	for( const auto cell : sel ) {cdv->push_back( cell ); }
	}
	cdv->setHasCalo(CaloCell_ID::LAREM);
	// TT HEC PART
	for(int sampling=0;sampling<4;sampling++){
	LArTT_Selector<LArCellCont> sel;
	ATH_CHECK(m_dataAccessSvc->loadCollections( context, *roiDescriptor, TTHEC, sampling, sel ));
	for( const auto cell : sel ) {cdv->push_back( cell ); }
	}
	cdv->setHasCalo(CaloCell_ID::LARHEC);
	// TILE PART
	{
	TileCellCollection sel;
	ATH_CHECK(m_dataAccessSvc->loadCollections( context, *roiDescriptor, sel ));
	for( const auto cell : sel ) { 
	  if(m_tileCellsInROI && !tileCellEtaInRoi(cell, roiDescriptor)) continue;
	  cdv->push_back( cell );
	}
	}
	cdv->setHasCalo(CaloCell_ID::TILE);
	// TT FCAL EM PART
	{
	LArTT_Selector<LArCellCont> sel;
	ATH_CHECK(m_dataAccessSvc->loadCollections( context, *roiDescriptor, FCALEM, 0, sel ));
	for( const auto cell : sel ) {cdv->push_back( cell ); }
	}
	cdv->setHasCalo(CaloCell_ID::LARFCAL);
	// TT FCAL HAD PART
	for(int sampling=0;sampling<2;sampling++){
	LArTT_Selector<LArCellCont> sel;
	ATH_CHECK(m_dataAccessSvc->loadCollections( context, *roiDescriptor, FCALHAD, sampling, sel ));
	for( const auto cell : sel ) {cdv->push_back( cell ); }
	}
	cdv->setHasCalo(CaloCell_ID::LARFCAL);
	cdv->updateCaloIterators();
      }
      ATH_MSG_DEBUG ("REGTEST: Producing "<<cdv->size()<<" cells");
      clN=cdv->size();
      if ( m_monCells ){
        auto monitoring = Monitored::Group( m_monTool, timer, clN, clET, clEta, clPhi);
      } else {
        auto monitoring = Monitored::Group( m_monTool, timer, clN );
      }
      auto ss = cellContainer.record( std::move(cdv) );
      ATH_CHECK( ss );

      // we have to take care of this
      if ( seedLess ) { delete roiCollection; }
      return StatusCode::SUCCESS;
    }

  } else {
    SG::WriteHandle<ConstDataVector<CaloCellContainerVector> > cellContainerV( m_cellContainerVKey, context );
    auto cdv = std::make_unique<ConstDataVector<CaloCellContainerVector> >();
    ATH_CHECK( cellContainerV.record( std::move(cdv) ) );
    for( const TrigRoiDescriptor* roiDescriptor : *roiCollection) {
      if ( roiDescriptor->isFullscan() ) {
	auto c = std::make_unique<CaloConstCellContainer >(SG::VIEW_ELEMENTS);
        auto clET = Monitored::Collection ("Cells_eT",*c,getCellPt);
        auto clEta = Monitored::Collection ("Cells_eta",*c,&CaloCell::eta);
        auto clPhi = Monitored::Collection ("Cells_phi",*c,&CaloCell::phi);
	ATH_CHECK(m_dataAccessSvc->loadFullCollections( context, *c ));
        clN=c->size();
        if ( m_monCells ){
          auto monitoring = Monitored::Group( m_monTool, timer, clN, clET, clEta, clPhi);
        } else {
          auto monitoring = Monitored::Group( m_monTool, timer, clN );
        }
	cellContainerV->push_back( c.release()->asDataVector() );
      } else {
	auto c = std::make_unique<CaloConstCellContainer >(SG::VIEW_ELEMENTS);
        auto clET = Monitored::Collection ("Cells_eT",*c,getCellPt);
        auto clEta = Monitored::Collection ("Cells_eta",*c,&CaloCell::eta);
        auto clPhi = Monitored::Collection ("Cells_phi",*c,&CaloCell::phi);

        // TT EM PART
        for(int sampling=0;sampling<4;sampling++){
        LArTT_Selector<LArCellCont> sel;
        ATH_CHECK(m_dataAccessSvc->loadCollections( context, *roiDescriptor, TTEM, sampling, sel ));
        for( const auto cell : sel ) {c->push_back( cell ); }
        }
        c->setHasCalo(CaloCell_ID::LAREM);
        // TT HEC PART
        for(int sampling=0;sampling<4;sampling++){
        LArTT_Selector<LArCellCont> sel;
        ATH_CHECK(m_dataAccessSvc->loadCollections( context, *roiDescriptor, TTHEC, sampling, sel ));
        for( const auto cell : sel ) {c->push_back( cell ); }
        }
        c->setHasCalo(CaloCell_ID::LARHEC);
        // TILE PART
        {
        TileCellCollection sel;
        ATH_CHECK(m_dataAccessSvc->loadCollections( context, *roiDescriptor, sel ));
        for( const auto cell : sel ) {
	  if(m_tileCellsInROI && !tileCellEtaInRoi(cell, roiDescriptor)) continue;
	  c->push_back( cell );
	}
        }
        c->setHasCalo(CaloCell_ID::TILE);
        // TT FCAL EM PART
        {
        LArTT_Selector<LArCellCont> sel;
        ATH_CHECK(m_dataAccessSvc->loadCollections( context, *roiDescriptor, FCALEM, 0, sel ));
        for( const auto cell : sel ) {c->push_back( cell ); }
        }
        c->setHasCalo(CaloCell_ID::LARFCAL);
        // TT FCAL HAD PART
        for(int sampling=0;sampling<2;sampling++){
        LArTT_Selector<LArCellCont> sel;
        ATH_CHECK(m_dataAccessSvc->loadCollections( context, *roiDescriptor, FCALHAD, sampling, sel ));
        for( const auto cell : sel ) {c->push_back( cell ); }
        }
        c->setHasCalo(CaloCell_ID::LARFCAL);
        c->updateCaloIterators();
        clN=c->size();
        if ( m_monCells ){
          auto monitoring = Monitored::Group( m_monTool, timer, clN, clET, clEta, clPhi);
        } else {
          auto monitoring = Monitored::Group( m_monTool, timer, clN);
        }
	cellContainerV->push_back( c.release()->asDataVector() );
      }
    }
  }
   
  if ( seedLess ) { delete roiCollection; }
  return StatusCode::SUCCESS;
}
