/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     T2CaloJetGridFromCells.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet

AUTHORS:  Kyle Cranmer <cranmer@cern.ch>
CREATED:  November, 2005

PURPOSE: Data preparation from Cells

********************************************************************/
#include "TrigT2CaloJet/T2CaloJetGridFromCells.h"
#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigT2CaloJet/T2AllRoiUnpacking.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"

// should use this, but needs fixes
//#include "TrigT2CaloCommon/ITrigDataAccess.h" 
// instead, use this one
#include "TrigT2CaloCommon/TrigDataAccess.h"
#include "AthenaKernel/Timeout.h"

#include <algorithm>
#include <sstream>

T2CaloJetGridFromCells::T2CaloJetGridFromCells(const std::string& type,
				     const std::string& name,
				     const IInterface* parent):
  T2CaloJetBaseTool(type, name, parent),
  m_noiseCutValue(2.) ,
  m_cablingSvc("LArCablingLegacyService"),
  m_noiseTool("CaloNoiseTool/CaloNoiseToolDefault"),
  m_forbiddenRegions(0),
  m_timerSvc(0)
{
  declareProperty("doTiming", m_doTiming= false );
  declareProperty("cablingSvc", m_cablingSvc, "cabling svc");
  declareProperty("noiseTool", m_noiseTool, "handle for noise tool");
  declareProperty("applyNoiseCut", m_applyNoiseCut = false);
  declareProperty("noiseCutValue", m_noiseCutValue );
  declareProperty("doHECcellQualityCleaning", m_doHECcellQualityCleaning = false); // switch for cell based jet cleaning
  declareProperty("cellQualityThresholdHEC", m_cellQualityThresholdHEC = 65535);   // for cell based jet cleaning

}

T2CaloJetGridFromCells::~T2CaloJetGridFromCells() 
{
}


StatusCode T2CaloJetGridFromCells::initialize() 
{

  ATH_MSG_VERBOSE("  In initalize() ");

  ATH_CHECK(m_data.retrieve());

/// noise suppression
  if(m_applyNoiseCut!=0){
    ATH_CHECK( m_noiseTool.retrieve() );

    ATH_CHECK( m_cablingSvc.retrieve() );
  } else {
    m_noiseTool.disable();
    m_cablingSvc.disable();
  }

  // Retrieve timing service

  if (m_doTiming) {

    StatusCode sc = service("TrigTimerSvc", m_timerSvc);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "unable to locate timing service TrigTimerSvc."
                     << " Setting doTiming = false!!! " );
      m_timerSvc = 0;
      m_doTiming=false;
      //return sc;
    }
    else{
      
      std::string prefix = name() + ":";

     // Define now the timers:
      std::string label = prefix+"LArUnpck";
      TrigTimer *timer = m_timerSvc->addItem(label);
      m_timers["LArUnpck"] = timer;

      label =  prefix+"TileUnpck";
      timer = m_timerSvc->addItem( label);
      m_timers["TileUnpck"] = timer;

      label =  prefix+"HECUnpck";
      timer = m_timerSvc->addItem(label);
      m_timers["HECUnpck"] = timer;

    }
  }

  // from IAlgToolCalo
  m_geometryTool.disable();

  m_gridElement = new Trig3Momentum();

  return StatusCode::SUCCESS;
}

TrigTimer* T2CaloJetGridFromCells::getTimer(const std::string& timerName){ 

  if (!m_doTiming ) return 0;

  std::map<std::string, TrigTimer*>::const_iterator t = m_timers.find(timerName);
  if (t == m_timers.end()) {
    ATH_MSG_DEBUG( " Timer " << timerName << " not found." );
    return 0;
  }
    

  return (*t).second;

}


StatusCode T2CaloJetGridFromCells::execute()
{
  return StatusCode::SUCCESS;
}


//StatusCode T2CaloJetGridFromCells::execute(TrigT2Jet
//* jet,double etamin, double etamax, double phimin, double phimax)
StatusCode T2CaloJetGridFromCells::execute(TrigT2Jet
* jet,const IRoiDescriptor& roi  )
{

  // reset error
  m_error = 0x0;

  // this method should use region selector to get cells and fill grid
  // of Trig3Momentum.

  // reset error
  m_error = 0x0;
  std::vector<Trig3Momentum>* grid = new std::vector<Trig3Momentum>();
  grid->reserve(8000);
  

  //m_gridElement = new Trig3Momentum();
  // add all cells to grid

  StatusCode sc = addAllCells(roi.etaMinus(),roi.etaPlus(), roi.phiMinus(), roi.phiPlus(), grid);
  if(sc.isFailure()){
    ATH_MSG_DEBUG( " Failure of addAllCells. Empty grid! " );
  }
  
  //then set the grid in the jet.  do not delete anything.  Cleanup
  //happens at the end of jet finding.
  jet->setGrid(grid);

  //delete m_gridElement;
  //m_gridElement = 0;

  return StatusCode::SUCCESS;
}


StatusCode T2CaloJetGridFromCells::addAllCells(double etamin, double etamax, 
					    double phimin, double phimax,
					    std::vector<Trig3Momentum>* grid)
{

  //This could check job options for a list of samplings to consider.
  //We may not want to use presampler or strips.

  if(m_doTiming) getTimer("LArUnpck")->start();

  //Now EM
  if(addLArCells(etamin, etamax, phimin, phimax, grid, TTEM, 0, true).isFailure() ){
    if(m_doTiming) getTimer("LArUnpck")->stop();
    return StatusCode::FAILURE;
  }
  if(addLArCells(etamin, etamax, phimin, phimax, grid, TTEM, 1, true).isFailure() ) {
    if(m_doTiming) getTimer("LArUnpck")->stop();
    return StatusCode::FAILURE;
  }
  if(addLArCells(etamin, etamax, phimin, phimax, grid, TTEM, 2, true).isFailure() ) {
    if(m_doTiming) getTimer("LArUnpck")->stop();
    return StatusCode::FAILURE;
  }
  if(addLArCells(etamin, etamax, phimin, phimax, grid, TTEM, 3, true).isFailure() ) {
    if(m_doTiming) getTimer("LArUnpck")->stop();
    return StatusCode::FAILURE;
  }
  if(m_doTiming){
    getTimer("LArUnpck")->stop();
    getTimer("HECUnpck")->start();
  }

  //Now HEC
  if(addLArCells(etamin, etamax, phimin, phimax, grid, TTHEC, 0, true).isFailure() ) {
    if(m_doTiming) getTimer("HECUnpck")->stop();
    return StatusCode::FAILURE;
  }
  if(addLArCells(etamin, etamax, phimin, phimax, grid, TTHEC, 1, true).isFailure() ) {
    if(m_doTiming) getTimer("HECUnpck")->stop();
    return StatusCode::FAILURE;
  }
  if(addLArCells(etamin, etamax, phimin, phimax, grid, TTHEC, 2, true).isFailure() ) {
    if(m_doTiming) getTimer("HECUnpck")->stop();
    return StatusCode::FAILURE;
  }
  if(addLArCells(etamin, etamax, phimin, phimax, grid, TTHEC, 3, true).isFailure() ) {
    if(m_doTiming) getTimer("HECUnpck")->stop();
    return StatusCode::FAILURE;
  }
  if(m_doTiming){
    getTimer("HECUnpck")->stop();
    getTimer("TileUnpck")->start();
  }

  //Now Tile
  // only prepare for first sampling, because containers created then.
  if(addTileCells(etamin, etamax, phimin, phimax, grid, TILE, 0, true).isFailure() ){
    if(m_doTiming) getTimer("TileUnpck")->stop();
    return StatusCode::FAILURE;
  }

  //Now FCAL
  if(addLArCells(etamin, etamax, phimin, phimax, grid, FCALEM, 0, true).isFailure() )
    return StatusCode::FAILURE;

  if(addLArCells(etamin, etamax, phimin, phimax, grid, FCALHAD, 0, true).isFailure() )
    return StatusCode::FAILURE;

  if(addLArCells(etamin, etamax, phimin, phimax, grid, FCALHAD, 1, true).isFailure() )
    return StatusCode::FAILURE;

/*  if(addTileCells(etamin, etamax, phimin, phimax, grid, TILE, 1, false).isFailure() )  {
    if(m_doTiming) getTimer("TileUnpck")->stop();
    return StatusCode::FAILURE;
    }
  if(addTileCells(etamin, etamax, phimin, phimax, grid, TILE, 2, false).isFailure() ) {
    if(m_doTiming) getTimer("TileUnpck")->stop();
    return StatusCode::FAILURE;
    }*/

  if(m_doTiming) getTimer("TileUnpck")->stop();

  return StatusCode::SUCCESS;
}

StatusCode T2CaloJetGridFromCells::addLArCells(double etamin, double etamax, 
					    double phimin, double phimax,
					    std::vector<Trig3Momentum>* grid,
					    DETID detectorID, int sampling,
					    bool prepare)
{

  // Get detector offline ID's for Collections
  m_data->RegionSelector(sampling,etamin,etamax,phimin,phimax, detectorID);

  // Prepare data, load collections
  if ( m_data->LoadCollections(m_iBegin,m_iEnd, sampling, prepare).isFailure() ){
    return StatusCode::FAILURE;
  }
  
  m_error|=m_data->report_error();
  
  bool vetoRegions(m_forbiddenRegions.size() > 0);  
  // Main Loop over cells
  const LArCell* larcell;
  for(m_it = m_iBegin;m_it != m_iEnd; ++m_it) {
   
    // possibly do some noise thresholding
    larcell = *m_it;

    // possibly do some calibration
    // may want sampling.  example:
    CaloSampling::CaloSample samp;
    //samp = CaloSampling::getSampling(*larcell);
    samp = larcell->caloDDE()->getSampling();
    
    // cell based cleaning:
    // --------------------
    // check if cell cleaning is requested and if the cell is in hec:
    bool isHECandDoCleaning = false;
    if(	m_doHECcellQualityCleaning && ( samp == CaloSampling::HEC0 || samp == CaloSampling::HEC1 || 
				samp == CaloSampling::HEC2 || samp == CaloSampling::HEC3 ) ) {
        isHECandDoCleaning = true;
		}
    // now test for cell quality
    if( isHECandDoCleaning && larcell -> quality() >= m_cellQualityThresholdHEC ) {
      continue;
    }
    // end (cell based) cleaning

    // possibly do some tower building

    double cellE = larcell->e();
    if(m_applyNoiseCut){
       double larnoise = m_noiseTool->getNoise(larcell,
                                  ICalorimeterNoiseTool::TOTALNOISE);
    //std::cout << "noise rms in LAr = " << larnoise << std::endl;
       if(m_applyNoiseCut && fabs(cellE) < (m_noiseCutValue*larnoise)) continue;
    }

    double eta(larcell->eta());
    double phi(larcell->phi());
    if(vetoRegions && isInVetoRegion(eta, phi)) continue;
    // for now fill grid elements like this
//    gridElement =  new Trig3Momentum();
    m_gridElement->setE(cellE);
    m_gridElement->setEta(eta);
    m_gridElement->setPhi(phi);
    m_gridElement->setCaloSample(samp);
    // jet cleaning:
    m_gridElement->setProvenance( larcell -> provenance() );
    m_gridElement->setQuality( larcell -> quality() );  
    m_gridElement->setTime( larcell -> time() );
    
    //push them back into the grid
    grid->push_back(*m_gridElement);

  }

  if ( Athena::Timeout::instance().reached() ) {
    ATH_MSG_ERROR( "Timeout reached in addLArCells " );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode T2CaloJetGridFromCells::addTileCells(double etamin, double etamax, 
					    double phimin, double phimax,
					    std::vector<Trig3Momentum>* grid,
					    DETID detectorID, int sampling,
					    bool /*prepare*/)
{

  // Get detector offline ID's for Collections
  m_data->RegionSelector(sampling,etamin,etamax,phimin,phimax, detectorID);

  // Prepare data, load collections
  
  for( unsigned int iR=0; iR!=m_data->TileContSize();++iR ){
    if ( m_data->LoadCollections(m_itBegin,m_itEnd, iR, !iR).isFailure() ){
       return StatusCode::FAILURE;
    }

    m_error|=m_data->report_error();
    
    bool vetoRegions(m_forbiddenRegions.size() > 0);    

//    std::cout << "No of cells in Tile = " << m_data->TileContSize() << std::endl;
  // Main Loop over cells
    const TileCell* tilecell;
    for(m_itt = m_itBegin;m_itt != m_itEnd; ++m_itt) {
    tilecell = *m_itt;
   
    // possibly do some noise thresholding

    // possibly do some calibration
    // may want sampling.  example:
      CaloSampling::CaloSample samp;
      //samp = CaloSampling::getSampling(*tilecell);
      samp = tilecell->caloDDE()->getSampling();


      double cellE = tilecell->e();
//  do noise thresholding 

      if(m_applyNoiseCut) {
        double tilenoise = m_noiseTool->getNoise(tilecell,
                                  ICalorimeterNoiseTool::TOTALNOISE);
        if(m_applyNoiseCut && fabs(cellE) < (m_noiseCutValue*tilenoise)) continue;
      }

    // possibly do some tower building

      double eta(tilecell->eta());
      double phi(tilecell->phi());
      if(vetoRegions && isInVetoRegion(eta, phi)) continue;

    // for now fill grid elements like this
//    gridElement =  new Trig3Momentum();
      m_gridElement->setE(cellE);
      m_gridElement->setEta(eta);
      m_gridElement->setPhi(phi);
      m_gridElement->setCaloSample(samp);
      // jet cleaning:
      m_gridElement->setProvenance( tilecell -> provenance() );
      m_gridElement->setQuality( tilecell -> quality() );  
      m_gridElement->setTime( tilecell -> time() );
    
      //push them back into the grid
      grid->push_back(*m_gridElement);


    }
    if ( Athena::Timeout::instance().reached() ) {
      ATH_MSG_ERROR( "Timeout reached in addTileCells " );
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode T2CaloJetGridFromCells::finalize()
{
  delete m_gridElement;
  m_gridElement = 0;
  return StatusCode::SUCCESS;
}

// Just a functor so that I can use EtaPhiRectangle::contains within a std algorithm.
struct EtaPhiRectInsideChecker : public std::unary_function<EtaPhiRectangle, bool> {
  double eta_, phi_;
  EtaPhiRectInsideChecker(const double &eta, const double &phi): eta_(eta), phi_(phi) {} ;
  bool operator() (const EtaPhiRectangle &r) const {
    return r.contains(eta_, phi_);
  }
};

bool T2CaloJetGridFromCells::isInVetoRegion(const double &eta, const double &phi){
  ATH_MSG_DEBUG("T2CaloJetGridFromCells::isInVetoRegion --DG-- checking"
                <<" ("<<eta<<", "<<phi<<")"
                <<" against "<<m_forbiddenRegions.size()<<" forbidden regions" );
  return
    m_forbiddenRegions.end() !=
    std::find_if(m_forbiddenRegions.begin(),
		 m_forbiddenRegions.end(),
		 EtaPhiRectInsideChecker(eta,phi));
}

