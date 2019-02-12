/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FastCaloSim/AddNoiseCellBuilderTool.h"
#include "FastCaloSim/FastSimCell.h"

#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandFlat.h"

#include "CaloEvent/CaloCellContainer.h"
#include "TileEvent/TileCell.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include <map>
#include <iomanip>
#include <fstream>

AddNoiseCellBuilderTool::AddNoiseCellBuilderTool(
                                                 const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent)
  : BasicCellBuilderTool(type, name, parent)
  , m_noiseTool("CaloNoiseTool/calonoisetool")
  , m_rndmSvc("AtRndmGenSvc", name)
{
  declareProperty("CaloNoiseTool",         m_noiseTool);
  declareProperty("doNoise",               m_donoise);
  declareProperty("RandomStreamName",      m_randomEngineName,     "Name of the random number stream");

}


AddNoiseCellBuilderTool::~AddNoiseCellBuilderTool()
{
}


StatusCode AddNoiseCellBuilderTool::initialize()
{
  ATH_MSG_DEBUG("Initialization started");

  ATH_CHECK(BasicCellBuilderTool::initialize());

  ATH_CHECK(m_noiseTool.retrieve());

  // Random number service
  ATH_CHECK(m_rndmSvc.retrieve());

  //Get own engine with own seeds:
  m_randomEngine = m_rndmSvc->GetEngine(m_randomEngineName);
  if (!m_randomEngine) {
    ATH_MSG_ERROR("Could not get random engine '" << m_randomEngineName << "'");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Initialization finished");
  return StatusCode::SUCCESS;
}

StatusCode
AddNoiseCellBuilderTool::process (CaloCellContainer* theCellContainer,
                                  const EventContext& ctx) const
{
  m_rndmSvc->print(m_randomEngineName);
  //m_randomEngine->showStatus();
  unsigned int rseed=0;
  while(rseed==0) {
    rseed=(unsigned int)( CLHEP::RandFlat::shoot(m_randomEngine) * std::numeric_limits<unsigned int>::max() );
  }

  ATH_MSG_INFO("Executing start calo size=" <<theCellContainer->size()<<" Event="<<ctx.evt()/*<<" rseed="<<rseed*/);

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
          ATH_MSG_DEBUG("sample="<<cell->caloDDE()->getSampling()<<" eta="<<cell->eta()<<" phi="<<cell->phi()<<" gain="<<gain<<" e="<<cell->energy()<<" sigma="<<sigma<<" enoise="<<enoise);
          }
        */
        cell->setEnergy(cell->energy()+enoise);
      }
      E_tot+=cell->energy();
      Et_tot+=cell->energy()/cosh(cell->eta());
    }

  ATH_MSG_INFO("Executing finished calo size=" <<theCellContainer->size()<<" ; e="<<E_tot<<" ; et="<<Et_tot);
  return StatusCode::SUCCESS;
}
