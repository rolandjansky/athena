/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "AddNoiseCellBuilderTool.h"

#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandFlat.h"

#include "CaloEvent/CaloCellContainer.h"
#include "TileEvent/TileCell.h"
#include "StoreGate/ReadCondHandle.h"


AddNoiseCellBuilderTool::AddNoiseCellBuilderTool(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent)
  : base_class(type, name, parent)
{
}


AddNoiseCellBuilderTool::~AddNoiseCellBuilderTool()
= default;


StatusCode AddNoiseCellBuilderTool::initialize()
{
  ATH_MSG_DEBUG("Initialization started");

  ATH_CHECK(m_noiseKey.initialize());
  ATH_CHECK(m_estimatedGain.retrieve());

  // Random number service
  ATH_CHECK(m_rndmGenSvc.retrieve());

  ATH_MSG_DEBUG("Initialization finished");
  return StatusCode::SUCCESS;
}

StatusCode
AddNoiseCellBuilderTool::process (CaloCellContainer* theCellContainer,
                                  const EventContext& ctx) const
{
  // Set the RNG to use for this event.
  ATHRNG::RNGWrapper* rngWrapper = m_rndmGenSvc->getEngine(this);
  rngWrapper->setSeed( m_randomEngineName, ctx );
  CLHEP::HepRandomEngine *randomEngine = rngWrapper->getEngine(ctx);

  unsigned int rseed=0;
  while(rseed==0) {
    rseed=(unsigned int)( CLHEP::RandFlat::shoot(randomEngine) * std::numeric_limits<unsigned int>::max() );
  }

  ATH_MSG_INFO("Executing start calo size=" <<theCellContainer->size()<<" Event="<<ctx.evt()/*<<" rseed="<<rseed*/);

  SG::ReadCondHandle<CaloNoise> noise (m_noiseKey, ctx);

  double E_tot=0;
  double Et_tot=0;
  for (CaloCell* cell : *theCellContainer)
    {
      if(!cell) continue;
      const CaloDetDescrElement* theDDE=cell->caloDDE();
      if(!theDDE) continue;

      CaloGain::CaloGain gain=m_estimatedGain->estimatedGain (ctx, *cell,ICaloEstimatedGainTool::Step::CELLS);

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
        double sigma = noise->getNoise (cell->caloDDE()->calo_hash(), cell->gain());
        double enoise=CLHEP::RandGaussZiggurat::shoot(randomEngine,0.0,1.0)*sigma;
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
