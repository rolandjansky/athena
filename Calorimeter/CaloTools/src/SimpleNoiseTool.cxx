/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGauss.h"


#include "CaloTools/SimpleNoiseTool.h"

#include "CaloEvent/CaloCell.h"

std::map<float, CaloDetDescrElement> NoiseMap;

SimpleNoiseTool::SimpleNoiseTool(const std::string& type,
				 const std::string& name,
				 const IInterface* parent)
  : AthAlgTool(type,name,parent)
{

  declareInterface<ICalorimeterNoiseTool>(this);

  ATH_MSG_INFO( " SimpleNoiseTool start"  );
}

SimpleNoiseTool::~SimpleNoiseTool()
{ }

StatusCode SimpleNoiseTool::initialize()
{
  ATH_MSG_INFO( " SimpleNoiseTool initialize()"  );
  return StatusCode::SUCCESS;
}

float SimpleNoiseTool::getNoise(const CaloCell* /*aCell*/, CalorimeterNoiseType /*type*/)
{
  float noise=10.0;
  return noise;
}

float SimpleNoiseTool::getNoise(const CaloDetDescrElement* /*caloDDE*/, CalorimeterNoiseType /*type*/)
{
  float noise=10.0;
  
  return noise;
}

/////////////////////////////////////////////////////////////////////////

bool SimpleNoiseTool::isEOverNSigma(const CaloCell* caloCell, float sigmaCut ,
                     CalorimeterNoiseSymmetryHandling symmetryHandling,
                     CalorimeterNoiseType type)
{
  float noise_cut = fabs(sigmaCut*this->getEffectiveSigma(caloCell,  symmetryHandling, type));
  float energy = fabs(caloCell->e());
  return (energy>noise_cut);

}

////////////////////////////////////////////////////////////////////

float SimpleNoiseTool::getRandomisedE(const CaloCell* caloCell , CLHEP::HepRandomEngine* engine, CalorimeterNoiseType type)
{
  double rms = this->getNoise(caloCell,type);
  return CLHEP::RandGauss::shoot(engine,0.,rms);
}

//////////////////////////////////////////////////////////////////////////////////////

float SimpleNoiseTool::getEffectiveSigma(const CaloCell* caloCell,
                     CalorimeterNoiseSymmetryHandling /*symmetryHandling*/,
                     CalorimeterNoiseType type )
{
     // simple implementation for gaussian noise
     return this->getNoise(caloCell,type);
}
