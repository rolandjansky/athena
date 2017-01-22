/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FastCaloSim/AddNoiseCellBuilderTool.h"
#include "FastCaloSim/FastSimCell.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h" 

#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "CLHEP/Random/RandFlat.h"

#include "CaloEvent/CaloCellContainer.h"
#include "TileEvent/TileCell.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "GaudiKernel/ListItem.h"

//#include "TRandom3.h"

#include <map>
#include <iomanip>
#include <fstream>

AddNoiseCellBuilderTool::AddNoiseCellBuilderTool(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
      :  BasicCellBuilderTool(type, name, parent),
                           m_noiseToolName("CaloNoiseTool/calonoisetool"),
			   m_rndmSvc("AtRndmGenSvc", name),
			   m_randomEngine(0),
			   m_randomEngineName("FastCaloSimNoiseRnd"),
			   m_donoise(true)
{ 
  declareInterface<ICaloCellMakerTool>( this );
  declareProperty("CaloNoiseTool",         m_noiseToolName);
  declareProperty("doNoise",               m_donoise);
  declareProperty("RandomStreamName",      m_randomEngineName,     "Name of the random number stream");

  //m_rand = new TRandom3();
}


AddNoiseCellBuilderTool::~AddNoiseCellBuilderTool()
{
}


StatusCode AddNoiseCellBuilderTool::initialize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO <<  "Initialisating started" << endmsg ;

  StatusCode sc=BasicCellBuilderTool::initialize();
  
  IToolSvc* p_toolSvc = 0;    // Pointer to Tool Service
  sc = service("ToolSvc", p_toolSvc);
  if (sc.isFailure()) {
    log << MSG::FATAL
	<< " Tool Service not found "
	<< endmsg;
    return StatusCode::FAILURE;
  }

  IAlgTool* algtool;
  ListItem corr(m_noiseToolName);	  
  sc = p_toolSvc->retrieveTool(corr.type(), corr.name(), algtool);
  if (sc.isFailure()) {
    log << MSG::INFO
	<< "Unable to find tool for " << m_noiseToolName
	<< endmsg;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "Noise Tool "
	<< m_noiseToolName << " is selected!" << endmsg;
  }
  m_noiseTool=dynamic_cast<ICaloNoiseTool*>(algtool); 

  // Random number service
  if ( m_rndmSvc.retrieve().isFailure() ) {
    log<<MSG::ERROR<< "Could not retrieve " << m_rndmSvc << endmsg;
    return StatusCode::FAILURE;
  }

  //Get own engine with own seeds:
  m_randomEngine = m_rndmSvc->GetEngine(m_randomEngineName);
  if (!m_randomEngine) {
    log<<MSG::ERROR << "Could not get random engine '" << m_randomEngineName << "'" << endmsg;
    return StatusCode::FAILURE;
  }

  log << MSG::INFO <<  "Initialisating finished" << endmsg ;
  return sc;
}

StatusCode AddNoiseCellBuilderTool::process(CaloCellContainer * theCellContainer)
{
  MsgStream log( msgSvc(), name() );
  
  m_rndmSvc->print(m_randomEngineName);
  //m_randomEngine->showStatus();
  unsigned int rseed=0;
  while(rseed==0) {
    rseed=(unsigned int)( CLHEP::RandFlat::shoot(m_randomEngine) * std::numeric_limits<unsigned int>::max() );
  }
  //m_rand->SetSeed(rseed);

  log << MSG::INFO << "Executing start calo size=" <<theCellContainer->size()<<" Event="<<m_nEvent;//<<" rseed="<<rseed;
  //if(m_rand)  log<<" seed(m_rand="<<m_rand->ClassName()<<")="<<m_rand->GetSeed();
  log<< endmsg;

  ++m_nEvent;
  
  double E_tot=0;
  double Et_tot=0;
  CaloCellContainer::iterator f_cell = theCellContainer->begin();
  CaloCellContainer::iterator l_cell = theCellContainer->end();
  for ( ; f_cell!=l_cell; ++f_cell)
  {
    CaloCell* cell = (*f_cell) ;
    if(!cell) continue;
    const CaloDetDescrElement* theDDE=cell->caloDDE();
    if(!theDDE) continue;
    
    CaloGain::CaloGain gain=m_noiseTool->estimatedGain(cell,ICaloNoiseToolStep::CELLS);
    
    #if FastCaloSim_project_release_v1 == 12
      CaloCell_ID::SUBCALO calo=theDDE->getSubCalo();
      if(calo==CaloCell_ID::TILE) {
        ((FastSimTileCell*)cell)->setGain(gain);
      } else {
        ((FastSimCaloCell*)cell)->setGain(gain);
      }
    #else  
      cell->setGain(gain);
    #endif  

    if(m_donoise) {
      double sigma=m_noiseTool->elecNoiseRMS(cell);
      //double enoise=m_rand->Gaus(0.0,1.0)*sigma;
      double enoise=CLHEP::RandGaussZiggurat::shoot(m_randomEngine,0.0,1.0)*sigma;

      
    
/*
    if(cell->energy()>1000) {
      log<<MSG::DEBUG<<"sample="<<cell->caloDDE()->getSampling()<<" eta="<<cell->eta()<<" phi="<<cell->phi()<<" gain="<<gain<<" e="<<cell->energy()<<" sigma="<<sigma<<" enoise="<<enoise<<endmsg;
    }
*/    
      cell->setEnergy(cell->energy()+enoise);
    }  
    E_tot+=cell->energy();
    Et_tot+=cell->energy()/cosh(cell->eta());
  }  

  log << MSG::INFO << "Executing finished calo size=" <<theCellContainer->size()<<" ; e="<<E_tot<<" ; et="<<Et_tot<< endmsg;
  return StatusCode::SUCCESS;
}
