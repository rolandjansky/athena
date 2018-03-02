/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     T2CaloJetGridFromFEBHeader.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloJet

AUTHORS:  Kyle Cranmer <cranmer@cern.ch>
          Ignacio Aracena <ignacio.aracena@cern.ch>

CREATED:  November, 2005

PURPOSE: Data preparation from FEB Header (Ex,Ey,Ez)

********************************************************************/
#include "TrigT2CaloJet/T2CaloJetGridFromFEBHeader.h"
#include "TrigCaloEvent/TrigT2Jet.h"

#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/IToolSvc.h"
#include "TrigT2CaloCommon/TrigDataAccess.h"
#include "AthenaKernel/Timeout.h"

T2CaloJetGridFromFEBHeader::T2CaloJetGridFromFEBHeader(const std::string& type,
				     const std::string& name,
				     const IInterface* parent):
  T2CaloJetBaseTool(type, name, parent),
  m_gridElement(NULL),
  m_usedfeb(NULL),
  m_timerSvc(0)
{  
  declareProperty("doTiming", m_doTiming= false );
}

T2CaloJetGridFromFEBHeader::~T2CaloJetGridFromFEBHeader() 
{
}

StatusCode T2CaloJetGridFromFEBHeader::initialize() 
{

  ATH_MSG_VERBOSE("  In initalize() ");

  ATH_CHECK( m_data.retrieve() );

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

  return StatusCode::SUCCESS;
}

TrigTimer* T2CaloJetGridFromFEBHeader::getTimer(const std::string& timerName){

  if (!m_doTiming ) return 0;

  std::map<std::string, TrigTimer*>::const_iterator t = m_timers.find(timerName);
  if (t == m_timers.end()) {
    ATH_MSG_DEBUG(" Timer " << timerName << " not found.");
    return 0;
  }


  return (*t).second;

}

StatusCode T2CaloJetGridFromFEBHeader::execute()
{
  return StatusCode::SUCCESS;
}


StatusCode T2CaloJetGridFromFEBHeader::execute(TrigT2Jet
* jet,double etamin, double etamax, double phimin, double phimax)
{

  // Time total AlgTool time
//  if (m_timersvc) m_timer[0]->start();

  // this method should use region selector to get RODs, retrieve FEB
  // Header, find eta for each FEB, and fill grid of Trig3Momentum.

  ATH_MSG_DEBUG(" executing T2CaloJetGridFromFEBHeader ");


  std::vector<Trig3Momentum>* grid = new std::vector<Trig3Momentum>();

  // now get ROD, FEB

  // turn ex,ey to e,eta,phi

  // possibly do some calibration

  // possibly do some tower building

  // now fill grid elements like this
  m_gridElement =  new Trig3Momentum();

  m_usedfeb = new std::vector<int>;
/*  if (m_timersvc) m_timer[1]->start();
  if (m_timersvc) m_timer[1]->pause();
  if (m_timersvc) m_timer[2]->start();
  if (m_timersvc) m_timer[2]->pause();
  if (m_timersvc) m_timer[3]->start();
  if (m_timersvc) m_timer[3]->pause();
  if (m_timersvc) m_timer[4]->start();
  if (m_timersvc) m_timer[4]->pause();

  if (m_timersvc) m_timer[5]->start();*/

  StatusCode sc = addFebEnergies(etamin, etamax, phimin, phimax, grid);
  if(sc.isFailure()){
    ATH_MSG_DEBUG(" Failure of addFebEnergies. Empty grid! ");
  }

//  gridElement->setE(10000.);
//  gridElement->setEta(1.);
//  gridElement->setPhi(0.);

  //push them back into the grid
//  grid->push_back(*gridElement);


  //then set the grid in the jet.  do not delete anything.  Cleanup
  //happens at the end of jet finding.
  jet->setGrid(grid);

  delete m_gridElement;
  m_gridElement = 0;
  delete m_usedfeb;
  m_usedfeb = 0;

/*  if (m_timersvc) m_timer[5]->stop();
  if (m_timersvc) m_timer[0]->stop();
  if (m_timersvc) m_timer[1]->stop();
  if (m_timersvc) m_timer[2]->stop();
  if (m_timersvc) m_timer[3]->stop();
  if (m_timersvc) m_timer[4]->stop();
*/
  return StatusCode::SUCCESS;
}

StatusCode T2CaloJetGridFromFEBHeader::addFebEnergies(double etamin, double etamax,
                                                    double phimin, double phimax,
                                                    std::vector<Trig3Momentum>* grid)
{


  if(m_doTiming) getTimer("LArUnpck")->start();
  if(addLArFebEnergy(etamin,etamax,phimin,phimax,grid,TTEM, 0, true).isFailure() ){
    if(m_doTiming) getTimer("LArUnpck")->stop();
    return StatusCode::FAILURE;
  }
  if(addLArFebEnergy(etamin,etamax,phimin,phimax,grid,TTEM, 1, true).isFailure() ){
    if(m_doTiming) getTimer("LArUnpck")->stop();
    return StatusCode::FAILURE;
  }
  if(addLArFebEnergy(etamin,etamax,phimin,phimax,grid,TTEM, 2, true).isFailure() ){
    if(m_doTiming) getTimer("LArUnpck")->stop();
    return StatusCode::FAILURE;
  }
  if(addLArFebEnergy(etamin,etamax,phimin,phimax,grid,TTEM, 3, true).isFailure() ){
    if(m_doTiming) getTimer("LArUnpck")->stop();
    return StatusCode::FAILURE;
  }

  if(m_doTiming){
    getTimer("LArUnpck")->stop();
    getTimer("HECUnpck")->start();
  }

  //Now HEC
  // Do not need to loop over all the samplings when reading HEC FEBs
  if(addLArFebEnergy(etamin, etamax, phimin, phimax, grid, TTHEC, 0, true).isFailure() ){
    if(m_doTiming) getTimer("HECUnpck")->stop();
    return StatusCode::FAILURE;
  }
//  if(addLArFebEnergy(etamin, etamax, phimin, phimax, grid, TTHEC, 1, true).isFailure() )
//    return StatusCode::FAILURE;
//  if(addLArFebEnergy(etamin, etamax, phimin, phimax, grid, TTHEC, 2, true).isFailure() )
//    return StatusCode::FAILURE;
//Now FCAL
  if(addLArFebEnergy(etamin, etamax, phimin, phimax, grid, FCALEM, 0, true).isFailure() )
    return StatusCode::FAILURE;
  //std::cout << "NOW TRY FCALHAD 0" << std::endl;
  if(addLArFebEnergy(etamin, etamax, phimin, phimax, grid, FCALHAD, 0, true).isFailure() )
    return StatusCode::FAILURE;
  //std::cout << "NOW TRY FCALHAD 1" << std::endl;
  if(addLArFebEnergy (etamin, etamax, phimin, phimax, grid, FCALHAD, 1, true).isFailure() )
    return StatusCode::FAILURE;
  if(m_doTiming){
    getTimer("HECUnpck")->stop();
    getTimer("TileUnpck")->start();
  }



  if(addTileCells(etamin, etamax, phimin, phimax, grid, TILE, 0, true).isFailure() ){
    if(m_doTiming) getTimer("TileUnpck")->stop();
    return StatusCode::FAILURE;
  }

    if(m_doTiming) getTimer("TileUnpck")->stop();

    return StatusCode::SUCCESS;
}

StatusCode T2CaloJetGridFromFEBHeader::addLArFebEnergy(double etamin, double etamax,
                                                 double phimin, double phimax,
                                                 std::vector<Trig3Momentum>* grid,
                                                 DETID detectorID, int sampling,
                                                 bool prepare)
{

  // Time to access RegionSelector
//  if (m_timersvc) m_timer[1]->resume();

  m_data->RegionSelector(sampling,etamin,etamax,phimin,phimax,detectorID);
//  if (m_timersvc) m_timer[1]->pause();

  // Time to access Collection
//  if (m_timersvc) m_timer[2]->resume();

  if ( m_data->LoadCollections(m_iFebBegin,m_iFebEnd, sampling , prepare).isFailure() )
    return StatusCode::FAILURE;
//  if (m_timersvc) m_timer[2]->pause();

  // Algorithmic time
//  if (m_timersvc) m_timer[4]->resume();

  for(m_Febit=m_iFebBegin ;m_Febit!=m_iFebEnd; ++m_Febit){

    bool used=false;
    CaloSampling::CaloSample samp=CaloSampling::Unknown;
    if(detectorID == TTHEC){
//
      samp = CaloSampling::HEC0;
    }
    if(detectorID == TTEM){
      int febid = (*m_Febit)->getFebId();
      int idit = 0;
      int iditend = m_usedfeb->size();
      /* avoid double-counting of EM FEBs */
      for(;idit!=iditend; ++idit){
        if(febid == (*m_usedfeb)[idit] ){ 
          used = true;
          idit++;
          break;
        }
      }
      m_usedfeb->push_back(febid);
      samp = CaloSampling::EMB1;
 //     std::cout << "In the TTEM; E_tot = " << FebEnergy << std::endl;
    }

    if(!used ){
      double FebEnergy = sqrt( ((*m_Febit)->getFebEx())*((*m_Febit)->getFebEx()) +
                           ((*m_Febit)->getFebEy())*((*m_Febit)->getFebEy()) + 
                           ((*m_Febit)->getFebEz())*((*m_Febit)->getFebEz()) );

      double phi = atan2((*m_Febit)->getFebEy(),(*m_Febit)->getFebEx());
      double eta = atanh((*m_Febit)->getFebEz()/FebEnergy);
      m_gridElement->setE(FebEnergy);
      m_gridElement->setEta(eta);
      m_gridElement->setPhi(phi);
      m_gridElement->setCaloSample(samp);
      grid->push_back(*m_gridElement);
    }
  }

  if ( Athena::Timeout::instance().reached() ) {
    ATH_MSG_ERROR("Timeout reached in addLArFebEnergy");
    return StatusCode::FAILURE;
  }
//  if (m_timersvc) m_timer[4]->pause();

  return StatusCode::SUCCESS;
}

StatusCode T2CaloJetGridFromFEBHeader::addTileCells(double etamin, double etamax,
                                            double phimin, double phimax,
                                            std::vector<Trig3Momentum>* grid,
                                            DETID detectorID, int sampling,
                                            bool /*prepare*/)
{

  // Time to access RegionSelector
//  if (m_timersvc) m_timer[1]->resume();
  // Get detector offline ID's for Collections
  m_data->RegionSelector(sampling,etamin,etamax,phimin,phimax, detectorID);

//  if (m_timersvc) m_timer[1]->pause();
  // Prepare data, load collections

  for( unsigned int iR=0; iR!=m_data->TileContSize();++iR ){
    // Time to access Collection
//    if (m_timersvc) m_timer[3]->resume();

    if ( m_data->LoadCollections(m_itBegin,m_itEnd, iR, !iR).isFailure() )
    return StatusCode::FAILURE;

//    if (m_timersvc) m_timer[3]->pause();
   // }


    // Algorithmic time
//    if (m_timersvc) m_timer[4]->resume();

  // Main Loop over cells
    for(m_itt = m_itBegin;m_itt != m_itEnd; ++m_itt) {

    // possibly do some noise thresholding

    // possibly do some calibration
    // may want sampling.  example:
    CaloSampling::CaloSample samp;
    //samp = CaloSampling::getSampling(*(*m_itt));
    samp = (*m_itt)->caloDDE()->getSampling();

    // possibly do some tower building

    // for now fill grid elements like this
//    gridElement =  new Trig3Momentum();
      m_gridElement->setE((*m_itt)->e());
      m_gridElement->setEta((*m_itt)->eta());
      m_gridElement->setPhi((*m_itt)->phi());
      m_gridElement->setCaloSample(samp);
    //push them back into the grid
      grid->push_back(*m_gridElement);

    }
//    if (m_timersvc) m_timer[4]->pause();
  }

  if ( Athena::Timeout::instance().reached() ) {
    ATH_MSG_ERROR( "Timeout reached in addTileCells " );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode T2CaloJetGridFromFEBHeader::finalize()
{
  return StatusCode::SUCCESS;
}
